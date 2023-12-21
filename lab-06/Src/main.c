#include "myDrivers.h"

// input pins
#define ArmButtonPin 13		// PC13 (Board User Button)
#define DisarmButtonPin 8	// PC8
#define FrontSensorPin 0	// PA0
#define BackSensorPin 1		// PA1
#define WindowSensorPin 4	// PA4
// output pins
#define ArmStatusPin 5		// PA5 (Board User LED2)
#define FrontAlarmPin 6		// PA6
#define BackAlarmPin 7		// PA7
#define WindowAlarmPin 6	// PB6

int main(void)
{
	// enable clock
	GPIOA_enable_clock();
	GPIOB_enable_clock();
	GPIOC_enable_clock();

	// configure inputs
	GPIO_configure_input_mode(GPIOC, ArmButtonPin);
	GPIO_configure_input_mode(GPIOC, DisarmButtonPin);
	GPIO_configure_input_mode(GPIOA, FrontSensorPin);
	GPIO_configure_input_mode(GPIOA, BackSensorPin);
	GPIO_configure_input_mode(GPIOA, WindowSensorPin);

	// enable resistors
	//GPIO_enable_pull_up_resistor(GPIOC, ArmButtonPin);
	GPIO_enable_pull_up_resistor(GPIOC, DisarmButtonPin);
	GPIO_enable_pull_up_resistor(GPIOA, FrontSensorPin);
	GPIO_enable_pull_up_resistor(GPIOA, BackSensorPin);
	GPIO_enable_pull_up_resistor(GPIOA, WindowSensorPin);

	// configure outputs
	GPIO_configure_output_mode(GPIOA, ArmStatusPin);
	GPIO_configure_output_mode(GPIOA, FrontAlarmPin);
	GPIO_configure_output_mode(GPIOA, BackAlarmPin);
	GPIO_configure_output_mode(GPIOB, WindowAlarmPin);

	unsigned int armed = 0;
	while (1)
	{
		// if ArmButton is pressed
		// NOTE: active-low button
		// 1 when not pushed (due to pull-up resistor to prevent floating inputs)
		// 0 when pushed
		if (GPIO_read_pin(GPIOC, ArmButtonPin) == 0)
		{
			// arm system
			armed = 1;
			// turn on ArmStatusPin (LED)
			GPIO_write_pin(GPIOA, ArmStatusPin, 1);
		}

		// if DisarmButton is pressed
		// NOTE: active-low button
		// 1 when not pushed (due to pull-up resistor to prevent floating inputs)
		// 0 when pushed
		if (GPIO_read_pin(GPIOC, DisarmButtonPin) == 0)
		{
			// disarm system
			armed = 0;
			// turn off all the LEDs
			GPIO_write_pin(GPIOA, ArmStatusPin, 0);
			GPIO_write_pin(GPIOA, FrontAlarmPin, 0);
			GPIO_write_pin(GPIOA, BackAlarmPin, 0);
			GPIO_write_pin(GPIOB, WindowAlarmPin, 0);
		}

		// if system is armed, check sensors
		if (armed)
		{
			// check the FrontSensor
			if (GPIO_read_pin(GPIOA, FrontSensorPin)){
				// system armed & front door is open:
				GPIO_write_pin(GPIOA, FrontAlarmPin, 1); // turn on FrontAlarm LED
			}
			// check the BackSensor
			if (GPIO_read_pin(GPIOA, BackSensorPin)){
				// system armed & back door is open:
				GPIO_write_pin(GPIOA, BackAlarmPin, 1); // turn on BackAlarm LED
			}
			// check the WindowSensor
			if (GPIO_read_pin(GPIOA, WindowSensorPin)){
				// system armed & window is open:
				GPIO_write_pin(GPIOB, WindowAlarmPin, 1); // turn on WindowAlarm LED
			}
		}
	}
}
