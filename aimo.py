#!/usr/bin/python3

# pyaimo # (2020) by ninnghazad and Metaln00b

# This sets the RGB leds of a Roccat Kone Aimo mouse.
# Supposed to be run using python 3 with pyusb module (pip install pyusb).
# Either execute as root or make sure your user has whatever permissions
# needed to control usb devices on your system.

# 1e7d:2e27 ROCCAT ROCCAT Kone Aimo
VID=0x1e7d
PID=0x2e27

# there are 11 leds, each gets three values, RGB, as float 0-1.
# the first led is the wheel, then leds are numbered anti-clockwise, the last two are the lower side ones.
# at least on my mouse the wheel led has slightly different colors than the other leds - set to white, it is slighty red
# if you hate the leds, set all colors to 0,0,0 - that will disable them.

# default colors: all bright red.
colors = [
	[1,0,0], # wheel, WHEEL
	[1,0,0], # left strip top, STRIP_LEFT_0
	[1,0,0], # left strip below top, STRIP_LEFT_1
	[1,0,0], # left strip above bottom, STRIP_LEFT_2
	[1,0,0], # left strip bottom, STRIP_LEFT_3
	[1,0,0], # right strip top, STRIP_RIGHT_0
	[1,0,0], # right strip below top, STRIP_RIGHT_1
	[1,0,0], # right strip above bottom, STRIP_RIGHT_2
	[1,0,0], # right strip bottom, STRIP_RIGHT_3
	[1,0,0], # left lower side, LOWER_LEFT
	[1,0,0]  # right lower side, LOWER_RIGHT
]

### internals

import usb.core
import usb.util
import atexit
import time
import sys
import math
import random
import codecs

RED=0
GREEN=1
BLUE=2

WHEEL=0
STRIP_LEFT_0=1
STRIP_LEFT_1=2
STRIP_LEFT_2=3
STRIP_LEFT_3=4
STRIP_RIGHT_0=5
STRIP_RIGHT_1=6
STRIP_RIGHT_2=7
STRIP_RIGHT_3=8
LOWER_LEFT=9
LOWER_RIGHT=10

def colors_to_msg(colors):
	# header
	msg = "0d2e";
	for color in colors:
		# each triplet is followed by a null byte
		msg += "%02x%02x%02x00" % (color[0]*255,color[1]*255,color[2]*255)
	# add padding, device expects 46 bytes.
	msg = msg.ljust(46,'0')
	# return as binary data
	return codecs.decode(msg,"hex")

def set_colors(colors):
	msg = colors_to_msg(colors)
	dev.ctrl_transfer(0x21, 0x9, wValue=0x030d, wIndex=0x0, data_or_wLength=msg)
	dev.ctrl_transfer(0xA1, 0x01, wValue=0x0304, wIndex=0x0, data_or_wLength=3)

def deinit():
	global dev
	usb.util.dispose_resources(dev)
	dev.attach_kernel_driver(0)
	dev.attach_kernel_driver(1)

def init():
	global dev
	dev = usb.core.find(idVendor=VID, idProduct=PID)
	if dev is None:
		raise ValueError('No ROCCAT KONE AIMO found.')

	atexit.register(deinit)
	dev.reset()

	# free up the device from the kernal
	if dev.is_kernel_driver_active(0):
		dev.detach_kernel_driver(0)
	if dev.is_kernel_driver_active(1):
		dev.detach_kernel_driver(1)


	dev.set_configuration()

	try:
		usb.util.claim_interface(dev, 0)
	except usb.core.USBError as e:
		sys.exit("Error occurred on claiming")

	# ask the device something we don't actually care about. swarm does it, but i think we might not need it.
	result=dev.ctrl_transfer(0xA1, 0x01, wValue=0x0304, wIndex=0x0, data_or_wLength=3)
	# print("INIT:",result)

	# send some magic init msg, just like swarm-software does
	data = codecs.decode("0E06010100FF","hex")
	result=dev.ctrl_transfer(0x21, 0x9, wValue=0x030E, wIndex=0x00, data_or_wLength=data)
	result=dev.ctrl_transfer(0x21, 0x9, wValue=0x030E, wIndex=0x00, data_or_wLength=data)

### end of internals

if __name__ == "__main__":
	init()

	# simply set colors specified. doing this once after plugging in is enough if you just want static colors.
	set_colors(colors)



	# stuff below doesn't work on linux, might on window: animating the colors currently makes the mouse unusable.



	# something a little more fancy, for this, the script has to run in a loop to update the colors:
	#    - slowly pulse the red component of the two 8 "stripe" leds on the back.
	#    - slowly pulse the red component of the two 2 "side", but at a different speed.
	#    - slowly pulse the red component of the wheel, but at yet another different speed.

	# while(True):
	#       # use good old sin(time) to get a pulsing value
	#       s0 = math.sin(time.time()) * .5 + .5
	#       s1 = math.sin(time.time()/2) * .5 + .5
	#       s2 = math.sin(time.time()*3.5) * .5 + .5
	#
	#       for i in range(1,9):
	#	       colors[i][RED] = s0
	#
	#       colors[LOWER_LEFT][RED] = s1
	#       colors[LOWER_RIGHT][RED] = s1
	#       colors[WHEEL][RED] = s2
	#
	#       set_colors(colors)
	#       time.sleep(1.0/30.0) # do this at 30hz - thats more than enough


	# example: total eyesore-party-mode (don't actually use this...)

	# while(True):
	#       for i in range(0,11):
	#	       colors[i][RED] = random.uniform(0,1)
	#	       colors[i][GREEN] = random.uniform(0,1)
	#	       colors[i][BLUE] = random.uniform(0,1)
	#       set_colors(colors)
	#       time.sleep(1.0/30.0) # do this at 30hz - thats more than enough
