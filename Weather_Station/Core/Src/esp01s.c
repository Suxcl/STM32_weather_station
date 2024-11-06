
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "spi.h"
#include "gpio.h"
//HAL_UART_Transmit(&uart, (uint8_t*)s, strlen(s), 1000);
//
//interfejs, adres, wielkosc_bufora_danych, timeout


char rst_cmd[] = "AT+RST\r\n";
char check_cmd[] = "AT\r\n";
char mode_station_cmd[] = "AT+CWMODE_CUR=1\r\n";
char mode_softap_cmd[] = "AT+CWMODE_CUR=2\r\n";
char mode_both_cmd[] = "AT+CWMODE_CUR=3\r\n";
char check_version_cmd[] = "AT+GMR\r\n";
char check_wifi_connection_cmd[] = "AT+CIFSR\r\n";

//char ATcommand[200];

uint8_t rxBuff[1000] = {0};


// restart and connect to defualt wifi

//ESP01s newESP01s(){
//	ESP new_ESP;
//	return new_ESP;
//
//}


void setup(){

}

// basic AT command
// OK - good

void check_at(uint8_t *response, uint8_t *uart){
	HAL_UART_Transmit(&uart, (uint8_t *)check_cmd, strlen(check_cmd),1000);
	HAL_UART_Receive(&uart, response, 512, 100);

	HAL_Delay(500);
}

void reset_by_wire(){
	// restart the esp by changing ESP_01S_RST pin to LOW
	HAL_GPIO_WritePin(ESP_01S_RST_GPIO_Port,ESP_01S_RST_Pin , GPIO_PIN_RESET);
	HAL_Delay(100);
	// tour it on and give it time to reset
	HAL_GPIO_WritePin(ESP_01S_RST_GPIO_Port,ESP_01S_RST_Pin , GPIO_PIN_SET);
	HAL_Delay(1000);
}
// reset the board
void reset_by_code(uint8_t *response, uint8_t *uart){
	HAL_UART_Transmit(&uart, (uint8_t *)rst_cmd, strlen(rst_cmd),1000);
	HAL_UART_Receive(&uart, response, 512, 100);

	HAL_Delay(1000);
}



// station mode
int mode_set_station(uint8_t *uart, uint8_t *response){
	HAL_UART_Transmit(&uart, (uint8_t *)mode_station_cmd, strlen(mode_station_cmd),1000);
	HAL_UART_Receive(&uart, response, 512, 100);

	HAL_Delay(500);
	if(strstr((char *)response,"OK")){
		return 0;
	}else{
		return 1;
	}


}



//		// ping test
//		clearBuffers();
//		ATisOK = 0;
//		while(!ATisOK){
//			sprintf(ATcommand,"AT+PING=\"192.168.1.2\"\r\n");
//			HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
//			HAL_UART_Receive (&huart1, rxBuffer, 512, 1000);
//			if(strstr((char *)rxBuffer,"OK")){
//				ATisOK = 1;
//			}
//			HAL_Delay(200);
//		}

