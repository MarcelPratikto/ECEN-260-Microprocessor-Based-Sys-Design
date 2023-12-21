/*
 * myDrivers.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Marcel Pratikto
 */

#ifndef MYDRIVERS_H_
#define MYDRIVERS_H_

// changes one bit at a time
#define bit(x)			(0b1 << x)
#define bitpair(x)		(0b11 << x*2)

// change upper bits & lower bits
#define ubit(x)			(0b10 << x*2)
#define lbit(x)			(0b01 << x*2)

// GPIO Port struct
typedef struct{
unsigned int MODER;		// offset: 0x00
unsigned int OTYPER;	// offset: 0x04
unsigned int OSPEEDR;	// offset: 0x08
unsigned int PUPDR;		// offset: 0x0C
unsigned int IDR;		// offset: 0x10
unsigned int ODR;		// offset: 0x14
} Port;

// Address of the Advanced High-performance Bus 2 Enable Register
unsigned int* AHB2ENR = (unsigned int*) 0x4002104c;
// Base addresses of the GPIO Port control registers (SFRs)
Port* GPIOA = (Port*) 0x48000000; // base address of GPIOA
Port* GPIOB = (Port*) 0x48000400; // base address of GPIOB
Port* GPIOC = (Port*) 0x48000800; // base address of GPIOC

// enable clock for each ports
void GPIOA_enable_clock(){*AHB2ENR |= bit(0);}
void GPIOB_enable_clock(){*AHB2ENR |= bit(1);}
void GPIOC_enable_clock(){*AHB2ENR |= bit(2);}

// This function configures a pin in the port specified by GPIOx as an input device
void GPIO_configure_input_mode(Port* GPIOx, unsigned int pin)
{
	// MODER input: 0b00
	// takes the MODER (Mode register)
	unsigned int mode = GPIOx->MODER;
	// reset the pin to 0b00
	mode &= ~bitpair(pin);
	// store
	GPIOx->MODER = mode;
}

// This function configures a pin in the port specified by GPIOx as an output device
void GPIO_configure_output_mode(Port* GPIOx, unsigned int pin)
{
	// MODER output: 0b01
	// takes the MODER (Mode Register)
	unsigned int mode = GPIOx->MODER;
	// reset the pin to 0b00
	mode &= ~bitpair(pin);
	// set to 0b01
	mode |= lbit(pin);
	// store
	GPIOx->MODER = mode;
}

// this function enables the pull-up resistor for a pin in the port specified by GPIOx
void GPIO_enable_pull_up_resistor(Port* GPIOx, unsigned int pin)
{
	// PUPDR pull-up enable: 0b01
	// takes the PUPDR (Pull-Up Pull-Down Register)
	unsigned int resistor = GPIOx->PUPDR;
	// reset the pin to 0b00
	resistor &= ~bitpair(pin);
	// set the pin to 0b01
	resistor |= lbit(pin);
	// store
	GPIOx->PUPDR = resistor;
}

// this function enables the pull-down resistor for a pin in the port specified by GPIOx
void GPIO_enable_pull_down_resistor(Port* GPIOx, unsigned int pin)
{
	// PUPDR pull-down enable: b10
	// takes the PUPDR (Pull-Up Pull-Down Register)
	unsigned int resistor = GPIOx->PUPDR;
	// reset the pin to 0b00
	resistor &= ~bitpair(pin);
	// set the pin to 0b10
	resistor |= ubit(pin);
	// store
	GPIOx->PUPDR = resistor;
}

// this function disables the resistor for a pin in the port specified by GPIOx
void GPIO_disable_pull_resistor(Port* GPIOx, unsigned int pin)
{
	// PUPDR disable: 0b00
	// takes the PUPDR (Pull-Up Pull-Down Register)
	unsigned int resistor = GPIOx->PUPDR;
	// reset the pin to 0b00
	resistor &= ~bitpair(pin);
	// store
	GPIOx->PUPDR = resistor;
}

/*
 * This function reads a pin in a port specified by GPIOx
 * return: int
 * 0 if not read
 * 1 if read
 */
unsigned int GPIO_read_pin(Port* GPIOx, unsigned int pin)
{
	// reads the value of the pin in the IDR (Input Data Register)
	unsigned int read = GPIOx->IDR & bit(pin);
	return read;
}

// this function writes a state to a pin in a port specified by GPIOx
void GPIO_write_pin(Port* GPIOx, unsigned int pin, unsigned int state)
{
	// takes the value from the ODR (Output Data Register)
	unsigned int write = GPIOx->ODR;
	// if state is off or 0b0
	if (!state)
	{
		// reset pin to 0b0
		write &= ~bit(pin);
		GPIOx->ODR = write;
	}
	else
	{
		// set pin to 0b1
		write |= bit(pin);
		GPIOx->ODR = write;
	}
}

#endif /* MYDRIVERS_H_ */
