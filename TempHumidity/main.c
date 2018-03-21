/*
 * TempHumidity.c
 *
 * Created: 3/14/2018 9:45:42 AM
 * Author : andyr
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "PSerial.h"
#include "string.h"
#include "EmSys.h"
#include "stdio.h"

volatile int microsec = 0;
volatile int changeCount = 0;

volatile int humidity = 0;
volatile int temperature = 0;
volatile int checkSum = 0;

static int port_mask = 1;

unsigned char portNum;
long baud;
int framingParam;

void recieveData();
void reset();
void setupTimer();
void setupSerial();
void processData();
void print();
char * toString(int);
char * toString_rec(int, int, char *);

int main(void)
{
	PCICR = 1; //enable group interrupts on PORTB
	PCMSK0 |= (1 << port_mask);
	setupSerial();
	setupTimer();
	
    while (1) 
    {
        cli();
        //We're going to need... 
        //an interrupt handler
        //and timer integration
		
		DDRB = (1 << port_mask);
		PORTB |= (1 << port_mask);
		
        recieveData();
		processData();
		reset();
	}
}

void recieveData() {
    PORTB &= 0;
    _delay_ms(5);
    PORTB |= (1 << port_mask);
    DDRB = 0;
    
    microsec = 0;
    //char temp[3];
    //sprintf(temp, "%d", microsec);
    //print(temp);
	
    sei();
    //ready to receive the bits
	_delay_ms(3000);
	cli();
}

ISR(PCINT0_vect) {
	//char temp[3];
	//sprintf(temp, "%d", microsec);
	//print(temp);
    if(((changeCount & 1) == 1) && changeCount > 1) {
		print("gets here");
        if(changeCount < 34) {
            humidity <<= 1;
            if(microsec > 45) {
                humidity |= 1;
				}
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
    if(changeCount == 83) {
        //This is the end of the transmission
    }       
}

ISR(TIMER1_COMPA_vect) {
    microsec++;
}

void reset() {
	//print(reset);
    changeCount = 0;

    humidity = 0;
    temperature = 0;
    checkSum = 0;
}

//This method needs to calculate the humidity and temperature and print to the terminal
void processData() {
	char temp[3];
	sprintf(temp, "%d", humidity);
	print(temp);
	sprintf(temp, "%d", temperature);
	print(temp);
	
	sprintf(temp, "%d", checkSum);
	print(temp);
}

void setupTimer() {
    TCCR1A = 0;    // set TCCR1A register to 0
    TCCR1B = 0; // set TCCR1B register to 0 
    
	// turn on PWM mode:
    TCCR1B |= (1 << WGM02);
	
    // Set CS12 bit so timer runs at clock speed: (no pre-scaling)
    TCCR1B |= (1 << CS10); // Sets bit CS10 in TCCR1B
    
    // TOP
    OCR0A = 0x0010;    
    // set compare match register to desired timer count:
    OCR1A = 0x0008;
    
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);
    
}

void setupSerial() {	
	portNum = 0;
	baud = 19200L;
	framingParam = SERIAL_8N1;
	
	PSerial_open(portNum, baud, framingParam);
}

void print(char *str) {
	print_String(portNum, str);
}

