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
    int button_val=0;
    int small_val=0;
    int big_val=0;
    button_val = digitalRead(PIN_BUTTON);
    small_val  = digitalRead(PIN_SMALL);
    big_val    = digitalRead(PIN_BIG);

    if(button_val == 0)
    {
        state = 1-state;
    }

    // printf("state: %d, button: %d, small: %d, big %d \n",state,button_val, small_val, big_val);
    if(state==0)
    {
        softPwmWrite(PIN_SMD_RED,0x00);
        softPwmWrite(PIN_SMD_GRN,0x00);
        softPwmWrite(PIN_SMD_BLU,0x00);

        softPwmWrite(PIN_DIP_RED,0x00);
        softPwmWrite(PIN_DIP_GRN,0x00);
        softPwmWrite(PIN_DIP_BLU,0x00);

        digitalWrite(PIN_ALED, LOW);
    }

    else
    {
        digitalWrite(PIN_ALED, HIGH);
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
    // delay(300);
    // Implement your sensor handling procedure.
    // Keep also this in mind:
    // - Make this fast and efficient.
    //   In Section 2, it's a part of scheduled tasks.
    //   For example, if it is slow, this will degradade energy efficiency.
    // - So, don't make any loop (e.g., don't use "for" & "while")
    //   This would hurt the performance of the task.
    // - Don't make any delay using delay(), sleep(), etc

}

void program_exit(SharedVariable* sv) {
    // Clean up everything if needed.
    // This is called when the program finishes.
    digitalWrite(PIN_ALED, LOW);
}

