#include"config.h"

int main(void)
{
	CLK_Config();
	GPIO_Config();
	EXTI_Config();
	TIM_BaseConfig();
	while(1)
	{
		if(blink_led!=(void*)0)
			blink_led();
	}
	return 0;
}