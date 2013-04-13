/*
 * spiflash.h
 *
 *  Created on: Dec 20, 2012
 *      Author: root
 */

#ifndef SPIFLASH_H_
#define SPIFLASH_H_

#ifndef bool
#define bool unsigned char
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

// M25P80 Memory Definitions
#define FLASH_SIZE (1 * 1024 * 1024) 	// 1MB
#define FLASH_SECTOR_SIZE (64 * 1024) 	// 64KB
#define FLASH_PAGE_SIZE (256)			// 256B

void SPIFLASH_Init(void);
void SPIFLASH_Write_Enable(void);
void SPIFLASH_Write_Disable(void);
void SPIFLASH_Read_Identification(unsigned char*);
void SPIFLASH_Read_Status_Register(unsigned char*);
void SPIFLASH_Write_Status_Register(unsigned char);
void SPIFLASH_Read_Data_Bytes(unsigned int, unsigned char*, unsigned int);
void SPIFLASH_Fast_Read_Data_Bytes(unsigned int, unsigned char*, unsigned int);
void SPIFLASH_Page_Program(unsigned int, unsigned char*, unsigned int);
void SPIFLASH_Sector_Erase(unsigned int);
void SPIFLASH_Bulk_Erase(void);
bool SPIFLASH_Write_In_Progress(void);

#endif /* SPIFLASH_H_ */
