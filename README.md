HC-SR04E
========

Firmware for the improved HC-SR04E ultrasonic rangefinder by UglyDuck

#Hardware setup for normal use

Connect the HC-SR04E VCC and GND pins to your 5V power supply.

The HC-SR04E uses the I2C bus for communication. What this means is that
the sensor's MOSI pin must be connected to the I2C data line (SDA) and
the SCK pin must be connected to the I2C clock line (SCL).

The pins used on the Arduino Uno for I2C are the analog input pins A4
(SDA) and A5 (SCL). Other Arduino models use different pins; the 
following link has a complete list.

http://arduino.cc/en/reference/wire

For other microprocessors, refer to the datasheet. Usually, the I2C pins
will be labelled SDA and SCL in the "Pin Configurations" diagrams
towards the beginning of the document.

#Hardware setup for programming

If using an Arduino Uno for programming, upload the ArduinoISP sketch
before proceeding. Connect the sensor's VCC and GND pins as before, 
then connect the RESET, MOSI, MISO and SCK pins to pins 10, 11, 12 
and 13, respectively, on the Arduino Uno. Bridge the Arduino's RESET
and GND pins with a 10uF electrolytic capacitor.

If using another AVR programmer, connect the ICSP header to the sensor's
pins as required.

#Software requirements

To compile the source code, you'll need gcc, avr-gcc (and the rest of
the toolchain), and avrdude. On Linux, you should be able to install 
the prerequisites using the following command:

    sudo apt-get install binutils-avr avr-libc avrdude gcc-avr

On Windows, you'll need to install WinAVR, Ruby, and the Arduino 
software package.

Set the environmental variable SERIAL_PORT to point to the USB port
connected to your Arduino. On my Linux laptop, this is /dev/ttyACM0,
but on your computer it probably be different. The easiest way to
find the correct port is by checking in the Arduino IDE, under 
TOOLS->SERIAL PORT.

    export SERIAL_PORT=/dev/ttyACM0

The following command will rebuild the software, run all the unit tests
and upload the newly built code to the HC-SR04, via your ArduinoISP:

    rake clobber clean test:all program

If you're not using an Arduino as your programmer, you'll probably have
to play around with the baud rate and other avrdude arguments in the
rakefile.rb file.
