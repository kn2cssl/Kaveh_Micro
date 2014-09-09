/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#define F_CPU 32000000UL
#include <util/delay.h>
#include "initialize.h"
#include "nrf24l01_L.h"
#include "transmitter.h" //warning mide az sendnewdata va senddata
#include <stdlib.h>

unsigned char Buf_Rx_L[_Buffer_Size] ;//= "00000000000000000000000000000000";
char Buf_Tx_L[_Buffer_Size];// = "abcdefghijklmnopqrstuvwxyz012345";
char Address[_Address_Width] = { 0x11, 0x22, 0x33, 0x44, 0x55};//pipe0 {0xE7,0xE7,0xE7,0xE7,0xE7};////

char ctrlflg=0;
int i=0;

int main (void)
{
	En_RC32M();

	//Enable LowLevel & HighLevel Interrupts
	PMIC_CTRL |= PMIC_HILVLEN_bm | PMIC_LOLVLEN_bm |PMIC_MEDLVLEN_bm;

	PORT_init();
	TimerD0_init();
	TimerC0_init();
	USARTE0_init();
	ADCA_init();
	//wdt_enable();

	// Globally enable interrupts
	sei();

	Address[0]=Address[0] + RobotID ;

	///////////////////////////////////////////////////////////////////////////////////////////////Begin NRF Initialize
	NRF24L01_L_CE_LOW;       //disable transceiver modes

	SPI_Init();

	_delay_us(10);
	_delay_ms(100);      //power on reset delay needs 100ms
	NRF24L01_L_Clear_Interrupts();
	NRF24L01_L_Flush_TX();
	NRF24L01_L_Flush_RX();
	NRF24L01_L_CE_LOW;
	if (RobotID < 3)
	NRF24L01_L_Init_milad(_TX_MODE, _CH_0, _2Mbps, Address, _Address_Width, _Buffer_Size, RF_PWR_MAX);
	else if(RobotID > 2 && RobotID < 6)
	NRF24L01_L_Init_milad(_TX_MODE, _CH_1, _2Mbps, Address, _Address_Width, _Buffer_Size, RF_PWR_MAX);
   else if(RobotID > 5 && RobotID < 9)
	NRF24L01_L_Init_milad(_TX_MODE, _CH_2, _2Mbps, Address, _Address_Width, _Buffer_Size, RF_PWR_MAX);
	else
	NRF24L01_L_Init_milad(_TX_MODE, _CH_3, _2Mbps, Address, _Address_Width, _Buffer_Size, RF_PWR_MAX);
	
	NRF24L01_L_WriteReg(W_REGISTER | DYNPD,0x01);
	NRF24L01_L_WriteReg(W_REGISTER | FEATURE,0x06);

	NRF24L01_L_CE_HIGH;
	_delay_us(130);
	/////////////////////////////////////////////////////////////////////////////////////////////END   NRF Initialize
	while(1)
	  {    
		  //LED_Red_PORT.OUTSET = LED_Red_PIN_bm;
		 // LED_White_PORT.OUTSET = LED_White_PIN_bm;
		  //LED_Green_PORT.OUTSET = LED_Green_PIN_bm;
		  //Buzzer_PORT.OUTSET = Buzzer_PIN_bm;
		    asm("wdr");
		    if (ctrlflg)
		    {
			    ctrlflg = 0;
			    NRF24L01_L_Write_TX_Buf(Buf_Tx_L,_Buffer_Size);
			    NRF24L01_L_RF_TX();
		    }
		    _delay_us(1);
	  }
}

ISR(PORTD_INT0_vect)////////////////////////////////////////PTX   IRQ Interrupt Pin
{
	uint8_t status_L = NRF24L01_L_WriteReg(W_REGISTER | STATUSe, _TX_DS|_MAX_RT|_RX_DR);
	  
	if((status_L & _RX_DR) == _RX_DR)
	{
		LED_White_PORT.OUTTGL = LED_White_PIN_bm;
		//1) read payload through SPI,
		NRF24L01_L_Read_RX_Buf(Buf_Rx_L, _Buffer_Size);
		
		if(Buf_Rx_L[0] == RobotID)
		{
			//Robot_D[RobotID].RID = Buf_Rx_L[0];
			//Robot_D[RobotID].M0a  = Buf_Rx_L[1];
			//Robot_D[RobotID].M0b  = Buf_Rx_L[2];
			//Robot_D[RobotID].M1a  = Buf_Rx_L[3];
			//Robot_D[RobotID].M1b  = Buf_Rx_L[4];
			//Robot_D[RobotID].M2a  = Buf_Rx_L[5];
			//Robot_D[RobotID].M2b  = Buf_Rx_L[6];
			//Robot_D[RobotID].M3a  = Buf_Rx_L[7];
			//Robot_D[RobotID].M3b  = Buf_Rx_L[8];
			//Robot_D[RobotID].KCK = Buf_Rx_L[9];
			//Robot_D[RobotID].CHP = Buf_Rx_L[10];
			//Robot_D[RobotID].ASK = Buf_Rx_L[11];
			//Robot_D[RobotID].P = Buf_Rx_L[12];
			//Robot_D[RobotID].I = Buf_Rx_L[13];
			//Robot_D[RobotID].D = Buf_Rx_L[14];

		}

		//2) clear RX_DR IRQ,
		//NRF24L01_R_WriteReg(W_REGISTER | STATUSe, _RX_DR );
		//3) read FIFO_STATUS to check if there are more payloads available in RX FIFO,
		//4) if there are more data in RX FIFO, repeat from step 1).
	}
	if((status_L&_TX_DS) == _TX_DS)
	{   LED_Red_PORT.OUTTGL = LED_Red_PIN_bm;
		//NRF24L01_R_WriteReg(W_REGISTER | STATUSe, _TX_DS);
	}
	if ((status_L&_MAX_RT) == _MAX_RT)
	{
		LED_Green_PORT.OUTTGL = LED_Green_PIN_bm;
		NRF24L01_L_Flush_TX();
		//NRF24L01_R_WriteReg(W_REGISTER | STATUSe, _MAX_RT);
	}
}

char timectrl;
ISR(TCD0_OVF_vect)
{
	wdt_reset();
	timectrl++;
	if (timectrl>=20)
	{
		ctrlflg=1;
		timectrl=0;
		i++;
	}
};