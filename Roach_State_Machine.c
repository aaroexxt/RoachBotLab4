#include <stdio.h>

#include "Roach_Events.h"
#include "roach.h"
#include "Roach_State_Machine.h"
#include "timers.h"

//a list of states that this SM uses:

enum {
    Moving_Forward,
    Reversing,
    Turning_Randomly,
    Hiding,
    WaitS,
    WaitAfterReversing
};
int turnCounter = 0;
int current_state;
int stallCount = 0;
//forward speed
const int RightMotorSpeed = 85;
const int LeftMotorSpeed = 85;



enum{
    Left,
    Right
};
int lastDirection = Left;
/* This function initializes the roach state machine.
 * At a minimum, this requires setting the first state.
 * Also, execute any actions associated with initializing the SM 
 * (that is, the actions on the arrow from the black dot in the SM diagram)*/
void Initialize_RoachStateMachine(void)
{
    TIMERS_Init();
    current_state = Moving_Forward;
    Roach_LeftMtrSpeed(100);
    Roach_RightMtrSpeed(100);

    //seed rand:
    srand(Roach_LightLevel());
    TIMERS_InitTimer(0, 2000);
};

/* 
 * @briefThis function feeds newly detected events to the roach state machine.
 * @param event:  The most recently detected event*/
void CheckStallCount() {
    if (!Roach_ReadFrontRightBumper() && !Roach_ReadFrontLeftBumper()) {
        printf("Stallcountreset");
        stallCount = 0;
    }
    if (Roach_ReadFrontRightBumper() && Roach_ReadFrontLeftBumper()) {
        stallCount++;
        if (stallCount > 2000) {
            current_state = Reversing;
            Roach_LeftMtrSpeed(-100);
            Roach_RightMtrSpeed(-100);
            
            TIMERS_InitTimer(0, 400);
        }
    }
    printf("Stallcount %d",stallCount);
}
void Run_RoachStateMachine(Event event)
{
    switch (current_state) {
        case Moving_Forward:
            printf("Current state:  Moving_Forward\r\n");
            if (event == ENTERED_DARK) {
                current_state = Hiding;
                //stop motors:
                Roach_LeftMtrSpeed(0);
                Roach_RightMtrSpeed(0);
                
            }
            if (event == FRONT_RIGHT_BUMP_PRESSED) {
                Roach_LeftMtrSpeed( -100);
                Roach_RightMtrSpeed(100);
                current_state = WaitS;
                if(turnCounter <= 20){
                    TIMERS_InitTimer(0, 150);
                }else{
                    TIMERS_InitTimer(0, 400);
                    turnCounter = 0;
                }
                lastDirection = Left;
                turnCounter++;

            }
            if (event == FRONT_LEFT_BUMP_PRESSED) {
                Roach_LeftMtrSpeed(100);
                Roach_RightMtrSpeed(-100);
                current_state = WaitS;
                if(turnCounter <= 20){
                    TIMERS_InitTimer(0, 150);
                }else{
                    TIMERS_InitTimer(0, 400);
                    turnCounter = 0;
                }
                lastDirection = Right;
                turnCounter++;

                
            }
            
            break;

        case Hiding:
            printf("Current state:  Hiding\r\n");
            //no transitions out of hiding (for now)
            break;
        
        case WaitS:
            if (event == NAV_TIMER_EXPIRED)
            {
                current_state = Moving_Forward;
                Roach_LeftMtrSpeed(LeftMotorSpeed);
                Roach_RightMtrSpeed(RightMotorSpeed);
                
            }
            if (event == REAR_RIGHT_BUMP_PRESSED) {
                Roach_LeftMtrSpeed( -LeftMotorSpeed);
                Roach_RightMtrSpeed(RightMotorSpeed);
                current_state = Reversing;
                TIMERS_InitTimer(0, 200);

            }
            if (event == REAR_LEFT_BUMP_PRESSED) {
                Roach_LeftMtrSpeed(LeftMotorSpeed);
                Roach_RightMtrSpeed(-RightMotorSpeed);
                current_state = Reversing;
                TIMERS_InitTimer(0, 200);

                
            }
            break;
        
        case Reversing:
            if (event == NAV_TIMER_EXPIRED) {
                if (lastDirection == Right)
                {
                    Roach_LeftMtrSpeed(-100);
                    Roach_RightMtrSpeed(100);
                    current_state = WaitAfterReversing;
                    TIMERS_InitTimer(0, 350);
                }
                else if (lastDirection == Left)
                {
                    Roach_LeftMtrSpeed(100);
                    Roach_RightMtrSpeed(-100);
                    current_state = WaitAfterReversing;
                    TIMERS_InitTimer(0, 350);

                }
                
            }
            if (event == REAR_RIGHT_BUMP_PRESSED || event == REAR_LEFT_BUMP_PRESSED) {
                current_state = Moving_Forward;
            }
            break;
        
        case WaitAfterReversing:
            if (event == NAV_TIMER_EXPIRED) {
                Roach_LeftMtrSpeed(100);
                Roach_RightMtrSpeed(100);
                current_state = Moving_Forward;
            }
    }
}
