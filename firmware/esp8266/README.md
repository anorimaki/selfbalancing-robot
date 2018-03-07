# SelfBalancing Robot: ESP8266
This directory contains the code for the [ESP8266](https://en.wikipedia.org/wiki/ESP8266) firmware. This page assumes that ESP8266 resides in a ESP12F breakout board, but any other board model with enough flash size can be used (maybe you should adapt makefile).

The code is written for [Arduino core for ESP8266](https://github.com/esp8266/Arduino) that allows you to write firmware for ESP8266 in the same way you will for an Arduino board.

In this project, ESP8266 acts as the bridge between the PIC microcontroller (which controls actuators, control algorithms and sensors) and HTTP clients.

## Firmware description
When application starts perform the following actions:
- Initializes MPU-9250 and loads stored calibration.
- Loads DMP firmware to MPU-9250.
- Signals PIC microcontroller to starts its activity.
- Connects with WiFi AP.

After initialization, the application:
- Attends the HTTP requests to its REST API. The REST API is documented [here](./doc/restapi.md).
- Monitor the `flash` button of the circuit. At runtime, this button allows you to change the PID offset calibration. A short pulse increases offset and a long pulse decreases it.

## Build
Build script is based on [makeEspArduino.mk](https://github.com/plerup/makeEspArduino). If you have problems to build the firmware, you could refer to this project that is pretty well documented. 

Build process has been tested on a Windows OS with the [Cygwin](https://www.cygwin.com/) environment but it is very likely that it will also work on a Linux OS.

### Dependencies
This software must be installed (in Cygwin environment, if you use Windows):
- Python
- Perl
- Make
- Git

### Generate firmware
#### WiFi settings
Create `include/wifiproperties.h` file with your WiFi AP definitions. This is an example:
```
namespace wifi {

static const char* ssid = "My AP SSID";
static const char* password = "Secret";

}
```
#### Compile
```
> make
```

## Upload firmware
[PCB](../schematics/pcb_top.pdf) incorporates a 3.3V serial interface for flashing and debugging ESP8266 firmware. The circuit between the serial connector and ESP12F is copied from [NodeMCU DEVKIT V1.0](https://github.com/nodemcu/nodemcu-devkit-v1.0) (the USB TO UART part includes this simple circuit), so you can use any software to upload firmware that is compatible with NodeMCU.

### Serial interface
You need a 3.3V serial interface for your PC to upload the generated firmware to ESP8266. The UART must control RTS and DTR signals. 

I use a cheap USB/Serial converter based on the CH340G chip. Originally, the RTS and DTR signals were not present in the output connector, but with a little hacking I get all the signals in a PIN strip that fits the PCB connector. 

![Serial interface](doc/images/usbtouart.jpg)

Alternatively, if you do not have RTS and DTR signals, you can set `GPIO0` to low manually with `Flash` button before loading the firmware. 

In https://github.com/esp8266/Arduino documentation, you will find more about ESP8266 boards and serials interfaces.

### Flash
The makefile includes a target to flash the firmware:
```
> make UPLOAD_PORT={COM} flash
```
Where `COM` is the serial port where the ESP8266 is connected. But you can use any software compatible with NodeMCU to do the job.

## Development IDE
[Eclipse](https://www.eclipse.org/) with [Sloeber plugin](http://sloeber.io/) has been the IDE used for the development process. If you do not like Eclipse, you can use the Arduino environment by following instructions explained in [Arduino core for ESP8266](https://github.com/esp8266/Arduino).
