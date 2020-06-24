/*
 * project.c
 *
 *  Created on: Jun 3, 2020
 *      Author: Dell
 */
#include <avr/io.h>
#include <avr/delay.h>
#include <stdlib.h>
#define F_CPU 8000000U
#define GET(reg,pin) (((reg)>>(pin))&0x01)

int main(void){

	DDRD=0xff; // output  for LCD DATA
	DDRC=0xff; // output for control LCD and motor motion
	DDRB|=(1<<3); //Set OC0 as output to use PWM signal

	/*ADC initialize*/
	DDRA=0x00; //input ADC1
	ADCSRA |= 1<<7 ; //Enable the ADC
	ADCSRA |= 0b00000110; // Set the prescaler to 64
	ADMUX |= 0b11000000; // reference volt is internal volt 2.56
	ADMUX|= 1<< ADLAR; // set adlar =1
	ADMUX |= 0b0000000; // select channel 0
	/*End of initialization*/

	/*LCD initialization*/
	_delay_ms(30) ; // datasheet requirment
	PORTC &= ~(1<<0); // clear RS pin
	PORTC &= ~(1<<1); //clear R/W pin
	PORTC |= (1<<2) ; //enable pin high
	PORTD = 0b00111000; // function set , 8-bit mode , 2 lines interface , 5*7 font
	PORTC &= ~(1<<2) ; //enable pin low
	_delay_ms(5);
	PORTC |= (1<<2) ; //enable pin high
	_delay_us(39); // datasheet requirment

	PORTC &= ~(1<<0); // clear RS pin
	PORTC &= ~(1<<1); //clear R/W pin
	PORTC |= (1<<2) ; //enable pin high
	PORTD = 0b00001111; // Display on, cursor on , cursor blinking on
	PORTC &= ~(1<<2) ; //enable pin low
	_delay_ms(5);
	PORTC |= (1<<2) ; //enable pin high
	_delay_us(39);// datasheet requirment

	PORTC &= ~(1<<0); // clear RS pin
	PORTC &= ~(1<<1); //clear R/W pin
	PORTC |= (1<<2) ; //enable pin high
	PORTD = 0x01; // Display clear
	PORTC &= ~(1<<2) ; //enable pin low
	_delay_ms(5);
	PORTC |= (1<<2) ; //enable pin high
	_delay_ms(2); //datasheet requirment

	PORTC &= ~(1<<0); // clear RS pin
	PORTC &= ~(1<<1); //clear R/W pin
	PORTC |= (1<<2) ; //enable pin high
	PORTD = 0b00000110; // entery mode set, increment mode, shift off
	PORTC &= ~(1<<2) ; //enable pin low
	_delay_ms(5);
	PORTC |= (1<<2) ; //enable pin high
	/*End of initialization*/

	/*PWM initialization*/
	TCCR0=0b01110101;// phase correct pwm , inverted pwm ,prescaler 1024
	TIMSK=0;
	OCR0=0;//Set OCR0 to 255 , duty cycle=0;
	/*end of initialization*/

	/*Set the motor to move clk wise*/
	PORTC &= ~(1<<6);
	PORTC|=(1<<7);
	char a[4];
	unsigned int temp_read;
	int i=0;
	while(1){
		i=0;
		ADCSRA |=(1<<ADSC) ; // STrat the conversion
		while (!(GET(ADCSRA,ADIF))); // wait till conversion is done
		temp_read=ADCH; // read the tempreture value
		itoa(temp_read,a,10); // convert from integer to string
		/*Display data*/
		PORTC |= (1<<0); // set RS pin
		PORTC &= ~(1<<1); //clear R/W pin
		PORTC |= (1<<2) ; //enable pin high
		/*As long as there is data in the array , the element is not string delemater , print the data*/
		while(a[i]!='\0'){
			PORTD=a[i];
			PORTC &= ~(1<<2) ; //enable pin low
			_delay_ms(5);
			PORTC |= (1<<2) ; //enable pin high
			_delay_ms(10); // for displaying
			i++;
		}

		PORTD=0b11011111;//° ASCII
		PORTC &= ~(1<<2) ; //enable pin low
		_delay_ms(5);
		PORTC |= (1<<2) ; //enable pin high
		_delay_ms(10); // for displaying

		PORTD='C';
		PORTC &= ~(1<<2) ; //enable pin low
		_delay_ms(5);
		PORTC |= (1<<2) ; //enable pin high
		_delay_ms(10); // for displaying


		/*Control Fan*/
		if(temp_read<5){
			OCR0=255;//0% duty cycle
		}
		else if(temp_read<10){
			OCR0=229;//10% duty cycle
		}
		else if(temp_read<15){
			OCR0=178; //30% duty cycle
		}
		else if(temp_read<20){
			OCR0=102; //60% duty cycle
		}
		else if(temp_read<25){
			OCR0=25; //90%duty cycle
		}
		else{
			OCR0=0; //100% duty cycle
		}

		/*Clear screen*/
		_delay_ms(400);
		PORTC &= ~(1<<0); // clear RS pin
		PORTC &= ~(1<<1); //clear R/W pin
		PORTC |= (1<<2) ; //enable pin high
		PORTD = 0x01; // Display clear
		PORTC &= ~(1<<2) ; //enable pin low
		_delay_ms(5);


	}
}

