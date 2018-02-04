CROSS_COMPILE := arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc
WIRINGPI_PATH := /home/randy/RPdev/wiringPi_armhf/

all:
	$(CC) -o main_section1 main_section1.c assignment1.c -lwiringPi -lpthread -I$(WIRINGPI_PATH) -L$(WIRINGPI_PATH)

clean:
	rm -rf main_section1
