/*
 Servo.cpp - interrupt driven servo library for tivac
 copyright (c) 2013 Kevin Balke.  all right reserved.

 derived from:
 servo.cpp - interrupt driven servo library for arduino using 16 bit timers- version 2
 copyright (c) 2009 michael margolis.  all right reserved.

 this library is free software; you can redistribute it and/or
 modify it under the terms of the gnu lesser general public
 license as published by the free software foundation; either
 version 2.1 of the license, or (at your option) any later version.

 this library is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  see the gnu
 lesser general public license for more details.

 you should have received a copy of the gnu lesser general public
 license along with this library; if not, write to the free software
 foundation, inc., 51 franklin st, fifth floor, boston, ma  02110-1301  usa
 */

#ifndef SERVO_H
#define SERVO_H

#include "Energia.h"
#include <inttypes.h>

// Hardware limitations information
#define MIN_SERVO_PULSE_WIDTH 		544
#define MAX_SERVO_PULSE_WIDTH 		2400
#define DEFAULT_SERVO_PULSE_WIDTH   1500
#define REFRESH_INTERVAL 		    20000

// Aliases for timer config and loading
#define SERVO_TIMER				TIMER2_BASE
#define SERVO_TIME_CFG			TIMER_CFG_PERIODIC
#define SERVO_TIMER_TRIGGER		TIMER_TIMA_TIMEOUT
#define SERVO_TIMER_INTERRUPT	INT_TIMER2A
#define SERVO_TIMER_A			TIMER_A
#define SERVO_TIMER_PERIPH		SYSCTL_PERIPH_TIMER2

// Other defines
#define SERVOS_PER_TIMER 	8
#define INVALID_SERVO 		255


typedef struct
{
    unsigned int pin_number;
    unsigned int pulse_width;
    bool enabled;
} servo_t;

class Servo
{
private:
    unsigned int index;
    int min;
    int max;
public:
    Servo();
    unsigned int attach(unsigned int pin, int min = MIN_SERVO_PULSE_WIDTH, int max = MAX_SERVO_PULSE_WIDTH);
    void detach();
    void writeMicroseconds(int value);
    int readMicroseconds();
    void write(int value);
    int read();
    bool attached();

};

extern "C" void ServoIntHandler(void);

#endif // SERVO_H
