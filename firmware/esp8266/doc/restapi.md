# SelfBalancing Robot: REST API Reference
This is the REST API exposed by ESP8266.

## Resources representation
All resources are represented in JSON format.

## CORS
All endpoint implements OPTIONS HTTP method that enables CORS for all origins. Thats allows to implement a pure javascript client for this API.

## Resources
* [Targets](./restapi/targets.md)
* [MpuCalibration](./restapi/mpu_calibration.md)
* [MpuSettings](./restapi/mpu_settings.md)
* [Pitch](./restapi/pitch.md)
* [Speed](./restapi/speed.md)
* [Heading](./restapi/heading.md)
