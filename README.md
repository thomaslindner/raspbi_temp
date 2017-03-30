# raspbi_temp
Readout of edev raspbi temperature board

Temperature readout through ADC readout by raspbi.  Original version of code from Keith Ong.

As currently written, the program (when logging) will log the first eight thermistor 
temperature readings to file at 5 second interval.  Data logged into this directory

/home/franciumpi/temperature_data

______________

Admin setup

Before being able to execute code in this repository, needed to do the following as root 
on the raspbi

ssh root@armdaq05
raspi-config // enable I2C
yum install i2c-tools
yum install libi2c-dev 

Add the following to /etc/rc.loca

[root@armdaq05]$ tail -n 3 /etc/rc.local
modprobe i2c-dev
modprobe i2c_bcm2708

Edit udev rules so any user can access the I2C bus

[root@armdaq05 ~]# more /lib/udev/rules.d/60-i2c-tools.rules
KERNEL=="i2c-0"     , GROUP="i2c", MODE="0660"
KERNEL=="i2c-[1-9]*", GROUP="i2c", MODE="0666"

Now test

[lindner@armdaq05 ~]# i2cdetect 1
WARNING! This program can confuse your I2C bus, cause data loss and worse!
I will probe file /dev/i2c-1.
I will probe address range 0x03-0x77.
Continue? [Y/n] y
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- 44 45 -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --

Install Keith's wiringPI stuff

[lindner@armdaq05 raspbi_temp]$ git clone git://git.drogon.net/wiringPi
[root@armdaq05 wiringPi]# pwd
/home/lindner/hardware/raspbi_temp/wiringPi
[root@armdaq05 wiringPi]# ./build 














