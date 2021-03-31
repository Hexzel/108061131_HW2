// cut off frequency is 245.38 Hz

#include "mbed.h"
#include "uLCD_4DGL.h"
using namespace std::chrono;

DigitalOut led(LED1);
AnalogOut aout(PA_4);
AnalogIn ain(PC_5);
InterruptIn up_btn(D9);
InterruptIn down_btn(D10);
InterruptIn ok_btn(D11);
Timer debounce;
Timeout f_ctrl;
uLCD_4DGL uLCD(D1, D0, D2);

const uint16_t peak = 59577;	// f(v) = 65535 / 3.3 * v -> f(3) = 59577
const uint16_t sample_number = 500;
const uint16_t frq_num = 3;
volatile bool lever;
bool up = 0, down = 0, ok = 0;


void turnDownLever(void);
void pushUp(void);
void pushDown(void);
void pushOk(void);
void boundedChange(uint8_t *num, uint8_t bound, int8_t change);
void moveArrow(uint8_t ith);

int main()
{
	uint16_t v_out_u16;
	float v_in[sample_number] = {0};
	float  frequency[3] = {247.7, 100, 40};
	uint16_t delay_time[3] = {62, 183, 483};		// 62 -> 246 Hz
	uint16_t v_in_index = sample_number;
	bool need_print = 0;
	uint8_t frq_index = 0;
	uint8_t state = 0, pre_state = 0;

	up_btn.rise(&pushUp);
    down_btn.rise(&pushDown);
    ok_btn.rise(&pushOk);
    debounce.start();
	led = 0;
	uLCD.printf("  Frquency table");
	uLCD.locate(0, 2);
	uLCD.printf("247.7 Hz");
	uLCD.locate(0, 4);
	uLCD.printf("100 Hz");
	uLCD.locate(0, 6);
	uLCD.printf("40 Hz");
	uLCD.locate(10, 2);
	uLCD.printf("<-");
	uLCD.locate(0, 14);
	uLCD.printf("    down  up  ok");

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
		if (up || down || ok) {
			if (ok && state == pre_state) {
				v_in_index = 0;
				need_print = 1;
				led = 1;
				uLCD.locate(0, 12);
				uLCD.printf(" sending...      ");
			}
			while (!ok) {
                if (up) {
                    boundedChange(&state, frq_num, 1);
                    up = 0;
                } else if (down) {
                    boundedChange(&state, frq_num, -1);
                    down = 0;
                }
				ThisThread::sleep_for(50ms);
				moveArrow(state);
				uLCD.locate(0, 12);
				uLCD.printf(" press ok to gen  ");
            }
            ok = 0;
			pre_state = state;
			frq_index = state;
			if (!need_print) {
				uLCD.locate(0, 12);
				uLCD.printf(" press ok to send  ");
			}
		}
		if (need_print &&  v_in_index >= sample_number) {
			need_print = 0;
			printf("%f\r\n", frequency[frq_index]);
			for (uint16_t i = 0; i < sample_number; i++) {
				printf("%f\r\n", v_in[i]);
			}
			led = 0;
			uLCD.locate(0, 12);
			uLCD.printf(" sending complete  ");
		}
	}
}

void pushUp(void)
{
    if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 400) {
        up = 1;
        debounce.reset();
    }
}

void pushDown(void)
{
    if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 400) {
        down = 1;
        debounce.reset();
    }
}

void pushOk(void)
{
    if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 400) {
        ok = 1;
        debounce.reset();
    }
}

void turnDownLever(void)
{
	lever = 0;
}

void boundedChange(uint8_t *num, uint8_t bound, int8_t change) {
    change = change % bound;
    if (*num + change < bound && *num + change >= 0) *num += change;
    else if (change > 0) *num = change + *num - bound;
    else if (change < 0) *num = *num + change + bound;
}

void moveArrow(uint8_t ith)
{
	for (uint8_t j = 1; j <= frq_num; j++) {
		uLCD.locate(10, 2 * j);
		uLCD.printf("  ");
	}
	uLCD.locate(10, 2 * (ith + 1));
	uLCD.printf("<-");
}
