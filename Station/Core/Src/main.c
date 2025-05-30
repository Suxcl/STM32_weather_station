/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main station of weather station setup
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SX1278.h"
#include "esp01s.h"
#include "readings.h"

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

/*
 * ===== Flags =====
 */

// If flag is set run code involving LoRa
#define LORA
// If flag is set run code involving ESP
#define ESP


/*
 * ===== Sensors variables =====
 */


// float values read from sensors
float temperature = 0;
float humidity = 0;
float pressure = 0;

// Reading converted to int before sending
int sensor_id = -1;
int temperature_int = 0;
int humidity_int = 0;
int pressure_int = 0;

// Reading
Readings readings;

/*
 * 	===== LoRa variables =====
 */
SX1278_hw_t SX1278_hw;
SX1278_t SX1278;
int receiver;
int ret;

char LoRaTxBuffer[512];
char LoRaRxBuffer[512];

int tx_len;

int control_val=0;

/*
 *	====== ESP variables =======
 */
Esp01s Esp;
HAL_StatusTypeDef status;
/*
 * ====== Timeout variables =======
 	 	 1000 == 1 sec

	Sensor_1 - Main station
	Sensor_2 - First Sensor
	Sensor_3 - Second Sensor

	get_check = How often station checks from changes in delays from browser

*/
uint32_t current_time;				// current tick

uint32_t get_timeout = 60 * 1000;
uint32_t get_wake_up = 60 * 1000;

uint32_t sensor_1_timeout = 20 * 1000;
//uint32_t sensor_2_timeout = 20 * 1000;
uint32_t sensor_2_timeout = 10 * 1000;
uint32_t sensor_3_timeout = 20 * 1000;

uint32_t sensor_1_wakeup = 20 * 1000;
//uint32_t sensor_2_wakeup = 20 * 1000;
uint32_t sensor_2_wakeup = 10 * 1000;
uint32_t sensor_3_wakeup = 20 * 1000;

uint32_t inside_counter;

int reverse_time = 3;
int test_val = 0;
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
	SX1278_LoRaEntryRx(&SX1278, 16, 2000);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	receiver = 1;
	HAL_Delay(500);
}

// Puts LoRa module in transmit mode
// -- LED should be on --
void LoRaSetTxMode(){
	clearTx();
	SX1278_LoRaEntryTx(&SX1278, 16, 2000);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	receiver = 0;
	HAL_Delay(500);
}

void retrieve_onboard_sensor_data(){
	AHT20_Read(&temperature, &humidity);
	pressure = BMP280_ReadPressure(&hi2c1);

	sensor_id = 1;
	temperature_int = (int)temperature;
	humidity_int = (int)humidity;
	pressure_int = (int)pressure;

	#ifdef ESP
	  send_single_sensor_post_req(&Esp, temperature_int, humidity_int, pressure_int, sensor_id);
	#endif
	addData(&readings, sensor_id, temperature_int, humidity_int, pressure_int);
}


void retrieve_sensor_data(int sensorID){
	// Send info to sensor
	tx_len = sprintf(LoRaTxBuffer, "%d,%d",sensorID, reverse_time);
	ret = SX1278_LoRaEntryTx(&SX1278, tx_len, 2000);
	ret = SX1278_LoRaTxPacket(&SX1278, (uint8_t*) LoRaTxBuffer,
							tx_len, 2000);
	// Set RX mode, Read sensor data
	HAL_Delay(500);
	LoRaSetRxMode();

	// Calculate how long keep listening for anwser
	current_time = HAL_GetTick();
	inside_counter = current_time+(reverse_time*1000);

	// Listen for response
	while(HAL_GetTick()<=inside_counter){
		ret = SX1278_LoRaRxPacket(&SX1278);		// return gives length of received data
		if(ret > 0){
			SX1278_read(&SX1278, (uint8_t*) LoRaRxBuffer, ret); // read data send

			int s_id;
			sscanf(LoRaRxBuffer, "%d,%d,%d,%d", &s_id, &temperature_int, &humidity_int, &pressure_int);

			if(s_id == sensorID){
				sensor_id = s_id;
				addData(&readings, sensorID, temperature_int, humidity_int, pressure_int);
				#ifdef ESP
					send_single_sensor_post_req(&Esp, temperature_int, humidity_int, pressure_int, sensorID);
				#endif
			}
		}
		break;
	}
	LoRaSetTxMode();

}



/*  ----------------------------------
 * 		Esp Functions
 *  ----------------------------------
 */
