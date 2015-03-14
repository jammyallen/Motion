/************************Coursework 1.0*****************************
 * Developer: James Allen
 * User ID: aj329
 * Date: 15/10/14
 * Course: COMP1659 Introduction to Smart Systems
 *
 * Program to combine a PIR sensor with a Keypad to create a motion detection
 * alarm with a security code. The alarm will give 10 seconds for the code to
 * be entered, if no code is entered an alarm will sound until the correct
 * code is entered.
 *********************************************************************/ 

#include <avr/io.h>			// Default include path is C:\WinAVR\avr\include\avr
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "LCD_LibraryFunctions_1281.h"
#include "Keypad_Input.h"
#include "EPROM.h"

unsigned char alarm = 0;
unsigned char initialStartup = 1;
unsigned char count, KeyValue, time;
unsigned char input[4];
unsigned char password[4];


int main(void) //Initial method
{		
	InitialiseGeneral();
	Menu();
}

int RunCode() //Stage of program when the motion sensor has been turned on
{
    lcd_Clear();
	lcd_SetCursor(0x00);
	lcd_WriteString("Alarm Set");
	alarm = 0;
	count = 0; //resets the count to 0
	KeyValue=0xFF;
	
    while(1)
    {	
 		KeyValue = ScanKeypad();

		if (alarm == 2) //for some reason when the PIR is initialised it automatically jumps into the interrupt vector once,
						//after this the PIR works successfully. 
		{			
			time = 10; //amount of seconds to leave for password input
			alarm = 0;
			Timer();
			MotionDetected();
		}
	}
}

