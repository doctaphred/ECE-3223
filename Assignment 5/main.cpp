#include "mbed.h"

PwmOut led1(LED1);
PwmOut led2(LED2);
PwmOut led3(LED3);
PwmOut led4(LED4);
PwmOut buzzer(p22);

// Using global constants allows you to change your program's behavior without
// hunting down each value within the body of your code and changing each
// occurrence. These are a little excessive, but you get the idea.
const int BRIGHTNESS_LEVELS = 10000; // Number of levels to fade LEDs.
const float FADE_TIME = 1.0; // Duration of LED fades.
const float ON_TIME = 1.0; // Duration to leave each LED on at full brightness.
const float OFF_TIME = 1.0; // Duration to leave each LED off.


void fade(PwmOut pin, float target, int numSteps, float time_s) {
    /** Gradually change the specified pin's duty cycle to the target value.

    This function sets the PWM period, which sets the period for all PWM
    sources on the LPC1768. See page 67 of the book, and
    http://mbed.org/questions/1868/PWM-outputs-interfere-with-each-other/

    Args:
        pin: the pin whose duty cycle to adjust.
        target (>= 0.0, <= 1.0): the final value of the pin's duty cycle.
        numSteps (>= 0): the number of steps to take in fading.
        time_s (>= 0): the duration, in seconds, of the fade.
    Side effects:
        Sets the global PWM period to 1.0 / 10000.
    */
    float dutyCycle = pin; // Shorthand for `float dutyCycle = pin.read();`
    float step = (target - dutyCycle) / numSteps;
    // We'll only delay in between PWM adjustments, so we'll call wait one time
    // fewer than we adjust the pin's duty cycle; hence, numSteps - 1.
    float stepDelay = time_s / (numSteps - 1);
    // Set the PWM period to make it cycle faster than we can see.
    pin.period(1.0 / 10000);
    // The statements `dutyCycle += step; pin = dutyCycle;` could also be
    // written as simply `pin = pin + step;`, but that's just shorthand for
    // `pin = pin.read() + step;`, which involves unnecessary reads and might
    // lose precision, depending on the granularity of the PwmOut duty cycle.
    dutyCycle += step;
    pin = dutyCycle;
    // Repeat the above instructions numSteps - 1 times, with waits in between.
    // Looping over floats is gross, so let's just use a throwaway counter `i`
    // and add up the duty cycle separately.
    for (int i = 0; i < numSteps - 1; ++i) {
        wait(stepDelay);
        dutyCycle += step;
        pin = dutyCycle;
    }
    // Note that there is only a delay in between levels; the pin is first
    // adjusted immediately, and the function returns immediately after its
    // final adjustment.
}


void fadeOn(PwmOut pin) {
    /** Fade the given PwmOut to 100% duty cycle, and make sure it stays ON.

    Args:
        pin: the pin to fade on.
    Side effects:
        Sets the global PWM period to 1.0 / 10000. See function `fade`.
        Sets the pin's pulse width to 999999 ms.
    */
    fade(pin, 1.0, BRIGHTNESS_LEVELS, FADE_TIME);
    // We need to make sure the LED stays ON, even if the PWM period is altered
    // afterward. Even if the period is changed, the pulse width remains unique
    // to the individual PwmOut, so set its pulse width to be larger than any
    // reasonable period.
    pin.pulsewidth_ms(999999);
}


void fadeOff(PwmOut pin) {
    /** Fade the given PwmOut to 0% duty cycle.

    Args:
        pin: the pin to fade off.
    Side effects:
        Sets the global PWM period to 1.0 / 10000. See function `fade`.
    */
    fade(pin, 0.0, BRIGHTNESS_LEVELS, FADE_TIME);
}


void buzz(float frequency_Hz) {
    /** Set the piezo buzzer to buzz at the given frequency at 0.5 duty cycle.

    Args:
        frequency_Hz (> 0): the frequency to play on the buzzer.
    Side effects:
        Sets the global PWM period to 1.0 / frequency_Hz.
    */
    buzzer.period(1.0 / frequency_Hz);
    buzzer = 0.5;
}


int main() {

    PwmOut leds[] = {led1, led2, led3, led4, led3, led2};

    while (1) {
        // Here is a demo of looping over an array of unknown length, using
        // sizeof. The length of the array can be found by dividing its total
        // size (`sizeof(leds)`) by the size of one of its elements
        // (`sizeof(*leds)`). Since we already know the size of this array,
        // this code could simply be written as `for (int i = 0; i < 6; ++i)`,
        // but this approach lets you change the array without also changing
        // the code that accesses it.
        for (int i = 0; i < sizeof(leds) / sizeof(*leds); ++i) {
            PwmOut led = leds[i];
            fadeOn(led);

            if (led == led1)
                buzz(659);
            else if (led == led4)
                buzz(784);

            wait(ON_TIME);
            // Silence the buzzer, even if it's already silent.
            buzzer.pulsewidth_us(0);
            fadeOff(led);
            wait(OFF_TIME);
        }
    }
}
