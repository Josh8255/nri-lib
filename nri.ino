// Test NRI Model 832 Hardware 
#include <Bounce2.h>
#include <nri-lib.h>

// Instruction Register lamps.
#define IR7 21
#define IR6 20
#define IR5 19
#define IR4 18
#define IR3 17
#define IR2 16
#define IR1 15
#define IR0 14

// Accumulator lamps.
#define AC7 2
#define AC6 3
#define AC5 4
#define AC4 5
#define AC3 6
#define AC2 7
#define AC1 8
#define AC0 9

// ROM row switches.
#define RR0 23
#define RR1 27
#define RR2 31
#define RR3 35
#define RR4 39
#define RR5 43
#define RR6 47
#define RR7 51

#define RR8 25
#define RR9 29
#define RRA 33
#define RRB 37
#define RRC 41
#define RRD 45
#define RRE 49
#define RRF 53

// ROM column switches.
#define RC0 30
#define RC1 32
#define RC2 34
#define RC3 36
#define RC4 38
#define RC5 40
#define RC6 42
#define RC7 44

// Switch Register switches.
#define SR0 58
#define SR1 59
#define SR2 60
#define SR3 61
#define SR4 62
#define SR5 63
#define SR6 64
#define SR7 65

// Control switches.
#define POWER 54
#define CLOCK 55
#define SPEED 56
#define DISPLAY 57

// Control buttons.
#define STEP 66
#define START 67
#define RESET 68
#define LOAD 69

// Number of buttons to debounce.
#define NUM_BUTTONS 4

// Time in ms for clock
const unsigned long SLOW = 1000;
const unsigned long FAST = 16;

unsigned long clock_wait = 0;

// Array of pins connected to LEDs.
int ledPins[] = {IR7, IR6, IR5, IR4, IR3, IR2, IR1, IR0, AC7, AC6, AC5, AC4, AC3, AC2, AC1, AC0}; 
int leds = 16;

int accPins[] = {AC7, AC6, AC5, AC4, AC3, AC2, AC1, AC0}; 
int accs = 8;

int irpPins[] = {IR7, IR6, IR5, IR4, IR3, IR2, IR1, IR0}; 
int irps = 8;

// Array of pins connected to ROM rows.
int rowPins[] = {RR0, RR1, RR2, RR3, RR4, RR5, RR6, RR7, RR8, RR9, RRA, RRB, RRC, RRD, RRE, RRF}; 
int rows = 16;

// Array of pins connected to ROM columns.
int colPins[] = {RC7, RC6, RC5, RC4, RC3, RC2, RC1, RC0};
int cols = 8;

// Array of pins connected to the Switch Register switches.
int srsPins[] = {SR7, SR6, SR5, SR4, SR3, SR2, SR1, SR0};
int srss = 8;

// Array of pins connected to the control switches.
int cswPins[] = {DISPLAY, SPEED, CLOCK, POWER};
int csws = 4;

// Array of pins connected to the control buttons.
int cbtPins[] = {STEP, START, RESET, LOAD};
int cbts = 4;

// Create an array of Bounce objects
Bounce buttons[NUM_BUTTONS] = Bounce(); 

NRI machine = NRI();

bool powered = false;
bool running = true;

unsigned long last_time = 0.00;

void setup() {

  Serial.begin(115200);
    
  // Setup the display leds as outputs in the LOW state.
  for (int i = 0; i < leds; i++) {
   pinMode(ledPins[i], OUTPUT);
   digitalWrite(ledPins[i], LOW);
  }

  // Setup the ROM rows as outputs in the LOW state.
  for (int i = 0; i < rows; i++) {
   pinMode(rowPins[i], OUTPUT);
   digitalWrite(rowPins[i], LOW);
  }

  // Setup the ROM columns as inputs. NOTE: Pins are pulled down on the board.
  for (int i = 0; i < cols; i++) {
   pinMode(colPins[i], INPUT);
  }

  // Setup the Switch Register switches as inputs. NOTE: Pins enable internal pullups.
  for (int i = 0; i < srss; i++) {
   pinMode(srsPins[i], INPUT_PULLUP);
  }

  // Setup the control switches. NOTE: Pins enable internal pullups.
  for (int i = 0; i < csws; i++) {
   pinMode(cswPins[i], INPUT_PULLUP);
  }

  // Setup the buttons for debounce.
  for (int i = 0; i < cbts; i++) {
    buttons[i].attach(cbtPins[i], INPUT_PULLUP); // Setup pin and mode
    buttons[i].interval(5); // Set debounce interval in ms
  }
}

