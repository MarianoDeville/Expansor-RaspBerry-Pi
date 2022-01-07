/****************************************************************************/
/*		ADAPTADOR RASPBERRY PI CON ALIMENTACIÓN E INTERFAZ RS232 - I2C		*/
/*..........................................................................*/
/*			Revisión:				1.00									*/
/*			Proyecto Protel:		SQX00755AG.								*/
/*			PIC:					PIC16F883.								*/
/*			Comunicación:			RS232 - I2C.							*/
/*			Compilador:				MPLAB IDE 8.70 - HI-TECH 9.60.			*/
/*			Checksum:				0X7195 									*/
/*			Fecha de creación:		28/08/2013								*/
/*			Fecha de modificación:	06/11/2013								*/
/*			Autor:					Mariano Ariel Deville.					*/
/****************************************************************************/
/*								MACROS										*/
/*..........................................................................*/
#define	_XTAL_FREQ	20000000		// Frecuencia del reloj - 20Mhz.		*/
#define	ENTRADA		1				//										*/
#define	SALIDA		0				//										*/
#define	ESCLAVO		0				// Modo de trabajo del I2C.				*/
#define	DIRECCION	0x18			// Dirección I2C del dispositivo.		*/
#define	RS232_ON	CREN=1;RCIE=1	// Prendo la recepción RS232.			*/
#define	RS232_OFF	RCIE=0;CREN=0	// Apago la recepción RS232.			*/
/*--------------------------------------------------------------------------*/
/*				Defino los nombres de los pines de E/S						*/
/*..........................................................................*/
#define	LED			RC2				// Led de debug.						*/
#define	BANDERA		RC0				// Bandera para informar lectura.		*/
#define AUX_RASP	RC1				// Auxiliar Raspberry Pi.				*/
/****************************************************************************/
/*							VARIABLES GLOBALES								*/
/*..........................................................................*/
bit estado_i2c;						// Banderas para I2C.					*/
unsigned char pos_i2c;				// Comunicación I2C.					*/
unsigned char str_rs[20],pos_str;	// Comunicación RS232.					*/
/****************************************************************************/
/*							ARCHIVOS CABECERA								*/
/*..........................................................................*/
#include "htc.h"		// Necesario para el compilador.					*/
#include "string.h"		//
#include "Delay.c"		// Rutinas de demoras.								*/
#include "RS232.c"		// Configuración y comunicación puerto serie.		*/
#include "I2C_883.c"	// Manejo del módulo I2C interno del PIC.			*/
#include "DS1307.c"		// Manejo del RTC - En modo maestro solamente.		*/
#include "Interrup.c"	// Manejo de interrupciones.						*/
/****************************************************************************/
__CONFIG(DEBUGDIS & HS & WDTEN & PWRTEN & MCLREN & UNPROTECT & BOREN &
		 IESOEN & FCMEN & LVPDIS & BORV40);
/****************************************************************************/



