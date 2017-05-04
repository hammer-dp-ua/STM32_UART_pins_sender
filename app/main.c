#include "stm32f0xx.h"
#include "main.h"

unsigned int general_flags_g;
unsigned int pins_interrupts_flags_g;

volatile unsigned int send_usart_data_timer_g = TIMER14_10S;
volatile unsigned int pins_interrupts_timer_g;

volatile unsigned short usart_overrun_errors_counter_g;
volatile unsigned short usart_idle_line_detection_counter_g;
volatile unsigned short usart_noise_detection_counter_g;
volatile unsigned short usart_framing_errors_counter_g;

char usart_data_received_buffer_g[USART_DATA_RECEIVED_BUFFER_SIZE];
volatile unsigned short usart_received_bytes_g;

void SysTick_Handler() {
}

void DMA1_Channel2_3_IRQHandler() {
   DMA_ClearITPendingBit(DMA1_IT_TC2);
   set_flag(&general_flags_g, USART_TRANSFER_COMPLETE_FLAG);
}

void DMA1_Channel1_IRQHandler() {
   if (DMA_GetITStatus(DMA1_IT_TC1)) {
      DMA_ClearITPendingBit(DMA1_IT_TC1);
   }
}

void TIM14_IRQHandler() {
   TIM_ClearITPendingBit(TIM14, TIM_IT_Update);

   if (send_usart_data_timer_g) {
      send_usart_data_timer_g--;
   }
}

void TIM3_IRQHandler() {
   TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

   // Some error eventually occurs when only the first symbol exists
   if (usart_received_bytes_g > 1) {
      set_flag(&general_flags_g, USART_DATA_RECEIVED_FLAG);
   }
   if (pins_interrupts_timer_g) {
      pins_interrupts_timer_g--;
   }
   usart_received_bytes_g = 0;
}

void set_pin_interrupt_flag(unsigned int flag) {
   set_flag(&pins_interrupts_flags_g, flag);
   pins_interrupts_timer_g = TIMER3_100MS;
}

void EXTI0_1_IRQHandler() {
   if (EXTI_GetITStatus(MOTION_SENSOR_1_EXTI_LINE)) {
      EXTI_ClearITPendingBit(MOTION_SENSOR_1_EXTI_LINE);
      set_pin_interrupt_flag(MOTION_SENSOR_1_PIN_INTERRUPT_FLAG);
   } else if (EXTI_GetITStatus(MOTION_SENSOR_3_EXTI_LINE)) {
      EXTI_ClearITPendingBit(MOTION_SENSOR_3_EXTI_LINE);
      set_pin_interrupt_flag(MOTION_SENSOR_3_PIN_INTERRUPT_FLAG);
   }
}

void EXTI2_3_IRQHandler() {
   if (EXTI_GetITStatus(IMMOBILIZER_LED_EXTI_LINE)) {
      EXTI_ClearITPendingBit(IMMOBILIZER_LED_EXTI_LINE);
      set_pin_interrupt_flag(IMMOBILIZER_LED_PIN_INTERRUPT_FLAG);
   } else if (EXTI_GetITStatus(MOTION_SENSOR_2_EXTI_LINE)) {
      EXTI_ClearITPendingBit(MOTION_SENSOR_2_EXTI_LINE);
      set_pin_interrupt_flag(MOTION_SENSOR_2_PIN_INTERRUPT_FLAG);
   }
}

void EXTI4_15_IRQHandler() {
   if (EXTI_GetITStatus(PIR_LED_1_EXTI_LINE)) {
      EXTI_ClearITPendingBit(PIR_LED_1_EXTI_LINE);
      set_pin_interrupt_flag(PIR_LED_1_PIN_INTERRUPT_FLAG);
   } else if (EXTI_GetITStatus(MW_LED_1_EXTI_LINE)) {
      EXTI_ClearITPendingBit(MW_LED_1_EXTI_LINE);
      set_pin_interrupt_flag(MW_LED_1_PIN_INTERRUPT_FLAG);
   } else if (EXTI_GetITStatus(PIR_LED_3_EXTI_LINE)) {
      EXTI_ClearITPendingBit(PIR_LED_3_EXTI_LINE);
      set_pin_interrupt_flag(PIR_LED_3_PIN_INTERRUPT_FLAG);
   } else if (EXTI_GetITStatus(MW_LED_3_EXTI_LINE)) {
      EXTI_ClearITPendingBit(MW_LED_3_EXTI_LINE);
      set_pin_interrupt_flag(MW_LED_3_PIN_INTERRUPT_FLAG);
   }
}

