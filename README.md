# catling-gun
Modded an electric squirtgun to trigger off of a passive IR sensor.

We (like most pet owners) have problems with pets on counters, so I modded an electric squirtgun to trigger off of a passive IR sensor. 

Hardware:

  Nodemcu
  
  PIR sensor
  
  Sky Rocket Fuze Cyclone Bike Water Blaster ($12 at the time)
  
  2222 transistor
  
  misc wire, resistors/etc.
  
Bulid log - https://imgur.com/a/oSWtWZh

Designed to be web-controlled for duration after startup. To avoid false positives from craptacular PIR sensors, it reads the first three triggers as a baseline for further events. (Evidently cheap PIR sensors go off randomly. )
![20190203_133620](https://user-images.githubusercontent.com/6173152/189506494-5bfd2d74-792f-4cdf-bc1a-fb4f8eb8d31a.jpg)
