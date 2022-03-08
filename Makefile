all: sensor
sensor: sensor.c
	gcc $< -o $@ -Wall
