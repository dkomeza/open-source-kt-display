# TFT_eSPI setup

Copy the code below into User_Setup.h
```h
#define DISABLE_ALL_LIBRARY_WARNINGS
#define ST7789_DRIVER

#define TFT_RGB_ORDER TFT_BGR // Colour order Blue-Green-Red

#define TFT_INVERSION_ON

#define TFT_MISO 21
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS 15 // Chip select control pin
#define TFT_DC 2  // Data Command control pin
#define TFT_RST 4 // Reset pin (could connect to RST pin)

#define LOAD_GLCD
#define LOAD_FONT2
#define SMOOTH_FONT

#define SPI_FREQUENCY 40000000
```
