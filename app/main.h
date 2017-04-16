/**
 * unsigned char  uint8_t
 * unsigned short uint16_t
 * unsigned int   uint32_t
 */

#define CLOCK_SPEED 16000000
#define USART_BAUD_RATE 115200
#define TIMER3_PERIOD_TICKS (unsigned int)(CLOCK_SPEED * 15 / USART_BAUD_RATE)
#define TIMER3_SEC_PER_PERIOD ((float)TIMER3_PERIOD_TICKS / CLOCK_SPEED)
#define TIMER3_MS_PER_PERIOD ((float)TIMER3_PERIOD_TICKS * 1000 / CLOCK_SPEED)
#define TIMER14_PERIOD 24
#define TIMER14_PRESCALER 0xFFFF
#define TIMER14_TACTS_PER_SECOND (CLOCK_SPEED / TIMER14_PERIOD / TIMER14_PRESCALER)

#define USART1_TX_DMA_CHANNEL DMA1_Channel2
#define USART1_TDR_ADDRESS (unsigned int)(&(USART1->TDR))

// General flags
#define USART_DATA_RECEIVED_FLAG    1

#define USART_DATA_RECEIVED_BUFFER_SIZE 50

#define TIMER3_10MS (unsigned short)(10 / TIMER3_MS_PER_PERIOD)
#define TIMER3_100MS (unsigned short)(100 / TIMER3_MS_PER_PERIOD)
#define TIMER14_100MS 1
#define TIMER14_200MS 2
#define TIMER14_500MS 5
#define TIMER14_1S 10
#define TIMER14_2S 20
#define TIMER14_3S 30
#define TIMER14_5S 50
#define TIMER14_10S 102
#define TIMER14_30S 305
#define TIMER14_60S 610
#define TIMER14_10MIN 6103

#define FAN_SWITCH_PIN GPIO_Pin_4
#define FAN_SWITCH_PORT GPIOA
#define FAN_SWITCH_EXTI_PIN_SOURCE EXTI_PinSource4
#define FAN_SWITCH_EXTI_PORT_SOURCE EXTI_PortSourceGPIOA
#define FAN_SWITCH_EXTI_LINE EXTI_Line4
#define FAN_SWITCH_NVIC_IRQChannel EXTI4_15_IRQn

void IWDG_Config();
void Clock_Config();
void Pins_Config();
void TIMER3_Confing();
void TIMER14_Confing();
void init_pin_as_output(GPIO_TypeDef* GPIOx, unsigned int pin);
void DMA_Config();
void USART_Config();
void EXTERNAL_Interrupt_Config();
void disable_echo();
void current_ap_connection_parameters();
void send_usard_data(char string[]);
unsigned char is_usart_response_contains_elements(char *data_to_be_contained[], unsigned char elements_count);
unsigned char is_usart_response_contains_element(char string_to_be_contained[]);
void clear_usart_data_received_buffer();
unsigned short get_received_data_length();
unsigned char is_received_data_length_equal(unsigned short length);
void set_flag(unsigned int *flags, unsigned int flag_value);
void reset_flag(unsigned int *flags, unsigned int flag_value);
unsigned char read_flag(unsigned int flags, unsigned int flag_value);
unsigned char contains_string(char being_compared_string[], char string_to_be_contained[]);
unsigned short get_string_length(char string[]);
