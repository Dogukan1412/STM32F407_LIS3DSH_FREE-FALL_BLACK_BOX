/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "BMP180.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define WHO_AM_I		(0x0F)
#define CTRL_REG1		(0x21)
#define CTRL_REG2		(0x22)
#define CTRL_REG3		(0x23)
#define CTRL_REG4		(0x20)
#define CTRL_REG5		(0x24)
#define CTRL_REG6		(0x25)
#define TIM1_1L			(0x55)
#define TIM1_1H			(0x54)
#define TIM1_2H			(0x74)
#define TIM1_2L			(0x75)
#define THRS2_1			(0x56)
#define THRS1_1			(0x57)
#define THRS2_2			(0x76)
#define THRS1_2			(0x77)
#define MASK1_B			(0x59)
#define MASK2_B			(0x79)
#define MASK1_A			(0x5A)
#define MASK2_A			(0x7A)
#define SETT1			(0x5B)
#define SETT2			(0x7B)
#define ST1_1			(0x40)
#define ST1_2			(0x41)
#define ST1_3			(0x42)
#define ST2_1			(0x60)
#define ST2_2			(0x61)
#define ST2_3			(0x62)
#define x_address		(0x29)
#define y_address		(0x2B)
#define z_address		(0x2D)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

FATFS fatfs;
FRESULT result;
FIL file_pointer;

float Temperature = 0;
float Pressure = 0;
float Altitude = 0;

uint16_t sd_buffer[20];

char buffer[100];
char okunan_karakter;

int8_t x = 0, y = 0, z = 0;
uint16_t timer_count = 0;

int flag_control = 0, timer_flag = 0, count_interrupt = 0, crash_flag = 0;

char time_minute, time_second, time_hour;
char date_week, date_month, date_year;

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

