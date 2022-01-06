//
// A bare-bones template for C code that works with the DE1-SOC
// simulation on godbolt
//
// 2021-03-26  KJHass
//
#include <https://raw.githubusercontent.com/kjoehass/godbolt/main/de1soc.h>

//
// ECEG 247 Final Project
//
// 2021-05-13  Pedro Carneiro Passos
//
// This program creates a reaction time tester in C.
//
// This code uses a structure similar to Arduino sketches. All of the ugly
// configuration and exception handling is done behind the scenes, and the
// user just provides a 'setup()' function (which is run exactly once) and
// a 'loop()' function (which is executed repeatedly forever).
//
#include <https://raw.githubusercontent.com/kjoehass/godbolt/main/de1soc.h>

//
// Prototypes for functions to be defined below
//
void config_gic(uint32_t irq_number); 
void SERVICE_IRQ();
void setup();
void loop();

//
// Global and static variables
//
// count is a variable that will be used to keep track of how many
// timer interrupts have occurred since the program started.
//
volatile uint32_t count;

//
// The RESET exception handler. This is really the main background program.
// This function is executed when power comes up or the reset button is
// pressed. It never ends or returns.
// 
void SERVICE_RESET() {
    // Initialize the stack pointers for both SVC and IRQ modes, then return
    // to SVC mode
    __set_SP(0x800);
    __set_mode(IRQ_MODE);
    __set_SP(0x700);
    __set_mode(SVC_MODE);

    // Call config_gic() for each IRQ we want to recognize
    
    config_gic(TIMER1_IRQ);

    // Call the user's 'setup()' function just once
    setup();

    // Enables the IRQ interrupt in the CPSR.
    __enable_irq();

    // Execute the user's 'loop()' function forever within an infinite loop.
    while (1) {
        loop();
    }
}

//
// The GIC is configured for a given IRQ number.
//
void config_gic(uint32_t irq_number) {
    uint32_t index, offset;

    // Configure the CPU Targets Register to forward the IRQ to CPU 0.
    index = irq_number / 4;
    offset = (irq_number % 4) * 8;
    GICDistributor->ITARGETSR[index] |= 0x1 << offset;

    // Enable the IRQ by writing to the Set Enable Register
    index = irq_number / 32;
    offset = irq_number % 32;
    GICDistributor->ISENABLER[index] = 0x1 << offset;

    // Enable all interrupts in the Control registers in the Distributor
    // and in the CPU Interface by writing a '1' to the LSB
    GICInterface->CTLR = 1;
    GICDistributor->CTLR = 1;
}

//
// The IRQ interrupt service routine.
//
__attribute__ ((interrupt ("IRQ"))) void SERVICE_IRQ() {
    uint32_t current_irq;
    // Get the current IRQ number from the GIC CPU Interface: Interrupt
    // Acknowledge Register
    current_irq = GICInterface->IAR;

    // If the IRQ number is for Timer1 then
    //   -- Increment the variable that counts timer interrupts
    //   -- Clear the timeout bit in the timer status register

    if (current_irq == TIMER1_IRQ){
        count += 1;
        uint32_t value = Timer1 -> Status = 1;
    }

    // Write the current IRQ number to the GIC CPU Interface: End Of
    // Interrupt Register to indicate that we are done servicing this IRQ
    GICInterface->EOIR = current_irq;
}

//
// The user's setup function.
//   -- Initialize the value of the variable that counts how many timer
//      interrupts have occurred 
//   -- Configure the Interval Timer 1. Set the start value for 100Hz
//      interrupts assuming a counter clock frequency of 100MHz. Enable
//      continuous operation, enable the timer interrupt, and set the
//      Start bit.
//
void setup() {

    count = 0;
    Timer1 -> StartLow = 0x4240;
    Timer1 -> StartHigh = 0xF;
    Timer1-> Control = (CONTINUOUS | START | INTERRUPTTO); 

}


// This function first waits for space to be available in the JTAG UART
// transmit FIFO, then writes the character to the JTAG UART data register.
void print_char(uint32_t number) {

    //check for available space
    uint32_t available_space = (UART -> Control) & WSPACE_MSK;
    
    //if there is available space
    if (available_space >= 5){
        
        //write character to the JTAG UART data register
        UART -> Data = number; 
    
    }
}


