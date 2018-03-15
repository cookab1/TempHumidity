/*
 * TempHumidity.c
 *
 * Created: 3/14/2018 9:45:42 AM
 * Author : andyr
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int microsec = 0;
int changeCount = 0;

int humidity = 0;
int temperature = 0;
int checkSum = 0;

void recieveData();

int main(void)
{
    DDRK = 0x1;
    PORTK = 0x1;
    
    while (1) 
    {
        cli();
        //We're going to need... 
        //an interupt handler
        //a timer integration
        
       recieveData();
    }
}

void recieveData() {
    PORTK = 0;
    _delay_ms(5);
    PORTK = 1;
    DDRK = 0;
    
    //start timer
    setupTimer();
    
    sei();
    //ready to recieve the bits
}

ISR(PCINT2_vect) 
    if(((changeCount % 2) == 1) && changeCount > 1) {
        if(changeCount < 34) {
            humidity <<= 1;
            if(microsec > 45)
                humidity |= 1;
        } else if (changeCount < 66) {
            temperature <<= 1;
            if(microsec > 45)
                temperature |= 1;
        } else if (changeCount < 82) {
            checkSum <<= 1;
            if(microsec > 45)
                checkSum |= 1;
        } 
    }
    changeCount++;
    if(changeCount == 82) {
        reset();
    }       
}

ISR(TIMER1_COMPA_vect) {
       microsec++;
}

void reset() {  
    int microsec = 0;
    int changeCount = 0;

    int humidity = 0;
    int temperature = 0;
    int checkSum = 0;
}

void setupTimer() {
    TCCR1A = 0;    // set TCCR1A register to 0
    TCCR1B = 0; // set TCCR1B register to 0 
    
	// turn on CTC mode:
    TCCR1B |= (1 << WGM12);
    
	// enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);
    
    // TOP
    OCR2A = 0x0010;    
    // set compare match register to desired timer count:
    OCR1A = 0x0008;   
    
    microsec = 0;
}

