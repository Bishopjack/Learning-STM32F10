#ifndef _CONFIG_H
#define _CONFIG_H

extern void (*blink_led)(void);

extern void CLK_Config(void);

extern void GPIO_Config(void);

extern void EXTI_Config(void);

extern void TIM_BaseConfig(void);

extern void KEY_Checking(void);

#endif