void Esp_send_get(){
	send_get_req(&Esp);
	sensor_1_timeout = Esp.timers[0];
	sensor_2_timeout = Esp.timers[1];
	sensor_3_timeout = Esp.timers[2];
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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  /*	=====================
   * 		  	LoRa Init
   * 	=====================
   */
  #ifdef LORA
	  receiver = 1;

      SX1278_hw.dio0.port = DIO0_GPIO_Port;
	  SX1278_hw.dio0.pin = DIO0_Pin;
	  SX1278_hw.nss.port = NSS_GPIO_Port;
	  SX1278_hw.nss.pin = NSS_Pin;
	  SX1278_hw.reset.port = RST_GPIO_Port;
	  SX1278_hw.reset.pin = RST_Pin;
	  SX1278_hw.spi = &hspi1;

	  SX1278.hw = &SX1278_hw;

	  SX1278_init(&SX1278, 434000000, SX1278_POWER_17DBM, SX1278_LORA_SF_7,
			  SX1278_LORA_BW_125KHZ, SX1278_LORA_CR_4_5, SX1278_LORA_CRC_EN, 10);

	  LoRaSetTxMode();						// Set mode to TX

  #endif
  /*	=====================
	 * 		  Sensors Init
	 * 	=====================
	 */
  // SAK
  BMP280_Init(&hi2c1);

  // Storing readings
  readings = newR();

  /*	=====================
   * 		  	ESP Init
   * 	=====================
   */

  #ifdef ESP
	  Esp = newEsp01s(&huart1);				// Define new ESP typedef
	  esp_setup(&Esp);						// Run setup for ESP
	  start_connection(&Esp);				// Start connection with data sever
	  Esp_send_get();
  #endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Read current time in milliseconds
	  current_time = HAL_GetTick();
	  // Sensor 1 wake up
	  if((sensor_1_wakeup<=current_time)){
		  sensor_1_wakeup = sensor_1_timeout + HAL_GetTick();
		  retrieve_onboard_sensor_data();
	  }

	  // Sensor 2 wake up
	  if((sensor_2_wakeup<=current_time)){
		  // Set wake up value for another data request
		  sensor_2_wakeup = sensor_2_timeout + HAL_GetTick();
		  retrieve_sensor_data(2);
	  }

	  // Sensor 3 wake up
	  if((sensor_3_wakeup<=current_time)){
		  // Set wake up value for another data request
		  sensor_3_wakeup = sensor_3_timeout + HAL_GetTick();
		  retrieve_sensor_data(3);
	  }

	  // In set timeout check for change in reading timeouts
	  #ifdef ESP
		  if((get_wake_up<=current_time)){
			  get_wake_up = get_timeout+HAL_GetTick();
			  Esp_send_get();
		  }
	  #endif



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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



//
// WHOLE SENSOR 2 FUNCTION
//

// Sensor 2 wake up
//	  if((sensor_2_wakeup<=current_time)){
//
//		  // Set wake up value for another data request
//		  sensor_2_wakeup = sensor_2_timeout + HAL_GetTick();
//
//		  retrieve_sensor_data(2);
////		  // Send info to sensor_1
////		  tx_len = sprintf(LoRaTxBuffer, "%d,%d",2, reverse_time);
////		  ret = SX1278_LoRaEntryTx(&SX1278, tx_len, 2000);
////		  ret = SX1278_LoRaTxPacket(&SX1278, (uint8_t*) LoRaTxBuffer,
////												tx_len, 2000);
////		  // Set RX mode, Read sensor data
////		  HAL_Delay(500);
////		  LoRaSetRxMode();
////
////		  // For 5 sec wait for value from sensor
////		  current_time = HAL_GetTick();
////		  inside_counter = current_time+(reverse_time*1000);
////
////
////		  test_val = 0;
////		  while(HAL_GetTick()<=inside_counter){
////			  test_val++;
////			  ret = SX1278_LoRaRxPacket(&SX1278);		// return gives length of received data
////			  if(ret > 0){
////				  SX1278_read(&SX1278, (uint8_t*) LoRaRxBuffer, ret); // read data send
//////				  char s_id[3];
//////				  int temp, humi, pres;
//////				  sscanf(LoRaRxBuffer, "%[^,],%d,%d,%d",
//////						  sensor_id, &temperature_int, &humidity_int, &pressure_int);
////
////				  sscanf(LoRaRxBuffer, "%d,%d,%d,%d",
////				  						  &sensor_id,
////										  &temperature_int,
////										  &humidity_int,
////										  &pressure_int);
////
//////				  sensor_id = 2;
//////				  temperature_int = temp;
//////				  humidity_int = humi;
//////				  pressure_int = pres;
////
////				  #ifdef ESP
////				  	  send_single_sensor_post_req(&Esp, temperature_int, humidity_int, pressure_int, 1);
////		  	      #endif
////
////				  control_val++;
////				  break;
////			  }
//////			  HAL_Delay(100);
////		  }
////		  LoRaSetTxMode();
////		  test_val = 0;
//
//	  }
//
