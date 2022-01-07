/********************************************************************************/
/*				UTILIZACION DEL MODULO I2C DEL PIC 16F883						*/
/********************************************************************************/
/*						PROTOTIPO DE FUNCIONES									*/
/*..............................................................................*/
void I2C_Setup(int modo);
void I2C_Wait_Idle(void);
void I2C_Start(void);
void I2C_RepStart(void);
void I2C_Stop(void);
unsigned char I2C_Read(unsigned char ack);
unsigned char I2C_Write_M(unsigned char i2c_dato);
void I2C_Write_S(unsigned char i2c_dato);
/********************************************************************************/
/*				CONFIGURACION E INICIALIZACION DEL MODULO						*/
/*------------------------------------------------------------------------------*/
/*	modos	=	ESCLAVO															*/
/*			=	velocidad en KHz - es la velocidad con la que va a trabajar 	*/
/*									como maestro. Ej. I2C_Setup(100);			*/
/*	SSPADD en modo maestro es igual a: clock=Osc/(4*(sspad+1)).					*/
/*..............................................................................*/
void I2C_Setup(int modo)
{
	unsigned long calculo;
	CLRWDT();
	TRISC3=1;				// Set SCL and SDA pins as inputs.
	TRISC4=1;
	SSPCON2=0x00;			// No util para el modo esclavo.
	if(modo==ESCLAVO)
	{
		SSPCON=0x36;		// I2C esclavo con 7 bits de dirección.
		SSPADD=DIRECCION<<1;	// Dirección a la cual respondo.
	}
	else
	{
		SSPCON=0x38;		// Set I2C master mode.
		calculo=_XTAL_FREQ/4000;		// Velocidad del puerto.
		calculo=(calculo/modo)-1;
		SSPADD=(unsigned char)calculo;	// Velocidad del puerto.
	}
	CKE=1;					// Use I2C levels worked also with '0'.
	SMP=1;					// Disable slew rate control  worked also with '0'.
	SSPIF=0;				// Clear SSPIF interrupt flag.
	BCLIF=0;				// Clear bus collision flag.
	return;
}
/************************************************************************/
/*					COMPRUEBO QUE LA LINEA ESTÉ DISPONIBLE.				*/
/*......................................................................*/
void I2C_Wait_Idle(void)
{
	while((SSPCON2&0x1F)|RW)
		CLRWDT();
}
/************************************************************************/
/*					COMIENZO LA TRANSMISION.							*/
/*......................................................................*/
void I2C_Start(void)
{
	I2C_Wait_Idle();
	SEN=1;				// Escribo un "start" en el puerto.
}
/************************************************************************/
/*					REINICIALIZO LA TRANSMISION.						*/
/*......................................................................*/
void I2C_RepStart(void)
{
	I2C_Wait_Idle();
	RSEN=1;				// Rescribo un "start" en el puerto.
}
/************************************************************************/
/*					TERMINO LA TRANSMISION.								*/
/*......................................................................*/
void I2C_Stop(void)
{
	I2C_Wait_Idle();
	PEN=1;				// Escribo un "stop" en el puerto.
}
/************************************************************************/
/*					LEO UN CARACTER. 									*/
/*......................................................................*/
unsigned char I2C_Read(unsigned char ack)
{
	unsigned char i2c_dato;
	I2C_Wait_Idle();
	RCEN=1;				// Recepción habilitada.
	I2C_Wait_Idle();
	i2c_dato=SSPBUF;	// Leo el buffer de entrada.
	I2C_Wait_Idle();
	ACKDT=!ack;			// Nivel de ACK que devuelvo.
	ACKEN=1;			// Envio el ACK.
	return(i2c_dato);
}
/************************************************************************/
/*					ESCRIBO UN CARACTER EN MODO MAESTRO.				*/
/*......................................................................*/
unsigned char I2C_Write_M(unsigned char i2c_dato)
{
	I2C_Wait_Idle();
	SSPBUF=i2c_dato;	// Escribo el byte en el puerto.
	return(!ACKSTAT);	// Devuel 1 si la transmisión fue correcta.
}
/************************************************************************/
/*					ESCRIBO UN CARACTER EN MODO ESCLAVO.				*/
/*......................................................................*/
void I2C_Write_S(unsigned char i2c_dato)
{
	SSPBUF=i2c_dato;	// Cargo el byte en el buffer de salida.
	CKP=1;				// Le doy salida al dato por el purto.
	while(BF);			// Espero que se vacíe el buffer.
	return;
}
