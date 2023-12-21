#include "drivers.h"
#include <stdbool.h>

// GPIO Pin Macros
#define B1_Port GPIOC
#define B1_Pin  13    // PC13 (on-board button)

#define L2_Port GPIOA
#define L2_Pin  5     // PA5 (on-board LED)

#define LR_Port GPIOA
#define LR_Pin  7     // PA7 (red LED)

#define LG_Port GPIOA
#define LG_Pin  8     // PA8 (green LED)

#define LB_Port GPIOA
#define LB_Pin  9     // PA9 (blue LED)

#define DA_Port GPIOC
#define DA_Pin  0     // PC0 (data available signal)

#define D0_Port GPIOA
#define D0_Pin  0     // PA0 (data bit 0)

#define D1_Port GPIOA
#define D1_Pin  1     // PA1 (data bit 1)

#define D2_Port GPIOA
#define D2_Pin  4     // PA4 (data bit 2)

#define D3_Port GPIOB
#define D3_Pin  0     // PB0 (data bit 3)

// function prototype
unsigned char keypad_decode();

// status variables that the ISRs access
int i = 0, password_index = 0, num_attempts = 0;
int current_password[10], temporary_password[10];
bool saving_password = false, entering_password = false, locked = false, alarm = false;


// main will setup the interrupts
// but it will not poll for the interrupts
int main(void){

  unsigned int temp;

  // enable clocks for GPIO ports A, B, C
  GPIOA_enable_clock();
  GPIOB_enable_clock();
  GPIOC_enable_clock();

  // Turn on system clock for interrupts
  SysConfig_enable_clock(); // see the file in I-Learn with code to add to your drivers.h file

  // Configure outputs
  GPIO_configure_output_mode(L2_Port, L2_Pin); // LED2 is output
  GPIO_configure_output_mode(LR_Port, LR_Pin); // Red LED is output
  GPIO_configure_output_mode(LG_Port, LG_Pin); // Green LED is output
  GPIO_configure_output_mode(LB_Port, LB_Pin); // Blue LED is output

  // Configure inputs
  GPIO_configure_input_mode(B1_Port, B1_Pin);  // Button 1 is input
  GPIO_configure_input_mode(D0_Port, D0_Pin);  // Data 0 pin is input
  GPIO_configure_input_mode(D1_Port, D1_Pin);  // Data 1 pin is input
  GPIO_configure_input_mode(D2_Port, D2_Pin);  // Data 2 pin is input
  GPIO_configure_input_mode(D3_Port, D3_Pin);  // Data 3 pin is input
  GPIO_configure_input_mode(DA_Port, DA_Pin);  // Data Available pin is input

  // Attach External Interrupt Line 13 to Port C for PC13 (Button 1)
  temp = SYSCFG->EXTICR[3];		// For line 13, get EXTICR[3] (4+4+4+1) - 16-bit register: 4 x 4-bit groups in each EXTICR[i]
  temp &= ~(0b1111 << 4);		// reset the 4 bits for position 1 (groups of 4)
  temp |=  (0b0010 << 4);		// set the 4 bits to 0x2 for GPIOC (GPIOA:0, GPIOB:1, GPIOC:2, ...)
  SYSCFG->EXTICR[3] = temp;		// put back into EXTICR[3]

  // Enable Interrupts for Line 13 (See 14.3.4)
  EXTI->IMR1  |= bit(B1_Pin);	// Remove mask for Line 13 (See 14.5.1)
  EXTI->FTSR1 |= bit(B1_Pin);	// Select the falling-edge trigger for Line 13 (See 14.5.4)
  NVIC->ISER[1] = (1<<8);		// Enable interrupts for vector position 40 (32 + 8). (See Table 58)

  // Attach External Interrupt Line 0 to Port C for PC0 (Data Available pin)
  temp = SYSCFG->EXTICR[0];		// get the EXTI Configuration Register for Line 0: EXTICR[0]
  temp &= ~(0b1111 << 0);		// reset the 4 bits in the correct group of 4 for Line 0
  temp |=  (0b0010 << 0);		// set the 4 bits to 0x2 for GPIOC
  SYSCFG->EXTICR[0] = temp;		// put back into EXTICR[0]

  // Enable Interrupts for Line 0 (See 14.3.4)
  EXTI->IMR1 |= bit(DA_Pin);	// Remove mask for Line 0 (See 14.5.1)
  EXTI->RTSR1 |= bit(DA_Pin);	// Select the rising-edge trigger for Line 0 (See 14.5.3)
  NVIC->ISER[0] = (1<<6);		// Enable interrupts for vector position 6. (See Table 58)

  // infinite loop, waiting for interrupts
  while (1);
}

