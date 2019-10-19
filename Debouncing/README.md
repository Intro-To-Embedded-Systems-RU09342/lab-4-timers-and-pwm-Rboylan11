# Debouncing
The objective of this lab is to make sure that a single button press is not mistaken for multiple by the processor. so that a single button press isn't read multiple times. To do so, enable a timer and diable the button. Once the timer hits the desired interrupt timing, the button will be enabled again. 

The difference in code between the two boards is that there are different pins for the LEDs and buttons. Also, for the FR6989 you must disable the GPIO power-on default high-impedance mode. 