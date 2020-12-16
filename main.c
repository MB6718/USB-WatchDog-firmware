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
void CLK_config(void);
void GPIO_config(void);
void TIMERS_config(void);
void UART_config(uint32_t baud_rate);

void write_to_eeprom(uint32_t address, uint8_t data);
void delay_ms(uint32_t ms);

void hard_reset(void);
void soft_reset(void);
void power_off(void);

/* ������� ��������� ---------------------------------------------------------*/
#define soft_reset_cmd (uint8_t)0xFF
#define hard_reset_cmd (uint8_t)0xFE
#define power_off_cmd (uint8_t)0xFD
#define min_time_cmd (uint8_t)0x01
#define max_time_cmd (uint8_t)0x7F
#define hello_cmd (uint8_t)0x80
#define check_device_cmd (uint8_t)0x81
#define get_device_version_cmd (uint8_t)0x88
#define soft_mode_cmd (uint8_t)0xA0
#define hard_mode_cmd (uint8_t)0xA1
#define power_off_mode_cmd (uint8_t)0xA2
#define accept_cmd (uint8_t)0xAA
#define null_cmd (uint8_t)0x00

/* ��������� � ��������������� -----------------------------------------------*/
#define FW_VER (uint8_t)0x01 // ������ �������� 0.1

EEPROM uint8_t eeprom_time_limit @0x4000;
EEPROM uint8_t eeprom_reset_mode @0x4001;

/* ���������� ���������� -----------------------------------------------------*/
bool reset_flag = FALSE;
bool busy_flag = FALSE;
enum reset_mode {soft_mode, hard_mode, power_off_mode} mode;

uint8_t	command;
uint16_t time_limit = (uint32_t)180; // time in sec (default: 180 sec)
volatile uint16_t time_counter = 0;
volatile uint32_t delay_counter = 0;

main() {
	CLK_config();
	GPIO_config();
	UART_config(9600);
	
	/* ���������� ���������� �� ���� � ��� */
	time_limit = (uint16_t)(eeprom_time_limit * 10);
	mode = eeprom_reset_mode - soft_mode_cmd;

	TIMERS_config();
	enableInterrupts();
	
	delay_ms(1000);
	soft_reset();
	
	while (1) {
		if (reset_flag) {
			if (mode == soft_mode)
				soft_reset();
			if (mode == hard_mode)
				hard_reset();
			if (mode == power_off_mode)
				power_off();
			time_counter = 0;
			TIM2_Cmd(ENABLE);
		} else {
			if (UART1_GetFlagStatus(UART1_FLAG_RXNE)) {
				command = UART1_ReceiveData8();
				if (command == soft_reset_cmd) {
					UART1_SendData8(command);
					mode = soft_mode;
					reset_flag = TRUE;
				}
				if (command == hard_reset_cmd) {
					UART1_SendData8(command);
					mode = hard_mode;
					reset_flag = TRUE;
				}
				if (command == power_off_cmd) {
					UART1_SendData8(command);
					mode = power_off_mode;
					reset_flag = TRUE;
				}
				if (command >= min_time_cmd && command <= max_time_cmd) {
					time_limit = (uint32_t)(command * 10);
					if (eeprom_time_limit != command)
						write_to_eeprom((uint32_t)&eeprom_time_limit, command);
					UART1_SendData8(accept_cmd);
				}
				if (command == soft_mode_cmd || command == hard_mode_cmd || \
				    command == power_off_mode_cmd) {
					mode = command - soft_mode_cmd;
					if (eeprom_reset_mode != command)
						write_to_eeprom((uint32_t)&eeprom_reset_mode, command);
					UART1_SendData8(accept_cmd);
				}
				if (command == get_device_version_cmd) {
					UART1_SendData8(FW_VER);
				}
				if (command == check_device_cmd) {
					UART1_SendData8(hello_cmd);
				}
				if (command == accept_cmd) {
					UART1_SendData8(accept_cmd);
				}
				if (command >= hello_cmd && command != check_device_cmd && \
						command != hard_mode_cmd && command != soft_mode_cmd && \
						command != hard_reset_cmd && command != soft_reset_cmd && \
						command != power_off_mode_cmd && command != power_off_cmd && \
						command != accept_cmd && command != accept_cmd - 1 && \
						command != get_device_version_cmd) {
					UART1_SendData8(command + 1);
				}
				if (command != null_cmd) {
					GPIO_WriteReverse(IND_PORT, IND);
					delay_ms(150);
					GPIO_WriteReverse(IND_PORT, IND);
					time_counter = 0;
				}
			}
		}
	}
}

