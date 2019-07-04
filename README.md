# atmel-studio-7-examples
Examples on how to use atmel studio on atmega328p microcontroller, using AVR C Library

By making these examples i assume that you already know the basics of how to write assembly code on any microcontroller (i learned to write on 8051 microcontroller before i started studying avr C to write on atmega micro controller)

Notes: 
<strong>FOR WINDOWS</strong>

You will need to install atmel studio.

*How to set up your atmel studio: https://www.youtube.com/watch?v=zEbSQaQJvHI

*If you get toolchain error while building or opening a project, download avr toolchain from their website and install in on your computer.

*Download doxygen via tools/extensions & updates, it may be usefull. It will be installed on: 
C:\Users\basil\AppData\Local\Atmel\AtmelStudio\7.0\Extensions\sm3hzemz.snp\Doxygen

<strong>FOR Ubuntu [hope it works for other OSs]</strong>

1. Installing avr-gcc and tools:

*sudo apt-get install gcc-avr avr-libc avrdude*
It may pop you up an error, reccomending you to use : *apt --fix-broken install*. If it does, type *sudo apt --fix-broken install* and wait for it to finish

2. Compiling and burning the code:

Lets try to compile a simple file. make a file named main.c and paste the following code inside it:

int
main(void)
{

        while (1);
}

Right click on the directory (in the same folder) you saved the file, then *open in terminal* and paste: *sudo avr-gcc -Wall -g -Os -mmcu=attiny13 -o main.bin main.c*

Now on the above code, you may need to change the 'attiny13' into the name of your atmega microcontroller. in my case, 'atmega328p'

Hit enter. Your main.c should be compiled, and you should see another file in the same folder, called main.bin

After performing successful compilation, you can check program and data memory size with this command: *avr-size -C main.bin*

3. Generating .HEX

You will need to make a third file, in order to upload the code. Without closing the terminal, paste *sudo avr-objcopy -j .text -j .data -O ihex main.bin main.hex* and hit enter.

You should see a third file, called 'main.hex' in the same folder.

4. Burning

Connect your board to usb. I use an FTDI basic to program my atmega328p, but i think using an Arduino board will work as well, the same.

With the command below, the file main.hex will be burned into the flash memory. The -p attiny13 option lets avrdude know that we are operating on an ATtiny13 chip. In other words – this option specifies the device. The full list of supported parts can be found here:  https://www.nongnu.org/avrdude/user-manual/avrdude_4.html. Note that full-length names are also acceptable (i.e. t13 equals attiny13).

*avrdude -p attiny13 -c usbasp -U flash:w:main.hex:i -F -P usb*

Now, in my case i needed to make those changes:

*sudo avrdude -p m328p -c arduino -U flash:w:main.hex:i -F -P /dev/ttyUSB0*

-Since i use atmega328p, i changed to -p m328p.

-From Arduino IDE (that i have already installed in ubuntu), i found the usb port that my programmer(and atmega) uses. (if you open Arduino IDE its under Tools/port), which was /dev/ttyUSB0.

Hit enter. the program should be uploaded. I remember the first time it poped me an error saying "microcontroller initialized, use -D parameter to avoid it."  Something like that. If that happens, just run the command again, it will work the second time, after the auto-initialization error.


Let me thank those tutorials, from where i tool all those infos:

https://blog.podkalicki.com/how-to-compile-and-burn-the-code-to-avr-chip-on-linuxmacosxwindows/

http://www.linuxandubuntu.com/home/setting-up-avr-gcc-toolchain-and-avrdude-to-program-an-avr-development-board-in-ubuntu
