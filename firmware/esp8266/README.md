# SelfBalancing Robot: ESP8266
This directory contains the code for the [ESP8266](https://en.wikipedia.org/wiki/ESP8266) firmware. This page assumes that ESP8266 resides in a ESP12F breakout board, but any other board model with enough flash size can be used (maybe you should adapt makefile).

The code is written for [Arduino core for ESP8266](https://github.com/esp8266/Arduino) that allows you to write firmware for ESP8266 in the same way you will for an Arduino board.

In this project, ESP8266 acts as the bridge between the PIC microcontroller (which controls actuators, control algorithms and sensors) and HTTP clients.

## Firmware description
When application starts perform the following actions:
- Initializes MPU-9250 and loads stored calibration.
- Loads DMP firmware to MPU-9250.
- Signals PIC microcontroller to starts its activity.
- Initializes WiFi connection.

After initialization, the application:
- Attends the HTTP requests to its REST API. The REST API is documented [here](./doc/restapi.md).
- Serves files of its filesystem (SPIFFS).
- Monitor the `flash` button of the circuit. At runtime, this button allows you to change the PID offset calibration. A short pulse increases offset and a long pulse decreases it.

### Third-parties dependencies
In addition to the ESP8266 Arduino core, the firmware uses third-party libraries located in the `lib` directory. The `lib` directory contains the copy of a specific version of those libraries that are available on GitHub.

## WiFi modes
The firmware supports the three ESP8266 WiFi modes: 
- Station (STA): used to connect the ESP8266 module to an existing WiFi network.
- Access point (SoftAP): ESP8266 will create its own Wifi network and will act as AP.
- A combination of two others (STA + SoftAP)

WiFi configuration can be changed with the [Console Application](#console-application) or with the [WiFi REST API](doc/restapi/wifi.md).

### SofAP mode
When the ESP8266 is started, SoftAP mode will be enabled with the last configuration. If SoftAP has not been configured (for example, at the first boot) it will use the following values:
- SSID: `selfbalancing`
- password: `selfbalancing`

The network configuration for this mode will be:
- ESP8266: 192.168.4.2
- Netmask: 255.255.255.0
- Gateway: 192.168.4.1

### Station mode
When the ESP8266 is started, it will attempt to connect to the last known AP. If the AP is not available or the connection fails, the station mode will be disabled. It can be re-enabled via the Console Application or the REST API (through SoftAP mode).

## Build
You can use `make` command or the [PlatformIO](https://platformio.org/) environment to build the firmware.

Makefile will use frozen versions of ESP8266 Arduino core and third-party libraries in the `lib` directory. On the other hand, PlatformIO build will use latest versions of those packages.

### Build with `make`
Build script is based on [makeEspArduino.mk](https://github.com/plerup/makeEspArduino). If you have problems to build the firmware, you could refer to this project that is pretty well documented.

Build process has been tested on a Windows OS with the [Cygwin](https://www.cygwin.com/) environment but it is very likely that it will also work on a Linux OS.

#### Dependencies
This software must be installed to use the provided Makefile (in Cygwin environment, if you use Windows):
- Python
- Perl
- Make
- Git
- Patch

#### Compile
```
> make
```

### Build with [PlatformIO](https://platformio.org/)
To build with PlatformIO you need to install [PlatformIO Core](http://docs.platformio.org/en/latest/core.html) and run
```
> platformio run
```

## Upload firmware
[PCB](../schematics/pcb_top.pdf) incorporates a 3.3V serial interface for flashing and debugging ESP8266 firmware. The circuit between the serial connector and ESP12F is copied from [NodeMCU DEVKIT V1.0](https://github.com/nodemcu/nodemcu-devkit-v1.0) (the USB TO UART part includes this simple circuit), so you can use any software to upload firmware that is compatible with NodeMCU.

### Serial interface
You need a 3.3V serial interface for your PC to upload the generated firmware to ESP8266. The UART must control RTS and DTR signals. 

I use a cheap USB/Serial converter based on the CH340G chip. Originally, the RTS and DTR signals were not present in the output connector, but with a little hacking I get all the signals in a PIN strip that fits the PCB connector. 

![Serial interface](doc/images/usbtouart.jpg)

Alternatively, if you do not have RTS and DTR signals, you can set `GPIO0` to low manually with `Flash` button before loading the firmware. 

In https://github.com/esp8266/Arduino documentation, you will find more about ESP8266 boards and serials interfaces.

### Flash with `make`
The makefile includes a target to flash the firmware:
```
> make UPLOAD_PORT={COM} flash
```
Where `COM` is the serial port where the ESP8266 is connected. But you can use any software compatible with NodeMCU to do the job.

### Flash with PlatformIO
Edit the `platformio.ini` file to set the correct value for the `upload_port` setting. The value can be the serial port where the ESP8266 is connected or the robot IP address to upload firmware using OTA. See more on http://docs.platformio.org/en/latest/projectconf/section_env_upload.html.

After setting `upload_port`, run
```
> platformio run --target upload
```

## Console Application
Web [Console Application](../../client/html) can be uploaded into the ESP filesystem (SPIFFS - SPI Flash File System).

To upload the application to ESP, build Console Application in production mode (see instructions in [README.md](../../client/html/README.md)). Then use `make` or PlatformIO to upload it.

### Upload with `make`
Just run:
```
> make UPLOAD_PORT={COM} flash_fs
```
`flash_fs` target will copy Console Application to the `data` directory and will upload it to ESP.

### Upload with PlatformIO
- Copy the build output of the Console Application to the `data` directory of this project (create it if it doesn't exist). You can do it manually or by running the command:
```
> make data
```
- Run:
```
> platformio run --target uploadfs
```

## Development IDE
[Eclipse](https://www.eclipse.org/) with [Sloeber plugin](http://sloeber.io/) (at the beginning of the project) and [PlatformIO Eclipse Integration](http://docs.platformio.org/en/latest/ide/eclipse.html) (when I discovered PlatformIO...) have been the IDEs used for the development process. If you do not like Eclipse, you can use the Arduino environment by following instructions explained in [Arduino core for ESP8266](https://github.com/esp8266/Arduino). An other interesting option is [PlatformIO IDE](https://platformio.org/platformio-ide).
