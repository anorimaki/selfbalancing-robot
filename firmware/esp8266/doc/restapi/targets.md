# SelfBalancing Robot: REST API - Targets
Current control algorithms targets. Control algorithms will try to archive those values.

## Resource
	{
		"speed": {number},
		"heading": {number}
	}

|Property|Value|Description|
|---|---|---|
|`speed`|`number`|Absolute linear velocity. Range: [-16384..16384]. Related with [Speed `state[].tar`](./speed.md) property.|
|`heading`|`number`|Relative heading from actual position. Range: [-16384..16384]. Related with [Heading `state[].tar`](./heading.md) property.|
## Methods
### set
Establishes new targets for control algorithms.
#### Request
```
PUT /targets
```
#### Body Request
The resource representation with desired values.
#### Request example
```
PUT /targets

{
	"speed": 5000,
	"heading": 0
}
```
#### Response
```
HTTP/1.1 204 No Content
```



