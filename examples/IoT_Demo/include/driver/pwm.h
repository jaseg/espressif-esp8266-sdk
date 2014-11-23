#ifndef __PWM_H__
#define __PWM_H__

#define PWM_CHANNEL 3

struct pwm_single_param {
	uint16_t gpio_set;
	uint16_t gpio_clear;
    uint16_t h_time;
};

struct pwm_param {
    uint16_t period;
    uint16_t freq;
    uint8_t  duty[PWM_CHANNEL];
};

#define PWM_DEPTH 255

#define PWM_1S 1000000

#define PWM_0_OUT_IO_MUX PERIPHS_IO_MUX_MTDI_U
#define PWM_0_OUT_IO_NUM 12
#define PWM_0_OUT_IO_FUNC  FUNC_GPIO12

#define PWM_1_OUT_IO_MUX PERIPHS_IO_MUX_MTDO_U
#define PWM_1_OUT_IO_NUM 15
#define PWM_1_OUT_IO_FUNC  FUNC_GPIO15

#define PWM_2_OUT_IO_MUX PERIPHS_IO_MUX_MTCK_U
#define PWM_2_OUT_IO_NUM 13
#define PWM_2_OUT_IO_FUNC  FUNC_GPIO13

void pwm_init(uint16_t freq, uint8_t *duty);
void pwm_start(void);

void pwm_set_duty(uint8_t duty, uint8_t channel);
uint8_t pwm_get_duty(uint8_t channel);
void pwm_set_freq(uint16_t freq);
uint16_t pwm_get_freq(void);
#endif

