/*
  ******************************************************************************
  * @Файл			main.c
  * @Автор		MB6718
  * @Версия	 	V0.1
  * @Дата			12-Декабря-2020
  * @Описание	Программа для управления неким китайским поделием
							под загадочным именем "Сторожевой Пёс".
	******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* определяем порты и пины для светодиодов индикации -------------------------*/
#define LED_PORT GPIOB
#define LED GPIO_PIN_5

#define IND_PORT GPIOA
#define IND GPIO_PIN_2

/* определяем порты и пины для каналов реле ----------------------------------*/
#define RELAY_RST_PORT GPIOD
#define RELAY_RST GPIO_PIN_4

#define RELAY_PWR_PORT GPIOC
#define RELAY_PWR GPIO_PIN_3

/* прототипы функций ---------------------------------------------------------*/
void CLK_config(void);
void GPIO_config(void);
void TIMERS_config(void);
void UART_config(uint32_t baud_rate);

void delay_ms(uint32_t ms);

/* команды протокола ---------------------------------------------------------*/
#define relay_on_cmd (uint8_t)0xFF
#define relay_off_cmd (uint8_t)0xFE
#define null_cmd (uint8_t)0x00

/* глобальные переменные -----------------------------------------------------*/
bool reset_flag = FALSE;

uint8_t	command;
uint16_t time_limit = (uint32_t)180; // time(sec) [default: 180 sec]
volatile uint16_t time_counter = 0;
volatile uint32_t delay_counter = 0;

main() {
	CLK_config();
	GPIO_config();
	UART_config(9600);

	TIMERS_config();
	enableInterrupts();
	
	time_limit = 10;
	
	while (1) { // (!) for debug delay
		if (reset_flag) {
			time_counter = 0;
			TIM2_Cmd(ENABLE);
			GPIO_WriteReverse(LED_PORT, LED);
			GPIO_WriteReverse(IND_PORT, IND);
			reset_flag = FALSE;
		} else {
			if (UART1_GetFlagStatus(UART1_FLAG_RXNE)) {
				command = UART1_ReceiveData8();
				if (command == relay_off_cmd) {
					UART1_SendData8(command);
					GPIO_WriteHigh(RELAY_RST_PORT, RELAY_RST);
				}
				if (command == relay_on_cmd) {
					UART1_SendData8(command);
					GPIO_WriteLow(RELAY_RST_PORT, RELAY_RST);
				}
				if (command != null_cmd) {
					GPIO_WriteReverse(IND_PORT, IND);
					delay_ms(150);
					GPIO_WriteReverse(IND_PORT, IND);
				}
				time_counter = 0;
			}
		}
	}
}

/* конфигурирование тактирования МК */
void CLK_config(void) {
	// тактирование от внутреннего генератора fMaster = 16Mhz
	CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
}

/* конфигурирование выводов GPIO */
void GPIO_config(void) {
	GPIO_Init(
		LED_PORT,
		LED, // GPIO_PIN_ALL
		GPIO_MODE_OUT_PP_LOW_FAST
	);
	GPIO_Init(
		IND_PORT,
		IND,
		GPIO_MODE_OUT_PP_HIGH_FAST
	);
	GPIO_Init(
		RELAY_RST_PORT,
		RELAY_RST,
		GPIO_MODE_OUT_PP_HIGH_FAST
	);
	GPIO_Init(
		RELAY_PWR_PORT,
		RELAY_PWR,
		GPIO_MODE_OUT_PP_LOW_FAST
	);
}

/* конфигурирование UART модуля */
void UART_config(uint32_t baud_rate) {
	UART1_Init(
		baud_rate, // (uint32_t)9600,
		UART1_WORDLENGTH_8D,
		UART1_STOPBITS_1,
		UART1_PARITY_NO,
		UART1_SYNCMODE_CLOCK_DISABLE,
		UART1_MODE_TXRX_ENABLE
	);
	UART1_Cmd(ENABLE);
}

/* конфигурирование режимов таймеров TIM4 и TIM2 */
void TIMERS_config(void) {
	/* конфигурируем таймер TIM4 для сработки 1 раз в 1мс (fMaster = 16Mhz) */
	TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	TIM4_Cmd(ENABLE);
	
	/* конфигурируем таймер TIM2 для сработки 1 раз в 1с (fMaster = 16Mhz) */
	TIM2_TimeBaseInit(TIM2_PRESCALER_1024, 15624);
	TIM2_ClearFlag(TIM2_FLAG_UPDATE);
	TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
	TIM2_Cmd(ENABLE);
}

/* Обработчик прерывания таймера TIM4 */
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23) {
	if (TIM4_GetITStatus(TIM4_IT_UPDATE) == SET) {
		if (delay_counter)
			delay_counter--;
		TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
	}
}

/* Обработчик прерывания таймера TIM2 */
INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13) {
	if (TIM2_GetITStatus(TIM2_IT_UPDATE) == SET) {
		if (time_counter < time_limit) {
			time_counter++;
			GPIO_WriteReverse(LED_PORT, LED);
		}	else {
			TIM2_Cmd(DISABLE);
			//time_counter = 0;
			reset_flag = TRUE;
		}
		TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
	}
}

/* функция задержки по прерыванию таймера TIM4 */
void delay_ms(uint32_t ms) {
	delay_counter = ms;
	TIM4_SetCounter(0);
	while (delay_counter);
}

/* */
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
	while (1) {	}
}
#endif
