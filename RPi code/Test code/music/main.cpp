#include <pigpio.h>
#include <iostream>
#include <stdio.h>

#define music_pin 25

void wave(int delay_us, float seconds);


int main () {
	
	if (gpioInitialise() < 0) {
		printf("setup pigpio failed !");
		return 0;
	}
	
	gpioSetMode(music_pin, PI_OUTPUT);
	
	
	wave(500, 0.3);
	wave(800, 0.2);
	wave(1200, 0.54);
	wave(500, 0.69);
	wave(700, 0.5);
	wave(1400, 0.8);
	wave(1800, 0.5);
	
	return 0;
}

void wave(int delay_us, float seconds) {
	
	int microsecs = seconds * 1000000;
	
	gpioPulse_t pulse[2];
	
	pulse[0].gpioOn = (1<<music_pin);
	pulse[0].gpioOff = 0;
	pulse[0].usDelay = 0.5* delay_us;
	
	pulse[1].gpioOn = 0;
	pulse[1].gpioOff = (1<<music_pin);
	pulse[1].usDelay = 0.5 * delay_us;
	
	gpioWaveAddNew();
	gpioWaveAddGeneric(2, pulse);
	
	int id = gpioWaveCreate();
		
	if (id >= 0) {
		
		std::cout << "Wave created...\n";
		
		gpioWaveTxSend(id, PI_WAVE_MODE_REPEAT);
		
		gpioSleep(PI_TIME_RELATIVE, 0, microsecs);
		
		gpioWaveTxStop();
	}	
}
