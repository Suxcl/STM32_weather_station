/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LoRa.h"
#include "stdio.h"
#include "string.h"
#include "myFunctions.h"
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

// THIS IS STATION

// vats
uint8_t count = 0;


// LoRa vars
LoRa myLoRa;
uint16_t LoRa_stat;
uint8_t received_data[10];
uint8_t packet_size = -1;


uint8_t read_data[128];
uint8_t read_data2[2];
uint8_t send_data[128];
int			RSSI;

// ESP-01S - wifi module


uint8_t f1;
uint32_t f2;
size_t ATSize = 150;
char ATcommand[150];
char toPost[512];
size_t rxSize = 512;
uint8_t rxBuffer[512] = {0};
uint8_t ATisOK;

HAL_StatusTypeDef status;

char str[] = "Hello!";


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  /* USER CODE BEGIN 2 */

  // --- LoRa setup ---

  myLoRa = newLoRa();

  myLoRa.CS_port         = NSS_GPIO_Port;
  myLoRa.CS_pin          = NSS_Pin;
  myLoRa.reset_port      = RST_GPIO_Port;
  myLoRa.reset_pin       = RST_Pin;
  myLoRa.DIO0_port       = DIO0_GPIO_Port;
  myLoRa.DIO0_pin        = DIO0_Pin;
  myLoRa.hSPIx           = &hspi1;

  myLoRa.frequency             = 433;							  // default = 433 MHz
  myLoRa.spredingFactor        = SF_7;							// default = SF_7
  myLoRa.bandWidth			   = BW_125KHz;				  // default = BW_125KHz
  myLoRa.crcRate			   = CR_4_5;						// default = CR_4_5
  myLoRa.power			       = POWER_20db;				// default = 20db
  myLoRa.overCurrentProtection = 100; 							// default = 100 mA
  myLoRa.preamble		       = 8;		  					// default = 8;

  LoRa_reset(&myLoRa);

  if(LoRa_init(&myLoRa)==LORA_OK){
	LoRa_stat = 1;
	blink(3, 500);

  };

  //LoRa_startReceiving(&myLoRa);

  // --- end of LoRa setup ---

  // ESP setup start


  void clearBuffers(){
	  memset(rxBuffer,0,sizeof(rxBuffer));
	  memset(ATcommand,0,sizeof(ATcommand));
	  memset(toPost,0,sizeof(toPost));
  }


//  clearBuffers();
//  sprintf(ATcommand,"AT+RST\r\n");
//  memset(rxBuffer,0,sizeof(rxBuffer));
//  HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
//  status = HAL_UART_Receive(&huart1, rxBuffer, 10, 100);
//  HAL_Delay(500);

  ATisOK = 0;
  while(!ATisOK){
	  sprintf(ATcommand,"AT+CWMODE_CUR=1\r\n");
	  memset(rxBuffer,0,sizeof(rxBuffer));
	  HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	  status = HAL_UART_Receive(&huart1, rxBuffer, 150, 1000);
	  if(strstr((char *)rxBuffer,"OK")){
		ATisOK = 1;
	  }

  }





  ATisOK = 0;
  while(!ATisOK){
	  sprintf(ATcommand,"AT+CWJAP_CUR=\"Netis Hifi Point\",\"kajaK123\"\r\n");
	  memset(rxBuffer,0,sizeof(rxBuffer));
	  HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	  HAL_UART_Receive(&huart1, rxBuffer, 150, 20000);
	  if(strstr((char *)rxBuffer,"CONNECTED")){
		  ATisOK = 1;
	  }

  }
  ATisOK = 0;
  while(!ATisOK){
	  sprintf(ATcommand,"AT+CIPMUX=0\r\n");
	  memset(rxBuffer,0,sizeof(rxBuffer));
	  HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	  HAL_UART_Receive(&huart1, rxBuffer, 50, 1000);
	  if(strstr((char *)rxBuffer,"OK")){
		ATisOK = 1;
	  }

  }
  ATisOK = 0;
  while(!ATisOK){
  	  sprintf(ATcommand,"AT+CIFSR\r\n");
  	  memset(rxBuffer,0,sizeof(rxBuffer));
  	  HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
  	  HAL_UART_Receive(&huart1, rxBuffer, 50, 2000);
  	  if(strstr((char *)rxBuffer,"STAIP")){
  		ATisOK = 1;
  	  }

  }

  clearBuffers();
  ATisOK = 0;
  while(!ATisOK){
	  sprintf(ATcommand,"AT+CIPSTART=\"TCP\",\"192.168.1.2\",8000\r\n");
	  HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	  HAL_UART_Receive (&huart1, rxBuffer, 512, 1000);
	  if(strstr((char *)rxBuffer,"CONNECT")){
		  ATisOK = 1;
	  }

  }

  clearBuffers();
  if (!strstr((char *)rxBuffer, "STATUS:3")) {
	  sprintf(ATcommand,"AT+CIPSTATUS\r\n");
	  HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	  HAL_UART_Receive (&huart1, rxBuffer, 512, 1000);

  }

  // ESP setup end


