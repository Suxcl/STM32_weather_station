/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Sensor 2
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SX1278.h"
#include "BMP280_sak.h"
#include "AHT20.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// ===== Sensors =====
float temperature = 0;
float humidity = 0;
float pressure = 0;

int sensor_id = 3;
int temperature_int = 0;
int humidity_int = 0;
int pressure_int = 0;

// ===== LoRa =====
SX1278_hw_t SX1278_hw;
SX1278_t SX1278;
int receiver;
int ret;

char LoRaTxBuffer[512];
char LoRaRxBuffer[512];

char buffer[512];

int tx_message;
int message_length;
int tx_len;
int rec_sensor_id = -1;

int control_val=0;

int reverse_time;

/* ====== Timeouts =======
 	 	 1000 == 1 sec

	Sensor_1 - Main station
	Sensor_2 - First Sensor
	Sensor_3 - Second Sensor

*/
uint32_t current_time;
uint32_t inside_counter;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*  ----------------------------------
 * 		LoRa Functions
 *  ----------------------------------
 */
void clearTx(){
	memset(LoRaTxBuffer,0,sizeof(LoRaTxBuffer));
}

void clearRx(){
	memset(LoRaRxBuffer,0,sizeof(LoRaRxBuffer));
}
// Puts LoRa module in receive mode
// -- LED should be off --
void LoRaSetRxMode(){
	clearRx();
	ret = SX1278_LoRaEntryRx(&SX1278, 16, 1000);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	receiver = 1;
	HAL_Delay(500);
}

// Puts LoRa module in transmit mode
// -- LED should be on --
void LoRaSetTxMode(){
	clearTx();
	ret = SX1278_LoRaEntryTx(&SX1278, 16, 1000);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	receiver = 0;
	HAL_Delay(500);
}


//while (GPIO_PIN_RESET == HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin));
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  // ===== BMP sensor setup =====
  // SAK
  BMP280_Init(&hi2c1);
// ===== LoRa setup =====
  receiver = 0;

  control_val++;
  	//initialize LoRa module
  SX1278_hw.dio0.port = DIO0_GPIO_Port;
  SX1278_hw.dio0.pin = DIO0_Pin;
  SX1278_hw.nss.port = NSS_GPIO_Port;
  SX1278_hw.nss.pin = NSS_Pin;
  SX1278_hw.reset.port = RST_GPIO_Port;
  SX1278_hw.reset.pin = RST_Pin;
  SX1278_hw.spi = &hspi1;

  SX1278.hw = &SX1278_hw;

  control_val++;
  SX1278_init(&SX1278, 434000000, SX1278_POWER_17DBM, SX1278_LORA_SF_7,
		  SX1278_LORA_BW_125KHZ, SX1278_LORA_CR_4_5, SX1278_LORA_CRC_EN, 10);
  control_val++;


  LoRaSetRxMode();
  control_val++;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  control_val = 0;
	  ret = SX1278_LoRaRxPacket(&SX1278);
	  if (ret > 0) {
		  // Setup Timers
		  current_time = HAL_GetTick();

		  // Read buffer
		  SX1278_read(&SX1278, (uint8_t*) LoRaRxBuffer, ret);
//		  sscanf(LoRaRxBuffer, "%[^,],%d",s_id, &reverse_time);
		  sscanf(LoRaRxBuffer, "%d,%d",&rec_sensor_id, &reverse_time);

//		  if(strstr((char *)LoRaRxBuffer,"S2")){	// 	If good sensor_id
		  if(rec_sensor_id == sensor_id){

			  LoRaSetTxMode();							// 	Go into TX mode

			  AHT20_Read(&temperature, &humidity); 				// Read sensor data
			  pressure = BMP280_ReadPressure(&hi2c1);
			  temperature_int = (int)temperature;
			  humidity_int = (int)humidity;
			  pressure_int = (int)pressure;

			  current_time = HAL_GetTick();
			  inside_counter = current_time + (reverse_time * 1000);

			  tx_len = sprintf(LoRaTxBuffer, "%d,%d,%d,%d",sensor_id, temperature_int, humidity_int, pressure_int);

			  while(HAL_GetTick()<=inside_counter){
				  ret = SX1278_LoRaEntryTx(&SX1278, tx_len, 2000);
				  ret = SX1278_LoRaTxPacket(&SX1278, (uint8_t*) LoRaTxBuffer,tx_len, 2000);
			  }

			  // Switch to RX mode
			  LoRaSetRxMode();
			  rec_sensor_id = -1;
		  }
	  }
	  control_val = -1;

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
