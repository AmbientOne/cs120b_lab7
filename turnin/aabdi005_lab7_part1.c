
/*	Author: Abdirahman Abdi
 *  Partner(s) Name: 
 *	Lab Section: 
 *	Assignment: Lab #6 Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *  https://www.youtube.com/watch?v=qWbC_c4X9K4
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
 TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
unsigned char flag = 0;
unsigned char i = 0;
unsigned char tmp = 0;
enum States{Start, firstState, increment, inc, dec, decrement, reset} state;


void Tick() {
    unsigned char press = ~PINA & 0x03;
    switch(state){
        case Start:
            PORTB = 0x07;
            state = firstState;
            break;

        case firstState:
            if(press == 0x01) 
                state = inc;
            else if(press == 0x02) {
                state = dec;
            }
            else if(press == 0x03){
                PORTB = 0x00;
                state = reset;
            }
            break;

        case inc:
            state = increment;
            i = 0;
            flag = 0;
            break;

        case dec:
            state = decrement;
            i = 0;
            flag = 0;
            break;

        case reset:
            if(press == 0x03) state = reset;
            else state = firstState;
            break;
        
        case increment:
            if(press == 0x01){
                flag = 1;

                if(i == 10) {
                    state = inc;
                }
                else state = increment;
            }
            else if(press == 0x00){
                state = firstState;
            }
            else if(press == 0x02) {
                state = decrement;
            }
            else if(press == 0x03){
                PORTB = 0x00;
                state = reset;
            }
            break;

        case decrement:
            if(press == 0x02){
                flag = 1;

                if(i == 10) {
                    state = dec;
                }
                else state = decrement;
            }
            else if(press == 0x00) state = firstState;
            
            else if(press == 0x01) state = inc;
    
            else if(press == 0x03) state = reset;
    
            break;
            
        default:
            break;
    }
    
    switch(state){
        case Start:
            
            break;
        case firstState:
            break;
        case inc:
            if(PORTB < 9){
                PORTB++;
            }
            flag = 0;
            i = 0;
            tmp = PORTB;
            LCD_ClearScreen();
            LCD_Cursor(1);
          	LCD_WriteData(tmp + '0');
            state = increment;
            break;
        case increment:
            break;
        case dec:
            if(PORTB > 0){
                PORTB--;
            }
            i = 0;
            flag = 0;
            tmp = PORTB;
            LCD_ClearScreen();
            LCD_Cursor(1);
          	LCD_WriteData(tmp + '0');
            state = decrement;
            break;
        case decrement:
            break;
        case reset:
                LCD_ClearScreen();
                LCD_Cursor(1);
			    LCD_WriteData(0 + '0');
                PORTB = 0x00;
            break;
        default:
            break;
    }
 
}
        


int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    TimerSet(100);
    TimerOn();
    LCD_init();
    LCD_Cursor(1);
    LCD_WriteData(0 + '0');
    while (1) {
        Tick();
        while(!TimerFlag);
        TimerFlag = 0;
        if(flag == 1) {
            i++;
        }

    }
    return 1;
}