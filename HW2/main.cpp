// cut off frequency is 245.38 Hz

#include "mbed.h"
using namespace std::chrono;

DigitalOut led(LED1);
AnalogOut aout(PA_4);
AnalogIn ain(PC_5);
Timeout f_ctrl;
InterruptIn button(USER_BUTTON);

const uint16_t peak = 59577;	// f(v) = 65535 / 3.3 * v -> f(3) = 59577
const uint16_t sample_number = 500;
volatile bool lever;
bool need_sample = 0;

void turnDownLever(void)
{
	lever = 0;
}

void sendData(void)
{
	need_sample = 1;
}

int main()
{
	uint16_t v_out_u16;
	float v_in[sample_number] = {0};
	float  frequency[3] = {247.7, 100, 40};
	uint16_t delay_time[3] = {62, 183, 483};		// 62 -> 246 Hz
	uint16_t v_in_index = sample_number;
	bool need_print = 0;
	uint8_t frq_index = 2;

	button.rise(&sendData);
	led = 0;
	while (1) {
		for (uint16_t i = 0; i < 50 ; i++) {
			f_ctrl.attach_us(&turnDownLever, delay_time[frq_index]);
			lever = 1;
			if (i <= 4) {
				v_out_u16 = (uint16_t)(peak / 5.0 * i);
			} else {
				v_out_u16 = (uint16_t)((49 - i) / 44.0 * peak);
			}
			aout.write_u16(v_out_u16);
			if (v_in_index < sample_number) {
				v_in[v_in_index] = ain;
				v_in_index++;
			}
			while (lever) ;
		}
		if (need_sample) {
			v_in_index = 0;
			need_sample = 0;
			need_print = 1;
			led = 1;
		}
		if (need_print &&  v_in_index >= sample_number) {
			need_print = 0;
			printf("%f\r\n", frequency[frq_index]);
			for (uint16_t i = 0; i < sample_number; i++) {
				printf("%f\r\n", v_in[i]);
			}
			led = 0;
		}
	}
}
