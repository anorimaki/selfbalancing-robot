# SelfBalancing Robot: schematics
This directory contains schematics for [KiCad](http://kicad-pcb.org) suite.

## Connectors, Buttons and LED locations
![Circuit](doc/images/circuit.jpg)

## Improvements

### Power rail generation
There are two lineal regulators to generate power rails, a 7805 and a LM1117-3.3. The 7805 is connected to the battery which can not be the same battery used for the motors because too much power would be dissipated in the regulator. The motors need a 12V battery and the circuit consumes about 250mA, so it would be about 1.75W! With a dedicated 7.4V battery, the power dissipation is reduced but remains too high (about 600mW). 

A buck converter to generate the 5V power rail would have been a better choice because it would be more efficient (extending battery life) and eliminate the need of a second battery (the circuit could be plugged with the 12V battery).
