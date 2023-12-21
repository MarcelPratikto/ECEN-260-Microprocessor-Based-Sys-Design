/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Marcel Pratikto
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

#include <stdint.h>
#include "interrupts.h"
#define B1_Port GPIOC	// User Button is PC13, pull-up is on the board
#define B1_Pin 13		// (no need to setup pull-up resistor for this button)
#define L2_Port GPIOA	// LED is PA5
#define L2_Pin 5		//

int main(void)
{
	SysConfig_enable_clock();
	GPIO_configure_input_mode(B1_Port, B1_Pin);		// onboard blue button as input
	GPIO_configure_output_mode(L2_Port, L2_Pin);	// onboard green LED as output

	// System Configuration Register (SYSCFG)
	// External Interrupt Lines (EXTI) Configuration Registers (EXTICR)
	/*
	 * EXTICR[0] holds the 4-bit groups for EXTI lines 0-3.
	 * EXTICR[1] holds the 4-bit groups for EXTI lines 4-7.
	 * EXTICR[2] holds the 4-bit groups for EXTI lines 8-11.
	 * EXTICR[3] holds the 4-bit groups for EXTI lines 12-15.
	 */
	unsigned int temp = SYSCFG->EXTICR[3];
	temp &= ~(0b1111 << 4);
	temp |= (0b0010 << 4);
	SYSCFG->EXTICR[3] = temp;
	// I've now set option 2 [Port C] for Line 13 EXTI

	// TODO
	EXTI->IMR1 |= bit(B1_Pin);
	EXTI->FTSR1 |= bit(B1_Pin);
	NVIC->ISER[1] |= bit(8);

    /* Loop forever */
	for(;;);
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI->PR1 & bit(B1_Pin))
	{
		GPIO_toggle_pin(L2_Port, L2_Pin);
	}
}
