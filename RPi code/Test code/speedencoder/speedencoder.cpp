/* compile: gcc switch_hall.c –lwiringPi
*  run: sudo ./a.out
*/
#include <wiringPi.h>
#include <stdio.h>

#define SIG 0

int main(void)
{
	if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return 1; 
	}
	pinMode(SIG, INPUT);
	//LED("GREEN");

	while(1){
		
		if(0 == digitalRead(SIG)){
			//delay(10);
			if(0 == digitalRead(SIG)){
				//LED("RED"); 
				printf("Magnet Detected!\n"); 
			}
		}
		else if(1 == digitalRead(SIG)){
			//delay(10);
			if(1 == digitalRead(SIG)){
				while(!digitalRead(SIG));
				//LED("GREEN");
				printf("Nothing...\n");				
			}
		}
	}
	return 0;
}



