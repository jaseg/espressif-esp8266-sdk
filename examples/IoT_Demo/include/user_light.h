#ifndef __USER_LIGHT_H__
#define __USER_LIGHT_H__

#include "driver/pwm.h"

/* NOTICE---this is for 512KB spi flash.
 * you can change to other sector if you use other size spi flash. */
#define PRIV_PARAM_START_SEC		0x3C

#define PRIV_PARAM_SAVE     0

#define LIGHT_RED       0
#define LIGHT_GREEN     1
#define LIGHT_BLUE      2
#define LIGHE_LEVEL     3

struct light_saved_param {
    uint16_t pwm_freq;
    uint8_t  pwm_duty[PWM_CHANNEL];
    uint8_t  pad[6-PWM_CHANNEL];
};

void user_light_init(void);
uint8_t user_light_get_duty(uint8_t channel);
void user_light_set_duty(uint8_t duty, uint8_t channel);
uint16_t user_light_get_freq(void);
void user_light_set_freq(uint16_t freq);

#endif

