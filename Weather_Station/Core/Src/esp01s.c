#include "esp01s.h"

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

// TODO
// setup (check if somethink is already connected)
// send request
// get somethinks maybe multiple data from multiple requests

// what data go to post for sensor info
// 	sensor data for each data collector(names)
//		- temperature
// 		- humidity
//
//
//

// what goes from the site into hardware
//	sensor info
//		- name
//		- wifi connection
//			- ssid
//			- password
//	communication
//		- timers
//			- for lora sending data (same timer goes for sending data from esp to server)
//			- timer for board to check if somethink is to change
//		- server info where to send and get data
//
//
//



// restart and connect to defualt wifi

Esp01s newEsp01s(UART_HandleTypeDef* _UART){
	Esp01s new_ESP;
	new_ESP.esp_uart = _UART;
	new_ESP.RxSize = 512;
	memset(new_ESP.RxBuffer,0,sizeof(new_ESP.RxBuffer));
//	// setup default values
//	new_ESP->ssid = "Netis Hifi Point";
//	new_ESP->password = "kajaK123";
//	new_ESP->server_protocol = "TCP";
//	new_ESP->server_ip = "192.168.1.2";
//	new_ESP->server_port = "8000";


	// return element with default values
	return new_ESP;
}


//void clearBuffers(Esp01s* esp){
//	memset(esp->RxBuffer,0,sizeof(esp->RxBuffer));
//}




// basic AT command
// OK - good

void esp_transmit(Esp01s* esp, char* c, int delay){
	HAL_UART_Transmit(esp->esp_uart, (uint8_t *)c, strlen(c),delay);
}
void esp_receive(Esp01s* esp, int delay){
	HAL_UART_Receive(esp->esp_uart, esp->RxBuffer, esp->RxSize,delay);
}

void check_at(Esp01s* esp){
	esp_transmit(esp, AT_cmd, 1000);
	esp_receive(esp, 1000);
	if(strstr((char *)esp->RxBuffer,"OK")){
		esp->ok_status = AT_OK;
	}else{
		esp->ok_status = AT_FAIL;
	}
	HAL_Delay(100);
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
//void reset_by_code(uint8_t *response, uint8_t *uart){
//	char atcommand[] = RST_cmd;
//	HAL_UART_Transmit(&uart, (uint8_t *)atcommand, strlen(atcommand),1000);
//	HAL_UART_Receive(&uart, response, 512, 100);
//
//	HAL_Delay(1000);
//}


// station mode
void mode_set_station(Esp01s* esp){
	esp_transmit(esp, MODE_STATION_cmd, 1000);
	esp_receive(esp, 1000);
	if(strstr((char *)esp->RxBuffer,"OK")){
		esp->station_status = STATION_SET;
	}else{
		esp->station_status = STATION_ERR;
	}
}

void connect_to_ap(Esp01s* esp){
	memset(esp->TxBuffer,0,sizeof(esp->TxBuffer));
	sprintf(esp->TxBuffer,"AT+CWJAP_CUR=%s,%s\r\n", esp->ssid, esp->password);
	esp_transmit(esp, esp->TxBuffer,1000);
	esp_receive(esp, 10000);
	if(strstr((char *)esp->RxBuffer,"CONNECTED")){
		esp->wifi_status = CWJAP_CUR_OK;
	}else{
		esp->wifi_status = CWJAP_CUR_CON_FAIL;
	}

}

void set_max_one_con(Esp01s* esp){
	esp_transmit(esp, SET_ONE_CON_cmd,1000);
	esp_receive(esp, 1000);
	if(strstr((char *)esp->RxBuffer,"OK")){
		esp->one_con_status = ONE_CON_MODE_OK;
	}else{
		esp->one_con_status = ONE_CON_MODE_ERR;
	}
}
void check_status(Esp01s* esp){
	esp_transmit(esp, CHECK_STATUS_cmd, 1000);
	esp_receive(esp, 1000);
}

void start_connection(Esp01s* esp){
	memset(esp->TxBuffer,0,sizeof(esp->TxBuffer));
	sprintf(esp->TxBuffer,"AT+CIPSTART=%s,%s,%s\r\n",
			esp->server_protocol,
			esp->server_ip,
			esp->server_port);

	esp_transmit(esp,esp->TxBuffer, 1000);
	esp_receive(esp, 1000);
	if(strstr((char *)esp->RxBuffer,"OK")){
		esp->one_con_status = SER_CON_OK;
	}else{
		esp->one_con_status = SER_CON_ERR;
	}
}

void end_connection(Esp01s* esp){
	esp_transmit(esp, TCP_CON_CLOSE_cmd, 1000);
	esp_receive(esp, 1000);
}

void send_get_req(Esp01s* esp){
	memset(esp->TxBuffer,0,sizeof(esp->TxBuffer));
//	sprintf();
	esp_transmit(esp, CHECK_STATUS_cmd, 1000);
	esp_receive(esp, 1000);
}

void send_post_req(Esp01s* esp){
	esp_transmit(esp, CHECK_STATUS_cmd, 1000);
	esp_receive(esp, 1000);
}

void esp_setup(Esp01s* esp){
	check_at(esp);
	while(esp->ok_status != AT_OK){
		reset_by_wire();
		check_at(esp);
		// fatal error no communication with board
	}
	mode_set_station(esp);
	connect_to_ap(esp);
	if(esp->wifi_status != CWJAP_CUR_OK){
		// error about bad wifi info
		return;
	}
	set_max_one_con(esp);

	// show that setup is ok

}


