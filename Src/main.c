
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "lwip.h"
#include "usb_host.h"

/* USER CODE BEGIN Includes */
#include "debug_leds.h"
#include "connection.h"
#include "objects.h"
#include "client_config.h"
#include "lwip_helpers.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;

osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 1024);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 120;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_G_Pin|LED_R_Pin|LED_B_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_GPIO_OUT_GPIO_Port, USB_GPIO_OUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, JTAG_TRST_Pin|JTAG_TDO_Pin|JTAG_TMS_Pin|JTAG_TCLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_G_Pin LED_R_Pin LED_B_Pin */
  GPIO_InitStruct.Pin = LED_G_Pin|LED_R_Pin|LED_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_GPIO_OUT_Pin */
  GPIO_InitStruct.Pin = USB_GPIO_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_GPIO_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_GPIO_IN_Pin */
  GPIO_InitStruct.Pin = USB_GPIO_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_GPIO_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : JTAG_TRST_Pin JTAG_TDO_Pin JTAG_TMS_Pin JTAG_TCLK_Pin */
  GPIO_InitStruct.Pin = JTAG_TRST_Pin|JTAG_TDO_Pin|JTAG_TMS_Pin|JTAG_TCLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : JTAG_TDI_Pin */
  GPIO_InitStruct.Pin = JTAG_TDI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(JTAG_TDI_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

typedef struct
{
    lwm2m_object_t * securityObjP;
    int sock;
    connection_t * connList;
    int addressFamily;
} client_data_t;

//WAKAAMA
static lwm2m_context_t *lwm2mContext = NULL;
static lwm2m_object_t *objArray[OBJ_COUNT];
static client_data_t data;
static int q_reset = 0;


static void print_state(lwm2m_context_t * lwm2mH)
{
    if(lwm2mH->state == STATE_READY)
        return;
    fprintf(stderr, "State: ");
    switch(lwm2mH->state)
    {
    case STATE_INITIAL:
        fprintf(stderr, "STATE_INITIAL");
        break;
    case STATE_BOOTSTRAP_REQUIRED:
        fprintf(stderr, "STATE_BOOTSTRAP_REQUIRED");
        break;
    case STATE_BOOTSTRAPPING:
        fprintf(stderr, "STATE_BOOTSTRAPPING");
        break;
    case STATE_REGISTER_REQUIRED:
        fprintf(stderr, "STATE_REGISTER_REQUIRED");
        break;
    case STATE_REGISTERING:
        fprintf(stderr, "STATE_REGISTERING");
        break;
    case STATE_READY:
        fprintf(stderr, "STATE_READY");
        break;
    default:
        fprintf(stderr, "Unknown !");
        break;
    }
fprintf(stderr, "\r\n");
}

static void taskWakaama(void *socket) {

    int result;
    while(1){

        printf("wakaama has started!\n");
        result = 1;

        memset(&data, 0, sizeof (client_data_t));
        data.sock = (int) socket;
        data.addressFamily = AF_INET;

        if(objArray[0] == NULL) {
          objArray[0] = get_security_object();          
        }
        if (NULL == objArray[0]) {
            printf("Failed to create security object\r\n");
            result = -1;
            continue;
        }
        data.securityObjP = objArray[0];
        printf("SecurityObject Created\n");

        if(objArray[1] == NULL) {
          objArray[1] = get_server_object(WAKAAMA_SHORT_ID, WAKAAMA_BINDING, 300, false);
        }
        
        if (NULL == objArray[1]) {
            printf("Failed to create server object\r\n");
            result = -2;
            continue;
        }
        printf("ServerObject Created\n\r");

        if(objArray[2] == NULL) {
          printf("Getting object device\n\r");
          objArray[2] = get_object_device();
        }
        if (NULL == objArray[2]) {
            printf("Failed to create Device object\r\n");
            result = -3;
            continue;
        }
        printf("DeviceObject Created\n");


        if (objArray[3] == NULL) {
          objArray[3] = get_test_object();
        }
        if (NULL == objArray[3]) {
            printf("Failed to create Test object\r\n");
            result = -4;
            continue;
        }
        printf("TestObject Created\n");

        // init context
        lwm2mContext = lwm2m_init(&data);
        if (NULL == lwm2mContext) {
            printf("lwm2m_init() failed\r\n");
            result = -8;
        }
        /*
        * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
        * the number of objects we will be passing through and the objects array
        */
        result = lwm2m_configure(lwm2mContext, DEVICE_NAME, NULL, NULL, OBJ_COUNT, objArray);
        if (result != COAP_NO_ERROR) {
            printf("lwm2m_configure() failed: 0x%X\r\n", result);
            result = -9;
        }

        printf("LWM2M Client has started\n");
        print_state(lwm2mContext);
        while (q_reset == 0) {
            struct timeval tv;
            fd_set readfds;

            tv.tv_sec = 60;
            tv.tv_usec = 0;

            FD_ZERO(&readfds);
            FD_SET(data.sock, &readfds);

            /*
             * This function does two things:
             *  - first it does the work needed by liblwm2m (eg. (re)sending some packets).
             *  - Secondly it adjusts the timeout value (default 60s) depending on the state of the transaction
             *    (eg. retransmission) and the time before the next operation
             */
            result = lwm2m_step(lwm2mContext, &(tv.tv_sec));
            // FOR TESTS
            print_state(lwm2mContext);
            // FOR TESTS
            if (result != 0) {
                printf("lwm2m_step() failed: 0x%X\r\n", result);
                lwm2mContext->state = STATE_INITIAL;
            }
            /*
             * This part wait for an event on the socket until "tv" timed out (set
             * with the precedent function)
             */
            result = lwip_select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

            if (result < 0) {
                if (errno != EINTR) {
                    printf("Error in select(): %d %s\r\n", errno, strerror(errno));
                }
            } else if (result > 0) {
                uint8_t buffer[MAX_PACKET_SIZE];
                int numBytes = 0;

                /*
                 * If an event happens on the socket
                 */
                if (FD_ISSET(data.sock, &readfds)) {
                    struct sockaddr_storage addr;
                    socklen_t addrLen= 0;

                    addrLen = sizeof (addr);

                    /*
                     * We retrieve the data received
                     */
                    numBytes = lwip_recvfrom(data.sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *) &addr, &addrLen);

                    if (0 > numBytes) {
                        fprintf(stderr, "Error in recvfrom(): %d %s\r\n", errno, strerror(errno));
                    } else if (0 < numBytes) {
                        connection_t * connP;

                        connP = connection_find(data.connList, &addr, addrLen);
                        if (connP != NULL) {
                            /*
                             * Let liblwm2m respond to the query depending on the context
                             */
                            lwm2m_handle_packet(lwm2mContext, buffer, numBytes, connP);
                        } else {
                            /*
                             * This packet comes from an unknown peer
                             */
                            fprintf(stderr, "received bytes ignored!\r\n");
                        }
                    }
                }
            }
        }
        BlinkBlue();



        /*
         * Finally when the loop is left, we unregister our client from it
         */
        lwm2m_close(lwm2mContext); // handles deregistration


        if(q_reset){
            lwip_close(data.sock);
            connection_free(data.connList);
            free_security_object(objArray[0]);
            clean_server_object(objArray[1]);
            free_object_device(objArray[2]);
            free_test_object(objArray[3]);
            fprintf(stdout, "\n\t RESET\r\n");
            q_reset = 0;
            NVIC_SystemReset(); // we assume we never end wakaama unless we want to restart
        }
    }
    vTaskDelete(NULL);
}