// Interrupt Service Routine for Line 13 - part of group 10 through 15 (see Table 58)
void EXTI15_10_IRQHandler(void){ // see startup code for NVIC IRQ handler names

  // Check for interrupt from B1 pin (13) for the Line 13 trigger in the Pending Register (See 14.5.6)
  if(EXTI->PR1 & bit(B1_Pin)){

    // Reset system
    alarm = false;
    locked = false;
    entering_password = false;
    saving_password = false;
    num_attempts = 0;
    password_index = 0;

    // Turn off lights
    GPIO_write_pin(L2_Port, L2_Pin, 0); // turn off L2 (LED2)
    GPIO_write_pin(LR_Port, LR_Pin, 0); // turn off Red LED (LR)
    GPIO_write_pin(LG_Port, LG_Pin, 0); // turn off Green LED (LG)
    GPIO_write_pin(LB_Port, LB_Pin, 0); // turn off Blue LED (LB)

    // De-bounce switch
    for (i=0;i<5000;i++); // wait while the button is bouncing before clearing the interrupt

    // Clear pending interrupt for B1 (PC13) on Line 13 (See 14.5.6)
    EXTI->PR1 = bit(B1_Pin); // clear by writing a '1' to the reset signal
  }
}

// Interrupt Service Routine for Line 0 (DA is PC0)
void EXTI0_IRQHandler(void){ // see startup code in reading for NVIC IRQ handler names for Line 0

  // Check the Pending Register for a trigger on Line 0 (See 14.5.6) because Data Available pin is pin 0.
  if(EXTI->PR1 & bit(DA_Pin)){

    // Read data
    unsigned char key = keypad_decode();  // determine which key was pressed

    if(!alarm){

      // Blink LED2 to show button press detected
      GPIO_write_pin(L2_Port, L2_Pin, 1); // LED2 on
      for (i=0;i<10000;i++);              // short delay
      GPIO_write_pin(L2_Port, L2_Pin, 0); // LED2 off

      // Based on what key was pressed, do something (each case)
      switch(key){

        case 0xA: // button "A" was pressed

          if(locked == false){ // if not already locked

            // Clear temporary password
            for(i=0; i < 10; i++)
              temporary_password[i] = -1;

            // Change status
            saving_password = true;
            password_index = 0;  // reset password index

            // Turn off RGB indicator lights
            GPIO_write_pin(LR_Port, LR_Pin, 0); // turn off Red LED (LR)
            GPIO_write_pin(LG_Port, LG_Pin, 0); // turn off Green LED (LG)
            GPIO_write_pin(LB_Port, LB_Pin, 0); // turn off Blue LED (LB)
          }

          break;

        case 0xB: // button "B" was pressed

          if(locked == false){ // if not already locked

            // Store temp password as current password
            for(i = 0; i < 10; i++)
              current_password[i] = temporary_password[i];

            // Update RGB indicator lights
            GPIO_write_pin(LR_Port, LR_Pin, 0);
            GPIO_write_pin(LG_Port, LG_Pin, 0);
            GPIO_write_pin(LB_Port, LB_Pin, 1); // turn *on* blue LED (new password entered)

            // Change status
            saving_password = false;
            locked = true;
          }

          break;

        case 0xC: // button "C" was pressed

          // Clear temporary password
          for(i = 0; i < 10; i++)
            temporary_password[i] = -1;

          // Change status
          entering_password = true;
          password_index = 0; // reset password index

          // Turn off RGB indicator lights
          GPIO_write_pin(LR_Port, LR_Pin, 0); // turn off Red LED (LR)
          GPIO_write_pin(LG_Port, LG_Pin, 0); // turn off Green LED (LG)
          GPIO_write_pin(LB_Port, LB_Pin, 0); // turn off Blue LED (LB)

          break;

        case 0xD: // button "D" was pressed

          if(entering_password){ // if password was being entered

            // Change status
            entering_password = false;
            password_index = 0;

            // Compare temporary password with current password
            locked = false;
            for(i = 0; i < 10; i++){
              if(current_password[i] != temporary_password[i])
                locked = true; // incorrect, keep locked
            }

            // If password incorrect
            if(locked){
              GPIO_write_pin(LR_Port, LR_Pin, 1); // turn *on* red (incorrect password)
              GPIO_write_pin(LG_Port, LG_Pin, 0);
              GPIO_write_pin(LB_Port, LB_Pin, 0);
              num_attempts++;
            }
            else{ // password correct
              GPIO_write_pin(LR_Port, LR_Pin, 0);
              GPIO_write_pin(LG_Port, LG_Pin, 1); // turn *on* green (correct password)
              GPIO_write_pin(LB_Port, LB_Pin, 0);
              num_attempts = 0; // reset attempt count
            }

            // if 3 wrong passwords
            if(num_attempts >= 3){
              GPIO_write_pin(L2_Port, L2_Pin, 1); // turn on LED2 (alarm)
              alarm=true;
            }
          }

          break;

        case 0xE: // key E (*) does nothing
          break;

        case 0xF: // key F (#) does nothing
          break;

        default: // this means a number was pressed

          // if current status == storing new password
          if(saving_password){
            if(password_index < 10){
              temporary_password[password_index++] = key; // store digit

              // if max digits reached, finish and store
              if(password_index == 10){

                // Store new password
                for(i = 0; i < 10; i++)
                  current_password[i] = temporary_password[i];

                // Update RGB indicater lights
                GPIO_write_pin(LR_Port, LR_Pin, 0);
                GPIO_write_pin(LG_Port, LG_Pin, 0);
                GPIO_write_pin(LB_Port, LB_Pin, 1); // turn on blue (new password entered)

                // Change status
                saving_password = false;
                locked = true;
              }
            }
          }
          // if status == entering password to check
          else if(entering_password){
            if(password_index<10)
              temporary_password[password_index++] = key; // store digit
          }
          break;
      }
    }

    // Clear a flag bit in the PR register
    EXTI->PR1 = bit(DA_Pin);
  }
}


