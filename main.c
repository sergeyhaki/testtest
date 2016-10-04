#define F_CPU			16000000

#include <math.h> 
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#define MYUBRR 			F_CPU/16/BAUD-1
#define BAUD 			9600

#define VERSION			3

#define EVENT_INT0			2
#define EVENT_INT1			3
#define EVENT_COMPA			4


//*****ФАЙЛЫ УСТРОЙСТВ***********************************************
//#include "dev/usart_m88.c"
//#include "dev/iic.c"
//#include "dev/ht1613.c"
//#include "dev/74HC595.c"
//#include "dev/PCF8563.c"
//#include "dev/PCF8574.c"
//#include "dev/DS1621.c"
#include "dev/LCD4.c"


//*****ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ******************************************/
static FILE lcd_stdout = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
//static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);volatile uint8_t Event = 0;

volatile uint8_t Event;
volatile uint32_t Counter = 0;
uint32_t temp;
uint32_t freq;

//***** ПРЕРЫВАНИЯ **********************************************************
	
ISR(TIMER0_OVF_vect)
{
	Counter++;
}

ISR(INT0_vect)
{
	Event = EVENT_INT0;
}

ISR(INT1_vect)
{
	Event = EVENT_INT1;
}

//*****ФУНКЦИИ*******************************************************
void startDebug(void)
{	
	LCD_XY(0,0);
	printf("DEBUG VERSION %i\n",VERSION);
	_delay_ms(1000);
}

void resetHC161(void)
{
	_delay_ms(300);
	PORTB &= ~(1<<PB0);
	_delay_ms(300);
	PORTB |= (1<<PB0);
}			

