//*******************************************************************************
// Project 		Keypad Demonstration - displays key value on LEDs (Embedded C) 
// Target 		ATMEL ATmega1281 micro-controller on STK300 board
// Program		KEYPAD_LEDs_1281_C.c
// Author		Richard Anthony
// Date			17th October 2013 (ATmega8535 version 8th October 2011)


// This code is all Richards, it started out as a sample program and I have removed all the extra code.
// Now this program just scans the keypad and returns the value as a decimal. It has been integrated into my program
// but the original code is not mine.
//******************************************************************************

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

#define ScanKeypadRow0 0b00111111	// Bits 4-7 pulled low depending on row being scanned, bits 0-2 (pullups) remain high at all times
#define ScanKeypadRow1 0b01011111
#define ScanKeypadRow2 0b01101111
#define ScanKeypadRow3 0b01110111

#define KeypadMaskColumns 0b11111000
#define KeypadMaskColumn0 0b00000100
#define KeypadMaskColumn1 0b00000010
#define KeypadMaskColumn2 0b00000001

#define Star	0x0A
#define Zero	0x0B
#define Hash	0x0C
#define NoKey	0xFF

unsigned char ScanKeypad(void);
unsigned char ScanColumns(unsigned char);
void DisplayKeyValue(unsigned char);
void DebounceDelay(void);

unsigned char ScanKeypad()
{
	unsigned char RowWeight;
	unsigned char KeyValue;

// ScanRow0					// Row 0 is connected to port bit 6
	RowWeight = 0x01;		// Remember which row is being scanned
	PORTD = ScanKeypadRow0;	// Set bit 6 low (Row 0), bits 5,4,3 high (rows 1,2,3)
	KeyValue = ScanColumns(RowWeight);	
	if(NoKey != KeyValue)
	{
		return KeyValue;
	}
	
// ScanRow1					// Row 1 is connected to port bit 5
	RowWeight = 0x04;		// Remember which row is being scanned
	PORTD = ScanKeypadRow1;	// Set bit 5 low (Row 1), bits 6,4,3 high (rows 0,2,3)
	KeyValue = ScanColumns(RowWeight);	
	if(NoKey != KeyValue)
	{
		return KeyValue;
	}

// ScanRow2					// Row 2 is connected to port bit 4
	RowWeight = 0x07;		// Remember which row is being scanned
	PORTD = ScanKeypadRow2;	// Set bit 4 low (Row 2), bits 6,5,3 high (rows 0,1,3)
	KeyValue = ScanColumns(RowWeight);	
	if(NoKey != KeyValue)
	{
		return KeyValue;
	}

// ScanRow3					// Row 3 is connected to port bit 3
	RowWeight = 0x0A;		// Remember which row is being scanned
	PORTD = ScanKeypadRow3;	// Set bit 3 low (Row 3), bits 6,5,4 high (rows 0,1,2)
	KeyValue = ScanColumns(RowWeight);	
	return KeyValue;
}

unsigned char ScanColumns(unsigned char RowWeight)
{
	// Read bits 7,6,5,4,3 as high, as only interested in any low values in bits 2,1,0
	unsigned char ColumnPinsValue; 
	ColumnPinsValue = PIND | KeypadMaskColumns; // '0' in any column position means key pressed
	ColumnPinsValue = ~ColumnPinsValue;			// '1' in any column position means key pressed

	if(KeypadMaskColumn0 == (ColumnPinsValue & KeypadMaskColumn0))
	{
		return RowWeight;		// Indicates current row + column 0
	}
	
	if(KeypadMaskColumn1 == (ColumnPinsValue & KeypadMaskColumn1))
	{
		return RowWeight + 1;	// Indicates current row + column 1
	}

	if(KeypadMaskColumn2 == (ColumnPinsValue & KeypadMaskColumn2))
	{
		return RowWeight + 2;	// Indicates current row + column 2
	}
	
	return NoKey;	// Indicate no key was pressed
}

void DebounceDelay() // The Debounce Delay ensures that a button press is not read twice
{
	_delay_ms(250);
}