//This function waits for ALL pushbutton switches to be released.
void wait_release() {

    while ((PBUTTONS -> Data & ((1<<0) | (1<<1) | (1<<2) | (1<<3))) != 0) {} 

}


//This function turns both LED 0 and LED 9 on.
void display_leds_nine_zero() {

    LEDS -> Data = 0x201;

}


//This function is equivalent to a one second delay.
//Since we want to wait 1s and the period of the timer 
//interrups is 10ms, the delay time must be 100.
void wait_one_sec() {

    uint32_t cur = count;    
    while ((count - cur) < 100){}

}


//This function turns ALL LEDs off.
void turn_off() {

    LEDS -> Data = 0x0;

}


//This function randomly illuminates either LED 0 or 9.
//It takes one parameter number which is an integer
//that holds value of the lower-half word of the snapshot
//of the current value of Interval Timer 1.
void random_led(uint32_t number){

    //if bit 9 of number is 1, turn LED 9 on.
    if ((number & (1<<9)) != 0){

        LEDS -> Data = 0x200;

    //otherwise, bit 9 of number must be 0, so turn LED 0 on.
    } else {

        LEDS -> Data = 0x01;
    }
}


//This fuction is created solely to organize how results are
//sent to the JTAG UART. 
void print_message(uint32_t feedback){

    //If user failed to click the correct pushbutton according to LED
    //send message FAIL to the JTAG UART
    if (feedback == 0) {
        print_char('F');
        print_char('A');
        print_char('I');
        print_char('L');

    //If the user clicks the correct pushbutton within 2s, send the
    //reaction time (in seconds) of the user to the JTAG UART
    } else {
        uint32_t first = (feedback / 100) + 48;
        uint32_t second = ((feedback % 100) / 10) + 48;
        uint32_t third = ((feedback % 100) % 10) + 48;
        print_char(first);
        print_char('.');
        print_char(second);
        print_char(third);
    }

    print_char('\n'); //print newline character to the JTAG UART
}


//This function is responsible of checking if the correct pushbutton is pressed
//according to the LED that turns on randomly within a period of 2s. This means
//if LED 0 turns on, click pushbutton 0. If LED 9 turns on, click pushbutton 3.
void response() {

    uint32_t cur = count;
    while (count - cur < 200){

        //if LED 9 is turns on and ONLY pushbutton 3 is pressed within 2s
        if (((PBUTTONS -> Data & (1 << 3)) != 0) && (LEDS -> Data == 0x200)) {

            //Ensuring no other pushbutton is on
            if ((PBUTTONS -> Data & ( (1<<2) | (1<<1) | (1<<0) )) == 0){

                uint32_t reaction_time = count - cur; //obtain reaction time
                print_message(reaction_time); //send the reaction time to the JTAG UART and return
                return;
            }
        //if LED 0 is turns on and ONLY pushbutton 0 is pressed within 2s
        } else if (((PBUTTONS -> Data & (1 << 0)) != 0) && (LEDS -> Data == 0x01)) {

            //Ensuring no other pushbutton is on
            if ((PBUTTONS -> Data & ( (1<<2) | (1<<1) | (1<<3) )) == 0){

                uint32_t reaction_time = count - cur; //obtain reaction time
                print_message(reaction_time); //send the reaction time to the JTAG UART and return
                return;
            }
        }
    }

    //wrong LED was clicked, more than 1 LED was clicked, or no LED was clicked
    //the line below will send the message FAIL to the JTAG UART
    print_message(0);
}


//
// The user's loop function to perform the reaction time tester
//
void loop() {

    //writing to snapshotlow
    Timer1 -> SnapshotLow = 0x29; 
    
    //save value in SnapshotLow register to generate random LED later
    uint32_t lower_half = Timer1 -> SnapshotLow;

    //wait for all pushbuttons to be released
    wait_release(); 

    //Turn both LED 9 and LED 0 on
    display_leds_nine_zero(); 

    //Wait one second
    wait_one_sec();

    //Turn all LEDs off
    turn_off();

    //Wait one second
    wait_one_sec();

    //Randomly select either LED 9 or LED 0 to be turned on
    random_led(lower_half);

    //check if the user failed to select the correct pushbutton in 2s
    //-> send FAIL to the JTAG UART
    //or if user selected correct pushbutton within 2s
    //-> send reaction time to the JTAG UART
    response();
}
