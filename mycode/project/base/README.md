## Base Node Build Instructions

```source $HOME/zephyr_install/env/bin/activate
west build -b -p nrf52840dongle/nrf52840 mycode/project/base
nrfutil nrf5sdk-tools pkg generate --hw-version 52 --sd-req=0x00 --application build/zephyr/zephyr.hex --application-version 1 base.zip
```

## Base Node Flash Instructions

`nrfutil nrf5sdk-tools dfu usb-serial -pkg base.zip -p /dev/ttyACM0`
