# SelfBalancing Robot: REST API Reference
This is the REST API exposed by ESP8266. REST endpoints are located after the `/rest` path. So base URL for REST API in the [SoftAP](../README.md#sofap-mode) interface is `http://192.168.4.2/rest`.

## Resources representation
All resources are represented in JSON format.

## CORS
All endpoints implement the HTTP OPTIONS method that enables CORS for all origins. That allows you to implement pure javascript clients for this API.

## Resources
* [Targets](./restapi/targets.md)
* [MpuCalibration](./restapi/mpu_calibration.md)
* [MpuSettings](./restapi/mpu_settings.md)
* [Pitch](./restapi/pitch.md)
* [Speed](./restapi/speed.md)
* [Heading](./restapi/heading.md)
* [Wifi](./restapi/wifi.md)