/* ���������������� ������������ �� */
void CLK_config(void) {
	// ������������ �� ����������� ���������� fMaster = 16Mhz
	CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
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

/* ���������������� UART ������ */
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

/* ���������������� ������� �������� TIM4 � TIM2 */
void TIMERS_config(void) {
	/* ������������� ������ TIM4 ��� �������� 1 ��� � 1�� (fMaster = 16Mhz) */
	TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	TIM4_Cmd(ENABLE);
	
	/* ������������� ������ TIM2 ��� �������� 1 ��� � 1� (fMaster = 16Mhz) */
	TIM2_TimeBaseInit(TIM2_PRESCALER_1024, 15624);
	TIM2_ClearFlag(TIM2_FLAG_UPDATE);
	TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
	TIM2_Cmd(ENABLE);
}

/* ������� "������ ������������" */
void hard_reset(void) {
	reset_flag = FALSE;
	TIM2_Cmd(DISABLE);
	if (!(busy_flag)) {
		busy_flag = TRUE;
		GPIO_WriteHigh(RELAY_PWR_PORT, RELAY_PWR);
		GPIO_WriteReverse(IND_PORT, IND);
		delay_ms(20000);
		GPIO_WriteLow(RELAY_PWR_PORT, RELAY_PWR);
		GPIO_WriteReverse(IND_PORT, IND);
		delay_ms(1500);
		GPIO_WriteHigh(RELAY_PWR_PORT, RELAY_PWR);
		delay_ms(1000);
		GPIO_WriteLow(RELAY_PWR_PORT, RELAY_PWR);
		delay_ms(1500);
		GPIO_WriteLow(RELAY_RST_PORT, RELAY_RST);
		delay_ms(1000);
		GPIO_WriteHigh(RELAY_RST_PORT, RELAY_RST);
		busy_flag = FALSE;
	}
}

/* ������� "������ ������������" */
void soft_reset(void) {
	reset_flag = FALSE;
	if (!(busy_flag)) {
		busy_flag = TRUE;
		GPIO_WriteLow(RELAY_RST_PORT, RELAY_RST);
		GPIO_WriteReverse(IND_PORT, IND);
		delay_ms(1000);
		GPIO_WriteHigh(RELAY_RST_PORT, RELAY_RST);
		GPIO_WriteReverse(IND_PORT, IND);
		busy_flag = FALSE;
	}
}

/* ������� "���������� �������" */
void power_off(void) {
	reset_flag = FALSE;
	if (!(busy_flag)) {
		busy_flag = TRUE;
		GPIO_WriteHigh(RELAY_PWR_PORT, RELAY_PWR);
		GPIO_WriteReverse(IND_PORT, IND);
		delay_ms(150);
		GPIO_WriteReverse(IND_PORT, IND);
		busy_flag = FALSE;
	}
}

/* ���������� ���������� ������� TIM4 */
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23) {
	if (TIM4_GetITStatus(TIM4_IT_UPDATE) == SET) {
		if (delay_counter)
			delay_counter--;
		TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
	}
}

/* ���������� ���������� ������� TIM2 */
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

/* ������� ������ � ���� ������ ����� �� ������ */
void write_to_eeprom(uint32_t address, uint8_t data) {
		if (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
			FLASH_Unlock(FLASH_MEMTYPE_DATA);
		while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);
		FLASH_ProgramByte(address, data);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
}

/* ������� �������� �� ���������� ������� TIM4 */
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
