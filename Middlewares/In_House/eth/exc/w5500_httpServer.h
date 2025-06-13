Additional code on top of the library code(httpServer.h)
line no 14
#include "stm32f1xx_hal.h"

Additional code on top of the library code(httpServer.c)
line no 521
if (!strcmp((char *)uri_name, "ledon.html")) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
if (!strcmp((char *)uri_name, "ledoff.html")) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
Remove from the library code(wizchip_conf.h)
line no 157
#include "W5500/w5500.h"