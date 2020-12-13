/*
  ******************************************************************************
  * @����			main.c
  * @�����		MB6718
  * @������	 	V0.1
  * @����			12-�������-2020
  * @��������	��������� ��� ���������� ����� ��������� ��������
							��� ���������� ������ "���������� ϸ�".
	******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* ���������� ����� � ���� ��� ����������� ��������� -------------------------*/
#define LED_PORT GPIOB
#define LED GPIO_PIN_5

#define IND_PORT GPIOA
#define IND GPIO_PIN_2

/* ���������� ����� � ���� ��� ������� ���� ----------------------------------*/
#define RELAY_RST_PORT GPIOD
#define RELAY_RST GPIO_PIN_4

#define RELAY_PWR_PORT GPIOC
#define RELAY_PWR GPIO_PIN_3

/* ��������� ������� ---------------------------------------------------------*/
void GPIO_config(void);

main() {
	GPIO_config();
	
	while (1) {
		
	}
}

/* ���������������� ������� GPIO */
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

/* ���������� ���������� ������� TIM4 */
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23) {

}

/* ���������� ���������� ������� TIM2 */
INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13) {
	
}

/* */
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
	while (1) {	}
}
#endif
