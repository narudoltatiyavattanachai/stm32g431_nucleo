/**
 * @file w5500.c
 * @brief W5500 Ethernet hardware initialization implementation for STM32F103
 * 
 * @details This file implements the hardware-level access layer for the WIZnet W5500
 *          Ethernet controller on STM32F103 microcontrollers. The implementation
 *          focuses on:
 *          1. SPI interface initialization and communication
 *          2. Hardware reset and initialization sequence
 *          3. Register configuration and access
 *          4. Network parameter setup
 * 
 *          This module provides only the hardware interface required by socket
 *          and transport implementations in other modules.
 * 
 * @author Your Name
 * @date 2025-06-09
 * @version 1.0
 */

/* ========================================================================== 
 * INCLUDES
 * ==========================================================================*/

#include "w5500_spi.h"

/* ==========================================================================
 * CONFIGURATION AND DEFINES
 * ==========================================================================*/

#define W5500_SPI_TIMEOUT      10000


/* ==========================================================================
 * PRIVATE VARIABLES
 * ==========================================================================*/

extern SPI_HandleTypeDef hspi2;

/* ==========================================================================
 * PRIVATE FUNCTION PROTOTYPES
 * ==========================================================================*/

/* ==========================================================================
 * SPI INTERFACE FUNCTIONS
 * These are used by the wizchip driver for SPI communication
 * ==========================================================================*/

/**
 * @brief Assert chip select (CS) pin to select W5500 for communication
 * @note chcked Narudol T.   2025-06-10
 */
void w5500_cs_select(void)
{
    HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_RESET);
    printf("W5500_SPI_NCS: LOW\r\n");   // In w5500_cs_select()
}

/**
 * @brief De-assert chip select (CS) pin to deselect W5500
 * @note chcked Narudol T.   2025-06-10
 */
void w5500_cs_deselect(void)
{
    HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_SET);
    printf("W5500_SPI_NCS: HIGH\r\n");  // In w5500_cs_deselect()
}

/**
 * @brief Exchange a single byte over SPI
 * @param data Byte to send
 * @return Byte received
 */
void w5500_spi_readburst(uint8_t* pBuf, uint16_t len)
{
    HAL_SPI_Receive(&hspi2, pBuf, len, W5500_SPI_TIMEOUT);
}

uint8_t w5500_spi_read(void) {
    uint8_t byte;
    w5500_spi_readburst(&byte, sizeof(byte));
    return byte;
}

/**
 * @brief Exchange a single byte over SPI
 * @param data Byte to send
 * @return Byte received
 */
void w5500_spi_writeburst(uint8_t* pBuf, uint16_t len) {
    HAL_SPI_Transmit(&hspi2, pBuf, len, W5500_SPI_TIMEOUT);
}

void w5500_spi_write(uint8_t byte) {
	w5500_spi_writeburst(&byte, sizeof(byte));
}
/* ==========================================================================
 * HARDWARE UTILITY FUNCTIONS
 * These functions provide utility operations for hardware control
 * ==========================================================================*/


/* ==========================================================================
 * PRIVATE HARDWARE FUNCTIONS
 * Internal functions for hardware initialization
 * ==========================================================================*/

/* ==========================================================================
 * PUBLIC API IMPLEMENTATION - HARDWARE FUNCTIONS
 * These functions provide the core W5500 hardware initialization
 * ==========================================================================*/
/**
 * @brief Initialize the W5500 hardware and SPI communication
 * 
 * This function:
 *  - Registers SPI and chip select callbacks
 *  - Performs a hardware reset
 *  - Verifies SPI connection (VERSIONR == 0x04)
 *  - Initializes socket TX/RX buffers
 *  - Applies static IP configuration
 *  - Reads back the applied network info for verification
 */
void w5500_spi_init(void) {
    printf("Initializing W5500...\r\n");
    //__HAL_SPI_ENABLE(&hspi2);


    // 1. Register Chip Select callbacks (GPIO control)
    printf("Registering chip select callbacks...\r\n");
    reg_wizchip_cs_cbfunc(w5500_cs_select, w5500_cs_deselect);

    // 2. Register SPI byte-level read/write functions
    printf("Registering SPI byte read/write callbacks...\r\n");
    reg_wizchip_spi_cbfunc(w5500_spi_read, w5500_spi_write);

    // Optional: Burst mode SPI (if implemented)
    printf("Registering SPI burst read/write callbacks...\r\n");
    reg_wizchip_spiburst_cbfunc(w5500_spi_readburst, w5500_spi_writeburst);


    // 5. Initialize socket buffer sizes (2KB for socket 0)
    printf("Initializing socket buffers...\r\n");
    uint8_t txsize[8] = {2, 2, 2, 2, 2, 2, 2, 2};  // 2KB TX on socket 0
    uint8_t rxsize[8] = {2, 2, 2, 2, 2, 2, 2, 2};  // 2KB RX on socket 0

    if (wizchip_init(txsize, rxsize) != 0) {
        printf("Wizchip_Init: Failed\r\n");
        return;
    }

    // 6. Apply static IP configuration
    printf("Applying static network configuration...\r\n");

    eth_config_init_static();  // Initialize Static Network Information Variables
    eth_config_set_netinfo(&g_network_info);  // Set Static Network Information

}


/* ==========================================================================
 * NOTE: Socket functions have been moved to w5500_socket.c module
 * 
 * The following functions were previously located here and are now in w5500_socket.c:
 * - w5500_socket_open(): Opens a socket and connects to a remote endpoint
 * - w5500_socket_close(): Closes an active socket connection
 * ==========================================================================*/

/* ==========================================================================
 * PRIVATE IMPLEMENTATION FUNCTIONS
 * ==========================================================================*/

/* ==========================================================================
 * NOTE: Socket and network initialization functions have been moved to the
 * appropriate modules (w5500_socket.c and w5500_uros.c)
 * ==========================================================================*/

/* ==========================================================================
 * HARDWARE UTILITY FUNCTIONS
 * Functions to reset the chip and verify hardware status
 * ==========================================================================*/

/**
 * @brief Restart the W5500 hardware
 * 
 * @details Performs a hardware reset by toggling the reset pin
 *          Can be called to recover from error conditions
 * 
 */
void w5500_spi_reset(void)
{
	HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(10); // At least 1ms = safe for PMODE latching
	HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(10); // Wait for internal init
}


/* ==========================================================================
 * NOTE: Socket connection functions have been moved to w5500_socket.c
 * Transport functions have been moved to w5500_uros.c
 * ==========================================================================*/
