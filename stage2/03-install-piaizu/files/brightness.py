#!/usr/bin/python

import smbus
import time
import sys

bus = smbus.SMBus(3)    # 3 = /dev/i2c-3
DEVICE_ADDRESS_L = 0x38   #Address bit=L (0x70)=>0111 0000=>011 1000(0x38)

ref_pulse = 398.0  #reference(average) pulse at 500nit

args = sys.argv
print "Your target brightness is " + args[1] + "nit"

#Calculate the target pwm value from pulse value.
#The relationship between pulse and pwm is non-linear. Need to be calculated.
def pulse2pwm(pulse):
	if pulse <= 255.0:
		pwm = pulse
	elif (255 < pulse) and (pulse <= 510):
		pwm = (pulse - 256.0)/2 + 256
	elif (510 < pulse) and (pulse <=1024):
		pwm = (pulse - 511.0)/4 + 384
	elif (1024 < pulse) and (pulse <= 2040):
		pwm = (pulse - 1021.0)/8 + 512
	elif (2040 < pulse) and (pulse <= 3825):
		pwm = (pulse - 2041.0)/15 + 640
	elif (3825 < pulse) and (pulse <= 7461):
		pwm = (pulse - 3826.0)/29 + 759
	elif 7461 < pulse:
		pwm = 1023
	return pwm

#Calculate the target NUM register and WIDTH register
#Not linear. So need to be calculated.
def pwm2num_width(pwm):
	if pwm <= 255.0:
		num = 5
		width = int(pwm)
	elif (255 < pwm) and (pwm <= 383):
		num = 4
		width = int(pwm - 128)
	elif (383 < pwm) and (pwm <= 511):
		num = 3
		width = int(pwm - 256)
	elif (511 < pwm) and (pwm <= 639):
		num = 2
		width = int(pwm - 384)
	elif (639 < pwm) and (pwm <= 758):
		num = 1
		width = int(pwm - 503)
	elif (758 < pwm) and (pwm <= 884):
		num = 0
		width = int(pwm - 630)
	elif 884 < pwm:
		num = 0
		width = 255
	return num, width


target_nit = long(args[1])	#Target nit

#Target pulse is calculated from reference pulse value
#The relationship between pulse and nit is linear. Can be calculated with simple formula.
target_pulse = (target_nit/500.0) * ref_pulse
target_pwm = pulse2pwm(target_pulse)
target_num, target_width = pwm2num_width(target_pwm)
print "NUM=" + hex(target_num)
print "WIDTH=" + hex(target_width)


#bus.write_byte_data(device address, register address, value)
bus.write_byte_data(DEVICE_ADDRESS_L, 0x4D, target_num)	#Write NUM register
bus.write_byte_data(DEVICE_ADDRESS_L, 0x4E, target_width)	#Write WIDTH register
