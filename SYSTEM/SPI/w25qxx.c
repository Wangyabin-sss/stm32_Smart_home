#include "w25qxx.h"


//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector 

uint16_t W25QXX_Read_Id(void)
{
	uint16_t ID=0;
	W25QXX_CS=0;
	SPI1_ReadWriteByte(W25X_ManufactDeviceID);
	SPI1_ReadWriteByte(0x0);
	SPI1_ReadWriteByte(0x0);
	SPI1_ReadWriteByte(0x0);
	ID  = SPI1_ReadWriteByte(0xff)<<8;
	ID |= SPI1_ReadWriteByte(0xff);
	W25QXX_CS=1;
	return ID;
}


void W25QXX_Read_Data(uint8_t *data,uint32_t addr,uint16_t num)
{
	W25QXX_CS=0;
	SPI1_ReadWriteByte(W25X_ReadData);
	SPI1_ReadWriteByte((uint8_t)(addr>>16));
	SPI1_ReadWriteByte((uint8_t)(addr>>8));
	SPI1_ReadWriteByte((uint8_t) addr);
	for(int i=0;i<num;i++)
    {
        data[i]=SPI1_ReadWriteByte(0xff);
    }
	W25QXX_CS=1;
}

void W25QXX_Write_Enable(void)
{
	W25QXX_CS=0;
	SPI1_ReadWriteByte(W25X_WriteEnable);
	W25QXX_CS=1;
}

void W25QXX_Write_Disable(void)
{
	W25QXX_CS=0;
	SPI1_ReadWriteByte(W25X_WriteDisable);
	W25QXX_CS=1;
}
//读取W25QXX的状态寄存器
//BIT7  6    5    4    3    2    1    0
//SPR   RV  TB   BP2  BP1  BP0  WEL  BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
static uint8_t W25QXX_ReadSR(void)   
{  
	uint8_t byte=0;   
	W25QXX_CS=0;                             //使能器件   
	SPI1_ReadWriteByte(W25X_ReadStatusReg);    //发送读取状态寄存器命令    
	byte=SPI1_ReadWriteByte(0Xff);             //读取一个字节  
	W25QXX_CS=1;                            //取消片选     
	return byte;   
} 

//等待空闲
static void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
} 

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    W25QXX_Write_Enable();                  //SET WEL 
	W25QXX_CS=0;                            //使能器件   
    SPI1_ReadWriteByte(W25X_PageProgram);      //发送写页命令   
    SPI1_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //发送24bit地址    
    SPI1_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    SPI1_ReadWriteByte((uint8_t)  WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);//循环写数  
	W25QXX_CS=1;                            //取消片选 
	delay_us(5);
	W25QXX_Wait_Busy();					   //等待写入结束
	W25QXX_Write_Disable();
} 
//扇区擦除，一个扇区4096字节
void W25X_Sector_Erase(uint32_t addr)
{
	W25QXX_Write_Enable();
	W25QXX_CS=0;                            //使能器件   
    SPI1_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除命令   
    SPI1_ReadWriteByte((uint8_t)((addr)>>16)); //发送24bit地址    
    SPI1_ReadWriteByte((uint8_t)((addr)>>8));   
    SPI1_ReadWriteByte((uint8_t)  addr);   
	W25QXX_CS=1;                            //取消片选 
	delay_us(5);
	W25QXX_Wait_Busy();					   //等待擦除结束
	W25QXX_Write_Disable();
}


//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
uint8_t W25QXX_BUFFER[4096];		 
void W25QXX_Write_Data(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		W25QXX_Read_Data(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			W25X_Sector_Erase(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
}



/*

FATFS fsObject;
FIL fp;
UINT bw;
UINT br;
 
uint8_t work[4096];
const char wData[] = "FatFs文件系统测试成功！";
char rData[4096] = "";



int main(void)
{
	FRESULT res;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	usart1_init(115200);
	
	// 挂载文件系统，就是初始化硬件GPIO接口和SPI，使能
	//   第一个参数为文件系统句柄，第二个参数为路径，第三个参数1表示立即挂载 
	res = f_mount(&fsObject, "0:", 1);	
	printf("f_mount res = %d\n", res);
	
	// 只有当存储介质不存在文件系统时，才进行格式化
	if (res == FR_NO_FILESYSTEM)
	{
		res = f_mkfs("0:", FM_FAT32 ,4096, work, sizeof work);		// 格式化存储介质
		printf("f_mkfs res = %d\n", res);
		// 格式化后要取消挂载，然后再重新挂载文件系统
		res = f_mount(NULL, "0:", 1);	
		res = f_mount(&fsObject, "0:", 1);	
		
		printf("second f_mount res = %d\n", res);
	}
	
	// 以可读可写权限打开一个文件
	res = f_open(&fp, "0:abc.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	printf("f_open res = %d\n", res);
	
	// 如果打开文件成功，就可以写入数据
	if (res == FR_OK)
	{
		res = f_write(&fp, wData, sizeof(wData), &bw);
		printf("bw = %d\n", bw);
		
		if (res == FR_OK)
		{
			f_lseek(&fp, 0);		// 把文件的光标移到最开头处
			res = f_read(&fp, rData, f_size(&fp), &br);
			if (res == FR_OK)
			{
				printf("文件内容： %s, br = %d\n", rData, br);
			}
		}
		f_close(&fp);					// 关闭文件
	}
	
	while(1)
	{
		
	}
}

*/













