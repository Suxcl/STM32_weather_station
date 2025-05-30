
#include "main.h"
// -- Basic AT commands --
#define AT_cmd "AT\r\n"
#define RST_cmd "AT+RST\r\n"
#define CHECK_VERSION_cmd "AT+GMR\r\n"
#define CHECK_WIFI_CON_cmd "AT+CIFSR\r\n"
#define SET_ONE_CON_cmd "AT+CIPMUX=0\r\n"
#define SET_MUL_CON_cmd "AT+CIPMUX=1\r\n"
// -- Modes commands --
#define MODE_STATION_cmd "AT+CWMODE_CUR=1\r\n"
#define MODE_SOFTAP_cmd "AT+CWMODE_CUR=2\r\n"
#define MODE_BOTH_cmd "AT+CWMODE_CUR=3\r\n"
// -- Sleep commands --
//#define ENTER_DEEP_SLEEP_cmd = "AT+GSLP\r\n"
#define WAKE_FROM_SLEEP_cmd "AT+SLEEP=0\r\n"
#define ENTER_SLEEP_cmd "AT+SLEEP=1\r\n"
#define MODEM_SLEEP_cmd "AT+SLEEP=2\r\n"
// -- Commands echo --
#define COM_ECHO_OFF_cmd "ATE0\r\n"
#define COM_ECHO_ON_cmd "ATE1\r\n"
// -- Wifi commands --
#define WIFI_CON_cmd "AT+CWJAP_CUR=";		// not full command
#define WIFI_AUTO_CON_cmd "AT+CWAUTOCONN=1\r\n"
// -- TCP commands --
#define CHECK_STATUS_cmd "AT+CIPSTATUS\r\n"
#define GET_IP_cmd "AT+CIFSR\r\n"
#define TCP_CON_START_cmd "AT+CIPSTART=" // not full command
#define TCP_CON_SEND_cmd "AT+CIPSEND=" // not full command
#define TCP_CON_CLOSE_cmd "AT+CIPCLOSE\r\n"
// ok status
#define AT_OK					0
#define AT_FAIL					-1
// wifi status
#define CWJAP_CUR_OK 			0
#define CWJAP_CUR_TIMEOUT   	-1
#define CWJAP_CUR_WRONG_PASS 	-2
#define CWJAP_CUR_BAD_AP		-3
#define CWJAP_CUR_CON_FAIL		-4
// station status
#define STATION_MODE			1
#define SOTFAP_MODE				2
#define SOFTSTATION_MODE		3
#define MODE_ERR				-1
// CIPMUX con_count status
#define CIPMUX_ONE				0
#define CIPMUX_MUL				1
#define CIPMUX_ERR				-1
// tcp con status
#define TCP_CON_STARTED			0
#define TCP_CON_ENDED			1
#define TCP_CON_START_ERR		-1
#define TCP_CON_END_ERR			-2
// get req status
#define GET_SEND_OK				0
#define GET_SEND_ERR			-1
// post req status
#define POST_SEND_OK			0
#define POST_SEND_ERR			-1
// STATUS status
#define STATUS_INACTIVE			0
#define STATUS_IDLE				1
#define STATUS_CON_GOT_IP		2
#define STATUS_GOT_TCP			3
#define STATUS_SERVER_TCP_ERR	4
#define STATUS_NO_AP_CON		5


// wifi
#define WIFI_CON				0
#define WIFI_ALREADY_CON		1
#define WIFI_ERR				-1


typedef struct Esp01s_settings{

	// buffers
	char TxBuffer[512];
	uint8_t RxBuffer[512];
	int  RxSize;
	char GetReq[1024];
	char PostReq[2048];
	char PostBody[1024];
	int timers[3];
	// hardware
	UART_HandleTypeDef* esp_uart;

	char esp_ip[20];
	char conn_server_ip[20];

	// server info

	// setup flags
	int 	ok_status;
	int 	station_status;
	int 	wifi_status;
	int 	con_count_status;
	int 	tcp_con_status;
	int 	get_req_status;
	int 	post_req_status;
	int 	post_req_1_status;
	int 	post_req_2_status;
	int 	post_req_3_status;

	// if status works theb get values



} Esp01s;

Esp01s newEsp01s(UART_HandleTypeDef* _UART);


void esp_transmit(Esp01s* esp, char* command, int delay);
void esp_receive(Esp01s* esp, int delay);
void check_at(Esp01s* esp);
void reset_by_wire();
void mode_set_station(Esp01s* esp);
void connect_to_ap(Esp01s* esp);
void set_max_one_con(Esp01s* esp);
void get_ip_from_wifi(Esp01s* esp);
void check_status(Esp01s* esp);
void start_connection(Esp01s* esp);
void end_connection(Esp01s* esp);
void send_get_req(Esp01s* esp);
//void send_post_req(Esp01s* esp,int t1,int h1, int p1,int sensor_id);
void send_post_req(Esp01s* esp,int t1,int h1, int p1,int t2,int h2, int p2,int t3,int h3, int p3);
void send_single_sensor_post_req(Esp01s* esp,int t, int h, int p, int id_sensor);
void esp_setup(Esp01s* esp);

