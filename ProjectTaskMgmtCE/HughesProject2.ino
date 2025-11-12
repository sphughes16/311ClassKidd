//Global Variables
unsigned long delay1; //Long to store time LED1 starts blinking
unsigned long delay2; //Long to store time LED2 starts blinking
bool LED1On = false; //Bool indicating LED1 is on
bool LED2On = false; //Bool indicating LED2 is on
unsigned long intervalArr[2] = {0}; //Array to store the user-specified intervals. Index 0 holds LED1 interval, Index 1 holds LED2 interval
bool waitForLED = false; //Bool indicating user is choosing an LED
bool waitForInterval = false; //Bool indicating user is choosing an interval
int chosenLED; //Holds the chosen pin's corresponding index

//Helper to clear escape sequences or whitelines after reading input
void flushLineEndings() {
  while (Serial.available()) {
    //Checks if the incoming byte is an escape sequence, if so, removes it from the buffer
    int myChar = Serial.peek();
    if (myChar == '\r' || myChar == '\n' || myChar == ' ' || myChar == '\t') {
      Serial.read();
    } 
    else {
      break;
    }
  }
}

//Handles user input and prompts
void task_serial_io() {
  if (Serial.available() > 0) {
    //If the user is choosing an LED, awaits and stores input then outputs interval prompt
    if (waitForLED == true) {
      int choice = Serial.parseInt();
      flushLineEndings();
      if (choice == 1) {
        chosenLED = 0;
        Serial.println("What interval (ms)?");
        waitForLED = false;
        waitForInterval = true;
      } 
      else if (choice == 2) {
        chosenLED = 1;
        Serial.println("What interval (ms)?");
        waitForLED = false;
        waitForInterval = true;
      } 
      else {
        Serial.println("Please enter 1 or 2 for the LED.");
        Serial.println("What LED? (1 or 2)");
      }
    } 
    //If the user is choosing an interval, awaits and stores input then outputs LED prompt
    else if (waitForInterval == true) {
      int userInterval = Serial.parseInt();
      flushLineEndings();
      if (userInterval > 0) {
        intervalArr[chosenLED] = (unsigned long)userInterval;
        Serial.print("Set LED on pin ");
        Serial.print(chosenLED + 1);
        Serial.print(" to ");
        Serial.print(userInterval);
        Serial.println(" ms.");
      } 
      else {
        Serial.println("Please enter a positive integer for the interval (ms).");
      }
      Serial.println("What LED? (1 or 2)");
      waitForInterval = false;
      waitForLED = true;
    }
  }
}

//Blinks LED 1 on the specified interval
void task_blink_led1() {
  if ((millis() - delay1) > intervalArr[0]) {
    //Turns the LED off if it is currently on, else turns it on and updates the delay start
    if (LED1On == true) {
      LED1On = false;
      digitalWrite(2, LOW);
    } 
    else {
      LED1On = true;
      digitalWrite(2, HIGH);
    }
    delay1 = millis();
  }
}

//Blinks LED 2 on the specified interval
void task_blink_led2() {
  if ((millis() - delay2) > intervalArr[1]) {
    //Turns the LED off if it is currently on, else turns it on and updates the delay start
    if (LED2On == true) {
      LED2On = false;
      digitalWrite(3, LOW);
    } 
    else {
      LED2On = true;
      digitalWrite(3, HIGH);
    }
    delay2 = millis();
  }
}

//Creates and populates an array of function pointers with void return type
void (*taskTable[])() = {
  task_serial_io,
  task_blink_led1,
  task_blink_led2
};
int NTASKS = sizeof(taskTable) / sizeof(taskTable[0]);

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50); //Keeps parseInt from hanging too long

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  LED1On = false;
  LED2On = false;
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);

  //Sets initial values for delay start, interval, and initial prompt
  delay1 = millis();
  delay2 = millis();
  intervalArr[0] = 500;
  intervalArr[1] = 500;
  Serial.println("What LED? (1 or 2)");
  waitForLED = true;
  waitForInterval = false;
}

void loop() {
  //Performs the cyclic executive dispatching
  for (int i = 0; i < NTASKS; i++) {
    taskTable[i]();
  }
}
