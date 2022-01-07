/********************************************************************************/
/*								INTERRUPCIONES									*/
/********************************************************************************/
void interrupt isr(void)
{
	unsigned char resp;
	if(RCIE && RCIF)				// Interrupcion por RS232?
	{
		resp=RCREG;					// Vacío el buffer del nódulo RS232.
		if(resp>31 && resp<126)		// Caracter imprimible?
		{
			str_rs[pos_str]=resp;	// Lleno la cadena con los byte que llegan.
			if(pos_str<16)			// Queda lugar en la cadena?
				pos_str++;			// Voy a la siguiente posición.
			return;
		}
		if(resp<14 && pos_str>4)	// Con culquier caracter de control salgo.
		{
			str_rs[pos_str]=0;		// Marco el final de cadena.
			pos_str=0;				// Reseteo la variable de posición.
			RS232_OFF;				// Deshabilito la interrupción del puerto.
		}
		return;
	}
	if(SSPIE && SSPIF)				// Interrupción por I2C
	{
		resp=SSPBUF;				// Cargo el dato que está en el buffer.
		SSPIF=0;					// Bajo la bandera de la interrupción.
		if(DA && !resp)				// Lo recibido es un dato? y es igual a 0x00.
		{
			estado_i2c=1;			// En la Proxima transmito.
			return;
		}
		if(DA)						// Es un dato?
			return;
		if(RW && estado_i2c)		// Van a leer información?
			I2C_Write_S(str_rs[pos_i2c++]); // Escribo en el puerto I2C.
	}
	return;							// Salgo de las interrupciones.
}
