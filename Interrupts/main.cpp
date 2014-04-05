#include "mbed.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
InterruptIn button1(p18);
InterruptIn button2(p17);


void led1On() {
    led1 = 1;
}

void led2On() {
    led2 = 1;
}

void led1Off() {
    led1 = 0;
}

void led2Off() {
    led2 = 0;
}

int main() {
    // Set up for buttons tied to ground.
    button1.mode(PullUp);
    button2.mode(PullUp);

    // Turn on LEDs when buttons are pressed.
    button1.fall(&led1On);
    button2.fall(&led2On);

    // Turn off LEDs when buttons are released.
    button1.rise(&led1Off);
    button2.rise(&led2Off);

    // All done. Interrupts will continue to function.
    return 0;
}
