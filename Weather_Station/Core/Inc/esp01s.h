
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
#define CHECK_STATUS_cmd "AT+STATUS\r\n"
#define TCP_CON_START_cmd "AT+CIPSTART=" // not full command
#define TCP_CON_SEND_cmd "AT+CIPSEND=" // not full command
#define TCP_CON_CLOSE_cmd "AT+CIPCLOSE\r\n"
// ok status
#define AT_OK					0
#define AT_FAIL					1
// wifi status
#define CWJAP_CUR_OK 			0
#define CWJAP_CUR_TIMEOUT   	1
#define CWJAP_CUR_WRONG_PASS 	2
#define CWJAP_CUR_BAD_AP		3
#define CWJAP_CUR_CON_FAIL		4
// CIPMUX
#define CIPMUX_ONE				0
#define CIPMUX_MUL				1
// STATION SETUP
#define STATION_SET				0
#define STATION_ERR				1
// one max setup
#define ONE_CON_MODE_OK			0
#define ONE_CON_MODE_ERR		1

// STATUS status
#define STATUS_INACTIVE			0
#define STATUS_IDLE				1
#define STATUS_CON_GOT_IP		2
#define STATUS_GOT_TCP			3
#define STATUS_SERVER_TCP_ERR	4
#define STATUS_NO_AP_CON		5


// tcp start
#define SER_CON_OK				0
#define SER_CON_ERR				1
// wifi
#define WIFI_CON				0
#define WIFI_ERR				1



typedef struct Esp01s_settings{

	// buffers
	char TxBuffer[512];
	char RxBuffer[512];
	int  RxSize;
	char PostData[512];

	// hardware
	UART_HandleTypeDef* esp_uart;

	// settings
	int 	mode;
	int 	connection_amount;
	// wifi
	char  	ssid[100];
	char 	password[100];
	// server info
	char 	server_protocol[50];
	char	server_ip[50];
	char	server_port[50];
	char 	server_post_link[100];
	char 	server_get_link[100];

	// setup flags
	int 	ok_status;
	int 	station_status;
	int 	wifi_status;
	int 	one_con_status;
	int 	conn_status;


} Esp01s;

Esp01s newEsp01s(UART_HandleTypeDef* _UART);


void esp_transmit(Esp01s* esp, char* command, int delay);
void esp_receive(Esp01s* esp, int delay);
void check_at(Esp01s* esp);
void reset_by_wire();
void mode_set_station(Esp01s* esp);
void connect_to_ap(Esp01s* esp);
void set_max_one_con(Esp01s* esp);
void check_status(Esp01s* esp);
void start_connection(Esp01s* esp);
void end_connection(Esp01s* esp);
void send_get_req(Esp01s* esp);
void send_post_req(Esp01s* esp);

void esp_setup(Esp01s* esp);

