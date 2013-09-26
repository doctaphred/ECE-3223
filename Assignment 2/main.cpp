#include "mbed.h"

AnalogIn LM35(p20);
InterruptIn button_C(p27);
InterruptIn button_F(p28);
BusOut display_bus(p5,p6,p7,p8,p9,p10,p11,p12);

int NUM_READINGS = 1000; // Number of temperature readings to take.
float READING_DELAY = 0.2; // Delay between temperature readings, in ms.

float DISP_TIME = 0.5; // Time to display each character, in s.
float BLANK_TIME = 0.1; // Time to blank display between characters, in s.

bool celsiusMode = true; // Temperature display mode. 'false' means Fahrenheit mode.


float readSensor() {
    // Return the average of many readings from the temperature sensor.

    float sensorAvg = 0;

    for (int i = 0; i < NUM_READINGS; i++) {
        sensorAvg += LM35.read();
        wait_ms(READING_DELAY);
    }

    sensorAvg = sensorAvg / NUM_READINGS;

    return sensorAvg;
}

float temp_C(float sensorReading) {
    // Convert sensorReading to temperature in degrees Celsius.

    // sensorReading is a floating-point value representing the output 
    // voltage of the sensor, which is a number in the range [0.0, 1.0].

    // Multiply the sensorReading by 3.3 V to get the voltage seen at the
    // AnalogIn pin, which is the output of the LM35.

    // The LM35 outputs approximately 10 mV per degree Celsius, so multiply 
    // that value by 100 to get the approximate temperature.

    // x * (3.3 V) * (1000 mV)/(1 V) * (1 degC)/(10 mV) == x * 330

    return sensorReading * 330;
}

float temp_F(float sensorReading) {
    // Convert sensorReading to temperature in degrees Fahrenheit.

    // 9.0 instead of 9 to ensure floating point division.
    return temp_C(sensorReading) * (9.0/5) + 32;
}

int onesDigit(int number) {
    // Return an integer representing the ones digit of the given number.
    return number % 10;
}

int tensDigit(int number) {
    // Return an integer representing the tens digit of the given number.
    return number / 10;
}

void display(int digit) {
    // Activate the seven-segment display to show the given digit.
    // Turn on the decimal point if an invalid (multi-digit) number is given.
    switch(digit) {
        case 0 : display_bus.write(0x3F); break;
        case 1 : display_bus.write(0x06); break;
        case 2 : display_bus.write(0x5B); break;
        case 3 : display_bus.write(0x4F); break;
        case 4 : display_bus.write(0x66); break;
        case 5 : display_bus.write(0x6D); break;
        case 6 : display_bus.write(0x7D); break;
        case 7 : display_bus.write(0x07); break;
        case 8 : display_bus.write(0x7F); break;
        case 9 : display_bus.write(0x6F); break;
        default: display_bus.write(0x80); // Turn on decimal point for errors.
    }
}

void displayTemp_C() {
    // Display the temperature in degrees Celsius, one digit at a time, 
    // followed by the letter 'C', then blank the display.

    // Add 0.5 and cast as an int to round.
    int temp = temp_C(readSensor()) + 0.5;

    display(tensDigit(temp));
    wait(DISP_TIME);
    display_bus.write(0);
    wait(BLANK_TIME);

    display(onesDigit(temp));
    wait(DISP_TIME);
    display_bus.write(0);
    wait(BLANK_TIME);

    display_bus.write(0x39); // 'C'
    wait(DISP_TIME);
    display_bus.write(0);
}

void displayTemp_F() {
    // Display the temperature in degrees Fahrenheit, one digit at a time, 
    // followed by the letter 'C', then blank the display.

    // Add 0.5 and cast as an int to round.
    int temp = temp_F(readSensor()) + 0.5;

    display(tensDigit(temp));
    wait(DISP_TIME);
    display_bus.write(0);
    wait(BLANK_TIME);

    display(onesDigit(temp));
    wait(DISP_TIME);
    display_bus.write(0);
    wait(BLANK_TIME);

    display_bus.write(0x71); // 'F'
    wait(DISP_TIME);
    display_bus.write(0);
}

void setMode_C() {
    // Set mode to display temperature in degrees Celsius.
    celsiusMode = true;
}

void setMode_F() {
    // Set mode to display temperature in degrees Fahrenheit.
    celsiusMode = false;
}

int main() {
    // Register these functions as interrupt service routines,
    // so they are called each time the buttons are pressed.
    button_C.rise(&setMode_C);
    button_F.rise(&setMode_F);
    while(1) {
        if (celsiusMode) {
            displayTemp_C();
        }
        else {
            displayTemp_F();
        }
    }
}