void USART1_IRQHandler() {
   if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
      TIM_SetCounter(TIM3, 0);
      usart_data_received_buffer_g[usart_received_bytes_g] = USART_ReceiveData(USART1);
      usart_received_bytes_g++;

      if (usart_received_bytes_g >= USART_DATA_RECEIVED_BUFFER_SIZE) {
         usart_received_bytes_g = 0;
      }
   }

   if (USART_GetFlagStatus(USART1, USART_FLAG_ORE)) {
      USART_ClearITPendingBit(USART1, USART_IT_ORE);
      USART_ClearFlag(USART1, USART_FLAG_ORE);
      usart_overrun_errors_counter_g++;
   } else if (USART_GetFlagStatus(USART1, USART_FLAG_IDLE)) {
      USART_ClearITPendingBit(USART1, USART_IT_ORE);
      USART_ClearFlag(USART1, USART_FLAG_IDLE);
      usart_idle_line_detection_counter_g++;
   } else if (USART_GetFlagStatus(USART1, USART_FLAG_NE)) {
      USART_ClearITPendingBit(USART1, USART_IT_ORE);
      USART_ClearFlag(USART1, USART_FLAG_NE);
      usart_noise_detection_counter_g++;
   } else if (USART_GetFlagStatus(USART1, USART_FLAG_FE)) {
      USART_ClearITPendingBit(USART1, USART_IT_ORE);
      USART_ClearFlag(USART1, USART_FLAG_FE);
      usart_framing_errors_counter_g++;
   }
}