//  LoRa_transmit(&myLoRa, TxData, 3, 500);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // LoRA

	  //packet_size = LoRa_receive(&myLoRa, received_data, 10);
	  //packet_size = LoRa_receive(&myLoRa, read_data, 128);
	  //HAL_Delay(500);


	  // ESP-01s


  clearBuffers();
	ATisOK = 0;
	while(!ATisOK){
	  sprintf(ATcommand,"AT+CIPSTART=\"TCP\",\"192.168.1.2\",8000\r\n");
	  HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	  HAL_UART_Receive (&huart1, rxBuffer, 512, 1000);
	  if(strstr((char *)rxBuffer,"CONNECT")){
		  ATisOK = 1;
	  }
	  HAL_Delay(200);
	}

//	clearBuffers();
//
//
//	sprintf(toPost, "GET /api/get-response/ HTTP/1.1\r\n"
//					"Host: 192.168.1.2:8000\r\n\r\n");
//	sprintf(ATcommand,"AT+CIPSEND=%d\r\n", strlen(toPost));
//	HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),3000);
//	status = HAL_UART_Receive(&huart1, rxBuffer, 512, 5000);
//	HAL_Delay(500);
//	if(strstr((char *)rxBuffer,">")){
//		memset(rxBuffer,0,sizeof(rxBuffer));
//		HAL_UART_Transmit(&huart1,(uint8_t *)toPost,strlen(toPost),1000);
//		status = HAL_UART_Receive(&huart1, rxBuffer, 512, 5000);
//		HAL_Delay(500);
//		}



	clearBuffers();
	// POST request
	sprintf(toPost,    	"POST /api/post-response/ HTTP/1.1\r\n"
						"Host: 192.168.1.2:8000\r\n"
						"Content-Type: text/plain\r\n"
						"Content-Length: 7\r\n\r\n"
						"1,2,3,4\r\n\r\n");


	sprintf(ATcommand,"AT+CIPSEND=%d\r\n", strlen(toPost));
	HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),3000);
	status = HAL_UART_Receive(&huart1, rxBuffer, 512, 5000);
	HAL_Delay(200);
	if(strstr((char *)rxBuffer,">")){
		memset(rxBuffer,0,sizeof(rxBuffer));
		HAL_UART_Transmit(&huart1,(uint8_t *)toPost,strlen(toPost),1000);
		status = HAL_UART_Receive(&huart1, rxBuffer, 512, 5000);
		HAL_Delay(200);
	}

	sprintf(ATcommand,"AT+CIPCLOSE\r\n");
	HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),3000);
	status = HAL_UART_Receive(&huart1, rxBuffer, 512, 5000);
	HAL_Delay(500);

	HAL_Delay(2000);



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

// EXTI interruption function
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin==DIO0_Pin){

	  LoRa_receive(&myLoRa,read_data, 2);
	  blink(5, 100);
  }

}
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
