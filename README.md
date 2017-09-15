# ledskull
First try to create lighting IoT device based on ESP8266 (Arduino).

Sketch support Glow, Fade and Rainbow effects for WS2812b RGB led strips and web-UI to control it from any device with web browser.

Project uses [WebSockets server](https://github.com/Links2004/arduinoWebSockets) and [SPI Flash filesystem](https://github.com/pellepl/spiffs) libraries.

Hardware
--------
First of all - ESP8266 chip. I've used [this](https://www.amazon.com/HiLetgo-Version-NodeMCU-Internet-Development/dp/B010O1G1ES/) NodeMCU 1.0 (ESP-12E) from Amazon. It's very usefull, because it bundled with power supply and USB to TTL serial adapter.

WS2812b RGB led strip like this [NeoPixel](https://www.adafruit.com/product/2562) by Adafruit. I've used strip with 5 leds from [Xadow Wearable Kit for Intel® Edison](http://wiki.seeed.cc/Xadow_Wearable_Kit_For_Edison)

Deploy
------
  1. Upload sketch to ESP8266.
  2. Upload data folder by [ESP8266 FS plugin](https://github.com/esp8266/arduino-esp8266fs-plugin).

Usage
-----
Go to http://skull.local

Have fun!

UI Screenshot
-----------
![web interface](https://user-images.githubusercontent.com/304916/29154880-883ed024-7d4c-11e7-8222-edeafd210567.png)

TODO
----
* Add photo of skull
* ~~Resolve some bugs in UI~~
* OTA updates
* ~~Fade effect~~
* Multicolor fade effect
* MQTT support?
* Update screenshots of UI