void * lwm2m_connect_server(uint16_t secObjInstID,
        void * userData) {
    client_data_t * dataP;
    char * uri;
    char * host;
    char * port;

    connection_t * newConnP = NULL;

    dataP = (client_data_t *) userData;

    uri = get_server_uri(dataP->securityObjP, secObjInstID);


    if (uri == NULL) return NULL;

    printf("Connecting to %s instance: %d\r\n", uri, secObjInstID);

    // parse uri in the form "coaps://[host]:[port]"
    if (0 == strncmp(uri, "coaps://", strlen("coaps://"))) {
        host = uri + strlen("coaps://");
    } else if (0 == strncmp(uri, "coap://", strlen("coap://"))) {
        host = uri + strlen("coap://");
    } else {
        goto exit;
    }
    port = strrchr(host, ':');
    if (port == NULL) goto exit;
    // remove brackets
    if (host[0] == '[') {
        host++;
        if (*(port - 1) == ']') {
            *(port - 1) = 0;
        } else goto exit;
    }
    // split strings
    *port = 0;
    port++;
    newConnP = connection_create(dataP->connList, dataP->sock, host, port, dataP->addressFamily);
    if (newConnP == NULL) {
        printf("Connection creation failed.\r\n");
    } else {
        dataP->connList = newConnP;
    }

exit:
    lwm2m_free(uri);
    return (void *) newConnP;
}

