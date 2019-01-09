# Please note that this directory is given purely as an example driver.

This driver was writting to compile on an embedded linux system that supports I2C.

If you want to use the SSD1306 I2C driver on a different system (MCU or CPU) then you will need to implement your own I2C.c to fit with your system.

That been said, its more likely that you have a different screen driver that you want to use. In this case, make a copy of the SSD1306.c and rename it to whatever your driver name is. Then go through each static function inside the c file and make the changes that you need for code to control your new screen. You will need to also make sure that you rename the following variable to match your display name.

*.h
```c
extern struct DisplayInterfaceType SSD1306;
```

*.c
```c
struct DisplayInterfaceType SSD1306 = {...}
```