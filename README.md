# bbn_sensors_hub_C
NMEA 0183 XDR sensors on esp32. Engine

- Thermocouple sensor for exhaust temperature
- Engine RPM
- Resistive sensors 240-33 ohms or 10-180 ohms support for fuel level, engine oil pressure, rudder position, or trim
- IR Open Flame sensor
- i2c connector for more external i2c sensors supported by the Hub C firmware

## Hardware

### MAX6675 thermocouple sensor

Connected to M5Stack AtomS3-Lite via GPIO pins:

SO = 5

CS = 6

CLK = 7

Generates NMEA-0183 XDR sentences (USB Serial) like this:

````
$BBXDR,C,26.75,C,THERMOCOUPLE*6F
````

https://www.amazon.com/HiLetgo-MAX6675-Thermocouple-Temperature-Arduino/dp/B01HT871SO

#### Use cases

- Higher Temperature Ranges
- Exhaust gas temperature

#### See Also

https://github.com/adafruit/Adafruit_MCP9600

Adafruit MCP9600 I2C Thermocouple Amplifier - K, J, T, N, S, E, B and R Type T

https://www.adafruit.com/product/4101

## Loading Firmware

### On Bareboat Necessities (BBN) OS (full)

````
# shutdown signalk
sudo systemctl stop signalk

if [ -f bbn-flash-sensors-hub-C.sh ]; then rm bbn-flash-sensors-hub-C.sh; fi
wget https://raw.githubusercontent.com/bareboat-necessities/my-bareboat/refs/heads/master/m5stack-tools/bbn-flash-sensors-hub-C.sh
chmod +x bbn-flash-sensors-hub-C.sh
./bbn-flash-sensors-hub-C.sh -p /dev/ttyACM1
````

### Reading Serial Port

on USB-C port with baud rate 38400

````
stty -F /dev/ttyACM1 38400
socat stdio /dev/ttyACM1
````


## Example Output

````
$BBTXT,01,01,01,FirmwareTag: bbn_sensors_hub_C*48
$BBXDR,T,1999.8,R,RPM*19
$BBXDR,S,1,,OPEN_FLAME*24
$BBXDR,T,2000.1,R,RPM*1A
$BBXDR,C,21.50,C,THERMOCOUPLE*6F
$BBXDR,T,1998.1,R,RPM*11
$BBXDR,G,327.5,O,Ohms_esp32*6A
````



## Other Bareboat Necessities Devices

Project Home:  https://bareboat-necessities.github.io/


- Alarms Box: https://github.com/bareboat-necessities/bbn_alarms_A
- Sensors Hub: https://github.com/bareboat-necessities/bbn_sensors_hub_AB
- NMEA N2K to USB: https://github.com/bareboat-necessities/bbn-m5-s3-n2k-usb
- Instruments Displays on esp32: https://github.com/bareboat-necessities/bbn-m5stack-tough
- Boat Heave Sensor: https://github.com/bareboat-necessities/bbn-wave-period-esp32
- I2C over USB for Linux: https://github.com/bareboat-necessities/bbn-i2c-over-usb
