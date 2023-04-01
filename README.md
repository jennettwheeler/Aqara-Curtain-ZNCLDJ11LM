# Esphome Aqara Curtain Motor (ZNCLDJ11LM)
Converts Aqara Zigbee Motor ZNCLDJ11LM to Esphome

I was having zigbee range issues in my home due to a tiny number of devices in my mesh (just two motors). I had improved range when adding in smart plugs (used as routers), but ultimately I came back to my mantra of avoiding wireless devices.

I am currently using a Wireless-Tag WT32-ETH01 to add my motor to my network via ethernet. However, my WiFi setup is very good, so I will likely switch to Wemos Mini D1 as I have other plans for the "more-expensive" WT32-ETH01.

## Setup
Add the uart_curtain.h file to a directory where esphome can access it.

See my yaml file for an example config.


## Todo Improvements:
- Check for "confirmation" (and ask again if failed?)
- Better error checking states
- One button calibrate (full open-close cycle)
- Consistent code-style (sorry)

## Credits
For having already decoded the uart protocol:
https://github.com/HomeACcessoryKid/AQARA-ZNCLDJ11LM

And for some inspiration for how to use c++, and custom covers in esphome
https://github.com/iphong/esphome-tuya-curtain

And for the CRC function:
https://github.com/LacobusVentura/MODBUS-CRC16