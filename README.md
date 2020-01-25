# roccat_kone_aimo
A few bits of code to set RGB leds on a Rocca Kone Aimo mouse.

Meant for linux, the python scripts might work on windows.  
<br/>
<br/>
The cpp tool is recommended, it's fast enough to be used in scripts for nice led animations.

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

For more infos read the sources. 
<br/>
<br/>
  
USB Captures provided by Metaln00b.

