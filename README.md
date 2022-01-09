# Mqtt-Info-Panel
See all your mqtt messages on a 95cm x 8cm LED panel build with WS2812 Leds

Dont forget your credentials for your local network and the ip adress of your mqtt server (running on raspberry pi)
The hardware consists of 3 flexible LED Matrix each 32 x 8 formatted, conncected to an ESP8266.
LED matrix can be found here: https://www.ebay.de/itm/293309695325?hash=item444a9ec55d:g:QPEAAOSw9ZtctDJ5
or search for "ws2812b matrix" at ebay

Depending on the message contens different message color can be selected.

Connections: D6 connected to Din of first Led Panel. Dout of first panel goes to Din of second Matrix and so on.

Powersupply is connected separately to each matrix to avoid current overload of first matrix. (check the pictures ;-))
