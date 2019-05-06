# Arduino-communicator
Short-wave text communicator based on Arduino and nRF24L01.

In order to create this device you will need:
- 2x Arduino (any board)
- 2x nRF24L01
- 2x OLED display 128x64px I2C
- 2x Arduino Keypad

#### nRF24L01 connection
|Arduino Pin|nRF24L01 Pin|
|:-:|:-:|
|1.9-3.3V|Vcc|
|GND|GND|
|15|CSN|
|14|CE|
|MOSI|MOSI|
|SCK|SCK|
|-|IRQ|
|MISO|MISO|

#### OLED connection
|Arduino Pin|OLED Pin|
|:-:|:-:|
|3.3-5V|Vcc|
|GND|GND|
|SCL|SCL|
|SDA|SDA|

#### Keypad connection
|Arduino Pin|Keypad Pin (from the left)|
|:-:|:-:|
|9|1|
|8|2|
|7|3|
|6|4|
|5|5|
|4|6|
|3|7|
|2|8|