int main() {
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_DBGMCU, ENABLE);
   iwdg_config();
   clock_config();
   pins_config();
   external_interrupt_config();
   dma_config();
   usart_config();
   timer3_confing();
   timer14_confing();

   set_flag(&general_flags_g, USART_TRANSFER_COMPLETE_FLAG);

   while (1) {
      if (pins_interrupts_flags_g && !pins_interrupts_timer_g && read_flag(general_flags_g, USART_TRANSFER_COMPLETE_FLAG)) {
         if (read_flag(pins_interrupts_flags_g, MOTION_SENSOR_1_PIN_INTERRUPT_FLAG)) {
            reset_flag(&pins_interrupts_flags_g, MOTION_SENSOR_1_PIN_INTERRUPT_FLAG);

            if (GPIO_ReadInputDataBit(MOTION_SENSOR_1_PORT, MOTION_SENSOR_1_PIN)) {
               send_usard_data("pin:MOTION_SENSOR_1");
            }
         } else if (read_flag(pins_interrupts_flags_g, MOTION_SENSOR_3_PIN_INTERRUPT_FLAG)) {
            reset_flag(&pins_interrupts_flags_g, MOTION_SENSOR_3_PIN_INTERRUPT_FLAG);

            if (GPIO_ReadInputDataBit(MOTION_SENSOR_3_PORT, MOTION_SENSOR_3_PIN)) {
               send_usard_data("pin:MOTION_SENSOR_3");
            }
         } else if (read_flag(pins_interrupts_flags_g, IMMOBILIZER_LED_PIN_INTERRUPT_FLAG)) {
            reset_flag(&pins_interrupts_flags_g, IMMOBILIZER_LED_PIN_INTERRUPT_FLAG);

            if (!GPIO_ReadInputDataBit(IMMOBILIZER_LED_PORT, IMMOBILIZER_LED_PIN)) {
               send_usard_data("pin:IMMOBILIZER_LED");
            }
         } else if (read_flag(pins_interrupts_flags_g, MOTION_SENSOR_2_PIN_INTERRUPT_FLAG)) {
            reset_flag(&pins_interrupts_flags_g, MOTION_SENSOR_2_PIN_INTERRUPT_FLAG);

            if (GPIO_ReadInputDataBit(MOTION_SENSOR_2_PORT, MOTION_SENSOR_2_PIN)) {
               send_usard_data("pin:MOTION_SENSOR_2");
            }
         } else if (read_flag(pins_interrupts_flags_g, PIR_LED_1_PIN_INTERRUPT_FLAG)) {
            reset_flag(&pins_interrupts_flags_g, PIR_LED_1_PIN_INTERRUPT_FLAG);

            if (!GPIO_ReadInputDataBit(PIR_LED_1_PORT, PIR_LED_1_PIN)) {
               send_usard_data("pin:PIR_LED_1");
            }
         } else if (read_flag(pins_interrupts_flags_g, MW_LED_1_PIN_INTERRUPT_FLAG)) {
            reset_flag(&pins_interrupts_flags_g, MW_LED_1_PIN_INTERRUPT_FLAG);

            if (!GPIO_ReadInputDataBit(MW_LED_1_PORT, MW_LED_1_PIN)) {
               send_usard_data("pin:MW_LED_1");
            }
         } else if (read_flag(pins_interrupts_flags_g, PIR_LED_3_PIN_INTERRUPT_FLAG)) {
            reset_flag(&pins_interrupts_flags_g, PIR_LED_3_PIN_INTERRUPT_FLAG);

            if (!GPIO_ReadInputDataBit(PIR_LED_3_PORT, PIR_LED_3_PIN)) {
               send_usard_data("pin:PIR_LED_3");
            }
         } else if (read_flag(pins_interrupts_flags_g, MW_LED_3_PIN_INTERRUPT_FLAG)) {
            reset_flag(&pins_interrupts_flags_g, MW_LED_3_PIN_INTERRUPT_FLAG);

            if (!GPIO_ReadInputDataBit(MW_LED_3_PORT, MW_LED_3_PIN)) {
               send_usard_data("pin:MW_LED_3");
            }
         }
      }
   }
}

unsigned char is_usart_response_contains_element(char string_to_be_contained[]) {
   if (contains_string(usart_data_received_buffer_g, string_to_be_contained)) {
      return 1;
   } else {
      return 0;
   }
}

//char *data_to_be_contained[] = {ESP8226_REQUEST_DISABLE_ECHO, USART_OK};
unsigned char is_usart_response_contains_elements(char *data_to_be_contained[], unsigned char elements_count) {
   for (unsigned char elements_index = 0; elements_index < elements_count; elements_index++) {
      if (!contains_string(usart_data_received_buffer_g, data_to_be_contained[elements_index])) {
         return 0;
      }
   }
   return 1;
}

void iwdg_config() {
   DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP, ENABLE);

   IWDG_Enable();
   IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
   IWDG_SetPrescaler(IWDG_Prescaler_256);
   IWDG_SetReload(156); // 1 second
   while (IWDG_GetFlagStatus(IWDG_FLAG_PVU) == SET);
   while (IWDG_GetFlagStatus(IWDG_FLAG_RVU) == SET);
}

void clock_config() {
   RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
   RCC_PLLCmd(DISABLE);
   while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == SET);
   RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_4); // 8MHz / 2 * 4
   RCC_PCLKConfig(RCC_HCLK_Div1);
   RCC_PLLCmd(ENABLE);
   while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

void init_pin_as_output(GPIO_TypeDef* GPIOx, unsigned int pin) {
   GPIO_InitTypeDef GPIO_InitType;
   GPIO_InitType.GPIO_Pin = pin;
   GPIO_InitType.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitType.GPIO_Speed = GPIO_Speed_Level_1;
   GPIO_InitType.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitType.GPIO_OType = GPIO_OType_PP;
   GPIO_Init(GPIOx, &GPIO_InitType);
}

