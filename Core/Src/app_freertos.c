/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN Variables */
uint32_t task00 = 0;
uint32_t task01 = 0;
uint32_t task02 = 0;
uint32_t task03 = 0;

/* USER CODE END Variables */
/* Definitions for Task00_1ms */
osThreadId_t Task00_1msHandle;
const osThreadAttr_t Task00_1ms_attributes = {
  .name = "Task00_1ms",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for Task01_10ms */
osThreadId_t Task01_10msHandle;
const osThreadAttr_t Task01_10ms_attributes = {
  .name = "Task01_10ms",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for Task02_100ms */
osThreadId_t Task02_100msHandle;
const osThreadAttr_t Task02_100ms_attributes = {
  .name = "Task02_100ms",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for Task03_1000ms */
osThreadId_t Task03_1000msHandle;
const osThreadAttr_t Task03_1000ms_attributes = {
  .name = "Task03_1000ms",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTast00(void *argument);
void StartTask01(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task00_1ms */
  Task00_1msHandle = osThreadNew(StartTast00, NULL, &Task00_1ms_attributes);

  /* creation of Task01_10ms */
  Task01_10msHandle = osThreadNew(StartTask01, NULL, &Task01_10ms_attributes);

  /* creation of Task02_100ms */
  Task02_100msHandle = osThreadNew(StartTask02, NULL, &Task02_100ms_attributes);

  /* creation of Task03_1000ms */
  Task03_1000msHandle = osThreadNew(StartTask03, NULL, &Task03_1000ms_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTast00 */
/**
  * @brief  Function implementing the Task00_1ms thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTast00 */
void StartTast00(void *argument)
{
  /* init code for USB_Device */
  MX_USB_Device_Init();
  /* USER CODE BEGIN StartTast00 */
  /* Infinite loop */
  for(;;)
  {
	task00++;
    osDelay(1);
  }
  /* USER CODE END StartTast00 */
}

/* USER CODE BEGIN Header_StartTask01 */
/**
* @brief Function implementing the Task01_10ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask01 */
void StartTask01(void *argument)
{
  /* USER CODE BEGIN StartTask01 */
  /* Infinite loop */
  for(;;)
  {
	task01++;
    osDelay(10);
  }
  /* USER CODE END StartTask01 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the Task02_100ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
	task02++;
	//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    osDelay(100);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the Task03_1000ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
	task03++;
	printf("Task03: %lu\n", (unsigned long)task03);

	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    osDelay(1000);
  }
  /* USER CODE END StartTask03 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

