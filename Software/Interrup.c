/********************************************************************************/
/*								INTERRUPCIONES									*/
/********************************************************************************/
void interrupt isr(void)
{
	unsigned char resp;
	if(RCIE && RCIF)				// Interrupcion por RS232?
	{
		resp=RCREG;					// Vac�o el buffer del n�dulo RS232.
		if(resp>31 && resp<126)		// Caracter imprimible?
		{
			str_rs[pos_str]=resp;	// Lleno la cadena con los byte que llegan.
			if(pos_str<16)			// Queda lugar en la cadena?
				pos_str++;			// Voy a la siguiente posici�n.
			return;
		}
		if(resp<14 && pos_str>4)	// Con culquier caracter de control salgo.
		{
			str_rs[pos_str]=0;		// Marco el final de cadena.
			pos_str=0;				// Reseteo la variable de posici�n.
			RS232_OFF;				// Deshabilito la interrupci�n del puerto.
		}
		return;
	}
	if(SSPIE && SSPIF)				// Interrupci�n por I2C
	{
		resp=SSPBUF;				// Cargo el dato que est� en el buffer.
		SSPIF=0;					// Bajo la bandera de la interrupci�n.
		if(DA && !resp)				// Lo recibido es un dato? y es igual a 0x00.
		{
			estado_i2c=1;			// En la Proxima transmito.
			return;
		}
		if(DA)						// Es un dato?
			return;
		if(RW && estado_i2c)		// Van a leer informaci�n?
			I2C_Write_S(str_rs[pos_i2c++]); // Escribo en el puerto I2C.
	}
	return;							// Salgo de las interrupciones.
}