void PasswordEntry() //Reads in the password entered and displays it on the LCD and also stores it in the input array
{
	if (count < 4 && KeyValue != 12 && KeyValue != 11 && KeyValue != 10)
	{
		lcd_WriteVariable_withValueAndPositionParameters_SingleDecimalDigit(1, count, KeyValue);
		for (int i = 200; i>0; i--)
		{
			PORTF = ~PORTF;
			_delay_us(500);
		}
		DebounceDelay();
		input[count] = KeyValue;
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

void MotionDetected() //Allows the user 10 seconds to enter the code,
					  //displays the countdown and checks if the countdown is reached
{
	lcd_Clear();
	lcd_SetCursor(0x00);
	lcd_WriteString("Motion Detected!");
 	lcd_SetCursor(0x40);
 	lcd_WriteString("Enter Key");
 	_delay_ms(1000);
 	lcd_Clear();
 	lcd_SetCursor(0x00);
 	lcd_WriteString("Countdown:");
	
	while (1)
	{
		lcd_WriteVariable_withValueAndPositionParameters_SingleDecimalDigit(0,10, time);
		KeyValue = ScanKeypad();
		if(KeyValue != 0xFF)
		{
			PORTB = 0XFF;
			PasswordEntry();
		}
		
		if (time == 0)
		{
			AlarmSound();
		}
	}	
}

void AlarmSound() //Plays a siren sound and sends the program into an infinite loop, program termination.
{
	lcd_Clear();
	lcd_SetCursor(0x00);
	lcd_WriteString("Incorrect!!");
	_delay_ms(1000);
	
	for (int j = 5; j>0; j--)
	{
		for (int i = 1000; i>0; i--)
		{
			PORTF = ~PORTF;
			_delay_us(500);
		}
	
		for (int i = 500; i>0; i--)
		{
			PORTF = ~PORTF;
			_delay_ms(1);
		}
	}
	
	while (1)
	{
		//loop forever
	}
}

void Menu() //Program's main menu, runs on program startup and when password has been successfully entered
{	
	lcd_Clear();
	lcd_WriteString("1 - Set Password");
	lcd_SetCursor(0x40);
	lcd_WriteString("2 - Arm Alarm");
	
	while(1)
	{
		KeyValue = ScanKeypad();
		PORTB = EEPROM_read_Byte(1); //Displays the second digit of the password for troubleshooting 
		if (KeyValue == 1)
		{
			DebounceDelay();
			EnterNewPassword();
		}
		
		if (KeyValue == 2)
		{
			if (EEPROM_read_Byte(0)!=0xFF) //Checks if a password is stored in the EPROM
			{
				DebounceDelay();
				RunCode();
			}
			
			else //If no password is stored a password will be requested
			{
				lcd_Clear();
				lcd_SetCursor(0x00);
				lcd_WriteString("No Password Set");
				_delay_ms(1000);
				EnterNewPassword();
			}
		}
	}
}

void CheckCode() //Ensures that the password is correct
{
	
	while (count > 0)
	{
		count--;
		password[count] = EEPROM_read_Byte(count);		
	}
	
	if (memcmp(input, password, 4)) // Compares the input array and the password array, returns 1 if the arrays are different
	{
		AlarmSound();
	}
	
	else
	{		
		lcd_Clear();
		lcd_WriteString("Correct!!");
		_delay_ms(2000);
		Menu();
	}	
}

void EnterNewPassword() //Allows users to change the password, the password will remain the same
						//even after the board has been restarted
{
	count = 0;
	lcd_Clear();
	lcd_WriteString("New Password:");
	
	KeyValue = 0xFF;
	
	while(1)
	{
		KeyValue = ScanKeypad();
		
		if(KeyValue != 0xFF)
		{
			NewPasswordEntry();
		}
	}	
}

void NewPasswordEntry() //Changes the password values in the EPROM
{
	if (count < 4 && KeyValue != 12 && KeyValue != 11 && KeyValue != 10) //the 0 key has been disabled
	{
		lcd_WriteVariable_withValueAndPositionParameters_SingleDecimalDigit(1, count, KeyValue);
		
		for (int i = 200; i>0; i--)
		{
			PORTF = ~PORTF;
			_delay_us(500);
		}
		
		DebounceDelay();
		EEPROM_write_Byte(count, KeyValue);
		count++;
	}
	
	else
	{
		if (KeyValue == 12 && count==4)
		{
			for (int i = 1000; i>0; i--)
			{
				PORTF = ~PORTF;
				_delay_us(200);
			}
			
			Menu();
		}
	}
}

void InitialiseGeneral() //Initialising all the ports, interrupts and LCD setup
{
	
	
	//*********************LCD Setup***************************
	//LCD Ports
	DDRA = 0xFF;			// Configure PortA direction Output
	DDRC = 0xFF;			// Configure PortC direction Output
	DDRG = 0xFF;			// Configure PortG direction Output
	
	//LCD Setup
	lcd_Clear();				// Clear the display
	lcd_StandardMode();			// Set Standard display mode
	lcd_on();					// Set the display on
	lcd_CursorOff();			// Set the cursor display off (underscore)
	lcd_CursorPositionOff();	// Set the cursor position indicator off (flashing square)
	
	//*******************************************************************


	//*********************Motion Sensor Setup***************************
	//-------------- RICHARD'S CODE --------------
	//Sensor Ports
	DDRE = 0b01111111; //Sets Port E bit 7 as input
	PORTE = 0b00000000; // Bits 0-6 form a groundplane, bit 7 pullup resistor NOT set
	
	//Interrupts
	EIFR = 0b11111111;  // Clear all HW interrupt flags
	EICRA = 0b00000000; //External Interrupt Control Register A not used
	EICRB = 0b11000000; //Sets the interrupt to the rising edge
	EIMSK = 0b10000000; // Initially enabled
	//*******************************************************************
	
	
	//*********************Keypad Setup***************************
	DDRD = 0b11111000;	// Port D data direction register (row pins output, column pins input)
	PORTD= 0b00000111;	// Set pullups on column pins (so they read '1' when no key is pressed)
	//*******************************************************************
		
	DDRB = 0xFF; // Configure PortD direction Output (LEDs)
	PORTB = 0XFF; //Set LEDs off at start
	
	DDRF = 0XFF; //Configure PortF for output (Buzzer)
	
	sei();	// Enable interrupts at global level set Global Interrupt Enable (I) bit
	
	WelcomeBeep();
	
}  

void WelcomeBeep() //A beep that informs the user that the program has started
{
	for (int i = 400; i>0; i--)
	{
		PORTF = ~PORTF;
		_delay_us(600);
	}
	
	for (int i = 750; i>0; i--)
	{
		PORTF = ~PORTF;
		_delay_us(300);
	}
	
	for (int i = 1500; i>0; i--)
	{
		PORTF = ~PORTF;
		_delay_us(200);
	}
}

void Timer() //A timer that goes for 1 second -------------- RICHARD'S CODE --------------
{
	TCCR1A = 0b00000000;	// Normal port operation (OC1A, OC1B, OC1C), Clear Timer on 'Compare Match' (CTC) waveform mode)
	TCCR1B = 0b00001101;	// CTC waveform mode, use prescaler 1024
	TCCR1C = 0b00000000;
	
	// For 1 MHz clock (with 1024 prescaler) to achieve a 1 second interval:
	// Need to count 1 million clock cycles (but already divided by 1024)
	// So actually need to count to (1000000 / 1024 =) 976 decimal, = 3D0 Hex
	OCR1AH = 0x03; // Output Compare Registers (16 bit) OCR1BH and OCR1BL
	OCR1AL = 0xD0;

	TCNT1H = 0b00000000;	// Timer/Counter count/value registers (16 bit) TCNT1H and TCNT1L
	TCNT1L = 0b00000000;
	TIMSK1 = 0b00000010;	// bit 1 OCIE1A		Use 'Output Compare A Match' Interrupt, i.e. generate an interrupt
	// when the timer reaches the set value (in the OCR1A register)
}

ISR(TIMER1_COMPA_vect) //Timer Interrupt, this is used to display a countdown on the LCD
{
	time--;	
}

ISR (INT7_vect) // PIR Sensor Interrupt, the alarm value is like a flag, this could be narrowed down to a single digit if needed.
				//Storing the change in a value means that this value can be checked only when needed, at other times the motion
				//sensor interrupt will make no difference to the program. This could also have been achieved by disabling and 
				//enabling the interrupt.
{	
	alarm++;
} 
