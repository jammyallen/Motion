/*
 * ChangePassword.h
 *
 * Created: 28/11/2014 13:38:23
 *  Author: aj329
 */ 


#include <avr/io.h>

void NewPassword()
{
	lcd_WriteString("Enter new password:");
	if(KeyValue != 0xFF)
	{
		NewPasswordEntry();
	}
}

void NewPasswordEntry()
{
	if (count < 4)
	{
		lcd_WriteVariable_withValueAndPositionParameters_SingleDecimalDigit(1, digitPosition, KeyValue);
		for (int i = 200; i>0; i--)
		{
			PORTF = ~PORTF;
			_delay_us(500);
		}
		DebounceDelay();
		digitPosition++;

		password[count] = KeyValue;
		count++;
	}
	
	else
	{
		if (KeyValue == 12)
		{
			for (int i = 1000; i>0; i--)
			{
				PORTF = ~PORTF;
				_delay_us(200);
			}
			CheckCode();
		}
	}
}