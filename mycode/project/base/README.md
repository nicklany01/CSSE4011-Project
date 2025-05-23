## Base Node Build Instructions

Run the following command to build the base node firmware. This will create a zip file containing the firmware and necessary metadata for flashing.
```source $HOME/zephyr_install/env/bin/activate
west build -b nrf52840dongle/nrf52840 mycode/project/base -p
nrfutil nrf5sdk-tools pkg generate --hw-version 52 --sd-req=0x00 --application build/zephyr/zephyr.hex --application-version 1 base.zip
```

## Base Node Flash Instructions

Press the reset button on the nRF52840 Dongle. You should see a red LED blinking softly. This indicates that the dongle is in bootloader mode and ready to receive a firmware update. Run the following command to flash the firmware:
```
nrfutil nrf5sdk-tools dfu usb-serial -pkg base.zip -p /dev/ttyACM0
```
