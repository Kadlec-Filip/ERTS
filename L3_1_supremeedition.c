#include <avr/io.h>
#include <util/delay.h>


void setPWM(float f){
    f = f < 0 ? 0 : f > 1 ? 1 : f;  // If f < 0  then f = 0, else if f > 1, then f = 1, else f = f
    OCR1A = (uint16_t)(320*f);		// 0CR1A used to create PWM in range 0-320 (ICR1)
}

void setupTimer(){
    // Set PWM frequency to 25khz (no prescale, count to 320)
    // TCCR1A/B are time/counter control 8-bit registers, 
    // Since OC1A, OC1B are 1, they override normal port functionality of I/O pin (DDR must be - and is- set)
    // When OC1A/B are set, they depend on WGM1[3:0] settings. OC1A/B are controlled by COM1A1/COM1B1
    // WGM[3:0] -> TOV1 Flag set on- BOTTOM, OCR1X- TOP, TOP- ICR1
    // CS10 here means NO PRESCALER and ICR1 is TOP set to 320
    // OCR1A/B are Compare registers: compared with time/counter value all time. used by Waveform generator to create PWM 
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);	
    TCCR1B = (1 << CS10) | (1 << WGM13);					
															
    ICR1 = 320;												

    OCR1A = 0;												
    OCR1B = 0;
}

void delay()
{
	// init count - TCNT0 continuously compares 0CR0A/B values
	// wait overflow - TIFR0 is Interrupt flag register TOV0 timer/counter overflow flag.
	// TOV0 dependent on WGM0[2:0] - that is 0, meaning timer/counter mode of operation is NORMAL (TOV flag set on MAX)
	unsigned int i;
	for (i=1;i<=100;i++)
	{
		TCNT0 = 0;						
		while((TIFR0 & (1<<TOV0))==0);	
										
		TIFR0 |= (1<<TOV0); // clear the overflow flag (by writing a '1'!)
	}
}

int main(void)
{
	DDRB = 0b00000010; 		// PB1 is output

	TCCR0A = 0x00;			// Normal mode - normal port operations
	TCCR0B |= (1<<CS02);	// Prescaler = 256  

	setupTimer();
	float output_val = 0.0;

	while(1){
		if(output_val>=1){
			output_val = 0.0;
		}
		setPWM(output_val);
		delay();
		output_val += 0.058;
	}

	return 0;
}