//*****MAIN**********************************************************
int main()
{
	//порты ввода-вывода
	DDRB = (1<<PB0)|(1<<PB1);	
	PORTB |= (1<<PB0);
	
	//подтяжка INT0 INT1;	
	PORTD |= (1<<PD2)|(1<<PD3);
	
	_delay_ms(100);
	
	//инициализация HD77480
	LCD_Init();	
	stdout = &lcd_stdout;
	printf("Hello!\n");
	
	//*****TIMER1  ctc mode output>OC1A*****
	TCCR1A = (1<<COM1A0);		//изменять значение выхода при совпадении
	TCCR1B = (1<<WGM12)|5;		//CTC mode - F_CPU/1024  
	OCR1A = 15624;	 			// 1sec 
	
	//разрешение прерывания по совпадению А
	//TIMSK1 = (1<<OCIE1A);
	
	//*****TIMER0 counter mode*****		
	TCCR0 = 7;				//по фронту внешнего сигнала		
	TIMSK = (1<<TOIE0);		//разрешение прерывания по переполнению
	
	//*****INT0/INT1*****
	GICR |= (1<<INT0)|(1<<INT1);	//разрешение прерывания по INT0/INT1
	MCUCR |= (1<<ISC01);			//падение -\_ вызывает прерывание EICRA
		
	//глобальное разрешение прерывания 
	sei();	
	
	//debug	
	startDebug();
		
	//end debug
	
	while(1)
	{
		if (Event == EVENT_INT0)
		{
			
			freq = (Counter * 256) + TCNT0;
			LCD_XY(0,0);
			printf("TCN=%i00,TCNT0);
			printf(" Co=%lu       \n",Counter);
			printf("Freq=%luHz      ",freq);
			
			resetHC161();
			
			TCNT0 = 0;
			Counter = 0;
			Event = 0;			
			
		}//end if	
	}//end while	
}//end main




/*
ISR(TIMER1_COMPA_vect)
{
	Event = EVENT_COMPA;
}


		switch(Event) 
		{
			case EVENT_INT0:
			//
			temp = TCNT0;
			
			freq = (Counter * 256) + (temp);
			//freq = Counter * 4096;			
			//freq = freq + (temp << 4);
			

			if(PIND & (1<<PD0))	
				freq = freq + 1;				
			if(PIND & (1<<PD1))	
				freq = freq + 2;				
			if(PIND & (1<<PD6))	
				freq = freq + 4;
			if(PIND & (1<<PD7))	
				freq = freq + 8;			
	
						
			LCD_XY(0,0);
			printf("TCN=%lu",temp);
			printf(" Co=%lu       \n",Counter);
			printf("Freq=%luHz      ",freq);
			//
						
			//reset hc161
			_delay_ms(10);
			PORTB &= ~(1<<PB0);
			_delay_ms(10);
			PORTB |= (1<<PB0);
			//
			freq = 0;
			TCNT0 = 0;
			Counter = 0;			
			Event = 0;			
			break;
		}//end switch		
	}//end while

	
	temp = 36;
	Counter = 273;
	freq = (Counter * 4096) + (temp * 36);
	//freq = 250000;
	LCD_XY(0,0);
	printf("TCN=%lu",temp);
	printf(" Co=%lu       \n",Counter);
	printf("Freq=%luHz      ",freq);
	_delay_ms(10000);



	//разрешение прерывания по совпадению А
	TIMSK1 = (1<<OCIE1A)|(1<<OCIE1B);


			freq = (Counter << 8) + TCNT0;			
			LCD_XY(0,0);
			printf("TCN=%i",TCNT0);
			printf(" Co=%i       \n",Counter);
			printf("Freq=%luHz      ",freq);			
			TCNT0 = 0;
			Counter = 0;
			Event = 0;


ISR(TIMER1_COMPA_vect)
{
	//PORTB ^= (1<<PB1);
	PORTB |= (1<<PB1);
	
	TCNT0Save = TCNT0;
	TCNT0 = 0;
	
	CounterSave = Counter;
	Counter = 0;	
}

		//проверяем флаг совпадения А таймера 1
		if (TIFR1 & (1<<OCF1A))
		{
			cli();
			PORTB &= ~(1<<PB0);
			
			freq = (Counter << 8) + TCNT0;			
			LCD_XY(0,0);
			printf("TCN=%i",TCNT0);
			printf(" Co=%i       \n",Counter);
			printf("Freq=%luHz      ",freq);			
			TCNT0 = 0;
			Counter = 0;
			//СБРОС ФЛАГА
			TIFR1 = (1<<OCF1A);	
			sei();		
		}

		//freq = (CounterSave << 8) + TCNT0Save;
		
		//LCD_XY(0,0);
		//printf("TCN=%i",TCNT0Save);
		//printf(" Co=%i       \n",CounterSave);
		//printf("Freq=%luHz      ",freq);
		//_delay_ms(1000);

	//ставим признак события совпадение
	Event = 1;
	PORTB ^= (1<<PB1);
	for (uint8_t i=0;i<=60; i++)
	{
		LCD_XY(0,1);
		printf("n=%d     ", i);
		_delay_ms(1000);	
	}
	
	
	

	printf("ttt!\n");
	
	//инициализация USART
	//USART_Init(MYUBRR);	
	//stdout = &uart_stdout;
	
	
	//printf("LCD4_DDR %d \n",LCD4_DDR);
	//printf("LCD4_DDR_E %d \n",LCD4_DDR_E);
	
		if (Event == 1){
			cli();//запрещаем прерывания
			PORTB ^= (1<<PB0);
			
			freq = (CountOvf<<8) + TCNT0;	

			
			LCD_XY(0,0);			
			printf ("F=%lu Hz          ", freq);			
			//printf ("CountOvf=%d", CountOvf);
			printf ("\nTCNT0= %d  ", TCNT0);			
			printf ("%d  ", count++);
			
			
			CountOvf = 0;
			TCNT1 = 0; // начинаем счет с начала
			TCNT0 = 0; // начинаем счет с начала			
			Event = 0; // признак ожидания события
			PORTB ^= (1<<PB0);
			sei();

*/



