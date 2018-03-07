# SelfBalancing Robot: REST API - Heading
Pitch resource satisfies [PID resource definition](./pid.md).
Heading is defined as the difference between left and right wheels speed in a PID algorithm execution period.
## Specific property semantics
|Property|Description|
|---|---|
|`state[].tar`|Target heading for this period.|
|`state[].cur`|Current heading for this period.|