// decode the keypad according to what we need each keys to be
unsigned char keypad_decode(){
  unsigned char key = 0x0;
  unsigned char data = 0b0000;

  // read the data pins and combine into the 4-bit value: D3_D2_D1_D0
  if (GPIO_read_pin(D0_Port, D0_Pin))
    data |= bit(0);
  if (GPIO_read_pin(D1_Port, D1_Pin))
    data |= bit(1);
  if (GPIO_read_pin(D2_Port, D2_Pin))
    data |= bit(2);
  if (GPIO_read_pin(D3_Port, D3_Pin))
    data |= bit(3);

  // The key encoder gives the following "data" values:
  // 0 1 2 3
  // 4 5 6 7
  // 8 9 A B
  // C D E F

  // The following switch statement re-maps it to these "key" names:
  // 1 2 3 A
  // 4 5 6 B
  // 7 8 9 C
  // E 0 F D, where E is * and F is #

  switch(data){
    case 0x0: key = 0x1; break; // fill out the missing key values (?) in this switch statement
    case 0x1: key = 0x2; break;
    case 0x2: key = 0x3; break;
    case 0x3: key = 0xA; break;
    case 0x4: key = 0x4; break;
    case 0x5: key = 0x5; break;
    case 0x6: key = 0x6; break;
    case 0x7: key = 0xB; break;
    case 0x8: key = 0x7; break;
    case 0x9: key = 0x8; break;
    case 0xA: key = 0x9; break;
    case 0xB: key = 0xC; break;
    case 0xC: key = 0xE; break;
    case 0xD: key = 0x0; break;
    case 0xE: key = 0xF; break;
    case 0xF: key = 0xD; break;
  }

  return key;
}
