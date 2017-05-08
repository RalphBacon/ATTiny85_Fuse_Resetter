#include "Arduino.h"
// AVR High-voltage Serial Programmer
// Originally created by Paul Willoughby 03/20/2010
// www.rickety.us slash 2010/03/arduino-avr-high-voltage-serial-programmer/
// Inspired by Jeff Keyzer mightyohm.com
// Serial Programming routines from ATtiny25/45/85 datasheet

// Desired fuse configuration
#define  HFUSE  0xDF   // Defaults for ATtiny25/45/85
#define  LFUSE  0x62

#define  RST     13    // Output to level shifter for !RESET from transistor to Pin 1
#define  CLKOUT  12    // Connect to Serial Clock Input (SCI) Pin 2
#define  DATAIN  11    // Connect to Serial Data Output (SDO) Pin 7
#define  INSTOUT 10    // Connect to Serial Instruction Input (SII) Pin 6
#define  DATAOUT  9    // Connect to Serial Data Input (SDI) Pin 5
#define  VCC      8    // Connect to VCC Pin 8

// Added by Ralph S Bacon just for enhanced user experience
#define  GND	  5		// GND for buzzer
#define  PWRLED	  6		// Power on LED
#define	 BUZZ	  7		// Beeper pin

int inData = 0;         // incoming serial byte AVR
int targetValue = HFUSE;

void setup() {
	// Set up control lines for HV parallel programming
	pinMode(VCC, OUTPUT);
	pinMode(RST, OUTPUT);
	pinMode(DATAOUT, OUTPUT);
	pinMode(INSTOUT, OUTPUT);
	pinMode(CLKOUT, OUTPUT);
	pinMode(DATAIN, OUTPUT);  // configured as input when in programming mode

	// RSB "enhancements"
	// Sink the Buzzer to ground
	pinMode(GND, OUTPUT);
	digitalWrite(GND, LOW);

	// Turn on power LED
	pinMode(PWRLED, OUTPUT);

	// Buzzer / beeper
	pinMode(BUZZ, OUTPUT);
	digitalWrite(PWRLED, HIGH);

	// Let user know we're ready to go
	digitalWrite(BUZZ, HIGH);
	delay(50);
	digitalWrite(BUZZ, LOW);
	// End of RSB

	// Initialize output pins as needed
	digitalWrite(RST, HIGH);  // Level shifter is inverting, this shuts off 12V

	// start serial port at 9600 bps:
	Serial.begin(9600);
}

void loop() {

	switch (establishContact()) {
		case 49:
			targetValue = HFUSE;
			break;
		case 50:
			targetValue = 0x5F;
			break;
		default:
			targetValue = HFUSE;
	}

	Serial.println("Entering programming Mode\n");

	// Initialize pins to enter programming mode
	pinMode(DATAIN, OUTPUT);  //Temporary
	digitalWrite(DATAOUT, LOW);
	digitalWrite(INSTOUT, LOW);
	digitalWrite(DATAIN, LOW);
	digitalWrite(RST, HIGH);  // Level shifter is inverting, this shuts off 12V

	// Enter High-voltage Serial programming mode
	digitalWrite(VCC, HIGH);  // Apply VCC to start programming process
	delayMicroseconds(20);
	digitalWrite(RST, LOW);   //Turn on 12v
	delayMicroseconds(10);
	pinMode(DATAIN, INPUT);   //Release DATAIN
	delayMicroseconds(300);

	//Programming mode
	int hFuse = readFuses();

	//Write hfuse if not already the value we want 0xDF (to allow RST on pin 1)
	if (hFuse != targetValue) {
		delay(1000);
		Serial.print("Writing hfuse as ");Serial.println(targetValue, HEX);
		shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x40, 0x4C);

		// The default RESET functionality
		//shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, HFUSE, 0x2C);

		// this turns the RST pin 1 into a (weak) IO port
		//shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x5F, 0x2C);

		// User selected option
		shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, targetValue, 0x2C);

		shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x00, 0x74);
		shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x00, 0x7C);
	}

	//Write lfuse
	delay(1000);
	Serial.println("Writing lfuse\n");
	shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x40, 0x4C);
	shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, LFUSE, 0x2C);
	shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x00, 0x64);
	shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x00, 0x6C);

	// Confirm new state of play
	hFuse = readFuses();

	digitalWrite(CLKOUT, LOW);
	digitalWrite(VCC, LOW);
	digitalWrite(RST, HIGH);   //Turn off 12v

	// Let user know we're done
	digitalWrite(BUZZ, HIGH);
	delay(50);
	digitalWrite(BUZZ, LOW);
	delay(50);
	digitalWrite(BUZZ, HIGH);
	delay(50);
	digitalWrite(BUZZ, LOW);

	Serial.println("\nProgramming complete. Press RESET to run again.");
	while (1==1){};
}

int establishContact() {
	Serial.println("Turn on the 12 volt power/\n\nYou can ENABLE the RST pin (as RST) "
			"to allow programming\nor DISABLE it to turn it into a (weak) GPIO pin.\n");

	// We must get a 1 or 2 to proceed
	int reply;

	do {
		Serial.println("Enter 1 to ENABLE the RST pin (back to normal)");
		Serial.println("Enter 2 to DISABLE the RST pin (make it a GPIO pin)");
		while (!Serial.available()) {
			// Wait for user input
		}
		reply = Serial.read();
	}
	while (!(reply == 49 || reply == 50));
	return reply;
}

int shiftOut2(uint8_t dataPin, uint8_t dataPin1, uint8_t clockPin, uint8_t bitOrder, byte val, byte val1) {
	int i;
	int inBits = 0;
	//Wait until DATAIN goes high
	while (!digitalRead(DATAIN)) {
		// Nothing to do here
	}

	//Start bit
	digitalWrite(DATAOUT, LOW);
	digitalWrite(INSTOUT, LOW);
	digitalWrite(clockPin, HIGH);
	digitalWrite(clockPin, LOW);

	for (i = 0; i < 8; i++) {

		if (bitOrder == LSBFIRST) {
			digitalWrite(dataPin, !!(val & (1 << i)));
			digitalWrite(dataPin1, !!(val1 & (1 << i)));
		}
		else {
			digitalWrite(dataPin, !!(val & (1 << (7 - i))));
			digitalWrite(dataPin1, !!(val1 & (1 << (7 - i))));
		}
		inBits <<= 1;
		inBits |= digitalRead(DATAIN);
		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);

	}

	//End bits
	digitalWrite(DATAOUT, LOW);
	digitalWrite(INSTOUT, LOW);
	digitalWrite(clockPin, HIGH);
	digitalWrite(clockPin, LOW);
	digitalWrite(clockPin, HIGH);
	digitalWrite(clockPin, LOW);

	return inBits;
}

// Returns the value of the HFUSE
int readFuses() {
	Serial.println("Reading fuses");

	//Read lfuse
	shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x04, 0x4C);
	shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x00, 0x68);
	inData = shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x00, 0x6C);
	Serial.print("lfuse reads as ");
	Serial.println(inData, HEX);

	//Read hfuse
	shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x04, 0x4C);
	shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x00, 0x7A);
	inData = shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x00, 0x7E);
	Serial.print("hfuse reads as ");
	Serial.println(inData, HEX);
	int hFuse = inData;

	//Read efuse
	shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x04, 0x4C);
	shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x00, 0x6A);
	inData = shiftOut2(DATAOUT, INSTOUT, CLKOUT, MSBFIRST, 0x00, 0x6E);
	Serial.print("efuse reads as ");
	Serial.println(inData, HEX);
	Serial.println();

	return hFuse;
}
