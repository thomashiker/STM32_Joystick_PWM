

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "uart.h"


/*The USART3 potr and pin setting , it's fixed var. cannot change*/
#define USART3_PORT                        GPIOB
#define USART3_TX_PIN                     GPIO_Pin_10
#define USART3_RX_PIN                     GPIO_Pin_11

#define USART6_PORT                        GPIOC
#define USART6_TX_PIN                     GPIO_Pin_6
#define USART6_RX_PIN                     GPIO_Pin_7

uint8_t Receive_String_Ready=0;
uint8_t Receive_String_Ready2=0;

void init_USART3(uint32_t baurate){

		/* This is a concept that has to do with the libraries provided by ST
		 * to make development easier the have made up something similar to 
		 * classes, called TypeDefs, which actually just define the common
		 * parameters that every peripheral needs to work correctly
		 * 
		 * They make our life easier because we don't have to mess around with 
		 * the low level stuff of setting bits in the correct registers
		 */
		GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
		USART_InitTypeDef USART_InitStruct; // this is for the USART3 initilization
		NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

		/* enable APB1 peripheral clock for USART3
		 * note that only USART1 and USART6 are connected to APB2
		 * the other USARTs are connected to APB1
		 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

		/* enable the peripheral clock for the pins used by 
		 * USART3, PB10 for TX and PB11 for RX
		 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

		/* This sequence sets up the TX and RX pins 
		 * so they work correctly with the USART1 peripheral
		 */
		GPIO_InitStruct.GPIO_Pin = USART3_TX_PIN | USART3_RX_PIN; // Pins 6 (TX) and 7 (RX) are used
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                         // the pins are configured as alternate function so the USART peripheral has access to them
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                // this defines the IO speed and has nothing to do with the baudrate!
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                        // this defines the output type as push pull mode (as opposed to open drain)
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;                        // this activates the pullup resistors on the IO pins
		GPIO_Init(USART3_PORT, &GPIO_InitStruct);                                        // now all the values are passed to the GPIO_Init() function which sets the GPIO registers



		/* The RX and TX pins are now connected to their AF
		 * so that the USART3 can take over control of the 
		 * pins
		 */
		GPIO_PinAFConfig(USART3_PORT, GPIO_PinSource10, GPIO_AF_USART3); //USART3, PB10 for TX
		GPIO_PinAFConfig(USART3_PORT, GPIO_PinSource11, GPIO_AF_USART3); //USART3, PB11 for RX

		/* Now the USART_InitStruct is used to define the 
		 * properties of USART3
		 */

		USART_InitStruct.USART_BaudRate = baurate;                                // the baudrate is set to the value we passed into this init function
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
		USART_InitStruct.USART_StopBits = USART_StopBits_1;                // we want 1 stop bit (standard)
		USART_InitStruct.USART_Parity = USART_Parity_No;                // we don't want a parity bit (standard)
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
		USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
		USART_Init(USART3, &USART_InitStruct);                                        // again all the properties are passed to the USART_Init function which takes care of all the bit setting


		/* Here the USART3 receive interrupt is enabled
		 * and the interrupt controller is configured 
		 * to jump to the USART3_IRQHandler() function
		 * if the USART3 receive interrupt occurs
		 */
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // enable the USART3 receive interrupt 

		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;                 // we want to configure the USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                 // this sets the subpriority inside the group
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                         // the USART1 interrupts are globally enabled
		NVIC_Init(&NVIC_InitStructure);                                                         // the properties are passed to the NVIC_Init function which takes care of the low level stuff        

		// finally this enables the complete USART3 peripheral
		USART_Cmd(USART3, ENABLE);
}

void init_USART6(uint32_t baurate){
		GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
		USART_InitTypeDef USART_InitStruct; // this is for the USART3 initilization
		NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

		RCC_AHB1PeriphClockCmd(USARTy_TX_GPIO_CLK | USARTy_RX_GPIO_CLK, ENABLE);

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		GPIO_InitStruct.GPIO_Pin = USART6_TX_PIN | USART6_RX_PIN; // Pins 6 (TX) and 7 (RX) are used
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                         // the pins are configured as alternate function so the USART peripheral has access to them
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                // this defines the IO speed and has nothing to do with the baudrate!
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                        // this defines the output type as push pull mode (as opposed to open drain)
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;                        // this activates the pullup resistors on the IO pins
		GPIO_Init(USART6_PORT, &GPIO_InitStruct);                                        // now all the values are passed to the GPIO_Init() function which sets the GPIO registers

		GPIO_PinAFConfig(USART6_PORT, GPIO_PinSource6, GPIO_AF_USART6); 
		GPIO_PinAFConfig(USART6_PORT, GPIO_PinSource7, GPIO_AF_USART6);

		USART_InitStruct.USART_BaudRate = baurate;                                // the baudrate is set to the value we passed into this init function
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
		USART_InitStruct.USART_StopBits = USART_StopBits_1;                // we want 1 stop bit (standard)
		USART_InitStruct.USART_Parity = USART_Parity_No;                // we don't want a parity bit (standard)
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
		USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
		USART_Init(USART6, &USART_InitStruct);                                        // again all the properties are passed to the USART_Init function which takes care of all the bit setting

		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); // enable the USART3 receive interrupt 

		NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;                 // we want to configure the USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                 // this sets the subpriority inside the group
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                         // the USART1 interrupts are globally enabled
		NVIC_Init(&NVIC_InitStructure);                                                         // the properties are passed to the NVIC_Init function which takes care of the low level stuff        

		USART_Cmd(USART6, ENABLE);
}


// this is the interrupt request handler (IRQ) for ALL USART3 interrupts
void USART3_IRQHandler(void){

		// check if the USART3 receive interrupt flag was set
		if( USART_GetITStatus(USART3, USART_IT_RXNE) ){
			static uint8_t cnt = 0; // this counter is used to determine the uart receive string length

			Receive_data = USART3->DR; // the character from the USART3 data register is saved in t

			if( cnt < MAX_STRLEN){ 
					received_string[cnt] = Receive_data;
                    
                    if(cnt>=2){
                        /*start determine the period of command.*/
                        if(received_string[cnt-2]=='e' && received_string[cnt-1]=='n' && received_string[cnt]=='d'){
                            Receive_String_Ready = 1; /*Ready to parse the command */
                            cnt=0; /*restart to accept next stream message.*/
                        }
                        else{
                            cnt++;
                        }
                    }else{
                        cnt++;
                    }
			}
			else{ // over the max string length, cnt return to zero.
					Receive_String_Ready=0;
					cnt = 0;  
					//USART_puts(USART3, received_string);
			}

		}
}

void USART6_IRQHandler(void)
{
  /* USART in Receiver mode */
  if (USART_GetITStatus(USART6, USART_IT_RXNE))
  {
	static uint8_t cnt2 = 0; // this counter is used to determine the uart receive string length

	Receive_data2 = USART6->DR; // the character from the USART3 data register is saved in t

	if( cnt2 < MAX_STRLEN){ 
		received_string2[cnt2] = Receive_data2;
        cnt2++;
	}
	else{ // over the max string length, cnt2 return to zero.
		Receive_String_Ready2=0;
		cnt2 = 0;  
		//USART_puts(USART6, received_string);
	}
  }
}


void USART_puts(USART_TypeDef* USARTx, volatile char *s)
{
		while(*s){
				// wait until data register is empty
				while( !(USARTx->SR & 0x00000040) ); 
				USART_SendData(USARTx, *s);
				*s++;
		}
}
