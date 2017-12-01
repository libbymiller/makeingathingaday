install this: https://github.com/Links2004/arduinoWebSockets

example based on https://github.com/Links2004/arduinoWebSockets/blob/master/examples/WebSocketClientSocketIO/WebSocketClientSocketIO.ino

for ESP: https://wiki.wemos.cc/tutorials:get_started:get_started_in_arduino

For wimos D1 mini ESPs the pin numbering isn't what I expected - use

RX for pin 3
D1 for pin 5

http://escapequotes.net/esp8266-wemos-d1-mini-pins-and-diagram/

To load the code you'll need to take the servo off pin 3 while the code is loading.

Servo 2 is a continuous rotation servo - servo 1 is a normal one.
