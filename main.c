/*
 * LAB 4 digital 2 (maestro).c
 *
 * Created: 1/08/2024 20:52:12
 * Author : diego
 */ 

#define Slave1 0x30
#define Slave2 0x40
#define F_CPU 16000000UL
#include "Librerias/I2CM.h"
#include "Librerias/LCD8b.h"
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
uint8_t buffer = 0;
uint8_t temp = 0;
uint8_t estado = 0;
uint8_t *dato1 = 0;
uint8_t dato1c = 0;
uint8_t dato1u = 0;
uint8_t dato1d = 0;
uint8_t *dato2 = 0;
uint8_t dato2d = 0;
uint8_t dato2u = 0;
uint8_t lol = 0;
int main(void)
{
	I2C_master_init(100000, 1);//inicializar master con fsc 100k Hz, prescaler 1
	initLCD8bits();
	LCD_Set_Cursor(1,1);
	LCD_Write_String("S1:");
	LCD_Set_Cursor(7, 1);
	LCD_Write_String("S2:");
	
	TCCR0B |= (1<<CS02) | (1<<CS00); //prescaler 1024
	DDRC |= (1<<DDD2);
	PORTC &= ~(1<<PORTC2);
	//TCNT0 = 255;
	TIMSK0 |= (1<<TOIE0); //habilitar int timer0
	
    /* Replace with your application code */
    while (1) 
    {
		
		/*buffer = Slave1 << 1; //& 0b11111110;//0 en el ultimo espacio porque vamos a escribir
		buffer &= ~(1<<0);
		temp = I2C_master_write(buffer);
		if (temp != 1){
			I2C_master_stop();
		}
		I2C_master_write('r'); //mandamos el dato
		I2C_master_stop();*/
		//PRIMER ESCLAVO
		
		buffer = 0;
		buffer = Slave1 << 1;
		buffer |= (1<<0); //leer del esclavo
		temp = I2C_master_start();
		estado = 1;
		do{
			if (temp != 1){
				I2C_master_stop();
				LCD_Set_Cursor(10, 2);
				LCD_WriteCaracter(temp);
				estado = 0;
			}
			temp = I2C_master_write(buffer);//leer adc del esclavo1
			if (temp != 1){
				I2C_master_stop();
				TCNT0 = 255;
				PORTC |= (1<<PORTC2);
				LCD_Set_Cursor(12, 2);
				LCD_WriteCaracter(temp);
				estado = 0;
			}
			temp = I2C_master_read(dato1,1);
			if (temp != 1){
				I2C_master_stop();
				TCNT0 = 255;
				PORTC |= (1<<PORTC2);//no se recibió dato
				LCD_Set_Cursor(14, 2);
				LCD_WriteCaracter(temp);
				estado = 0;
			}else{
				I2C_master_stop();
				
				//TCNT0 = 255;
				//PORTC |= (1<<PORTC2);
			}
			
			estado = 0;
		}while(estado == 1);
		//LCD_Write_String(dato1);
		_delay_ms(100);
		dato1c = *dato1/100 + 48;
		dato1d = (*dato1%100)/10 + 48;
		dato1u = *dato1%10 + 48;
		LCD_Set_Cursor(1,2);
		LCD_WriteCaracter(dato1c);
		LCD_Set_Cursor(2,2);
		LCD_WriteCaracter(dato1d);
		LCD_Set_Cursor(3,2);
		LCD_WriteCaracter(dato1u);
		//SEGUNDO ESCLAVO
		buffer = 0;
		buffer = Slave2 << 1;
		buffer |= (1<<0); //leer del esclavo
		temp = I2C_master_start();
		estado = 2;
		do{
			if (temp != 1){
				I2C_master_stop();
				LCD_Set_Cursor(16, 2);
				LCD_WriteCaracter(temp);
				estado = 0;
			}
			temp = I2C_master_write(buffer);//leer adc del esclavo1
			if (temp != 1){
				I2C_master_stop();
				LCD_Set_Cursor(13,2);
				LCD_WriteCaracter(temp);
				TCNT0 = 255;
				PORTC |= (1<<PORTC2);
				estado = 0;
			}
			temp = I2C_master_read(dato2,1);
			if (temp != 1){
				TCNT0 = 255;
				PORTC |= (1<<PORTC2);
				I2C_master_stop();//no se recibió dato
				LCD_Set_Cursor(15,2);
				LCD_WriteCaracter(temp);
				estado = 0;
				}else{
				//TCNT0 = 255;
				//PORTC |= (1<<PORTC2);
				I2C_master_stop();
			}
			
			
			//LCD_Write_String(dato2);
			estado = 0;
			}while(estado == 2);
		_delay_ms(100);
		dato2d = *dato2/10 + 48;
		dato2u = *dato2%10 + 48;
		LCD_Set_Cursor(7,2);
		LCD_WriteCaracter(dato2d);
		LCD_Set_Cursor(8,2);
		LCD_WriteCaracter(dato2u);
    }
}
ISR(TIMER0_OVF_vect){
	TCNT0 = 6; //5ms
	TIFR0 |= (1<<TOV0); //apagar bandera
	lol++;
	if (lol == 200){
		TIMSK0 &= ~(1<<TOIE0);
		PORTC &= ~(1<<PORTC2);
		lol = 0;
	}
}
