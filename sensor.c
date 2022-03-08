/*
 * sensor.c
 * 
 * Process the data from an indoor and an
 * outdoor tsl2561 ambient light sensor.
 * 
 */

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/* path to the i2c bus file */
#define INDOOR_I2C_BUS "/dev/i2c-0"
#define OUTDOOR_I2C_BUS "/dev/i2c-1"

/* length of registers */
#define CONTROL_REGS 2
#define TIMING_REGS 2
#define READ_REGS 1
#define DATA_REGS 4

/* sensor magic numbers */
/* fixed TSL2561 address (0x39) */
#define SENSOR_ADDR 0x39
/* select control register (0x80) */
/* set power on mode (0x03) */
const char control[CONTROL_REGS] = {0x80, 0x03};
/* select timing register (0x81) */
/* set 402ms resolution  (0x02) */
const char timing[TIMING_REGS] = {0x81, 0x02};
/* select read register (0x8c) */
const char reading[READ_REGS] = {0x8c};

/* sensor threshold */
int dim = 200;
int bright = 400;

/* is light on? */
int light = 0;

/* program error codes */
enum ERR_CODE
{
	SUCCESS,
	ERR_OPEN = -1,
	ERR_IOCTL = -2,
	ERR_WRITE_CONTROL = -3,
	ERR_WRITE_TIMING = -4,
	ERR_WRITE_READ = -5,
	ERR_READ_DATA = -6
};

int init_sensor(const char* bus_name)
{
	/* stores the fd of the i2c bus */
	int fd = ERR_OPEN;
	
	/* attempt to open the i2c bus */
	fd = open(INDOOR_I2C_BUS, O_RDWR);
	if(fd < SUCCESS) 
	{
		perror("failed to open i2c bus");
		return ERR_OPEN;
	}

	/* select the sensor */
	if (ioctl(fd, I2C_SLAVE, SENSOR_ADDR) < SUCCESS)
	{
		perror("failed to find sensor");
		return ERR_IOCTL;
	}
	
	/* configure the sensor */
	if (write(fd, control, CONTROL_REGS) != CONTROL_REGS)
	{
		perror("failed to power on sensor");
		return ERR_WRITE_CONTROL;
	}
	if (write(fd, timing, TIMING_REGS) != TIMING_REGS)
	{
		perror("failed to set sensor timing");
		return ERR_WRITE_TIMING;
	}
	
	return fd;
}

int read_sensor(int fd)
{
	char data[DATA_REGS] = {0};
	
	/* read data from sensor */
	if (write(fd, reading, READ_REGS) != READ_REGS)
	{
		perror("failed to initialize read");
		return ERR_WRITE_READ;
	}
	
	if(read(fd, data, DATA_REGS) != DATA_REGS)
	{
		perror("failed to read data");
		return ERR_READ_DATA;
	}
	
	/* ch0 lsb, ch0 msb, ch1 lsb, ch1 msb */
	/* channel 0 accounts for the full spectrum */
	/* channel 1 accounts for the infrared spectrum */
	/* reformat the data */
	int full = data[1] * 256 + data[0];
	int infrared = data[3] * 256 + data[2];
	/* get the visible light spectrum by calculating the difference */
	return (full - infrared);
}

int main(int argc, char* argv[])
{
	int indoor_fd, outdoor_fd;
	int indoor, outdoor;
	
	indoor_fd = init_sensor(INDOOR_I2C_BUS);
	if (indoor_fd < SUCCESS)
	{
		printf("failed to initialize indoor sensor, stopping.\n");
		return indoor_fd;
	}
	
	outdoor_fd = init_sensor(OUTDOOR_I2C_BUS);
	if (outdoor_fd < SUCCESS)
	{
		printf("failed to initialize outdoor sensor, stopping.\n");
		return outdoor_fd;
	}
	
	while (1)
	{
		indoor = read_sensor(indoor_fd);
		if (indoor < SUCCESS)
		{
			/* failed to read from indoor sensor */
			printf("failed to read from indoor sensor, stopping.\n");
			return indoor;
		}
		else if (indoor < dim)
		{
			outdoor = read_sensor(outdoor_fd);
			if (outdoor < SUCCESS)
			{
				/* failed to read from outdoor sensor*/
				printf("failed to read from outdoor sensor, stopping.\n");
				return outdoor;
			}
			else if (outdoor < dim)
			{
				if (light)
				{
					printf("too dim even with the light on.\n");
				}
				else
				{
					light = 1;
					printf("too dim, turing on the light.\n");
				}
			}
			else
			{
				printf("too dim, raising the curtain.\n");
			}
		}
		else if (indoor > bright)
		{
			if (light)
			{
				light = 0;
				printf("too bright, turning off the light.\n");
			}
			else
			{
				printf("too bright, lowering the curtain.\n");
			}
		}
		else
		{
			if (light)
			{
				/* check if the light is on and it's bright outside */
				outdoor = read_sensor(outdoor_fd);
				if (outdoor < SUCCESS)
				{
					/* failed to read from outdoor sensor*/
					printf("failed to read from outdoor sensor, stopping.\n");
					return outdoor;
				}
				else if (outdoor > bright)
				{	
					light = 0;
					printf("bright outside, turning off the light to save energy.\n");
				}
				else
				{
					printf("comfortable with light on.\n");
				}
			}
			else
			{
				printf("comfortable with light off.\n");
			}
		}
		
		/* todo: switch to timer. */
		sleep(1);
	}
	
	return SUCCESS;
}
