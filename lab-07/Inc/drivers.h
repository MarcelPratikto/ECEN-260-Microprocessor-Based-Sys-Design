/*
 * drivers.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Marcel Pratikto
 */

#ifndef DRIVERS_H_
#define DRIVERS_H_

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

//void SysConfig_enable_clock(){} //TODO

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

// External Interrupt (EXTI) struct
typedef struct
{
  unsigned int IMR1;        /*!< EXTI Interrupt mask register 1,             Address offset: 0x00 */
  unsigned int EMR1;        /*!< EXTI Event mask register 1,                 Address offset: 0x04 */
  unsigned int RTSR1;       /*!< EXTI Rising trigger selection register 1,   Address offset: 0x08 */
  unsigned int FTSR1;       /*!< EXTI Falling trigger selection register 1,  Address offset: 0x0C */
  unsigned int SWIER1;      /*!< EXTI Software interrupt event register 1,   Address offset: 0x10 */
  unsigned int PR1;         /*!< EXTI Pending register 1,                    Address offset: 0x14 */
  unsigned int RESERVED1;   /*!< Reserved, 0x18                                                   */
  unsigned int RESERVED2;   /*!< Reserved, 0x1C                                                   */
  unsigned int IMR2;        /*!< EXTI Interrupt mask register 2,             Address offset: 0x20 */
  unsigned int EMR2;        /*!< EXTI Event mask register 2,                 Address offset: 0x24 */
  unsigned int RTSR2;       /*!< EXTI Rising trigger selection register 2,   Address offset: 0x28 */
  unsigned int FTSR2;       /*!< EXTI Falling trigger selection register 2,  Address offset: 0x2C */
  unsigned int SWIER2;      /*!< EXTI Software interrupt event register 2,   Address offset: 0x30 */
  unsigned int PR2;         /*!< EXTI Pending register 2,                    Address offset: 0x34 */
} EXTI_TypeDef;

// Base address of EXTI control registers
EXTI_TypeDef* EXTI = (EXTI_TypeDef*) 0x40010400;


// System Configuration (SYSCFG) struct
typedef struct
{
  unsigned int MEMRMP;      /*!< SYSCFG memory remap register,                      Address offset: 0x00      */
  unsigned int CFGR1;       /*!< SYSCFG configuration register 1,                   Address offset: 0x04      */
  unsigned int EXTICR[4];   /*!< SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 */
  unsigned int SCSR;        /*!< SYSCFG SRAM2 control and status register,          Address offset: 0x18      */
  unsigned int CFGR2;       /*!< SYSCFG configuration register 2,                   Address offset: 0x1C      */
  unsigned int SWPR;        /*!< SYSCFG SRAM2 write protection register,            Address offset: 0x20      */
  unsigned int SKR;         /*!< SYSCFG SRAM2 key register,                         Address offset: 0x24      */
} SYSCFG_TypeDef;

// Base address of system configuration registers
SYSCFG_TypeDef* SYSCFG = (SYSCFG_TypeDef*) 0x40010000;


// Nested Vector Interrupt Control (NVIC) struct
typedef struct
{
  unsigned int ISER[8U];               /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register */
  unsigned int RESERVED0[24U];
  unsigned int ICER[8U];               /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register */
  unsigned int RESERVED1[24U];
  unsigned int ISPR[8U];               /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register */
  unsigned int RESERVED2[24U];
  unsigned int ICPR[8U];               /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register */
  unsigned int RESERVED3[24U];
  unsigned int IABR[8U];               /*!< Offset: 0x200 (R/W)  Interrupt Active bit Register */
  unsigned int RESERVED4[56U];
  unsigned int IP[240U];               /*!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
  unsigned int RESERVED5[644U];
  unsigned int STIR;                   /*!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register */
}  NVIC_Type;

// Base address of NVIC registers:
NVIC_Type* NVIC = (NVIC_Type*) 0xe000e100;

// Advanced Peripheral Bus 2 Enable Register
unsigned int* RCC_APB2ENR = (unsigned int*) 0x40021060;

// enables the System Configuration clock (for external interrupts)
void SysConfig_enable_clock(void){

	// set bit 0 for the System Configuration Clock
	*RCC_APB2ENR |= 0b1;
}

// toggle a pin
void GPIO_toggle_pin(Port* GPIOx, unsigned int pin){
	// flip the bit for pin (1->0, 0->1)
	GPIOx->ODR ^= bit(pin);
}

#endif /* DRIVERS_H_ */
