# ATTiny85_Fuse_Resetter
Want to use all SIX GPIO pins on the ATTiny85? Now you can.

So you like the ATTiny85 chip but wish you could use ALL the pins as GPIO pins? Your wish is granted!

OK, you have to build (or buy) a fuse resetter but it's a small project that just about anyone with a soldering iron can successfully complete (Benny built mine and he's got no thumbs, remember).

If you really don't want to build one for a few dollars you can always buy one ready made, but at least by watching this video you will understand what it is doing.

There are some "gotchas" to watch out for (just one or two but definitely worth knowing about) but when it comes down to it YES you CAN have all SIX GPIO pins working on a Tiny85, as I prove in this video.

I wish I had known this a few years ago as I would now be using my ATTiny85 instead of a Nano to control my fridge door alarm, that's for sure. Sometimes a Tiny85 is all you need. And it can run off batteries for weeks if not months, as my previous video on the Tiny85 mentioned.

# LINKS LINKS LINKS and MORE LINKS!

High Voltage programming/Unbricking for Attiny - Yikes! Invalid device signature
https://arduinodiy.wordpress.com/2015/05/16/high-voltage-programmingunbricking-for-attiny/

ATTiny85 Spec Sheet (Good Bedtime Reading, really!) Page 184-185 has graphs showing the current/voltage.
http://www.atmel.com/images/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf

Embedded Fuse Calculator (for all ATMEL chips)
http://www.engbedded.com/fusecalc/

All about ATMEGA328P (Uno/Nano) Fuse Settings
http://www.martyncurrey.com/arduino-atmega-328p-fuse-settings/

Setting and reading AtTiny85 fuses
https://dntruong.wordpress.com/2015/07/08/setting-and-reading-attiny85-fuses/

You can use AVRDUDE (that's the AVR DownandUploadDEr - contrived or what?) to give you info on fuses too.
Run PowerShell from Windows Start Button, then execute these commands:

CD "C:\Program Files (x86)\Arduino\hardware\tools\avr\bin"
./avrdude.exe -c stk500v1 -p attiny85 -P com7 -U lfuse:r:-:i -v -C ..\etc\avrdude.conf -b 19200 -F

1. You can remove the -F that just skips an invalid signature (like if you have the RESET pin configured as an IO pin!)
2. *** Note the path must be in quotes ***
3. If you are not running a 64-bit Windows version (eg Windows 10) your path might be different so remove the (x86) bit from the path.
