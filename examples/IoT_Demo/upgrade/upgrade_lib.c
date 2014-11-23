#include "ets_sys.h"
#include "spi_flash.h"

//#include "net80211/ieee80211_var.h"
#include "lwip/mem.h"

#include "upgrade.h"

struct upgrade_param {
    uint32_t fw_bin_addr;

    uint8_t fw_bin_sec;
    uint8_t fw_bin_sec_num;

    uint8_t fw_bin_sec_earse;

    uint8_t extra;

    uint8_t save[4];

    uint8_t *buffer;
};

static struct upgrade_param *upgrade;

extern SpiFlashChip *flashchip;

/******************************************************************************
 * FunctionName : system_upgrade_internal
 * Description  : a
 * Parameters   :
 * Returns      :
*******************************************************************************/
static uint8_t ICACHE_FLASH_ATTR
system_upgrade_internal(struct upgrade_param *upgrade, uint8_t *data, uint16 len)
{
    uint8_t ret = 0;
    if(data == NULL || len == 0)
    {
    	return 1;
    }
    upgrade->buffer = (uint8_t *)os_zalloc(len + upgrade->extra);

    os_memcpy(upgrade->buffer, upgrade->save, upgrade->extra);
    os_memcpy(upgrade->buffer + upgrade->extra, data, len);

    len += upgrade->extra;
    upgrade->extra = len & 0x03;
    len -= upgrade->extra;

    os_memcpy(upgrade->save, upgrade->buffer + len, upgrade->extra);

    do {
        if (upgrade->fw_bin_addr + len >= (upgrade->fw_bin_sec + upgrade->fw_bin_sec_num) * SPI_FLASH_SEC_SIZE) {
            break;
        }

        if (len > SPI_FLASH_SEC_SIZE) {

        } else {
//			os_printf("%x %x\n",upgrade->fw_bin_sec_earse,upgrade->fw_bin_addr);
            /* earse sector, just earse when first enter this zone */
            if (upgrade->fw_bin_sec_earse != (upgrade->fw_bin_addr + len) >> 12) {
                upgrade->fw_bin_sec_earse = (upgrade->fw_bin_addr + len) >> 12;
                spi_flash_erase_sector(upgrade->fw_bin_sec_earse);
//				os_printf("%x\n",upgrade->fw_bin_sec_earse);
            }
        }

        if (spi_flash_write(upgrade->fw_bin_addr, (uint32_t *)upgrade->buffer, len) != SPI_FLASH_RESULT_OK) {
            break;
        }

        ret = 1;
        upgrade->fw_bin_addr += len;
    } while (0);

    os_free(upgrade->buffer);
    upgrade->buffer = NULL;
    return ret;
}

/******************************************************************************
 * FunctionName : system_upgrade
 * Description  : a
 * Parameters   :
 * Returns      :
*******************************************************************************/
static uint8_t ICACHE_FLASH_ATTR
system_upgrade(uint8_t *data, uint16 len)
{
    uint8_t ret;

    ret = system_upgrade_internal(upgrade, data, len);

    return ret;
}

/******************************************************************************
 * FunctionName : system_upgrade_init
 * Description  : a
 * Parameters   :
 * Returns      :
*******************************************************************************/
static void ICACHE_FLASH_ATTR
system_upgrade_init(void)
{
    uint32_t user_bin2_start;
    uint8_t flash_buf[4];
    uint8_t high_half;

    spi_flash_read(0, (uint32_t *)flash_buf, 4);
    high_half = (flash_buf[3] & 0xF0) >> 4;

    if (upgrade == NULL) {
        upgrade = (struct upgrade_param *)os_zalloc(sizeof(struct upgrade_param));
    }

    system_upgrade_flag_set(UPGRADE_FLAG_IDLE);

    if (high_half == 2 || high_half == 3 || high_half == 4) {
		user_bin2_start = 129;	// 128 + 1
		upgrade->fw_bin_sec_num = 123;	// 128 - 1 - 4
	} else {
		user_bin2_start = 65;	// 64 + 1
		upgrade->fw_bin_sec_num = 59;	// 64 - 1 - 4
	}

    upgrade->fw_bin_sec = (system_upgrade_userbin_check() == USER_BIN1) ? user_bin2_start : 1;

    upgrade->fw_bin_addr = upgrade->fw_bin_sec * SPI_FLASH_SEC_SIZE;
}

/******************************************************************************
 * FunctionName : system_upgrade_deinit
 * Description  : a
 * Parameters   :
 * Returns      :
*******************************************************************************/
static void ICACHE_FLASH_ATTR
system_upgrade_deinit(void)
{
    os_free(upgrade);
    upgrade = NULL;
}