void lwm2m_close_connection(void * sessionH,
        void * userData) {
    client_data_t * app_data;
    connection_t * targetP;

    app_data = (client_data_t *) userData;
    targetP = (connection_t *) sessionH;

    if (targetP == app_data->connList) {
        app_data->connList = targetP->next;
        lwm2m_free(targetP);
    } else {
        connection_t * parentP;

        parentP = app_data->connList;
        while (parentP != NULL && parentP->next != targetP) {
            parentP = parentP->next;
        }
        if (parentP != NULL) {
            parentP->next = targetP->next;
            lwm2m_free(targetP);
        }
    }
}


int __io_putchar(int ch)
{
 uint8_t c[1];
 c[1] = '\0';
 c[0] = ch & 0x00FF;
 if(c[0] == '\n') {
   __io_putchar('\r');
 }
 HAL_UART_Transmit(&huart3, &*c, 1, 10);
 return ch;
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();

  /* init code for USB_HOST */
  MX_USB_HOST_Init();

  /* init code for FATFS */
  MX_FATFS_Init();

  /* USER CODE BEGIN 5 */
  printf("\n\n\n-----------------------START-------------------------\n\n");
  osDelay(3000); // wait for DHCP initialisation
// Initialize Wakaama LWM2M Client
  lwip_socket_init();
  int socket = getUDPSocket(LWM2M_PORT, AF_INET);
  if(socket != -1){ 
    printf("Start wakaama\r\n");
    int res = xTaskCreate(taskWakaama, "wakaama", 3000, (void *) socket, 2, NULL);
    if (res != pdPASS) {
      printf("\r\nerror creating taskWakaama: xTaskCreate returned %d\n", res);  
    }
  } else {
    printf("Error creating socket: ");
  }

  // TODO: We cannot wait actively for user input since it kills the connection
  int x = 0;
  osDelay(500);
  HAL_GPIO_WritePin(USB_GPIO_OUT_GPIO_Port, USB_GPIO_OUT_Pin, GPIO_PIN_SET);
  BlinkBlue();
  char usrInput[2];
  /* Infinite loop */
  debug_init(&huart3);
  for(;;)
  {
    osDelay(5);
    printf("in loop %d\n\r", x);
    x++;
    printf("$ ");
    fflush(stdout);
    get_line(usrInput, 2);
    printf("%s\n", usrInput);
    switch(usrInput[0]) {
      case 'l':
        if (get_usb_ready()) {
          usb_ls();
        }
        break;
      case 'w':
        if (get_usb_ready()) {
          usb_write("asd", 3);
        }
        break;
    }
  }
  /* USER CODE END 5 */ 
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
