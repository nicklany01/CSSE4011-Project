## Pet Node Build Instructions

Run the following command to build the pet node firmware.
```
source $HOME/zephyr_install/env/bin/activate
west build -b m5stack_core2/esp32/procpu mycode/project/pet -p
```

## Pet Node Flash Instructions

```
west flash
```
