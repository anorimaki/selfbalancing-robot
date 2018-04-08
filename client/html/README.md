# SelfBalancing Robot: Web Console Application
This is a single web page application based on [Angular](https://angular.io/) framework.

It consist in two sub-consoles: one for manage robot properties and other one to manage WiFi properties.

## Robot Console
This console allows:
- Managing control algorithms parameters (PID constants).
- Setting MPU calibration.
- Visualizing internal data of control algorithms at real time.

## WiFi Console
WiFi console is a generic console to manage ESP8266 WiFi settings. It allows configuring ESP8266 [WiFi modes](../../firmware/esp8266/README.md#wifi-modes).

## Build

```
> npm run-script build-prod
```

## Upload to ESP8266
See instructions [here](../../firmware/esp8266/README.md#console-application) to see how to upload the application to the ESP8266.

