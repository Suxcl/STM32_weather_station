
#include "i2c.h"
#include "BMP280_sak.h"

/* Calibration data */
typedef struct {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
} BMP280_CalibData;

BMP280_CalibData calibData;
int32_t t_fine;


/* Function to initialize BMP280 */
HAL_StatusTypeDef BMP280_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t chip_id = 0;
    uint8_t data[24];

    /* Check chip ID */
    HAL_I2C_Mem_Read(hi2c, BMP280_I2C_ADDR << 1, BMP280_CHIP_ID_REG, 1, &chip_id, 1, 1000);
    if (chip_id != 0x58) // BMP280 chip id
        return HAL_ERROR;

    /* Reset the sensor */
    uint8_t reset_cmd = 0xB6;
    HAL_I2C_Mem_Write(hi2c, BMP280_I2C_ADDR << 1, BMP280_RESET_REG, 1, &reset_cmd, 1, 1000);
    HAL_Delay(10);

    /* Read calibration data */
    HAL_I2C_Mem_Read(hi2c, BMP280_I2C_ADDR << 1, BMP280_DIG_T1_LSB_REG, 1, data, 24, 1000);

    calibData.dig_T1 = (data[1] << 8) | data[0];
    calibData.dig_T2 = (data[3] << 8) | data[2];
    calibData.dig_T3 = (data[5] << 8) | data[4];
    calibData.dig_P1 = (data[7] << 8) | data[6];
    calibData.dig_P2 = (data[9] << 8) | data[8];
    calibData.dig_P3 = (data[11] << 8) | data[10];
    calibData.dig_P4 = (data[13] << 8) | data[12];
    calibData.dig_P5 = (data[15] << 8) | data[14];
    calibData.dig_P6 = (data[17] << 8) | data[16];
    calibData.dig_P7 = (data[19] << 8) | data[18];
    calibData.dig_P8 = (data[21] << 8) | data[20];
    calibData.dig_P9 = (data[23] << 8) | data[22];

    /* Configure the sensor */
    uint8_t config = 0;
    config = (0x04 << 5) | (0x04 << 2) | 0x00; // Standby time 500ms, Filter coefficient 16, SPI disabled
    HAL_I2C_Mem_Write(hi2c, BMP280_I2C_ADDR << 1, BMP280_CONFIG_REG, 1, &config, 1, 1000);

    /* Set measurement settings */
    uint8_t ctrl_meas = 0;
    ctrl_meas = (0x02 << 5) | (0x05 << 2) | 0x03; // Oversampling x4 for temp, x16 for pressure, normal mode
    HAL_I2C_Mem_Write(hi2c, BMP280_I2C_ADDR << 1, BMP280_CTRL_MEAS_REG, 1, &ctrl_meas, 1, 1000);

    return HAL_OK;
}

/* Function to read pressure */
float BMP280_ReadPressure(I2C_HandleTypeDef *hi2c)
{
    uint8_t data[6];
    int32_t adc_P, adc_T;
    int32_t var1, var2;
    uint32_t pressure;

    /* Read pressure and temperature data */
    HAL_I2C_Mem_Read(hi2c, BMP280_I2C_ADDR << 1, BMP280_PRESS_MSB_REG, 1, data, 6, 1000);

    adc_P = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4);
    adc_T = ((uint32_t)data[3] << 12) | ((uint32_t)data[4] << 4) | ((uint32_t)data[5] >> 4);

    /* Calculate temperature (just to get t_fine value) */
    var1 = ((((adc_T >> 3) - ((int32_t)calibData.dig_T1 << 1))) * ((int32_t)calibData.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)calibData.dig_T1)) * ((adc_T >> 4) - ((int32_t)calibData.dig_T1))) >> 12) * ((int32_t)calibData.dig_T3)) >> 14;
    t_fine = var1 + var2;

    /* Calculate pressure */
    var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)calibData.dig_P6);
    var2 = var2 + ((var1 * ((int32_t)calibData.dig_P5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)calibData.dig_P4) << 16);
    var1 = (((calibData.dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)calibData.dig_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t)calibData.dig_P1)) >> 15);

    if (var1 == 0)
        return 0;

    pressure = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
    if (pressure < 0x80000000)
        pressure = (pressure << 1) / ((uint32_t)var1);
    else
        pressure = (pressure / (uint32_t)var1) * 2;

    var1 = (((int32_t)calibData.dig_P9) * ((int32_t)(((pressure >> 3) * (pressure >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(pressure >> 2)) * ((int32_t)calibData.dig_P8)) >> 13;
    pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + calibData.dig_P7) >> 4));

    return pressure / 100.0f; // Return pressure in hPa
}
