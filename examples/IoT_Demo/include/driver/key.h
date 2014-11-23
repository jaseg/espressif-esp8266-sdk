#ifndef __KEY_H__
#define __KEY_H__

#include "gpio.h"

typedef void (* key_function)(void);

struct single_key_param {
    uint8_t key_level;
    uint8_t gpio_id;
    uint8_t gpio_func;
    uint32_t gpio_name;
    os_timer_t key_5s;
    os_timer_t key_50ms;
    key_function short_press;
    key_function long_press;
};

struct keys_param {
    uint8_t key_num;
    struct single_key_param **single_key;
};

struct single_key_param *key_init_single(uint8_t gpio_id, uint32_t gpio_name, uint8_t gpio_func, key_function long_press, key_function short_press);
void key_init(struct keys_param *key);

#endif
