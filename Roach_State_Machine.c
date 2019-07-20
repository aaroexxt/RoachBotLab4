#include <stdio.h>

#include "Roach_Events.h"
#include "roach.h"
#include "Roach_State_Machine.h"
#include "timers.h"

//Constants
/*MAZE 1 EASY WORKING CONSTANTS*/
//const int RightMotorSpeed = 85;
//const int LeftMotorSpeed = 85;
//const int stallThreshold = 10000;
//const int reverseTime = 400;
//const int reverseTurnTime = 325;
//
//const int turnTimeShort = 195;
//const int turnTimeLong = 410;

const int RightMotorSpeed = 85;
const int LeftMotorSpeed = 85;
const int stallThreshold = 10000;
const int reverseTime = 400;
const int reverseTurnTime = 400;

const int turnTimeShort = 300;
const int turnTimeLong = 450;

//a list of states that this SM uses:
enum {
    Moving_Forward,
    Reversing,
    Turning_Randomly,
    Hiding,
    WaitTurn,
    WaitAfterReversing
};

int turnCounter = 0;
int current_state;
int stallCount = 0; //tracks how long the roach has been stalled

enum {
    Left,
    Right
};
int lastBumper = Left;
/* This function initializes the roach state machine.
 * At a minimum, this requires setting the first state.
 * Also, execute any actions associated with initializing the SM 
 * (that is, the actions on the arrow from the black dot in the SM diagram)*/
void Initialize_RoachStateMachine(void)
{
    TIMERS_Init(); //init timers
    current_state = Moving_Forward; //set initial state
    Roach_LeftMtrSpeed(LeftMotorSpeed); //set motor speeds
    Roach_RightMtrSpeed(RightMotorSpeed);

    //seed rand:
    srand(Roach_LightLevel());
    //Init timer0
    TIMERS_InitTimer(0, 1000);
};


void Run_CheckStallCount() {
    if (!Roach_ReadFrontRightBumper() && !Roach_ReadFrontLeftBumper()) { //reset stall count because neither bumper is pressed
        stallCount = 0;
    }
    if (Roach_ReadFrontRightBumper() && Roach_ReadFrontLeftBumper()) { //if bumpers are stuck
        stallCount++;
        if (stallCount > stallThreshold) { //if stallCount is greater than threshold
            current_state = Reversing;
            Roach_LeftMtrSpeed(-LeftMotorSpeed);
            Roach_RightMtrSpeed(-RightMotorSpeed);
            
            TIMERS_InitTimer(0, reverseTime);
        }
    }
//    printf("Stallcount %d",stallCount);
}

void Run_BumperPressed() { //code that runs when bumper is pressed
    current_state = WaitTurn;
    
    if (turnCounter <= 17) { //makes a longer turn every 30 just in case bot is stuck
        TIMERS_InitTimer(0, turnTimeShort);
    } else {
        TIMERS_InitTimer(0, turnTimeLong);
        turnCounter = 0;
    }
    turnCounter++;
}

void Run_TurnOffset(){
    if (lastBumper == Right) {
        Roach_LeftMtrSpeed(LeftMotorSpeed+10);
        Roach_RightMtrSpeed(RightMotorSpeed);
    } else {
        Roach_LeftMtrSpeed(LeftMotorSpeed);
        Roach_RightMtrSpeed(RightMotorSpeed+7);
    }
}
/* 
 * @briefThis function feeds newly detected events to the roach state machine.
 * @param event:  The most recently detected event
 */
void Run_RoachStateMachine(Event event)
{
    switch (current_state) {
        case Moving_Forward:
            printf("Current state:  Moving_Forward\r\n");
            switch (event) {
                case ENTERED_DARK:
                    current_state = Hiding;
                    //stop motors:
                    Roach_LeftMtrSpeed(0);
                    Roach_RightMtrSpeed(0);
                    break;
                case FRONT_RIGHT_BUMP_PRESSED:
                    Roach_LeftMtrSpeed( -LeftMotorSpeed);
                    Roach_RightMtrSpeed(RightMotorSpeed);
                    lastBumper = Left;
                    
                    Run_BumperPressed();
                    break;
                case FRONT_LEFT_BUMP_PRESSED:
                    Roach_LeftMtrSpeed(LeftMotorSpeed);
                    Roach_RightMtrSpeed(-RightMotorSpeed);
                    lastBumper = Right;

                    Run_BumperPressed();
                    break;
            }
            break;

        case Hiding:
            printf("Current state:  Hiding\r\n");
            //no transitions out of hiding (for now)
            break;
        
        case WaitTurn:
            switch (event) {
                case NAV_TIMER_EXPIRED:
                case REAR_RIGHT_BUMP_PRESSED:
                case REAR_LEFT_BUMP_PRESSED:
                    Run_TurnOffset();
                    current_state = Moving_Forward;
                    break;
            }
            break;
        
        case Reversing:
            switch (event) {
                case NAV_TIMER_EXPIRED:
                    if (lastBumper == Left) {
                        Roach_LeftMtrSpeed(-LeftMotorSpeed);
                        Roach_RightMtrSpeed(RightMotorSpeed);
                        current_state = WaitAfterReversing;
                        TIMERS_InitTimer(0, reverseTurnTime);
                    } else if (lastBumper == Right) {
                        Roach_LeftMtrSpeed(LeftMotorSpeed);
                        Roach_RightMtrSpeed(-RightMotorSpeed);
                        current_state = WaitAfterReversing;
                        TIMERS_InitTimer(0, reverseTurnTime);

                    }
                    break;
                case REAR_RIGHT_BUMP_PRESSED:
                case REAR_LEFT_BUMP_PRESSED:
                    Run_TurnOffset();
                    
                    current_state = Moving_Forward;
                    break;
            }
            break;
        
        case WaitAfterReversing:
            if (event == NAV_TIMER_EXPIRED) {
                Run_TurnOffset();
                current_state = Moving_Forward;
            }
    }
}
