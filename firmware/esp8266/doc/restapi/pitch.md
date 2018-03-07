# SelfBalancing Robot: REST API - Pitch
Pitch resource satisfies [PID resource definition](./pid.md).
## Specific property semantics
|Property|Description|
|---|---|
|`state[].tar`|Target pitch angle expressed in radians in [Q16 format](https://en.wikipedia.org/wiki/Q_(number_format)). Value range is limited to fractional part. You must divide this value by 65536 to get the angle in radians.|
|`state[].cur`|Current pitch angle expressed in radians in [Q16 format](https://en.wikipedia.org/wiki/Q_(number_format)). Value range is limited to fractional part. You must divide this value by 65536 to get the angle in radians.|