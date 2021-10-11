#include "w25qxx.h"


//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q128
//����Ϊ16M�ֽ�,����128��Block,4096��Sector 

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
//��ȡW25QXX��״̬�Ĵ���
//BIT7  6    5    4    3    2    1    0
//SPR   RV  TB   BP2  BP1  BP0  WEL  BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
static uint8_t W25QXX_ReadSR(void)   
{  
	uint8_t byte=0;   
	W25QXX_CS=0;                             //ʹ������   
	SPI1_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI1_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
} 

//�ȴ�����
static void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
} 

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    W25QXX_Write_Enable();                  //SET WEL 
	W25QXX_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
    SPI1_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //����24bit��ַ    
    SPI1_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    SPI1_ReadWriteByte((uint8_t)  WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	W25QXX_CS=1;                            //ȡ��Ƭѡ 
	delay_us(5);
	W25QXX_Wait_Busy();					   //�ȴ�д�����
	W25QXX_Write_Disable();
} 
//����������һ������4096�ֽ�
void W25X_Sector_Erase(uint32_t addr)
{
	W25QXX_Write_Enable();
	W25QXX_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_SectorErase);      //����������������   
    SPI1_ReadWriteByte((uint8_t)((addr)>>16)); //����24bit��ַ    
    SPI1_ReadWriteByte((uint8_t)((addr)>>8));   
    SPI1_ReadWriteByte((uint8_t)  addr);   
	W25QXX_CS=1;                            //ȡ��Ƭѡ 
	delay_us(5);
	W25QXX_Wait_Busy();					   //�ȴ���������
	W25QXX_Write_Disable();
}


//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
uint8_t W25QXX_BUFFER[4096];		 
void W25QXX_Write_Data(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		W25QXX_Read_Data(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25X_Sector_Erase(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}



/*

FATFS fsObject;
FIL fp;
UINT bw;
UINT br;
 
uint8_t work[4096];
const char wData[] = "FatFs�ļ�ϵͳ���Գɹ���";
char rData[4096] = "";



int main(void)
{
	FRESULT res;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	usart1_init(115200);
	
	// �����ļ�ϵͳ�����ǳ�ʼ��Ӳ��GPIO�ӿں�SPI��ʹ��
	//   ��һ������Ϊ�ļ�ϵͳ������ڶ�������Ϊ·��������������1��ʾ�������� 
	res = f_mount(&fsObject, "0:", 1);	
	printf("f_mount res = %d\n", res);
	
	// ֻ�е��洢���ʲ������ļ�ϵͳʱ���Ž��и�ʽ��
	if (res == FR_NO_FILESYSTEM)
	{
		res = f_mkfs("0:", FM_FAT32 ,4096, work, sizeof work);		// ��ʽ���洢����
		printf("f_mkfs res = %d\n", res);
		// ��ʽ����Ҫȡ�����أ�Ȼ�������¹����ļ�ϵͳ
		res = f_mount(NULL, "0:", 1);	
		res = f_mount(&fsObject, "0:", 1);	
		
		printf("second f_mount res = %d\n", res);
	}
	
	// �Կɶ���дȨ�޴�һ���ļ�
	res = f_open(&fp, "0:abc.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	printf("f_open res = %d\n", res);
	
	// ������ļ��ɹ����Ϳ���д������
	if (res == FR_OK)
	{
		res = f_write(&fp, wData, sizeof(wData), &bw);
		printf("bw = %d\n", bw);
		
		if (res == FR_OK)
		{
			f_lseek(&fp, 0);		// ���ļ��Ĺ���Ƶ��ͷ��
			res = f_read(&fp, rData, f_size(&fp), &br);
			if (res == FR_OK)
			{
				printf("�ļ����ݣ� %s, br = %d\n", rData, br);
			}
		}
		f_close(&fp);					// �ر��ļ�
	}
	
	while(1)
	{
		
	}
}

*/













