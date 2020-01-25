#!/usr/bin/python

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
import sys,os
import math
import random

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

def colors_to_args(colors):
	args = ""
	for color in colors:
		args += "%02x %02x %02x " % (color[0]*255,color[1]*255,color[2]*255)

	return args
def set_colors(colors):
	device = "/dev/hidraw0"
	if len(sys.argv)>1:
		device = sys.argv[1]
	cmd = "/usr/local/bin/roccat_kone_aimo_rgb_tool "+device+" "+colors_to_args(colors)
	os.system(cmd)

if __name__ == "__main__":
	# something a little more fancy, for this, the script has to run in a loop to update the colors:
	#    - slowly pulse the red component of the "stripe" leds on the back.
	#    - slowly pulse the red component of the "side", but at a different speed.
	#    - slowly pulse the red component of the wheel, but at yet another different speed.

	while(True):
		# use good old sin(time) to get a pulsing value
		s0 = math.sin(time.time()) * .5 + .5
		s1 = math.sin(time.time()/2) * .5 + .5
		s2 = math.sin(time.time()*3.5) * .5 + .5

		for i in range(1,9):
			colors[i][RED] = s0

		colors[LOWER_LEFT][RED] = s1
		colors[LOWER_RIGHT][RED] = s1
		colors[WHEEL][RED] = s2

		set_colors(colors)
		time.sleep(1.0/30.0) # do this at 30hz - thats more than enough

	# example: total eyesore-party-mode (don't actually use this...)

	# while(True):
	# 	for i in range(0,11):
	# 		colors[i][RED] = random.uniform(0,1)
	# 		colors[i][GREEN] = random.uniform(0,1)
	# 		colors[i][BLUE] = random.uniform(0,1)
	# 	set_colors(colors)
	# 	time.sleep(1.0/30.0) # do this at 30hz - thats more than enough
