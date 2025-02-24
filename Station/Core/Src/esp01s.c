#include "esp01s.h"

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "spi.h"
#include "gpio.h"

// hard values
// wifi
//#define motoHotspot
#define Netis
// Ip Adress
#define MATIWAN
//#define MATILAN
//#define MOTOHOTSPOT

// HOTSPOT
#ifdef motoHotspot
//char ssid[] = "moto";
//char password[] = "jdjdjdjd";
#endif
#ifdef Netis
char ssid[] = "Netis Hifi Point";
char password[] = "kajaK123";
#endif




#ifdef MATILAN
 	char server_ip[] = "192.168.0.69";
#endif
#ifdef MATIWAN
	char server_ip[] = "85.193.245.146";
#endif
#ifdef MOTOHOTSPOT
#endif


char server_protocol[] = "TCP";
int server_port = 3000;
char api_get_path[] = "/sensors/frequency/";
char api_post_path[] = "/local_weathers/";





// fill buffers with 0's
void clearBuffers(Esp01s* esp){
	memset(esp->RxBuffer,0,sizeof(esp->RxBuffer));
	memset(esp->TxBuffer,0,sizeof(esp->TxBuffer));
}


Esp01s newEsp01s(UART_HandleTypeDef* _UART){
	Esp01s new_ESP;
	new_ESP.esp_uart = _UART;
	new_ESP.RxSize = 512;
	clearBuffers(&new_ESP);
	return new_ESP;
}

void esp_transmit(Esp01s* esp, char* c, int delay){

	HAL_UART_Transmit(esp->esp_uart, (uint8_t *)c, strlen(c),delay);
}
void esp_receive(Esp01s* esp, int delay){
	memset(esp->RxBuffer,0,sizeof(esp->RxBuffer));
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
		esp->station_status = STATION_MODE;
	}else{
		esp->station_status = MODE_ERR;
	}
}