void loop() {

  // Turn off all lights and reset if no power
  if (digitalRead(POWER) == HIGH) {
    if (powered == true) {
      for (int i = 0; i < leds; i++) {
        digitalWrite(ledPins[i], LOW);
      }
      powered = false;
    }
    clock_wait = SLOW;
    running = true;
    machine.reset();
    return;
  }
  else {
    powered = true;
  }

  //Split into functions like these incase stuff needs to be moved
  handle_clock();
  handle_input();
  display_machine_info();
}

void handle_clock() {

  unsigned long time = millis();
  unsigned long dt = time - last_time;

  last_time = time;

  // If clock is on, minus the wait time with the DT
  if (digitalRead(CLOCK) == LOW) {
    clock_wait -= dt;
    if (clock_wait <= 0) {
      simulate_instruction();
      // If speed is HIGH set the new wait time to FAST, otherwise SLOW
      if (digitalRead(SPEED) == LOW) {
        clock_wait = FAST;
      }
      else {
        clock_wait = SLOW;
      }
    }
  }
}

void handle_input() {
  // Loop through buttons
  for (int i = 0; i < cbts; i++) {
    buttons[i].update(); // Update the debouncer state
    
    if (buttons[i].fell()) { // Check if button was pressed (transitioned to LOW).
      switch (i) {
        case 0:
          //STEP pressed
          if (digitalRead(CLOCK) == HIGH) {
            simulate_instruction();  
          }
          break;
        
        case 1:
          //START pressed

          running = !running;
          break;
        
        case 2:
          //RESET pressed

          machine.reset();
          clock_wait = SLOW;
          break;
        
        case 3:
          //LOAD pressed

          int data = 0;

          for (int j = 0; j < srss; j++) {
            if (digitalRead(srsPins[j])==LOW) {
              data = data | (0b10000000 >> j);
            }
          }

          machine.set_accumulator(data);
          
          break;
        
        default:
          break;     
      }
    }
  }
}

void display_machine_info() {
  // Write to accumulator LEDs
  int accumulator = machine.get_accumulator();
  for (int i = 0; i < accs; i++) {
    if (accumulator & (0b10000000 >> i)) {
      digitalWrite(accPins[i], HIGH); 
    } else {
      digitalWrite(accPins[i], LOW); 
    }
  }

  if (digitalRead(DISPLAY) == LOW) {
    //Display program counter and other info
    
    if (machine.is_overflow() ) {
      digitalWrite(irpPins[0], HIGH);
    } else {
      digitalWrite(irpPins[0], LOW);
    }

    if (machine.is_executing() ) {
      digitalWrite(irpPins[1], HIGH);
    } else {
      digitalWrite(irpPins[1], LOW);
    }

    if (running) {
      digitalWrite(irpPins[2], HIGH);
    } else {
      digitalWrite(irpPins[2], LOW);
    }

    // Start at 3 because bits 0 - 4 will be written
    int program_register = machine.get_program_register();
    for (int i = 3; i < irps; i++) {
      if (program_register & (0b10000000 >> i)) {
        digitalWrite(irpPins[i], HIGH);
      } else {
        digitalWrite(irpPins[i], LOW);
      }
    }
  }
  
  else {
    //Display instruction register
    int instruction_register = machine.get_instruction_register();
    for (int i = 0; i < irps; i++) {
      if (instruction_register & (0b10000000 >> i)) {
        digitalWrite(irpPins[i], HIGH);
      } else {
        digitalWrite(irpPins[i], LOW);
      }
    }
  }
}

void simulate_instruction() {
  if (!running) { 
    return;
  }

  loadROM();
  
  for(int i = 0; i < 16; i++) {
    machine.clock_cycle();
  }
}

// Loads all ROM switches into memory
void loadROM() {
  for(int i = 0; i < rows; i++) {
    loadRowIntoMemory(i);
  }
}

// Loads ROM at row i to memory address i
void loadRowIntoMemory(int i) {
  digitalWrite(rowPins[i], HIGH);

  int data = 0;

  for (int j = 0; j < cols; j++) {
    if (digitalRead(colPins[j])==HIGH) {
      data = data | (0b10000000 >> j);
    }
  }

  machine.set_memory(data, i);

  digitalWrite(rowPins[i], LOW);
}
