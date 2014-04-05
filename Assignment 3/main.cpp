#include "mbed.h"

Serial pc(USBTX, USBRX);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

InterruptIn p1Button(p18);
InterruptIn p2Button(p17);

Timer t;

// C-style strings for output messages.
const char* EARLY_MSG = "Player %d was %d ms too early and lost.\r\n";
const char* WIN_MSG = "Player %d won with a reaction time of only %d ms.\r\n";
const char* POINTS_MSG = "Player 1: %d points, Player 2: %d points\r\n\r\n";

// Minimum time for a winning button press, in ms.
// LED4 should turn on at this time.
int COUNTDOWN_TIME_ms = 3000;

volatile bool buttonPressed;
volatile int firstResponder;
volatile int secondResponder;
// This variable will hold the time of the first button press each round, in ms.
volatile int buttonPressTime_ms;
// An array is used to hold the players' points for ease of use and readability.
// points[1] is player 1's score; points[2] is player 2's.
// points[0] will remain unused, but that's ok---it's just one word.
volatile int points[] = {0, 0, 0};


void button1Response() {
    if(!buttonPressed) {
        buttonPressTime_ms = t.read_ms();
        buttonPressed = true;
        firstResponder = 1;
        secondResponder = 2;
    }
}

void button2Response() {
    if(!buttonPressed) {
        buttonPressTime_ms = t.read_ms();
        buttonPressed = true;
        firstResponder = 2;
        secondResponder = 1;
    }
}

void setup() {
    // Reset the timer and buttonPressed at the start of each round.
    t.stop();
    t.reset();
    buttonPressed = false;
}

void countdown() {
    // Turn on each LED in sequence for 1 second each.
    t.start();
    led1 = 1;
    wait(1);
    led1 = 0;
    led2 = 1;
    wait(1);
    led2 = 0;
    led3 = 1;
    wait(1);
    led3 = 0;
    // Note that the interrupt service routines could have been called by this
    // point, delaying the actual time LED4 turns on, but they take only
    // microseconds to execute, so you would have to press the buttons hundreds
    // of times in order to delay the LED by even one millisecond---and at that
    // point, you've already forfeited the game anyway.
    led4 = 1;
    wait(1);
    led4 = 0;
}

void findWinner() {
    // Wait for button to be pressed, if it hasn't been already.
    while(!buttonPressed) { /* empty loop */ }

    if(buttonPressTime_ms < COUNTDOWN_TIME_ms) {
        // The first button pressed was too early.
        // Display how early the player was, and give the other player a point.
        pc.printf(EARLY_MSG, firstResponder, COUNTDOWN_TIME_ms - buttonPressTime_ms);
        points[secondResponder] += 1;
    } else {
        // The first button pressed was not too early.
        // Display the player's reaction time, and give the player a point.
        pc.printf(WIN_MSG, firstResponder, buttonPressTime_ms - COUNTDOWN_TIME_ms);
        points[firstResponder] += 1;
    }
    // Display each player's total score.
    pc.printf(POINTS_MSG, points[1], points[2]);
}


int main() {

    // Set buttons to read logic 1 by default.
    p1Button.mode(PullUp);
    p2Button.mode(PullUp);

    // Buttons are connected to GND, so a falling edge indicates a press.
    p1Button.fall(button1Response);
    p2Button.fall(button2Response);

    while(1) {
        // The functions called here could just as easily be written inline,
        // but this way you can see how simple the overall program really is.
        setup();
        countdown();
        // Note that the LED countdown continues even if a player presses their
        // button too early, and scores aren't updated until after it completes.
        findWinner();
        wait(3);
    }
}
