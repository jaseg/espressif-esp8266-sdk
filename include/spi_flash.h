/* 
 * copyright (c) Espressif System 2010
 * 
 */

#ifndef SPI_FLASH_H
#define SPI_FLASH_H

typedef enum {
    SPI_FLASH_RESULT_OK,
    SPI_FLASH_RESULT_ERR,
    SPI_FLASH_RESULT_TIMEOUT
} SpiFlashOpResult;

typedef struct{
	uint32_t	deviceId;
	uint32_t	chip_size;    // chip size in byte
	uint32_t	block_size;
	uint32_t  sector_size;
	uint32_t  page_size;
	uint32_t  status_mask;
} SpiFlashChip;

#define SPI_FLASH_SEC_SIZE      4096

SpiFlashOpResult spi_flash_erase_sector(uint16_t sec);
SpiFlashOpResult spi_flash_write(uint32_t des_addr, uint32_t *src_addr, uint32_t size);
SpiFlashOpResult spi_flash_read(uint32_t src_addr, uint32_t *des_addr, uint32_t size);

#endif
