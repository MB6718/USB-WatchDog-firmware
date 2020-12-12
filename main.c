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

main() {
	while (1);
}

/* Обработчик прерывания таймера TIM4 */
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23) {

}

/* Обработчик прерывания таймера TIM2 */
INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13) {
	
}

/* */
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
	while (1) {	}
}
#endif
