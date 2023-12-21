.text
.global main
.cpu cortex-m4
.syntax unified

main:
		// Clock address: 0x4002104C
		MOV R6, #0x104C
		MOVT R6, #0x4002
		MOV R0, #0b00000101
		STR R0, [R6]

		// R4 has the GPIO base
		MOV R4, #0x0000
		MOVT R4, #0x4800
		// set to output mode
		// we want bits [11:10] to be 0b01
		LDR R1, [R4] // get current mode
		// R2 will be our mode mask
		MOV R2, #0x0800 // AND mask
		MVN R2, R2 // AND mask
		AND R3, R1, R3

		LDR R1, [R4] // get current mode

inf: B inf

	.end
