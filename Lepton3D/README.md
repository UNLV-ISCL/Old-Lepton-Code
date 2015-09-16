# 3D Lepton Interface

A heavily modified version of the demo code at https://github.com/PureEngineering/LeptonModule/ (probably should have forked it, but ah well).  This application should run on a Raspberry Pi 2 running either Raspbian or Ubuntu.  It has been tested on the Ubuntu 14.04 LTS arhmf build at https://wiki.ubuntu.com/ARM/RaspberryPi (http://www.finnie.org/software/raspberrypi/2015-04-06-ubuntu-trusty.zip).

Once I'm finished, it will interface with ROS Indigo, so you'll probably need Ubuntu and not Raspbian, unless you really want to build ROS from source.

This requires the qt4-dev-tools package among other things.  Yes, it's still QT, like the original demo, but that's just for display/diagnostic purposes.  Eventually we'll move this code to a pure-ROS setup.

To avoid having to run as root, I recommend either making the SPI and I2C interfaces world-accessible or group-accessible, adding the 'ubuntu' user to 'spi' and 'i2c' groups).  You can set up a udev file /etc/udev/rules.d/39-spidev-and-i2c.rules:
KERNEL=="spidev*", GROUP="spi", MODE="0660"
KERNEL=="i2c*", GROUP="i2c", MODE="0660"

You'll need to build the Lepton SDK binary in order to issue commands to the lepton over I2C, such as performing flat-field calibration. To do this, cd into LeptonSDKEmb32PUB and run the command: `make`

Once everything's in place, cd into the raspberrypi_video folder, and run the command: `qmake && make`. This will compile the application.

The application expects two Leptons to be connected. Both devices share the following pins:
RPi pin header diagram: http://www.element14.com/community/servlet/JiveServlet/previewBody/73950-102-4-309126/GPIO_Pi2.png
Lepton GND to any RPi GND
Lepton Vin to RPi 3.3V (pin 01)
Lepton MISO to RPi SPI_MISO (pin 21)
Lepton CLK to RPi SPI_CLK (pin 23)
Lepton SDA to RPi SDA1 (pin 03)
Lepton SCL to RPi SCL1 (pin 05)

The CS pins of the Leptons are connected to separate pins:
"Left eye" Lepton CS to RPi SPI_CE0_N (pin 24)
"Right eye" Lepton CS to RPi SPI_CE1_N (pin 26)
This ensures that we always know which Lepton is which -- Left is CE0 (/dev/spidev0.0), Right is CE1 (/dev/spidev0.1).

Note that both devices use the same I2C bus, and they also share the same device ID.  This makes reading data from the devices probably impossible, but allows settings and commands to be sent to both devices simultaneously and simplifies wiring.  In the future we may set up a separate I2C bus using WiringPi or some other GPIO approach.