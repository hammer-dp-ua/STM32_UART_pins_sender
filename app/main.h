/**
 * unsigned char  uint8_t
 * unsigned short uint16_t
 * unsigned int   uint32_t
 */

#define CLOCK_SPEED 16000000
#define USART_BAUD_RATE 115200
#define TIMER3_PERIOD_TICKS (unsigned int) (CLOCK_SPEED * 15 / USART_BAUD_RATE)
#define TIMER3_SEC_PER_PERIOD ((float) TIMER3_PERIOD_TICKS / CLOCK_SPEED)
#define TIMER3_MS_PER_PERIOD ((float) TIMER3_PERIOD_TICKS * 1000 / CLOCK_SPEED)
#define TIMER14_PERIOD 24
#define TIMER14_PRESCALER 0xFFFF
#define TIMER14_TACTS_PER_SECOND (CLOCK_SPEED / TIMER14_PERIOD / TIMER14_PRESCALER)

#define USART1_TX_DMA_CHANNEL DMA1_Channel2
#define USART1_TDR_ADDRESS (unsigned int)(&(USART1->TDR))

// General flags
#define USART_DATA_RECEIVED_FLAG          1
#define USART_TRANSFER_COMPLETE_FLAG      2

// Pins interrupts flags
#define MOTION_SENSOR_1_PIN_INTERRUPT_FLAG      1
#define MOTION_SENSOR_3_PIN_INTERRUPT_FLAG      2
#define IMMOBILIZER_LED_PIN_INTERRUPT_FLAG      4
#define MOTION_SENSOR_2_PIN_INTERRUPT_FLAG      8
#define PIR_LED_1_PIN_INTERRUPT_FLAG            16
#define MW_LED_1_PIN_INTERRUPT_FLAG             32
#define PIR_LED_3_PIN_INTERRUPT_FLAG            64
#define MW_LED_3_PIN_INTERRUPT_FLAG             128

#define USART_DATA_RECEIVED_BUFFER_SIZE 50

#define TIMER3_10MS (unsigned short) (10 / TIMER3_MS_PER_PERIOD)
#define TIMER3_100MS (unsigned short) (100 / TIMER3_MS_PER_PERIOD)
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

#define USART_TX_PIN GPIO_Pin_9
#define USART_TX_PORT GPIOA
#define IMMOBILIZER_LED_PORT GPIOA
#define IMMOBILIZER_LED_PIN GPIO_Pin_3
#define IMMOBILIZER_LED_EXTI_PIN_SOURCE EXTI_PinSource3
#define IMMOBILIZER_LED_EXTI_PORT_SOURCE EXTI_PortSourceGPIOA
#define IMMOBILIZER_LED_EXTI_LINE EXTI_Line3
#define IMMOBILIZER_LED_NVIC_IRQChannel EXTI2_3_IRQn
// The first motion sensor from the street
#define MOTION_SENSOR_1_PIN GPIO_Pin_0 // White
#define MOTION_SENSOR_1_PORT GPIOA
#define MOTION_SENSOR_1_EXTI_PIN_SOURCE EXTI_PinSource0
#define MOTION_SENSOR_1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOA
#define MOTION_SENSOR_1_EXTI_LINE EXTI_Line0
#define MOTION_SENSOR_1_NVIC_IRQChannel EXTI0_1_IRQn
#define PIR_LED_1_PIN GPIO_Pin_8 // Green
#define PIR_LED_1_PORT GPIOA
#define PIR_LED_1_EXTI_PIN_SOURCE EXTI_PinSource8
#define PIR_LED_1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOA
#define PIR_LED_1_EXTI_LINE EXTI_Line8
#define PIR_LED_1_NVIC_IRQChannel EXTI4_15_IRQn
#define MW_LED_1_PIN GPIO_Pin_11 // Yellow
#define MW_LED_1_PORT GPIOA
#define MW_LED_1_EXTI_PIN_SOURCE EXTI_PinSource11
#define MW_LED_1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOA
#define MW_LED_1_EXTI_LINE EXTI_Line11
#define MW_LED_1_NVIC_IRQChannel EXTI4_15_IRQn
// Second motion sensor from the street. Without microwave detector
#define MOTION_SENSOR_2_PIN GPIO_Pin_2 // White
#define MOTION_SENSOR_2_PORT GPIOA
#define MOTION_SENSOR_2_EXTI_PIN_SOURCE EXTI_PinSource2
#define MOTION_SENSOR_2_EXTI_PORT_SOURCE EXTI_PortSourceGPIOA
#define MOTION_SENSOR_2_EXTI_LINE EXTI_Line2
#define MOTION_SENSOR_2_NVIC_IRQChannel EXTI2_3_IRQn
// Third motion sensor from the street
#define MOTION_SENSOR_3_PIN GPIO_Pin_1 // White
#define MOTION_SENSOR_3_PORT GPIOA
#define MOTION_SENSOR_3_EXTI_PIN_SOURCE EXTI_PinSource1
#define MOTION_SENSOR_3_EXTI_PORT_SOURCE EXTI_PortSourceGPIOA
#define MOTION_SENSOR_3_EXTI_LINE EXTI_Line1
#define MOTION_SENSOR_3_NVIC_IRQChannel EXTI0_1_IRQn
#define PIR_LED_3_PIN GPIO_Pin_12 // Green
#define PIR_LED_3_PORT GPIOA
#define PIR_LED_3_EXTI_PIN_SOURCE EXTI_PinSource12
#define PIR_LED_3_EXTI_PORT_SOURCE EXTI_PortSourceGPIOA
#define PIR_LED_3_EXTI_LINE EXTI_Line12
#define PIR_LED_3_NVIC_IRQChannel EXTI4_15_IRQn
#define MW_LED_3_PIN GPIO_Pin_15 // Yellow
#define MW_LED_3_PORT GPIOA
#define MW_LED_3_EXTI_PIN_SOURCE EXTI_PinSource15
#define MW_LED_3_EXTI_PORT_SOURCE EXTI_PortSourceGPIOA
#define MW_LED_3_EXTI_LINE EXTI_Line15
#define MW_LED_3_NVIC_IRQChannel EXTI4_15_IRQn


void iwdg_config();
void clock_config();
void pins_config();
void timer3_confing();
void timer14_confing();
void init_pin_as_output(GPIO_TypeDef* GPIOx, unsigned int pin);
void dma_config();
void usart_config();
void external_interrupt_config();
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
