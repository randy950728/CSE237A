// UCSD CSE237A - WI18
// Important! You WILL SUBMIT this file.
// Replace this with your implementation of Section 2
// For more details, please see the instructions in the class website.

#include "section1.h"
#include "shared_var.h"
#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

void program_init(SharedVariable* sv) {
	// You can initialize the shared variable if needed.
    sv->bProgramExit = 0;
    pinMode(PIN_ALED, OUTPUT);
    digitalWrite(PIN_ALED, HIGH);

    //Set DIP LED Pin Mode
    pinMode(PIN_DIP_RED, OUTPUT);
    pinMode(PIN_DIP_GRN, OUTPUT);
    pinMode(PIN_DIP_BLU, OUTPUT);
    softPwmCreate(PIN_DIP_RED, 0, 0xFF);
    softPwmCreate(PIN_DIP_GRN, 0, 0xFF);
    softPwmCreate(PIN_DIP_BLU, 0, 0xFF);

    //Set SMD LED Pin Mode
    pinMode(PIN_SMD_RED, OUTPUT);
    pinMode(PIN_SMD_GRN, OUTPUT);
    pinMode(PIN_SMD_BLU, OUTPUT);
    softPwmCreate(PIN_SMD_RED, 0, 0xFF);
    softPwmCreate(PIN_SMD_GRN, 0, 0xFF);
    softPwmCreate(PIN_SMD_BLU, 0, 0xFF);

    //Set senors Pin Mode
    pinMode(PIN_BIG, INPUT);
    pinMode(PIN_SMALL, INPUT);
    pinMode(PIN_BUTTON, INPUT);

    // You also need to initalize sensors here
}

void program_body(SharedVariable* sv) {
    static int state=1;
    static int big_val=0;
    static int small_val=0;
    static int button_val=0;

    static int prev_state=1;
    static int prev_big_val=0;
    static int prev_small_val=0;
    static int prev_button_val=0;

    prev_big_val    = big_val;
    prev_button_val = button_val;
    prev_small_val  = small_val;
    button_val = digitalRead(PIN_BUTTON);
    small_val  = digitalRead(PIN_SMALL);
    big_val    = digitalRead(PIN_BIG);

    if(button_val == 0 && prev_button_val!=button_val)
    {
        // prev_state = state;
        state = 1-state;
    }

    // printf("state: %d, button: %d, small: %d, big %d \n",state,button_val, small_val, big_val);

    // State-0 turn off every led we have
    //-----------------------------------------------------//
    if(state==0)
    {
        if(prev_state!= state)
        {
            prev_state=state;
            softPwmWrite(PIN_SMD_RED,0x00);
            softPwmWrite(PIN_SMD_GRN,0x00);
            softPwmWrite(PIN_SMD_BLU,0x00);

            softPwmWrite(PIN_DIP_RED,0x00);
            softPwmWrite(PIN_DIP_GRN,0x00);
            softPwmWrite(PIN_DIP_BLU,0x00);

            digitalWrite(PIN_ALED, LOW);
        }
    }
    printf("we are here!!\n");
    // State-1 turn on leds we have, change color based on input
    //-----------------------------------------------------//
    else
    {
        if(prev_state!=state)
            digitalWrite(PIN_ALED, HIGH);

        // If there is a change in sensor value or state, reassign LED values
        if(prev_small_val != small_val || prev_big_val!=big_val || prev_state!=state)
        {
            // Update previous state
            prev_state = state;

            //Change DIP LED based on small sensors
            //--------------------------------------------//
            if(small_val==0)
            {
                softPwmWrite(PIN_DIP_RED,0x00);
                softPwmWrite(PIN_DIP_GRN,0x00);
                softPwmWrite(PIN_DIP_BLU,0xFF);
            }

            else
            {
                softPwmWrite(PIN_DIP_RED,0xFF);
                softPwmWrite(PIN_DIP_GRN,0x00);
                softPwmWrite(PIN_DIP_BLU,0x00);
            }

            //Change SMD LED based on small sensors
            //--------------------------------------------//
            if(small_val==0 && big_val==0)
            {
                softPwmWrite(PIN_SMD_RED,0xFF);
                softPwmWrite(PIN_SMD_GRN,0x00);
                softPwmWrite(PIN_SMD_BLU,0x00);
            }

            else if(small_val==1 && big_val==0)
            {
                softPwmWrite(PIN_SMD_RED,0xEE);
                softPwmWrite(PIN_SMD_GRN,0x00);
                softPwmWrite(PIN_SMD_BLU,0xC8);
            }

            else if(small_val==0 && big_val==1)
            {
                softPwmWrite(PIN_SMD_RED,0x80);
                softPwmWrite(PIN_SMD_GRN,0xFF);
                softPwmWrite(PIN_SMD_BLU,0x00);
            }

            else if(small_val==1 && big_val==1)
            {
                softPwmWrite(PIN_SMD_RED,0x00);
                softPwmWrite(PIN_SMD_GRN,0xFF);
                softPwmWrite(PIN_SMD_BLU,0xFF);
            }
        }
    }

}

void program_exit(SharedVariable* sv) {
    // Clean up everything if needed.
    // This is called when the program finishes.
    digitalWrite(PIN_ALED, LOW);
}

