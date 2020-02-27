/*
 * main.c
 *
 *  Created on: 5 Oct 2015
 *      Author: EmbeddedFab
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "MCAL/UART.h"
#include <avr/interrupt.h>

#define MQTT         	   0
#define HTTP               1

#define Client_ID      "11232siy1weu1e21"        //16 digits
#define User_Name      "nmr"                     //3 digits
#define Password       "GXQD8VN78RAISAUH"        //MQTT Key

#define Channel_ID     "996736"                  //Your Channel ID
#define Write_Key      "RNNN90XKATASUZZG"        //Api Write Key
#define Data           "5"       				 //Data

#if MQTT
#define Publish       0
#define Subscribe     1
#endif

#if HTTP
#define Uploade_Data       1
#define Get_Last_reading   0
#endif

#define TRUE 1
#define FALSE 0
#define DEFAULT_BUFFER_SIZE 100
char Rec_Data[DEFAULT_BUFFER_SIZE];
char Counter=0;


char Check_Respond(char * Expected_Respond);
char Check_Word_in_Respond(char * Word);
void Clear_REC_Buffer(void);


ISR (USART_RXC_vect)
{
	uint8_t oldsrg = SREG;
	cli();
	Rec_Data[Counter] = UDR;
	Counter++;
	if(Counter == DEFAULT_BUFFER_SIZE){
		Counter = 0; //pointer = 0;
	}
	SREG = oldsrg;
}


int main()
{


	DDRD=(1<<PD4);
	init_UART();
	sei();

	Counter=0;


	_delay_ms(3000);


	UART_SEND_string("+++");
	//	while(!Check_Respond("a"))
	//	{
	//		_delay_ms(1);
	//	}

	_delay_ms(1000);
	UART_SEND_string("a");
	//	while(!Check_Respond("+ok"))
	//	{
	//		_delay_ms(1);
	//	}

	_delay_ms(3000);

	UART_SEND_string("AT+E=ON\r");
	//	while(!Check_Respond("\r\n+OK\r\n"))
	//	{
	//		_delay_ms(1);
	//	}

	_delay_ms(3000);
	UART_SEND_string("AT+WANN=DHCP\r\n");
	//	while(!Check_Respond("\r\n+OK"))
	//	{
	//		_delay_ms(1);
	//	}


	_delay_ms(3000);

#if HTTP
	UART_SEND_string("AT+SOCK=TCPC,api.thingspeak.com,80\r\n");
#endif

#if MQTT
	UART_SEND_string("AT+SOCK=TCPC,mqtt.thingspeak.com,1883\r\n");
#endif

	// 	while(!Check_Respond("\r\n+OK"))
	// 	{
	// 		_delay_ms(1);
	// 	}



	_delay_ms(3000);
	UART_SEND_string("AT+Z\r");
	// 	 	while(!Check_Respond("\r\n+OK"))
	// 	 	{
	// 	 		_delay_ms(1);
	// 	 	}


	_delay_ms(3000);

#if Uploade_Data
	UART_SEND_string("GET /update?api_key=");
	UART_SEND_string(Write_Key);

	UART_SEND_string("&field2=");
	UART_SEND_string(Data);
	UART_SEND_string("\r\n");


#endif

#if Get_Last_reading

	UART_SEND_string("GET /channels/");
	UART_SEND_string(Channel_ID);
	UART_SEND_string("/feeds/last.txt\r\n");

	_delay_ms(3000);

#endif

#if MQTT

	UART_SendChar(0x10);
	UART_SendChar(0x33);
	UART_SendChar(0x00);
	UART_SendChar(0x04);
	UART_SEND_string("MQTT");
	UART_SendChar(0x04);
	UART_SendChar(0xC2);
	UART_SendChar(0x00);
	UART_SendChar(0x3C);
	UART_SendChar(0x00);
	UART_SendChar(0x10);
	UART_SEND_string(Client_ID);
	UART_SendChar(0x00);
	UART_SendChar(0x03);
	UART_SEND_string(User_Name);
	UART_SendChar(0x00);
	UART_SendChar(0x10);
	UART_SEND_string(Password);




#if Publish

	UART_SendChar(0x30);
	UART_SendChar(0x39);
	UART_SendChar(0x00);
	UART_SendChar(0x36);

	UART_SEND_string("channels/");
	UART_SEND_string(Channel_ID);
	UART_SEND_string("/publish/fields/field1/");
	UART_SEND_string(Write_Key);
	UART_SEND_string(Data);

#endif

#if Subscribe

	UART_SendChar(0x82);
	UART_SendChar(0x2c);
	UART_SendChar(0x00);
	UART_SendChar(0x0A);
	UART_SendChar(0x00);
	UART_SendChar(0x27);
	UART_SEND_string("channels/");
	UART_SEND_string(Channel_ID);
	UART_SEND_string("/subscribe/fields/field1");
	UART_SendChar(0x00);

#endif

#endif

return 0;
}



char Check_Respond(char * Expected_Respond)
{
	char Respond_Length=0;

	Respond_Length=strlen(Expected_Respond);

	if(strncmp(Rec_Data,Expected_Respond, Respond_Length)==0)
	{
		Clear_REC_Buffer();
		return TRUE;
	}
	return FALSE;


}


char Check_Word_in_Respond(char * Word)
{


	if(strstr(Rec_Data,Word) != 0)
	{
		Clear_REC_Buffer();
		return TRUE;
	}

	return FALSE;
}

void Clear_REC_Buffer(void)
{
	Counter=0;
	memset(Rec_Data,0,DEFAULT_BUFFER_SIZE);
}







//
