#include "AHT20.h"
#include "i2c.h"

void AHT20_Read(float* Temp, float* Humid)
{
	uint8_t dum[6];
	HAL_I2C_Mem_Read(&HI2C, AHT_ADDR, 0x71, 1, dum, 1, 100);

	if(!(dum[0]&(1<<3)))
	{
		dum[0] = 0xBE, dum[1] = 0x08, dum[2] = 0x00;
		HAL_I2C_Master_Transmit(&HI2C, AHT_ADDR, dum, 3, 100);
		HAL_Delay(10);
	}

	dum[0] = 0xAC, dum[1] = 0x33, dum[2] = 0x00;
	HAL_I2C_Master_Transmit(&HI2C, AHT_ADDR, dum, 3, 100);
	HAL_Delay(80);

	do {
		HAL_I2C_Mem_Read(&HI2C, AHT_ADDR, 0x71, 1, dum, 1, 100);
		HAL_Delay(1);
	} while(dum[0]&(1<<7));

	HAL_I2C_Master_Receive(&HI2C, AHT_ADDR, dum, 6, 100);
	uint32_t h20 = (dum[1])<<12 | (dum[2])<<4 | (dum[3]>>4);
	uint32_t t20 = (dum[3]&0x0F)<<16 | (dum[4])<<8 | dum[5];
	*Temp = (t20 / 1048576.0)*200.0 - 50.0;
	*Humid = h20 / 10485.76;
}
