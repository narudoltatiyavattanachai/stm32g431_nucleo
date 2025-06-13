/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "socket.h"
#include "dhcp.h"
#include "httpServer.h"
/* USER CODE END Includes */

/* USER CODE BEGIN 0 */
#define DHCP_SOCKET     0
#define DNS_SOCKET      1
#define HTTP_SOCKET     2
#define SOCK_TCPS       0
#define SOCK_UDPS       1
#define PORT_TCPS       5000
#define PORT_UDPS       3000
#define MAX_HTTPSOCK    6
#define index_page "<!DOCTYPE html>"\
  "<html>"\
    "<head>"\
      "<title>W5500-STM32 Web Server</title>"\
      "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
      "<link href=\"data:image/x-icon;base64,A\" rel=\"icon\" type=\"image/x-icon\">"\
      "<style>"\
        "html {display: inline-block; margin: 0px auto; text-align: center;}"\
        "body{margin-top: 50px;}"\
        ".button {display: block;"\
          "width: 70px;"\
          "background-color: #008000;"\
          "border: none;"\
          "color: white;"\
          "padding: 14px 28px;"\
          "text-decoration: none;"\
          "font-size: 24px;"\
          "margin: 0px auto 36px;"\
          "border-radius: 5px;}"\
        ".button-on {background-color: #008000;}"\
        ".button-on:active{background-color: #008000;}"\
        ".button-off {background-color: #808080;}"\
        ".button-off:active {background-color: #808080;}"\
        "p {font-size: 20px;color: #808080;margin-bottom: 20px;}"\
      "</style>"\
    "</head>"\
    "<body>"\
      "<h1>STM32 - W5500</h1>"\
      "<p>Control the light via Ethernet</p>"\
      "<a class=\"button button-on\" href=\"/ledon.html\">ON</a>"\
      "<a class=\"button button-off\" href=\"/ledoff.html\">OFF</a>"\
    "</body>"\
  "</html>"

#define ledon_page" <!DOCTYPE html>"\
  "<html>"\
    "<head>"\
      "<title>W5500-STM32 Web Server</title>"\
      "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
      "<link href=\"data:image/x-icon;base64,A\" rel=\"icon\" type=\"image/x-icon\">"\
      "<style>"\
        "html {display: inline-block; margin: 0px auto; text-align: center;}"\
        "body{margin-top: 50px;}"\
        ".button {display: block;"\
          "width: 70px;"\
          "background-color: #008000;"\
          "border: none;"\
          "color: white;"\
          "padding: 14px 28px;"\
          "text-decoration: none;"\
          "font-size: 24px;"\
          "margin: 0px auto 36px;"\
          "border-radius: 5px;}"\
        ".button-on {background-color: #008000;}"\
        ".button-on:active{background-color: #008000;}"\
        ".button-off {background-color: #808080;}"\
        ".button-off:active {background-color: #808080;}"\
        "p {font-size: 20px;color: #808080;margin-bottom: 20px;}"\
      "</style>"\
    "</head>"\
    "<body>"\
      "<h1>STM32 - W5500</h1>"\
      "<p>Light is currently on</p>"\
      "<a class=\"button button-off\" href=\"/ledoff.html\">OFF</a>"\
    "</body>"\
  "</html>"

