#ifndef __UPGRADE_H__
#define __UPGRADE_H__

#define SPI_FLASH_SEC_SIZE      4096

#define USER_BIN1               0x00
#define USER_BIN2               0x01

#define UPGRADE_FLAG_IDLE       0x00
#define UPGRADE_FLAG_START      0x01
#define UPGRADE_FLAG_FINISH     0x02

typedef void (*upgrade_states_check_callback)(void * arg);

//#define UPGRADE_SSL_ENABLE

struct upgrade_server_info {
    uint8_t ip[4];
    uint16 port;

    uint8_t upgrade_flag;

    uint8_t pre_version[8];
    uint8_t upgrade_version[8];

    uint32_t check_times;
    uint8_t *url;

    upgrade_states_check_callback check_cb;
    struct espconn *pespconn;
};

#ifdef UPGRADE_SSL_ENABLE
uint8_t system_upgrade_start_ssl(struct upgrade_server_info *server);
#else
uint8_t system_upgrade_start(struct upgrade_server_info *server);
#endif
#endif
