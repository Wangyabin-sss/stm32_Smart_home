/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "w25qxx.h"
#include "sdio_sdcard.h"

/* Definitions of physical drive number for each drive */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#define SPI_FLASH 	0
#define SD_CARD		1


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;

	switch (pdrv) {
	case SPI_FLASH :
		//result = RAM_disk_status();
		if(W25Q128_ID == W25QXX_Read_Id())
		{
			// 状态正常
			stat = 0;
		}
		else
		{
			// 状态异常
			stat = STA_NOINIT;
		}
	
		// translate the reslut code here

		return stat;

	case SD_CARD :
		
		
		return 0;

	case DEV_USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int res;

	switch (pdrv) {
	case SPI_FLASH :
		//result = RAM_disk_initialize();
		spi1_init();
		// translate the reslut code here
		stat = RES_OK;
		return stat;

	case SD_CARD :
		//result = MMC_disk_initialize();
		if(SD_Init()==0)//SD卡初始化 
			stat = RES_OK;
		else 
			stat = STA_NOINIT;
		// translate the reslut code here

		return stat;

	case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case SPI_FLASH :
		// translate the arguments here

		//result = RAM_disk_read(buff, sector, count);
		for(;count>0;count--)
		{
			W25QXX_Read_Data(buff,sector*512,512);
			sector++;
			buff+=512;
		}
		// translate the reslut code here
		res = RES_OK;
		return res;

	case SD_CARD :
		result=SD_ReadDisk(buff,sector,count);	 
		while(result)//读出错
		{
			SD_Init();	//重新初始化SD卡
			result=SD_ReadDisk(buff,sector,count);	
			//printf("sd rd error:%d\r\n",res);
		}
		if(result==0x00)return RES_OK;	 
		else return RES_ERROR;	   
		
	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case SPI_FLASH :
		// translate the arguments here

		for(;count>0;count--)
		{										    
			W25QXX_Write_Data((uint8_t *)buff,sector*512,512);
			sector++;
			buff+=512;
		}

		// translate the reslut code here
		res = RES_OK;
		return res;

	case SD_CARD :
		result=SD_WriteDisk((u8*)buff,sector,count);
		while(result)//写出错
		{
			SD_Init();	//重新初始化SD卡
			result=SD_WriteDisk((u8*)buff,sector,count);	
			//printf("sd wr error:%d\r\n",res);
		}

		if(result==0x00)return RES_OK;	 
		else return RES_ERROR;	  

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case SPI_FLASH :

		switch (cmd)
		{
			case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = 1024*2*12;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
		}

		return res;

	case SD_CARD :

		switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
				*(WORD*)buff = SDCardInfo.CardBlockSize;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SDCardInfo.CardCapacity/512;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}


DWORD get_fattime (void)
{				 
	return 0;
}	

/*
//动态分配内存
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
//释放内存
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}
*/

