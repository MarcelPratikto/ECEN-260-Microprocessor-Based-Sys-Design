.text
.global main
.cpu cortex-m4
.syntax unified

main:
/*
===========================================================================================
SETUP GPIO PORTS A & C
===========================================================================================
*/
	// Base Address of Port A (PA) = 0x48000000
	MOV		R4, #0x0000
	MOVT	R4, #0x4800
	// Base Address of Port C (PC) = 0x48000800
	MOV		R5, #0x0800
	MOVT	R5, #0x4800

/*
===========================================================================================
CLOCK
===========================================================================================
*/
	// Address of Reset/Clock Control
	// Advanced High-Performance Bus 2 Enable Register
	// RCC AHB2 ENR = 0x4002104C
	MOV		R6, #0x104C
	MOVT	R6, #0x4002
	// Enable the I/O Port clocks with RCC AHB2 ENR (R6)
	MOV		R0, #0b00000101	// bit0 for PA, bit2 for PC
	STR		R0, [R6]

/*
===========================================================================================
CONFIGURE INPUTS
===========================================================================================
*/
	// ***** Configure Button 1 (PC13) as input *****
	// Create bitmask to reset bits 26 and 27 of PC MODER
	MOV		R1, #0x0000		// 0000 1100 0000 0000 0000 0000 0000 0000
	MOVT	R1, #0x0C00		// top half of the above number
	MVN		R1, R1			// flip the bits, mask = 1111 0011 1111 1111 1111 1111 1111 1111
	// Read PC MODER value, reset bits 26 and 27, and write back to PC MODER
	LDR		R0, [R5]		// load PC MODER
	AND		R0, R1			// bitmask to reset bits 26 and 27
	STR		R0, [R5]		// store PC MODER

	// ***** Configure Button 3 (PA1) as input *****
	// Create bitmask to reset bits 2 and 3 of PA MODER
	MOV		R1, #0x000C		// 0000 0000 0000 0000 0000 0000 0000 1100
	MOVT	R1, #0x0000		// top half of the above number
	MVN		R1, R1			// 1111 1111 1111 1111 1111 1111 1111 0011
	// Read PA MODER value, reset bits 2 and 3, and write back to PA MODER
	LDR		R0, [R4]		// load PA MODER
	AND		R0, R1			// bitmask to reset bits 0 and 1
	STR		R0, [R4]		// store PA MODER

	// ***** enable pull-up resistor for Button 3 *****
	// bitmask to set bit 2
	MOV		R1, #0x0004		// 0000 0000 0000 0000 0000 0000 0000 0100
	MOVT	R1, #0x0000
	// bitmask to reset bit 3
	MOV		R2, #0x0008		// 0000 0000 0000 0000 0000 0000 0000 1000
	MOVT	R2, #0x0000
	MVN		R2, R2			// 1111 1111 1111 1111 1111 1111 1111 0111
	// Read PA PUPDR value, reset bit 3, set bit 2, store back to PA PUPDR
	LDR		R0, [R4, #0xC]
	AND		R0, R2
	ORR		R0, R1
	STR		R0, [R4, #0xC]

/*
===========================================================================================
CONFIGURE OUTPUTS
===========================================================================================
*/
	// ***** Configure LED2 (PA5) as output *****
	// Create bitmask to reset bit 11 of PA MODER (bit 10 already initializes to 1)
	MOV		R1, #0x0800		// 0000 1000 0000 0000
	MVN		R1, R1			// flip the bits, mask = 1111 0111 1111 1111
	// Read PA MODER value, reset bit 11, and write back to PA MODER
	LDR		R0, [R4]		// load PA MODER (PA base + 0x0)
	AND		R0, R1			// bitmask to reset bit 11
	STR		R0, [R4]		// store PA MODER

	// ***** Configure LED4 (PA0) as output *****
	// Create bitmask to reset bit 1 of PA MODER
	MOV		R1, #0x0002		// 0000 0000 0000 0000 0000 0000 0000 0010
	MOVT	R1, #0x0000		// top half of the above number
	MVN		R1, R1			// 1111 1111 1111 1111 1111 1111 1111 1101
	// Create bitmask to set bit 0 of PA MODER
	MOV		R2, #0x0001		// 0000 0000 0000 0000 0000 0000 0000 0001
	MOVT	R2, #0x0000		// top half of the above number
	// Read PA MODER value, set bit 2 and reset bit 3, and write back to PA MODER
	LDR		R0, [R4]		// load PA MODER (PA base + 0x0)
	ORR		R0, R2			// bitmask to set bit 0
	AND		R0, R1			// bitmask to reset bit 1
	STR		R0, [R4]		// store PA MODER

/*
===========================================================================================
LOOP
checks if none, either, or both buttons are pressed
===========================================================================================
*/
loop:
	// Check Button 1. Zero =  button pushed. Not zero = button not pushed.
	BL		readButton1		// result stored in R0
	// Check Button 3. Zero =  button pushed. Not zero = button not pushed.
	BL		readButton3		// result stored in R3
	CMP		R0, #0			// compare Button 1 with zero
	BNE		elseButton1		// if not pushed, go to "elseButton1", otherwise continue to "Button1"

// if Button 1 is pushed
Button1:
	// Check Button 3. Zero =  button pushed. Not zero = button not pushed.
	CMP		R3, #0
	BEQ		twoButtons		// if both buttons are pushed
	B		oneButton		// if only Button 1 is pushed

// if Button 1 is not pushed
elseButton1:
	// Check Button 3. Zero =  button pushed. Not zero = button not pushed.
	CMP		R3, #0
	BNE		noButton		// if no buttons are pushed
	B		oneButton		// if only Button 3 is pushed

// no buttons pushed
noButton:
	BL		turnoffLED2
	BL		turnoffLED4
	B		loop

// just one button. Button 1 or Button 3 pushed
oneButton:
	BL		turnoffLED2
	BL		turnonLED4
	B		loop

// both buttons pressed. Buttons 1 and 3 pushed
twoButtons:
	BL		turnonLED2
	BL		turnoffLED4
	B		loop

/*
===========================================================================================
READ BUTTONS
===========================================================================================
*/
// ***** Read status of Button 1 (PC13) *****
readButton1:
	// Create bitmask to read bit 13 of PC IDR
	MOV		R1, #0x2000		// mask = 0010 0000 0000 0000
	// Read PC IDR Value, reset all bits except bit 13
	LDRH	R0, [R5, #0x10]	// load PC IDR (PC base + 0x10)
	AND		R0, R1			// bitmask to reset all bits except bit 13
	//STRH	R0, [R5, #0x10]
	// return
	BX		LR

// ***** Read status of Button 3 (PA1) *****
readButton3:
	// Create bitmask to read bit 1 of PA IDR
	MOV		R1, #0x0002		// mask = 0000 0000 0000 0010
	// Read PA IDR Value, reset all bits except bit 0
	LDRH	R3, [R4, #0x10]	// load PA IDR (PA base + 0x10)
	AND		R3, R1			// bitmask to reset all bits except bit 1
	//STRH	R3, [R4, #0x10]
	// return
	BX		LR


/*
===========================================================================================
TURNS LED ON OR OFF
===========================================================================================
*/

// This function turns off LED2 (PA5)
// Note: This function assumes R4 still has the PA base address
turnoffLED2:
	// Create bitmask to reset bit 5
	MOV		R1, #0x0020		// 0000 0000 0010 0000
	MVN		R1, R1			// flip the bits (MOVE NOT), mask = 1111 1111 1101 1111

	// read PA ODR value, reset bit 5, and write back to PA ODR
	LDRH	R0, [R4, #0x14]	// load PA ODR (PA base + 0x14)
	AND		R0, R1			// reset bit 5 using mask
	STRH	R0, [R4, #0x14]	// store PA ODR

	// return (end function)
	BX		LR

// This function turns on LED2 (PA 5)
// Note: This function assumes R4 still has the PA base address
turnonLED2:
	// Create bitmask to set bit 5
	MOV		R1, #0x0020		// mask = 0000 0000 0010 0000

	// read PA ODR value, reset bit 5, and write back to PA ODR
	LDRH	R0, [R4, #0x14]	// load PA ODR (PA base + 0x14)
	ORR		R0, R1			// reset bit 5 using mask
	STRH	R0, [R4, #0x14]	// store PA ODR

	// return (end function)
	BX		LR

//TODO
// This function turns off LED4 (PA0)
// Note: This function assumes R4 still has the PA base address
turnoffLED4:
	// Create bitmask to reset bit 0
	MOV		R1, #0x0001		// 0000 0000 0000 0001
	MVN		R1, R1			// flip the bits (MOVE NOT), mask = 1111 1111 1111 1110
	// read PA ODR value, reset bit 0, and write back to PA ODR
	LDRH	R0, [R4, #0x14]	// load PA ODR (PA base + 0x14)
	AND		R0, R1			// reset bit 1 using mask
	STRH	R0, [R4, #0x14]	// store PA ODR
	// return
	BX		LR

// This function turns on LED4 (PA0)
// Note: This function assumes R4 still has the PA base address
turnonLED4:
	// Create bitmask to set bit 0
	MOV		R1, #0x0001		// mask = 0000 0000 0000 0001
	// read PA ODR value, reset bit 0, and write back to PA ODR
	LDRH	R0, [R4, #0x14]	// load PA ODR (PA base + 0x14)
	ORR		R0, R1			// reset bit 1 using mask
	STRH	R0, [R4, #0x14]	// store PA ODR
	// return
	BX		LR

/*
===========================================================================================
END OF FILE
===========================================================================================
*/
	.end