void lis_init(void);
void WriteSpi(uint8_t addres, uint8_t data);
uint8_t ReadSpi(uint8_t addres);
void Read_all(void);
uint8_t lis_convert_time(float miliseconds);
uint8_t lis_convert_threshold(float miliG);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  __HAL_SPI_ENABLE(&hspi1); // Enable spi 1.

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_FATFS_Init();
  MX_RTC_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  lis_init();
  HAL_TIM_Base_Start_IT(&htim2);
  BMP180_Start();
  HAL_Delay(40);



  result = f_mount(&fatfs, "", 0);			// Bu kısmı her seferinde bi önceki verileri silip yeniden dosya acmak icin kullanıyoruz
  if(result != FR_OK)
  {
	  while(1);
  }

  result = f_open(&file_pointer, "DATA.txt", FA_OPEN_APPEND);
  if(result == FR_OK)
  {


    if(result == FR_OK) __NOP();
    else __NOP();
   }
   else
   {
     __NOP();
    }
   f_close(&file_pointer);


   result = f_mount(&fatfs, "", 0);			// Bu kısmı her seferinde bi önceki verileri silip yeniden dosya acmak icin kullanıyoruz
   if(result != FR_OK)
   {
 	  while(1);
   }

   result = f_open(&file_pointer, "CRASH.txt", FA_OPEN_APPEND);
   if(result == FR_OK)
   {


     if(result == FR_OK) __NOP();
     else __NOP();
    }
    else
    {
      __NOP();
     }
    f_close(&file_pointer);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  timer_count = TIM2->CNT;

	  if(flag_control == 1 && timer_flag == 1)
	  {
		  //__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
		  crash_flag = 1;
		  Read_all();
		  HAL_Delay(30);
		  while(1);
	  }
	  else
	  {
		  if(count_interrupt == 5)
		  {
			  Read_all();
			  count_interrupt = 0;
		  }
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x12;
  sTime.Minutes = 0x16;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
  sDate.Month = RTC_MONTH_MAY;
  sDate.Date = 0x05;
  sDate.Year = 0x23;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 41999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(lis_cs_GPIO_Port, lis_cs_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(sd_cs_GPIO_Port, sd_cs_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : lis_cs_Pin */
  GPIO_InitStruct.Pin = lis_cs_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(lis_cs_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : sd_cs_Pin */
  GPIO_InitStruct.Pin = sd_cs_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(sd_cs_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);


}

/* USER CODE BEGIN 4 */


void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */
	  flag_control = 1;

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */


  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

	timer_flag = 1;
	count_interrupt++;

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}


void lis_init(void)
{

	if(ReadSpi(WHO_AM_I) == 0x3F)
	{
		ReadSpi(0x5F);		// interrupt sıfırlama
	}


	HAL_Delay(50);
	if(ReadSpi(WHO_AM_I) == 0x3F)
	{
		//FREE-FALL CONFIG
		WriteSpi(CTRL_REG4, 0x77);
		WriteSpi(CTRL_REG3, 0x78);							//48
		WriteSpi(TIM1_1L, lis_convert_time(100));			//28-->100ms,  0x04-->10ms  TIM1_H
		WriteSpi(TIM1_1H, lis_convert_time(100));			//28-->100ms,  0x04-->10ms  TIM1_L
		WriteSpi(THRS2_1, lis_convert_threshold(375));		//06-->100mg, 18-->375mg
		//WriteSpi(THRS1_1, 0x06);							//06-->100mg, 18-->375mg
		WriteSpi(MASK1_B, 0xA8);
		WriteSpi(MASK1_A, 0xA8);
		WriteSpi(SETT1, 0xA3);
		WriteSpi(ST1_1, 0x0A);
		WriteSpi(ST1_2, 0x06);   							//61,  06
		WriteSpi(ST1_3, 0x11);
		WriteSpi(CTRL_REG5, 0x00);
		WriteSpi(CTRL_REG1, 0x01);

		ReadSpi(0x5F);										// reset interrupt flag
	}


}

void WriteSpi(uint8_t addres, uint8_t data)
{

	HAL_GPIO_WritePin(lis_cs_GPIO_Port, lis_cs_Pin, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi1, &addres, 1, 100);

	HAL_SPI_Transmit(&hspi1, &data, 1, 100);

	HAL_GPIO_WritePin(lis_cs_GPIO_Port, lis_cs_Pin, GPIO_PIN_SET);

}


uint8_t ReadSpi(uint8_t addres)
{

	uint8_t buffer;

	if(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_READY)
	{
		if(flag_control == 1)
		{
			addres = addres;
		}
		else if (flag_control == 0)
		{
			addres = addres | 0x80;		// addresin basina 1 yazarak okuma yapcagimizi soyluyoruz
		}

		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);

		HAL_SPI_Transmit(&hspi1, &addres, 1, 100);

		HAL_SPI_Receive(&hspi1, &buffer, 1, 100);

		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
	}

	return buffer;

}


void Read_all(void)
{
	  x = ReadSpi(x_address);

	  y = ReadSpi(y_address);

	  z = ReadSpi(z_address);

	  Temperature = BMP180_GetTemp();

	  Pressure = BMP180_GetPress (0);

	  Altitude = BMP180_GetAlt(0);


	  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	  time_second = sTime.Seconds;
	  time_minute = sTime.Minutes;
	  time_hour = sTime.Hours;
	  date_week = sDate.WeekDay;
	  date_month = sDate.Month;
	  date_year = sDate.Year;


	  if(crash_flag == 1)
	  {
		  result = f_open(&file_pointer, "CRASH.txt", FA_OPEN_APPEND | FA_WRITE);			// Bu kısmı her seferinde bi önceki verileri silip yeniden dosya acmak icin kullanıyoruz
		  //result = f_open(&file_pointer, "CRASH.txt", FA_CREATE_ALWAYS | FA_WRITE);
		  if(result == FR_OK)
		  {
			 f_printf(&file_pointer, "\n");
			 f_printf(&file_pointer, "==================================\n");
			 f_printf(&file_pointer, "============CRASH DATA============\n");
			 f_printf(&file_pointer, "==================================\n");
			 f_printf(&file_pointer, "Clock : ");
			 f_printf(&file_pointer, "%d:%d:%d",(int16_t)time_second,(int16_t)time_minute,(int16_t)time_hour);
			 f_printf(&file_pointer, "\n");

			 f_printf(&file_pointer, "Date : ");
			 f_printf(&file_pointer, "%d.%d.%d",(int16_t)date_week,(int16_t)date_month,(int16_t)date_year);
			 f_printf(&file_pointer, "\n");

			 f_printf(&file_pointer, "Pressure = ");
			 f_printf(&file_pointer, "%d",(int32_t)Pressure);
			 f_printf(&file_pointer, " Pa");
			 f_printf(&file_pointer, "\n");
			 f_printf(&file_pointer, "Temperature = ");
			 f_printf(&file_pointer, "%d",(int16_t)Temperature);
			 f_printf(&file_pointer, "°C");
			 f_printf(&file_pointer, "\n");
			 f_printf(&file_pointer, "\n");




		  	 if(result == FR_OK) __NOP();
		  	 else __NOP();
		   }
		   HAL_Delay(20);
		   f_close(&file_pointer);
	  }

	  else
	  {
		  result = f_open(&file_pointer, "DATA.txt", FA_OPEN_APPEND | FA_WRITE);			// Bu kısmı her seferinde bi önceki verileri silip yeniden dosya acmak icin kullanıyoruz
		  //result = f_open(&file_pointer, "DATA.txt", FA_CREATE_ALWAYS | FA_WRITE);
		  if(result == FR_OK)
		  {
			 f_printf(&file_pointer, "\n");
			 f_printf(&file_pointer, "==================================\n");
			 f_printf(&file_pointer, "===========NORMAL DATA============\n");
			 f_printf(&file_pointer, "==================================\n");
			 f_printf(&file_pointer, "Clock : ");
			 f_printf(&file_pointer, "%d:%d:%d",(int16_t)time_second,(int16_t)time_minute,(int16_t)time_hour);
			 f_printf(&file_pointer, "\n");

			 f_printf(&file_pointer, "Date : ");
			 f_printf(&file_pointer, "%d.%d.%d",(int16_t)date_week,(int16_t)date_month,(int16_t)date_year);
			 f_printf(&file_pointer, "\n");

			 f_printf(&file_pointer, "Pressure = ");
			 f_printf(&file_pointer, "%d",(int32_t)Pressure);
			 f_printf(&file_pointer, " Pa");
			 f_printf(&file_pointer, "\n");
			 f_printf(&file_pointer, "Temperature = ");
			 f_printf(&file_pointer, "%d",(int16_t)Temperature);
			 f_printf(&file_pointer, "°C");
			 f_printf(&file_pointer, "\n");
			 f_printf(&file_pointer, "\n");


		  	 if(result == FR_OK) __NOP();
		  	 else __NOP();
		   }
		   HAL_Delay(20);
		   f_close(&file_pointer);
	  }

}


uint8_t lis_convert_time(float miliseconds)
{
	// 1 LSB = 1/ODR = 1/400 Hz
	// The minimum duration (in milliseconds) of subthreshold accelerations for recognize a free-fall condition.
	// Allowed range [2,5 - 637,5]ms.

	float var = miliseconds / (2.5);
	int byte = (int)var;
	if(byte < 0)
		return 0;
	else if(byte > 255)
		return 255;
	else
		return (uint8_t)byte;
}

uint8_t lis_convert_threshold(float miliG)
{
	// 1 LSB = 2g/(2^7)
	// The maximum acceleration (in milli-g) that is recognized as free-fall condition.
	// The lower is the threshold, more accurate is the recognition.
	// Allowed range [15,625 - 3984]mg.

	float var = miliG / (15.625);
	int byte = (int)var;
	if(byte < 0)
		return 0;
	else if(byte > 255)
		return 255;
	else
		return (uint8_t)byte;
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
