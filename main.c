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
void delay_ms(uint32_t ms);

/* глобальные переменные -----------------------------------------------------*/
volatile uint32_t delay_counter = 0;

main() {
	CLK_config();
	GPIO_config();
	TIMERS_config();
	
	enableInterrupts();
	
	while (1) { // (!) for debug delay
		GPIO_WriteReverse(LED_PORT, LED);
		delay_ms(1000);
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

/* конфигурирование режимов таймеров TIM4 и TIM2 */
void TIMERS_config(void) {
	/* конфигурируем таймер TIM4 для сработки 1 раз в 1мс (fMaster = 16Mhz) */
	TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	TIM4_Cmd(ENABLE);
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
