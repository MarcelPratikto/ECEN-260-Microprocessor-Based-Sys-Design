.text				// interpret stuff below as text instead of ascii
.global main		// where to start
.cpu cortex-m4
.syntax unified

main:
	// assign values to R0
	MOV R0, #-5
	BL abs

// infinite loop that ends program
end: BL end

// This subroutine returns the absolute value
// input parameters: R0
// return parameters: R1
abs:
	// check
	CMP R0, #0
	// if positive
	IT GE
	MOVGE R1, R0
	BGE end
	// if negative
	MVN R1, R0
	ADD R1, R1, #1
	BX LR

.end