void pins_config() {
   // Connect BOOT0 directly to ground, RESET to VDD with a resistor

   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);

   GPIO_InitTypeDef ports_default_config;
   ports_default_config.GPIO_Pin = GPIO_Pin_All & ~(GPIO_Pin_13 | GPIO_Pin_14); // PA13, PA14 - Debugger pins
   ports_default_config.GPIO_Mode = GPIO_Mode_IN;
   ports_default_config.GPIO_Speed = GPIO_Speed_Level_1; // 2 MHz
   ports_default_config.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_Init(GPIOA, &ports_default_config);

   ports_default_config.GPIO_Pin = GPIO_Pin_All;
   GPIO_Init(GPIOB, &ports_default_config);

   GPIO_Init(GPIOF, &ports_default_config);

   GPIO_InitTypeDef motion_sensors_pir_and_mw_pins_config;
   motion_sensors_pir_and_mw_pins_config.GPIO_Pin = PIR_LED_1_PIN;
   motion_sensors_pir_and_mw_pins_config.GPIO_Mode = GPIO_Mode_IN;
   motion_sensors_pir_and_mw_pins_config.GPIO_Speed = GPIO_Speed_Level_1; // 2 MHz
   motion_sensors_pir_and_mw_pins_config.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_Init(PIR_LED_1_PORT, &motion_sensors_pir_and_mw_pins_config);
   motion_sensors_pir_and_mw_pins_config.GPIO_Pin = MW_LED_1_PIN;
   GPIO_Init(MW_LED_1_PORT, &motion_sensors_pir_and_mw_pins_config);
   motion_sensors_pir_and_mw_pins_config.GPIO_Pin = PIR_LED_3_PIN;
   GPIO_Init(PIR_LED_3_PORT, &motion_sensors_pir_and_mw_pins_config);
   motion_sensors_pir_and_mw_pins_config.GPIO_Pin = MW_LED_3_PIN;
   GPIO_Init(MW_LED_3_PORT, &motion_sensors_pir_and_mw_pins_config);

   GPIO_InitTypeDef immobilizer_led_pin_config;
   immobilizer_led_pin_config.GPIO_Pin = IMMOBILIZER_LED_PIN;
   immobilizer_led_pin_config.GPIO_Mode = GPIO_Mode_IN;
   immobilizer_led_pin_config.GPIO_Speed = GPIO_Speed_Level_1; // 2 MHz
   immobilizer_led_pin_config.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_Init(IMMOBILIZER_LED_PORT, &immobilizer_led_pin_config);
}

/**
 * USART frame time Tfr = (1 / USART_BAUD_RATE) * 10bits
 * Timer time to be sure the frame is ended Tt = Tfr + 0.5 * Tfr
 * Frequency = 16Mhz, USART_BAUD_RATE = 115200. Tt = 0.13ms
 */
