#include <elapsedMillis.h>
#define SW_INPUT_PIN 2      // change this for your board configuration
#define LED_OUTPUT_PIN 6    // change this for your board configuration
#define CYCLE_TIME 500      // total duration of each cycle (on+off) is 500 ms
#define TIME_INCREMENT 100  // increase on time by 100 ms for each pushbutton press

  // Data structure (named "led") containing all variables related to the LED state
struct {
  elapsedMillis timer;    // single timer used for both on and off delays
  unsigned int onDelay;   // duration of "on" portion of cycle
  unsigned int offDelay;  // duration of "off" portion of cycle
  bool onState;           // true if LED is currently on
} led;


  // This function is called periodically to change the LED output when necessary
void UpdateLed() {
  if (led.onState) {                        // if LED is currently on
    if (led.offDelay > 0) {                 // and duty cycle is less than 100%
      if (led.timer > led.onDelay) {        // and if the on time has expired
        led.timer = 0;                      // then reset the timer for the next delay          
        digitalWrite(LED_OUTPUT_PIN, LOW);  // and turn the LED off
        led.onState = false;                // and remember the new state
      }
    }
  }
  else {                                    // LED is currently off
    if (led.onDelay > 0) {                  // if duy cycle is greater than 0%
      if (led.timer > led.offDelay) {       // and if the off time has expired
        led.timer = 0;                      // then reset the timer for the next delay
        digitalWrite(LED_OUTPUT_PIN, HIGH); // and turn the LED on
        led.onState = true;                 // and remember the new state
      }
    }
  }
}

  // This function detects pushbutton switch presses and implements debouncing
bool SwitchPressed() {
  static elapsedMillis debounceTimer;   // timer to implement debounce lockout period
  unsigned int debounceDelay = 100;     // duration of the debounce lockout period (ms)
  static bool debounceLockout = false;  // true during enforcement of the debounce lockout
  static int oldLevel;                  // last switch input reading
  int newLevel;                         // current switch input value
  bool edgeDetected;                    // set true if a falling or rising edge is detected
  bool newSwitchPress;                  // set true if a falling edge is detected

  newSwitchPress = false;               // initialize to false, may be set true later
  if (debounceLockout) {                // if currently in the debounce lockout period
    if (debounceTimer > debounceDelay)  // and if the lockout delay has expired
      debounceLockout = false;          // clear this flag to indicate end of lockout
  } 
  else {                                    // not in the lockout period
    newLevel = digitalRead(SW_INPUT_PIN);   // read the current switch level
    edgeDetected = (newLevel != oldLevel);  // edge = diff between current and previous values
    oldLevel = newLevel;          // current level becomes previous value for next time around
    if (edgeDetected) {           // if an edge has been detected (rising or falling)
      debounceTimer = 0;          // start the lockout period
      debounceLockout = true;     // update the switch state to indicate the lockout is in effect
      if (newLevel == LOW)        // falling edge was detected
        newSwitchPress = true;    // so switch was pressed
    }
  }
  return newSwitchPress;  // return value is true only if a new switch press is detected
}


  // This function is used to increase the duty cycle (by increasing the on duration)
void IncreaseDutyCycle() {
  led.onDelay += TIME_INCREMENT;     // increase the on time by a fixed amount
  if (led.onDelay > CYCLE_TIME)      // if the on time has reached or exceeded 100%
    led.onDelay = 0;                 // reset it back to 0 on time (0% duty cycle)
  led.offDelay = CYCLE_TIME - led.onDelay;   // compute and save the off time 
}


void setup() {
  pinMode(SW_INPUT_PIN, INPUT_PULLUP); 
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  digitalWrite(LED_OUTPUT_PIN, LOW);  // start with LED off (0% duty cycle)
  led.onState = false;                // initialize on/off state variable
  led.onDelay = 0;                    // on time is zero at 0% duty cycle
  led.offDelay = CYCLE_TIME;          // off time is equal to cycle time at 0% duty cycle
}


void loop() {
  if (SwitchPressed())    // if the pushbutton switch is pressed
    IncreaseDutyCycle();  // increase the LED duty cycle by a fixed amount
  UpdateLed();            // call this function every loop iteration to update the LED
  // Do other stuff
}  
