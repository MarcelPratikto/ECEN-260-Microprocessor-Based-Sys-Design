// Callback: timer has rolled over
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 // Check which version of the timer triggered this callback and toggle LED
 if (htim == &htim16)
 {
	 int PWM_PERIOD = 40000;
	 int ADC_RANGE = 4096; // 2^12 (12-bit resolution)
	 // Start ADC Conversions
	 HAL_ADC_Start(&hadc1);
	 HAL_ADC_Start(&hadc2);
	 HAL_ADC_Start(&hadc3);
	 // Wait for ADC conversions to complete
	 HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	 HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
	 HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
	 // Read ADC values
	 uint16_t knob_measurement = HAL_ADC_GetValue(&hadc1);
	 uint16_t xjoy_measurement = HAL_ADC_GetValue(&hadc2);
	 uint16_t yjoy_measurement = HAL_ADC_GetValue(&hadc3);
	 // Convert ADC levels to a fraction of total
	 float knob_value = ((float) knob_measurement) / ADC_RANGE;
	 float xjoy_value = ((float) xjoy_measurement) / ADC_RANGE;
	 float yjoy_value = ((float) yjoy_measurement) / ADC_RANGE;
	 // Write the PWM duty cycle values for the tri-color RGB LED
	 TIM3->CCR1 = (int) (knob_value * PWM_PERIOD); // red
	 TIM3->CCR2 = (int) (xjoy_value * PWM_PERIOD); // green
	 TIM3->CCR3 = (int) (yjoy_value * PWM_PERIOD); // blue
	 // Use the three single-color LEDs as the three
	 // most-significant bits of the knob measurement
	 // PA10:	bit 11
	 // PB5:	bit 10
	 // PA8:	bit 9
	 // 1110 0000 0000
	 int bits = knob_measurement >> 9;
	 int bit11 = (bits & 0b100) >> 2;
	 int bit10 = (bits & 0b010) >> 1;
	 int bit9 = (bits & 0b001) >> 0;

	 if (bit11){
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	 } else {
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	 }

	 if (bit10){
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	 } else {
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	 }

	 if (bit9){
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	 } else {
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	 }
 }
}
