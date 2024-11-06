#include <myFunctions.h>
#include "main.h"
#include "spi.h"
#include "gpio.h"

#include "stdio.h"
#include "string.h"
void blink(int times, int interval){
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
	while(times>0){
		times--;
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(interval);
	}
}
