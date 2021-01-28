# roccat_kone_aimo
A few bits of code to set RGB leds on a Rocca Kone Aimo mouse.<br/>
And since 2021, other Rocca Kone * mice might also work.<br/>
<br/>
Meant for linux, the python scripts might work on windows.  
<br/>
<br/>
The cpp tool `roccat_kone_aimo_rgb_tool.cpp` is recommended, it's fast enough to be used in scripts for nice led animations.

Or just to disable the leds (Set all to 0).

It only works with the hidraw driver on linux.
<br/>
Build and usage instructions can be found in the source file. 

<br/>

`aimo_animate_example.py` is meant to be used with the cpp tool to make animations.
<br/>
<br/>
`aimo.py` can set colors on its own, but cannot be used to animate leds under linux, because libusb cannot do that.  
<br/>
<br/>

To compile the cpp tool:<br/>
<code>g++ roccat_kone_aimo_rgb_tool.cpp -o roccat_kone_aimo_rgb_tool -I/usr/include -O3 -march=native</code><br/>
Use the tool to set all leds to a nice red:<br/>
<code>./roccat_kone_aimo_rgb_tool /dev/hidraw0 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00</code><br/>
To use a single color for all LEDs you may use the shorthand version:<br/>
<code>./roccat_kone_aimo_rgb_tool /dev/hidraw0 FF 00 00</code><br/>
<br/>
For more infos read the sources. 
<br/>
<br/>
  
USB Captures provided by Metaln00b.