void connect_to_ap(Esp01s* esp){
	memset(esp->TxBuffer,0,sizeof(esp->TxBuffer));
	// To działało u matiego
//	sprintf(esp->TxBuffer,"AT+CWJAP_CUR=\"ARRIS-7A66\",\"ucNEjnjBqc6J\"\r\n");
	// To nie działało u matiego
//	sprintf(esp->TxBuffer,%s,%s\r\n", ssid, password);

//	sprintf(esp->TxBuffer,"AT+CWJAP_CUR=%s,%s\r\n", ssid, password);
	sprintf(esp->TxBuffer,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", ssid, password);

	esp_transmit(esp, esp->TxBuffer,1000);
	esp_receive(esp, 10000);
	if(strstr((char *)esp->RxBuffer,"CONNECTED")){
		esp->wifi_status = CWJAP_CUR_OK;
	}
	else if(strstr((char *)esp->RxBuffer,"TIMEOUT")){
		esp->wifi_status = CWJAP_CUR_TIMEOUT;
	}
	else if(strstr((char *)esp->RxBuffer,"PASS")){
		esp->wifi_status = CWJAP_CUR_WRONG_PASS;
	}
	else if(strstr((char *)esp->RxBuffer,"BAD")){
		esp->wifi_status = CWJAP_CUR_BAD_AP;
	}
	else{
		esp->wifi_status = CWJAP_CUR_BAD_AP;
	}

}

void set_max_one_con(Esp01s* esp){
	esp_transmit(esp, SET_ONE_CON_cmd,1000);
	esp_receive(esp, 1000);
	if(strstr((char *)esp->RxBuffer,"OK")){
		esp->con_count_status = CIPMUX_ONE;
	}else{
		esp->con_count_status = CIPMUX_ERR;
	}
}

void get_ip_from_wifi(Esp01s* esp){
	esp_transmit(esp, GET_IP_cmd, 1000);
	esp_receive(esp, 1000);
//	if(NULL==strstr())
	HAL_Delay(100);
}

void check_status(Esp01s* esp){
	esp_transmit(esp, CHECK_STATUS_cmd, 1000);
	esp_receive(esp, 1000);
	HAL_Delay(100);
}

void start_connection(Esp01s* esp){
	memset(esp->TxBuffer,0,sizeof(esp->TxBuffer));
	sprintf(esp->TxBuffer,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", server_ip, server_port);


	esp_transmit(esp,esp->TxBuffer, 1000);
	esp_receive(esp, 1000);
	if(strstr((char *)esp->RxBuffer,"OK")){
		esp->tcp_con_status = TCP_CON_STARTED;
	}else{
		esp->tcp_con_status = TCP_CON_START_ERR;
	}
}

void end_connection(Esp01s* esp){
	esp_transmit(esp, TCP_CON_CLOSE_cmd, 1000);
	esp_receive(esp, 1000);
	if((strstr((char *)esp->RxBuffer,"OK"))|| (strstr((char *)esp->RxBuffer,"ALREADY"))){
		esp->tcp_con_status = TCP_CON_ENDED;
	}else{
		esp->tcp_con_status = TCP_CON_END_ERR;
	}

}

void send_get_req(Esp01s* esp){
	memset(esp->TxBuffer,0,sizeof(esp->TxBuffer));

	sprintf(esp->GetReq,
			"GET %s HTTP/1.1\r\n"
			"Host: %s:%d\r\n\r\n"
			"Accept: application/json\r\n",
			api_get_path, server_ip, server_port);

	// check if connection is valid
	sprintf(esp->TxBuffer,"AT+CIPSTATUS\r\n");
	esp_transmit(esp, esp->TxBuffer, 1000);
	esp_receive(esp, 1000);
	if(strstr((char *)esp->RxBuffer,"STATUS:3")){
		// good connectio
	}else{
		// bad conncetion
	}

	sprintf(esp->TxBuffer,"AT+CIPSEND=%d\r\n", strlen(esp->GetReq));
	//	sending CIPSEND with length of GetReg
	esp_transmit(esp, esp->TxBuffer, 1000);
	esp_receive(esp, 1000);

	//	searching for > char in response stating that esp is ready for post req
	// check for "Link not valid"
	if(strstr((char *)esp->RxBuffer,">")){
		memset(esp->RxBuffer,0,sizeof(esp->RxBuffer));
		esp_transmit(esp,esp->GetReq,1000);
		esp_receive(esp, 1000);
		if(strstr((char *)esp->RxBuffer,"SEND OK")){
			esp->get_req_status = GET_SEND_OK;

			int timers[3] = {0};

			char *index_s1 = strstr((char *)esp->RxBuffer,"\"S1\":");
			char *index_s2 = strstr((char *)esp->RxBuffer,"\"S2\":");
			char *index_s3 = strstr((char *)esp->RxBuffer,"\"S3\":");

			sscanf((index_s1+5), "%d", &timers[0]);
			sscanf((index_s2+5), "%d", &timers[1]);
			sscanf((index_s3+5), "%d", &timers[2]);

			esp->timers[0] = timers[0];
			esp->timers[1] = timers[1];
			esp->timers[2] = timers[2];

		}else{
			esp->get_req_status = POST_SEND_ERR;
			esp->timers[0] = 60;
			esp->timers[1] = 60;
			esp->timers[2] = 60;
			reset_by_wire();
			HAL_Delay(5000);
		}
	}
}





//void send_post_req(Esp01s* esp,int t1,int h1, int p1, int sensor_id){
void send_post_req(Esp01s* esp,int t1,int h1, int p1,int t2,int h2, int p2,int t3,int h3, int p3){

	memset(esp->TxBuffer,0,sizeof(esp->TxBuffer));
	memset(esp->RxBuffer,0,sizeof(esp->RxBuffer));
	memset(esp->PostBody,0,sizeof(esp->PostBody));
	memset(esp->PostReq,0,sizeof(esp->PostReq));

	sprintf(esp->PostBody,
	"{"
		"\"local_weathers\":["
			  "{"
				  "\"temperature\":%d,"
				  "\"humidity\":%d,"
				  "\"pressure\":%d,"
				  "\"sensor\":1"
			  "},"
			  "{"
				  "\"temperature\":%d,"
				  "\"humidity\":%d,"
				  "\"pressure\":%d,"
				  "\"sensor\":2"
			  "},"
			  "{"
				  "\"temperature\": %d,"
				  "\"humidity\": %d,"
				  "\"pressure\": %d,"
				  "\"sensor\":3"
			  "}"
		"]"
	"}",
	t1,h1,p1,t1,h2,p2,t3,h3,p3);


	sprintf(esp->PostReq,
			"POST %s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: %d\r\n\r\n"
			"%s\r\n\r\n",
			api_post_path, server_ip, server_port,
			strlen(esp->PostBody), esp->PostBody);

	// check if connection is valid
	sprintf(esp->TxBuffer,"AT+CIPSTATUS\r\n");
	esp_transmit(esp, esp->TxBuffer, 1000);
	esp_receive(esp, 1000);
	if(strstr((char *)esp->RxBuffer,"STATUS:3")){
		// good connectio
	}else{
		// bad conncetion
	}

	sprintf(esp->TxBuffer,"AT+CIPSEND=%d\r\n", strlen(esp->PostReq));
	//	sending CIPSEND with length of PostReg
	esp_transmit(esp, esp->TxBuffer, 1000);
	esp_receive(esp, 1000);

	//	searching for > char in response stating that esp is ready for post req
	// check for "Link not valid"
	if(strstr((char *)esp->RxBuffer,">")){
		memset(esp->RxBuffer,0,sizeof(esp->RxBuffer));
		esp_transmit(esp,esp->PostReq,1000);
		esp_receive(esp, 1000);
		if(strstr((char *)esp->RxBuffer,"SEND OK")){
			esp->post_req_status = POST_SEND_OK;
		}else{
			esp->post_req_status = POST_SEND_ERR;
			reset_by_wire();
			HAL_Delay(5000);
		}
	}
}


void send_single_sensor_post_req(Esp01s* esp,int t, int h, int p, int id_sensor){

	memset(esp->TxBuffer,0,sizeof(esp->TxBuffer));
	memset(esp->RxBuffer,0,sizeof(esp->RxBuffer));
	memset(esp->PostBody,0,sizeof(esp->PostBody));
	memset(esp->PostReq,0,sizeof(esp->PostReq));

	sprintf(esp->PostBody,
	"{"
		"\"local_weather\":"
			  "{"
				  "\"temperature\":%d,"
				  "\"humidity\":%d,"
				  "\"pressure\":%d,"
				  "\"sensor_id\":%d"
			  "}"
	"}",
	t, h, p, id_sensor);


	sprintf(esp->PostReq,
			"POST %s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: %d\r\n\r\n"
			"%s\r\n\r\n",
			api_post_path, server_ip, server_port,
			strlen(esp->PostBody), esp->PostBody);

	// check if connection is valid
	sprintf(esp->TxBuffer,"AT+CIPSTATUS\r\n");
	esp_transmit(esp, esp->TxBuffer, 1000);
	esp_receive(esp, 1000);
	if(strstr((char *)esp->RxBuffer,"STATUS:3")){
		// good connectio
	}else{
		end_connection(esp);
		start_connection(esp);
	}
	memset(esp->TxBuffer,0,sizeof(esp->TxBuffer));
	sprintf(esp->TxBuffer,"AT+CIPSEND=%d\r\n", strlen(esp->PostReq));
	//	sending CIPSEND with length of PostReg
	esp_transmit(esp, esp->TxBuffer, 1000);
	esp_receive(esp, 1000);

	//	searching for > char in response stating that esp is ready for post req
	// check for "Link not valid"
	if(strstr((char *)esp->RxBuffer,">")){
		memset(esp->RxBuffer,0,sizeof(esp->RxBuffer));
		esp_transmit(esp,esp->PostReq,1000);
		esp_receive(esp, 1000);
		if(strstr((char *)esp->RxBuffer,"SEND OK")){
			esp->post_req_status = POST_SEND_OK;
		}else{
			esp->post_req_status = POST_SEND_ERR;
			reset_by_wire();
			HAL_Delay(5000);
		}

	}
}


/*
 * 	This setup
 *
 */

void esp_setup(Esp01s* esp){
	check_at(esp);
	while(esp->ok_status != AT_OK){
		reset_by_wire();
		HAL_Delay(2000);
		check_at(esp);
	}
	mode_set_station(esp);
	connect_to_ap(esp);
	set_max_one_con(esp);
	get_ip_from_wifi(esp);
	check_status(esp);
}