void timer3_confing() {
   DBGMCU_APB1PeriphConfig(DBGMCU_TIM3_STOP, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

   TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
   TIM_TimeBaseStructure.TIM_Period = TIMER3_PERIOD_TICKS;
   TIM_TimeBaseStructure.TIM_Prescaler = 0;
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

   NVIC_EnableIRQ(TIM3_IRQn);
   TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

   TIM_Cmd(TIM3, ENABLE);
}

/**
 * 0.0983s with 16MHz clock
 */
void timer14_confing() {
   DBGMCU_APB1PeriphConfig(DBGMCU_TIM14_STOP, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

   TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
   TIM_TimeBaseStructure.TIM_Period = TIMER14_PERIOD;
   TIM_TimeBaseStructure.TIM_Prescaler = TIMER14_PRESCALER;
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

   TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
   NVIC_EnableIRQ(TIM14_IRQn);
   TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

   TIM_Cmd(TIM14, ENABLE);
}

void dma_config() {
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

   // USART DMA config
   DMA_InitTypeDef usartDmaInitType;
   usartDmaInitType.DMA_PeripheralBaseAddr = USART1_TDR_ADDRESS;
   //dmaInitType.DMA_MemoryBaseAddr = (uint32_t)(&usartDataToBeTransmitted);
   usartDmaInitType.DMA_DIR = DMA_DIR_PeripheralDST; // Specifies if the peripheral is the source or destination
   usartDmaInitType.DMA_BufferSize = 0;
   usartDmaInitType.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   usartDmaInitType.DMA_MemoryInc = DMA_MemoryInc_Enable; // DMA_MemoryInc_Enable if DMA_InitTypeDef.DMA_BufferSize > 1
   usartDmaInitType.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   usartDmaInitType.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
   usartDmaInitType.DMA_Mode = DMA_Mode_Normal;
   usartDmaInitType.DMA_Priority = DMA_Priority_Low;
   usartDmaInitType.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(USART1_TX_DMA_CHANNEL, &usartDmaInitType);

   DMA_ITConfig(USART1_TX_DMA_CHANNEL, DMA_IT_TC, ENABLE);
   NVIC_SetPriority(USART1_IRQn, 10);
   NVIC_EnableIRQ(USART1_IRQn);
   DMA_Cmd(USART1_TX_DMA_CHANNEL, ENABLE);
}

void usart_config() {
   // For USART1
   GPIO_InitTypeDef usart_pins_config;
   usart_pins_config.GPIO_Pin = USART_TX_PIN;
   usart_pins_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
   usart_pins_config.GPIO_Mode = GPIO_Mode_AF;
   usart_pins_config.GPIO_OType = GPIO_OType_PP; // GPIO_OType_OD for USART RX
   GPIO_Init(USART_TX_PORT, &usart_pins_config);
   GPIO_PinAFConfig(USART_TX_PORT, GPIO_PinSource9, GPIO_AF_1);
   //GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

   USART_OverSampling8Cmd(USART1, DISABLE);

   USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate = USART_BAUD_RATE;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Tx;
   USART_Init(USART1, &USART_InitStructure);

   //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
   USART_ITConfig(USART1, USART_IT_ERR, ENABLE);

   NVIC_SetPriority(DMA1_Channel2_3_IRQn, 11);
   NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

   USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

   USART_Cmd(USART1, ENABLE);
}

void external_interrupt_config() {
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

   SYSCFG_EXTILineConfig(IMMOBILIZER_LED_EXTI_PORT_SOURCE, IMMOBILIZER_LED_EXTI_PIN_SOURCE);
   SYSCFG_EXTILineConfig(MOTION_SENSOR_1_EXTI_PORT_SOURCE, MOTION_SENSOR_1_EXTI_PIN_SOURCE);
   SYSCFG_EXTILineConfig(PIR_LED_1_EXTI_PORT_SOURCE, PIR_LED_1_EXTI_PIN_SOURCE);
   SYSCFG_EXTILineConfig(MW_LED_1_EXTI_PORT_SOURCE, MW_LED_1_EXTI_PIN_SOURCE);
   SYSCFG_EXTILineConfig(MOTION_SENSOR_2_EXTI_PORT_SOURCE, MOTION_SENSOR_2_EXTI_PIN_SOURCE);
   SYSCFG_EXTILineConfig(MOTION_SENSOR_3_EXTI_PORT_SOURCE, MOTION_SENSOR_3_EXTI_PIN_SOURCE);
   SYSCFG_EXTILineConfig(PIR_LED_3_EXTI_PORT_SOURCE, PIR_LED_3_EXTI_PIN_SOURCE);
   SYSCFG_EXTILineConfig(MW_LED_3_EXTI_PORT_SOURCE, MW_LED_3_EXTI_PIN_SOURCE);

   EXTI_InitTypeDef exti_init_structure;
   exti_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
   exti_init_structure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
   exti_init_structure.EXTI_LineCmd = ENABLE;
   exti_init_structure.EXTI_Line = IMMOBILIZER_LED_EXTI_LINE | MOTION_SENSOR_1_EXTI_LINE | PIR_LED_1_EXTI_LINE |
         MW_LED_1_EXTI_LINE | MOTION_SENSOR_2_EXTI_LINE | MOTION_SENSOR_3_EXTI_LINE | PIR_LED_3_EXTI_LINE | MW_LED_3_EXTI_LINE;
   EXTI_Init(&exti_init_structure);

   NVIC_InitTypeDef nvic_init_type_structure;
   nvic_init_type_structure.NVIC_IRQChannelPriority = 3;
   nvic_init_type_structure.NVIC_IRQChannelCmd = ENABLE;
   nvic_init_type_structure.NVIC_IRQChannel = EXTI0_1_IRQn;
   NVIC_Init(&nvic_init_type_structure);
   nvic_init_type_structure.NVIC_IRQChannel = EXTI2_3_IRQn;
   NVIC_Init(&nvic_init_type_structure);
   nvic_init_type_structure.NVIC_IRQChannel = EXTI4_15_IRQn;
   NVIC_Init(&nvic_init_type_structure);
}

void send_usard_data(char *string) {
   reset_flag(&general_flags_g, USART_TRANSFER_COMPLETE_FLAG);
   clear_usart_data_received_buffer();
   DMA_Cmd(USART1_TX_DMA_CHANNEL, DISABLE);
   unsigned short bytes_to_send = get_string_length(string);

   if (bytes_to_send == 0) {
      return;
   }

   DMA_SetCurrDataCounter(USART1_TX_DMA_CHANNEL, bytes_to_send);
   USART1_TX_DMA_CHANNEL->CMAR = (unsigned int) string;
   USART_ClearFlag(USART1, USART_FLAG_TC);
   DMA_Cmd(USART1_TX_DMA_CHANNEL, ENABLE);
}

void clear_usart_data_received_buffer() {
   for (unsigned short i = 0; i < USART_DATA_RECEIVED_BUFFER_SIZE; i++) {
      if (usart_data_received_buffer_g[i] == '\0') {
         break;
      }

      usart_data_received_buffer_g[i] = '\0';
   }
}

unsigned short get_received_data_length() {
   for (unsigned short i = 0; i < USART_DATA_RECEIVED_BUFFER_SIZE; i++) {
      if (usart_data_received_buffer_g[i] == '\0') {
         return i;
      }
   }
   return 0;
}

unsigned char is_received_data_length_equal(unsigned short length) {
   for (unsigned short i = 0; i < USART_DATA_RECEIVED_BUFFER_SIZE; i++) {
      if (i == length && usart_data_received_buffer_g[i] == '\0') {
         return 1;
      } else if ((i < length && usart_data_received_buffer_g[i] == '\0') || i > length) {
         return 0;
      }
   }
   return 0;
}

void set_flag(unsigned int *flags, unsigned int flag_value) {
   *flags |= flag_value;
}

void reset_flag(unsigned int *flags, unsigned int flag_value) {
   *flags &= ~(*flags & flag_value);
}

unsigned char read_flag(unsigned int flags, unsigned int flag_value) {
   return (flags & flag_value) > 0 ? 1 : 0;
}

unsigned char contains_string(char being_compared_string[], char string_to_be_contained[]) {
   unsigned char found = 0;

   if (*being_compared_string == '\0' || *string_to_be_contained == '\0') {
      return found;
   }

   for (; *being_compared_string != '\0'; being_compared_string++) {
      unsigned char all_chars_are_equal = 1;

      for (char *char_address = string_to_be_contained; *char_address != '\0';
            char_address++, being_compared_string++) {
         if (*being_compared_string == '\0') {
            return found;
         }

         all_chars_are_equal = *being_compared_string == *char_address ? 1 : 0;

         if (!all_chars_are_equal) {
            break;
         }
      }

      if (all_chars_are_equal) {
         found = 1;
         break;
      }
   }
   return found;
}

unsigned short get_string_length(char string[]) {
   unsigned short length = 0;

   for (char *string_pointer = string; *string_pointer != '\0'; string_pointer++, length++) {
   }
   return length;
}
