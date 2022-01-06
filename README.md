# Reaction Timer
This is a reaction timer initially programmed in C. The C code is translated to armv7 assembly in order to use features from CPUlator. 

You can access CPUlator @ https://cpulator.01xz.net/?sys=arm-de1soc 

Once in CPUlator, open the .s file. Next, compile and load the code. Once the code is compiled and loaded, hit continue and test how fast you can react to the LEDs lighting up.

Only LEDs 9 and 0 light up, and pushbutton 3 corresponds to LED 9, and pushbutton 0 corresponds to LED 0. If the LED has been on for over 2 seconds, you fail.

Good Luck!
