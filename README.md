# Real-time Photo-sensitive Curtain

Sensor thread and motor thread on the Raspberry Pi for Real-time Photo-sensitive Curtain.

## Usage

### Sensor Setup

To setup our model, one should connect the indoor sensor and the outdoor sensor to a separate set of I2C pins on the Raspberry Pi, or any other Linux device that one may chose to use.
Make sure the I2C buses are enabled and made available to user space programs by checking if files named `i2c-0`, `i2c-1`, etc. exists under the `/dev` directory.
On the Raspberry Pi, one may enable the I2C buses to user space programs by adding `dtparam=i2c_vc=on` and `dtparam=i2c_arm=on` to `/boot/config.txt`.
To make it available to uesr space programs, add `i2c-dev` to `/etc/modules`.
Reboot the Pi, now one should be able to see those files.
This setting is persistent across system reboots so we decided not to automatically enable the settings as this requires root privilege.
Now one may execute the sensor thread program with 

```
<program_name> <i2c_file> <port>
```

to run the thread on the port specified with the sensor associated with the i2c file.

### Motor Setup

To setup our model, one should connect the ENA+, DIR+, and PUL+ signals to the GPIO pins on the Raspberry Pi, and connect ENA-, DIR-, PUL- signals to the ground pins on the Raspberry Pi.
The program will automatically export the GPIO pins and unexport the pins when the program terminates.
However, one should note that exporting and unexporting GPIO pins requires root privilege and thus should run the program as root.
Now one may execute the stepper motor thread program with

```
<program_name> <ENA+> <DIR+> <PUL+> <port>
```

as the root user or with sudo to run the the thread on the port specified.

### Quick Start Script Setup

To use the quick start script, specify the starting port number that you want to use and update commands in the quick start script with valid arguments, as specified above.
Since the quick script runs the stepper motor thread program, which requires root privilege, the script needs to be run with root privilege as well.
Now one may run the script with

```
<script_name>
```

as the root user or with sudo.

To run the quick start script at boot time, one may use the cron deamon.
Edit the crontab file using

```
crontab -e
```

as the root user or with sudo, and add

```
@reboot sudo <script_path>
```

to the file.

### Light Setup

The model does not require any additional setup besides having a basic Hue bridge setup.
One may set their Hue bridge up following the manual available from https://www.philips-hue.com/en-us/support/connect-hue-product/accessories/hue-bridge.

### Client

To interact with our model, open the client application and connect to the sensor and motor threads, and the Hue bridge by clicking on their respective area.
A connection page will appear.
If connecting to a thread, one only have to supply the address and port of the thread and click OK to connect.
If connecting to the Hue bridge, one should supply the address of the Hue bridge and their username.
To generate a new username, click on the new user button with the bridge in link mode.
Then the application will either prompt the user to press the link button to put the bridge into link mode, or update the username field with the newly generated username.

When the application is connected to the motor thread, the curtain raise button and the curtain lower button will be enabled.
The user may press the curtain raise button or the curtain lower button to raise or lower the curtain, respectively.
When the application is connected to the Hue bridge, the toggle light button will be enabled.
The user may click the button to toggle the first light connected to the Hue bridge.

When both sensor threads and either a motor thread or a Hue bridge is connected to the application, the application will allow the user to put the application in auto mode.
Under auto mode, the controller service will automatically adjust the curtain and light, if connected, to reach the comfortable zone specified by the user.
To adjust the comfortable zone, slide the sliders at the center of the control panel.
