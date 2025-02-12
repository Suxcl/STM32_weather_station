//#define BMP280_I2C_ADDR         0x76
#define BMP280_I2C_ADDR         0x77
// or 0x77 depending on SDO pin state

/* BMP280 registers */
#define BMP280_CHIP_ID_REG      0xD0
#define BMP280_RESET_REG        0xE0
#define BMP280_STATUS_REG       0xF3
#define BMP280_CTRL_MEAS_REG    0xF4
#define BMP280_CONFIG_REG       0xF5
#define BMP280_PRESS_MSB_REG    0xF7

/* Compensation registers */
#define BMP280_DIG_T1_LSB_REG   0x88
#define BMP280_DIG_T1_MSB_REG   0x89
#define BMP280_DIG_P1_LSB_REG   0x8E
#define BMP280_DIG_P1_MSB_REG   0x8F


HAL_StatusTypeDef BMP280_Init(I2C_HandleTypeDef *hi2c);
float BMP280_ReadPressure(I2C_HandleTypeDef *hi2c);
