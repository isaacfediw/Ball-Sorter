#include "CControl.h"
#include "pigpio.h"
#include <sstream>
#include <iostream>
#include <math.h>
#include <unistd.h>

#define CE0 8

using namespace std;

CControl::CControl() {servo_pos = 0;}

CControl::~CControl() {gpioTerminate();}

float CControl::get_analog(int channel, int n, int& result) {
    get_data(ANALOG, channel, result);

    return (result / pow(2, n)) * 100;
}

bool CControl::get_data(int type, int channel, int& result) {
    if (gpioInitialise() < 0) {
        gpioTerminate();
        return false;
    }


    if (type == ANALOG) {
        unsigned char inBuf[3];
        char cmd[] = {1, channel == 0 ? 0b10000000 : 0b10010000, 0}; // 0b1XXX0000 where XXX is the channel

        int handle = spiOpen(0, 200000, 3); // Mode 0, 200kHz

        spiXfer(handle, cmd, (char*) inBuf, 3); // Transfer 3 bytes
        result = ((inBuf[1] & 3) << 8) | inBuf[2]; // Format 10 bits

        spiClose(handle);
    }

    if (type == DIGITAL) {
        gpioSetMode(channel, PI_INPUT);
        result = gpioRead(channel);
    }

    if (type == SERVO) {
        result = servo_pos;
    }

    return true;
}


bool CControl::set_data(int type, int channel, int val, int dir) {
    if (gpioInitialise() < 0) return false;
    if (channel != -1) gpioSetMode(channel, PI_OUTPUT);

    if (type == DIGITAL) {
        gpioWrite(channel, val);
    }

    if (type == SERVO) {
        gpioServo(channel, (2.0/180 * val + 0.5) * 1000);
        servo_pos = val;
    }

    if (type == STEPPER) {
         gpioSetMode(STEP, PI_OUTPUT);
         gpioSetMode(DIR, PI_OUTPUT);
         gpioWrite(DIR, dir);

         for (int i = 0; i < val; i++) { //val is # of steps
            gpioWrite(STEP, 1);
            usleep(500);
            gpioWrite(STEP, 0);
            usleep(500);
         }
    }

    return true;
}