void main(void)
{
/****************************************************************************/
/*					Configuración de los puertos							*/
/*..........................................................................*/
	ADCON1=0x06;	// Sin entradas analógicas.								*/
	PORTA=0;		// Inicializo todos los puertos a cero.					*/
	PORTB=0;		//														*/
	PORTC=0;		//														*/
/*--------------------------------------------------------------------------*/
	TRISA0=SALIDA;	// Auxiliar al conector J6.								*/
	TRISA1=SALIDA;	// Auxiliar al conector J6.								*/
	TRISA2=SALIDA;	// Auxiliar al conector J6.								*/
	TRISA3=SALIDA;	// Auxiliar al conector J6.								*/
	TRISA4=SALIDA;	// Auxiliar al conector J6.								*/
	TRISA5=SALIDA;	// Auxiliar al conector J6.								*/
/*--------------------------------------------------------------------------*/
	TRISB0=SALIDA;	// Pin no ruteado.										*/
	TRISB1=SALIDA;	// Pin no ruteado.										*/
	TRISB2=SALIDA;	// Pin no ruteado.										*/
	TRISB3=SALIDA;	// Pin no ruteado.										*/
	TRISB4=SALIDA;	// Pin no ruteado.										*/
	TRISB5=SALIDA;	// Pin no ruteado.										*/
	TRISB6=ENTRADA;	// ICSPCLK - Programación y debug.						*/
	TRISB7=ENTRADA;	// ICSPDAT - Programación y debug.						*/
/*--------------------------------------------------------------------------*/
	TRISC0=SALIDA;	// Bandera para disparar la lectura I2C de la Rasp.		*/
	TRISC1=ENTRADA;	// Auxiliar al GPIO de la Raspberry pi - No usado.		*/
	TRISC2=SALIDA;	// Led de debug en la placa.							*/
	TRISC3=ENTRADA;	// I2C - SCL serial clock.								*/
	TRISC4=ENTRADA;	// I2C - SDA serial data.								*/
	TRISC5=SALIDA;	// Pin no ruteado.										*/
	TRISC6=SALIDA;	// RS232 - Salida TX.									*/
	TRISC7=ENTRADA;	// RS232 - Entrada RX.									*/
/****************************************************************************/
/*					TIMER 0 - NO UTILIZADO									*/
/*..........................................................................*/
	T0CS=0;			// Oscilador interno.									*/
	T0SE=0;			// Flanco ascendente.									*/
	PSA=1;			// Asigno el preescaler a WDT.							*/
	PS0=1;			// Configuro el Preescaler.								*/
	PS1=1;			// 														*/
	PS2=1;			// 														*/
/****************************************************************************/
/*					TIMER 1 - NO UTILIZADO									*/
/*..........................................................................*/
	T1CKPS0=1; 		// Preescaler TMR1 a 1:8.								*/
	T1CKPS1=1; 		//														*/
	T1SYNC=1;		// No sincronizo con clock externo.						*/
	T1OSCEN=0;		// Oscilador deshabilitado.								*/
	TMR1CS=1;  		// Reloj interno Fosc/4.								*/
	TMR1IF=0;		// Bajo la bandera de la interrupción.					*/
	TMR1ON=0;		// Apago el TMR1.										*/
	TMR1H=0;		// Configuro el tiempo que tarda en generar				*/
	TMR1L=0;		// la interrupcion.										*/
/****************************************************************************/
/*					TIMER 2 - NO UTILIZADO									*/
/*..........................................................................*/
	TMR2ON=0;		// Timer 2 apagado.										*/
	T2CKPS0=0;		// Configuro el Preescaler.								*/
	T2CKPS1=0;		// 														*/
	TMR2IF=0;		// Bajo la bandera de la interrupción.					*/
/****************************************************************************/
/*					Configuración de las interrupciones						*/
/*..........................................................................*/
	GIE=1;			// Utilizo interrupciones.								*/
	RBIE=0;			// Interrupcion por RB deshabilitada.					*/
	PEIE=1;			// Interrupcion externa habilitada.						*/
	INTE=0;			// Interrupcion RB0/INT deshabilitada.					*/
	T0IE=0;			// Interrupcion desborde TMR0 deshabilitada.			*/
	TMR1IE=0;		// Interrupcion desborde TMR1 habilitada.				*/
	TMR2IE=0;		// Interrupcion desborde TMR2 deshabilitada.			*/
	CCP1IE=0;		// CCP1 Interrupt disable.								*/
	CCP2IE=0;		// CCP2 Interrupt disable.								*/
	C1IE=0;			// Comparator Interrupt disable.						*/
	C2IE=0;			// Comparator Interrupt disable.						*/
	EEIE=0;			// EEPROM Write Operation Interrupt disable.			*/
	SSPIE=1;		// Interrupcion por comunicacion I2C.					*/
	OSFIE=0;		// Interrupción por falla en el oscilador deshabilitada.*/
	ULPWUIE=0;		// Ultra Low-Power Wake-up Interrupt Enable bit.		*/
	BCLIE=0;		// Bus Collision Interrupt disable.						*/
	ADIE=0;			// Interrupcion del conversor AD deshabilitada.			*/
	RCIE=1;			// Interrupcion recepcion USART habilitada.				*/
 	INTEDG=0;		// Interrupcion en el flanco descendente de RB0.		*/
	RBPU=1;			// RB pull-ups estan deshabilitadas.					*/
/****************************************************************************/
	Serial_Setup(9600);				// Inicializo el puerto RS232.
//////////////////////////////////////////////////////////////////////////////////////
/* EL MICRO TRABAJA COMO MAESTRO EN EL I2C Y OBTIENE LA HORA Y LA ENVIA POR RS232.	*/
/*..................................................................................*/
/*
	I2C_Setup(100);
	DelayMs(100);
	DS1307_Setup();
	DelayMs(100);
	strcpy(str_rs,"0611130919");
	Seteo_Hora(str_rs);
	while(1)
	{
		LED=!LED;
		Obtener_Hora(str_rs,1);
		PutStr(str_rs);
		PutStr("\r\n");
		DelayS(2);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
*/

	I2C_Setup(ESCLAVO);				// Inicializo el puerto I2C.
	LED=1;							// Led de la placa prendido.
	while(1)
	{
		if(!RCIE)
		{
			estado_i2c=0;
			pos_i2c=0;
			BANDERA=1;				// Levanto la señal para la Raspberry Pi.
			LED=!BANDERA;			// Reflejo en el led el estado de la bandera a la Raspberry Pi.
			CLRWDT();
			while(!estado_i2c);		// Espero que entre el requerimiento desde la Raspberry Pi.
			BANDERA=0;				// Bajo la señal para la Raspberry Pi.
			LED=!BANDERA;			// Reflejo en el led el estado de la bandera a la Raspberry Pi.
			CLRWDT();
			while(str_rs[pos_i2c]);	// Espero a terminar de transmitir.
			str_rs[0]=0;
			RS232_ON;				// Habilito el RS232 nuevamente.
		}
		CLRWDT();					// Limpio el WatchDog.
	}
}

