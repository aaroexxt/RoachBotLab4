/* 
 * File:   Roach_Events.h
 * Author: AutonomousSystemsLab
 *
 * Created on July 15, 2019, 10:49 AM
 */

#ifndef ROACH_EVENTS_H
#define	ROACH_EVENTS_H

//A list of events in this system:

typedef enum {
    NO_EVENT,
    FRONT_RIGHT_BUMP_PRESSED,
    FRONT_LEFT_BUMP_PRESSED,
    REAR_RIGHT_BUMP_PRESSED,
    REAR_LEFT_BUMP_PRESSED,
    FRONT_RIGHT_BUMP_RELEASED,
    FRONT_LEFT_BUMP_RELEASED,
    REAR_RIGHT_BUMP_RELEASED,
    REAR_LEFT_BUMP_RELEASED,
    ENTERED_DARK,
    ENTERED_LIGHT,
    NAV_TIMER_EXPIRED
} Event;

Event CheckForAllEvents(void);

Event CheckForBumperEvents(void);

Event CheckForLightEvents(void);

Event CheckForTimerEvents(void);

#endif	/* ROACH_EVENTS_H */