#define ledoff_page"<!DOCTYPE html>"\
  "<html>"\
    "<head>"\
      "<title>W5500-STM32 Web Server</title>"\
      "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
      "<link href=\"data:image/x-icon;base64,A\" rel=\"icon\" type=\"image/x-icon\">"\
      "<style>"\
        "html {display: inline-block; margin: 0px auto; text-align: center;}"\
        "body{margin-top: 50px;}"\
        ".button {display: block;"\
          "width: 70px;"\
          "background-color: #008000;"\
          "border: none;"\
          "color: white;"\
          "padding: 14px 28px;"\
          "text-decoration: none;"\
          "font-size: 24px;"\
          "margin: 0px auto 36px;"\
          "border-radius: 5px;}"\
        ".button-on {background-color: #008000;}"\
        ".button-on:active{background-color: #008000;}"\
        ".button-off {background-color: #808080;}"\
        ".button-off:active {background-color: #808080;}"\
        "p {font-size: 20px;color: #808080;margin-bottom: 20px;}"\
      "</style>"\
    "</head>"\
    "<body>"\
      "<h1>STM32 - W5500</h1>"\
      "<p>Light is currently off</p>"\
      "<a class=\"button button-on\" href=\"/ledon.html\">ON</a>"\
    "</body>"\
  "</html>"

uint8_t socknumlist[] = {2, 3, 4, 5, 6, 7};
uint8_t RX_BUF[1024];
uint8_t TX_BUF[1024];
wiz_NetInfo net_info = {
    .mac  = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED },
    .dhcp = NETINFO_DHCP
};

void wizchipSelect(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}

void wizchipUnselect(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void wizchipReadBurst(uint8_t* buff, uint16_t len) {
    HAL_SPI_Receive(&hspi1, buff, len, HAL_MAX_DELAY);
}

void wizchipWriteBurst(uint8_t* buff, uint16_t len) {
    HAL_SPI_Transmit(&hspi1, buff, len, HAL_MAX_DELAY);
}

uint8_t wizchipReadByte(void) {
    uint8_t byte;
    wizchipReadBurst(&byte, sizeof(byte));
    return byte;
}

void wizchipWriteByte(uint8_t byte) {
    wizchipWriteBurst(&byte, sizeof(byte));
}

volatile bool ip_assigned = false;

void Callback_IPAssigned(void) {
    ip_assigned = true;
}

void Callback_IPConflict(void) {
    ip_assigned = false;
}

uint8_t dhcp_buffer[1024];
uint8_t dns_buffer[1024];

void W5500Init() {
    // Register W5500 callbacks
    reg_wizchip_cs_cbfunc(wizchipSelect, wizchipUnselect);
    reg_wizchip_spi_cbfunc(wizchipReadByte, wizchipWriteByte);
    reg_wizchip_spiburst_cbfunc(wizchipReadBurst, wizchipWriteBurst);

    uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};
    wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);

    // set MAC address before using DHCP
    setSHAR(net_info.mac);
    DHCP_init(DHCP_SOCKET, dhcp_buffer);

    reg_dhcp_cbfunc(
        Callback_IPAssigned,
        Callback_IPAssigned,
        Callback_IPConflict
    );

    uint32_t ctr = 10000;
    while((!ip_assigned) && (ctr > 0)) {
        DHCP_run();
        ctr--;
    }
    if(!ip_assigned) {
        return;
    }

    getIPfromDHCP(net_info.ip);
    getGWfromDHCP(net_info.gw);
    getSNfromDHCP(net_info.sn);

//    char charData[200]; // Data holder
//    sprintf(charData,"IP:  %d.%d.%d.%d\r\nGW:  %d.%d.%d.%d\r\nNet: %d.%d.%d.%d\r\n",
//        net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3],
//        net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3],
//        net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]
//    );
//    HAL_UART_Transmit(&huart1,(uint8_t *)charData,strlen(charData),1000);

    wizchip_setnetinfo(&net_info);
}
/* USER CODE END 0 */

  /* USER CODE BEGIN 2 */
  W5500Init();
  httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);
  reg_httpServer_cbfunc(NVIC_SystemReset, NULL);
  /* Web content registration */
  reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page);
  reg_httpServer_webContent((uint8_t *)"ledon.html", (uint8_t *)ledon_page);
  reg_httpServer_webContent((uint8_t *)"ledoff.html", (uint8_t *)ledoff_page);

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    for(int i = 0; i < MAX_HTTPSOCK; i++) httpServer_run(i); // HTTP Server handler
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */