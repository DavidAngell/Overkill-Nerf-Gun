# Flywheel Circuitry

![Get CDDX File Here: [https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a](https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a)](Flywheel%20Circuitry/motor_circuit.png)

Get CDDX File Here: [https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a](https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a)

I ended up using an N- Channel MOSFET to switch on and off the motors because well… I gave away my only high current switch to a girl in a futile attempt to win her approval :) In all seriousness, a high current switch like this [21A microswitch](https://outofdarts.com/collections/electronics/products/21a-microswitch-genuine-omron-v-212-1c6-spdt) is both less complicated and probably cheaper than my design; I just happened to have a MOSFET lying around from a previous project that I could use. Also, if you go with a switch, you don’t need the 10k resistor; it is only there because MOSFETs have some capacitance to them that needs to be drained in order to ensure fast and proper switching. I would still recommended the Shockley diode in parallel with the motors to reduce back EMF and save your switch it in the long term.

In case you are wondering wire the wires are crosses for the motors, it’s because they have to spin in opposite directions because the dart goes in between the two motors. If there weren’t crossed, then the motors would be pushing the dart in opposite directions —which I heard isn’t very good for making a gun… gun.