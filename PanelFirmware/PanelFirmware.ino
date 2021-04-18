//--------------------------------------------
// Firmware for my Elite Dangerous controller 
// Benjamin VanderSluis (ben@bjv.us)
//--------------------------------------------

#include <Bounce.h>
#include <Keyboard.h>

///////////////////////////////
//       KEYBINDS             /
///////////////////////////////
#include "ELITE_BINDS.h"
//#include "DEBUG_BINDS.h"


///////////////////////////////
//       PINS                 /
///////////////////////////////
// PANEL 1 DEFINITIONS //////////////////////
#define p1_nrow 5
#define p1_ncol 2
const int p1_rowpins[p1_nrow] = {27, 1, 2, 3, 4};
const int p1_colpins[p1_ncol] = {5,0};
Bounce *p1_state[p1_nrow][p1_ncol];

// PANEL 2 DEFINITIONS //////////////////////
#define p2_nrow 5
#define p2_ncol 2
const int p2_rowpins[p2_nrow] = {11, 9, 8, 7, 36};
const int p2_colpins[p2_ncol] = {10, 12};
Bounce *p2_state[p2_nrow][p2_ncol];

// PANEL 3 DEFINITIONS //////////////////////
#define p3_nrow 4
const int p3_rowpins[p3_nrow] = {38, 18, 19, 20};
Bounce *p3_state[p3_nrow];

// PANEL 4 DEFINITIONS //////////////////////
#define p4_nrow 3
#define p4_ncol 3
const int p4_rowpins[p4_nrow] = {25, 22, 21};
const int p4_colpins[p4_ncol] = {26, 24, 23};
Bounce *p4_state[p4_nrow][p4_ncol];

// PANEL 5 DEFINITIONS //////////////////////
#define p5_nrow 5
#define p5_ncol 3
const int p5_rowpins[p5_nrow] = {28, 29, 30, 35, 32};
const int p5_colpins[p5_ncol] = {33, 34, 31};
Bounce *p5_state[p5_nrow][p5_ncol];

// PANEL 6 DEFINITIONS //////////////////////
#define p6_nxyzt 4
#define p6_nrow 3
#define p6_ncol 2
const int p6_xyztpins[4] = {42, 43, 44, 45};
const int p6_rowpins[p6_nrow] = {13, 16, 15};
const int p6_colpins[p6_ncol] = {14, 17};
Bounce *p6_state[p6_nrow][p6_ncol];
int axisVals[3] = {0, 0, 0}; // XYZ

// MOTCON DEFINITIONS
int throttle_val;
bool down_throttle;
const int throttle_tol = 18;   // analog thresh for zero acheived
const int throttle_near = 50; // thresh for blocking seek
const int motcon_pins[2] = {40, 41};

// DUMP DEFINITIONS
const int dump_pin = 39;
Bounce *dump_state;
bool release_dump;


/////////////////////////////////////////////////
// MATRIX CODE ASSIGNMENTS            ///////////
/////////////////////////////////////////////////
const unsigned int p1_codes[p1_nrow][p1_ncol] = { 
   {C_FSS,        C_FREQ_UP},
   {C_DISCOVERY,  C_FREQ_DN},
   {C_EXIT,       C_ANALYSIS},
   {C_CODEX,      C_SYSTEM_MAP},
   {C_ORBITS,     C_GALAXY}};

const unsigned int p2_codes[p2_nrow][p2_ncol] = { 
   {C_SRV_BRAKE,  C_FTR_DEFEND},
   {C_SRV_ASSIST, C_FTR_HPOS}, 
   {C_FTR_ATTACK, C_FTR_DOCK},
   {C_SRV_SHIP,   C_FTR_HFIRE},
   {C_SRV_TURRET, C_FTR_FOLLOW}};

const unsigned int p3_codes[p3_nrow] = { 
   C_ECM, C_HEATSINK, C_CHAFF, C_SHIELD_CELL};

const unsigned int p4_codes[p4_nrow][p4_ncol] = { 
   {C_INTERN_PANEL, C_SILENT_RUN,     C_HARD_POINTS},
   {C_ROLE_PANEL,   C_NIGHT_VISION,   C_CARGO_SCOOP},
   {C_COMMS_PANEL,  C_RUNNING_LIGHTS, C_LANDING_GEAR}};

const unsigned int p5_codes[p5_nrow][p5_ncol] = { 
   {C_WING_3,      C_WING_2,       C_WING_1},
   {C_WING_TARGET, C_NEXT_HOSTILE, C_HIGH_THREAT},
   {C_NEXT_SUBSYS, C_PREV_SUBSYS,  C_NAV_LOCK},
   {C_JUMP,        C_CRUISE,       C_NEXT_SYSTEM},
   {C_FRAME_SHIFT, C_NULL,         C_NULL}};

const unsigned int p6_codes[p6_nrow][p6_ncol] = { 
   {C_ROTATION_CORRECT, C_NULL},
   {C_FULL_REVERSE,     C_FLIGHT_ASSIST}, 
   {C_ALL_STOP,         C_NAV_PANEL}};


/////////////////////////////////////////////////
// SETUP                                       //
/////////////////////////////////////////////////
void setup() {
   // P1 setup /////////////////////////////////////////////
   // col pins, set high to ignore
   for (int c=0; c<p1_ncol; c++){
      pinMode(p1_colpins[c], OUTPUT);
      digitalWrite(p1_colpins[c], HIGH);
   }

   // row pins, set as inputs with pull ups
   for (int r=0; r<p1_nrow; r++){
      pinMode(p1_rowpins[r], INPUT_PULLUP);
   }

   // initialize the State Matrix
   for (int r=0; r<p1_nrow; r++) {
      for (int c=0; c<p1_ncol; c++) {
         p1_state[r][c] = new Bounce(p1_rowpins[r], 10);
      }
   }

   // p2 setup /////////////////////////////////////////////
   // col pins, set high to ignore
   for (int c=0; c<p2_ncol; c++){
      pinMode(p2_colpins[c], OUTPUT);
      digitalWrite(p2_colpins[c], HIGH);
   }

   // row pins, set as inputs with pull ups
   for (int r=0; r<p2_nrow; r++){
      pinMode(p2_rowpins[r], INPUT_PULLUP);
   }

   // initialize the State Matrix
   for (int r=0; r<p2_nrow; r++) {
      for (int c=0; c<p2_ncol; c++) {
         p2_state[r][c] = new Bounce(p2_rowpins[r], 10);
      }
   }

   // p3 setup /////////////////////////////////////////////
   // row pins, set as inputs with pull ups
   // init the state vector
   for (int r=0; r<p3_nrow; r++){
      pinMode(p3_rowpins[r], INPUT_PULLUP);
      p3_state[r] = new Bounce(p3_rowpins[r], 10);
   }

   // P4 setup /////////////////////////////////////////////
   // col pins, set high to ignore
   for (int c=0; c<p4_ncol; c++){
      pinMode(p4_colpins[c], OUTPUT);
      digitalWrite(p4_colpins[c], HIGH);
   }

   // row pins, set as inputs with pull ups
   for (int r=0; r<p4_nrow; r++){
      pinMode(p4_rowpins[r], INPUT_PULLUP);
   }

   // initialize the State Matrix
   for (int r=0; r<p4_nrow; r++) {
      for (int c=0; c<p4_ncol; c++) {
         p4_state[r][c] = new Bounce(p4_rowpins[r], 10);
      }
   }

   // P5 setup /////////////////////////////////////////////
   // col pins, set high to ignore
   for (int c=0; c<p5_ncol; c++){
      pinMode(p5_colpins[c], OUTPUT);
      digitalWrite(p5_colpins[c], HIGH);
   }

   // row pins, set as inputs with pull ups
   for (int r=0; r<p5_nrow; r++){
      pinMode(p5_rowpins[r], INPUT_PULLUP);
   }

   // initialize the State Matrix
   for (int r=0; r<p5_nrow; r++) {
      for (int c=0; c<p5_ncol; c++) {
         p5_state[r][c] = new Bounce(p5_rowpins[r], 10);
      }
   }

   // P6 setup /////////////////////////////////////////////
   for (int c=0; c<p6_ncol; c++){
      pinMode(p6_colpins[c], OUTPUT);
      digitalWrite(p6_colpins[c], HIGH);
   }

   // row pins, set as inputs with pull ups
   for (int r=0; r<p6_nrow; r++){
      pinMode(p6_rowpins[r], INPUT_PULLUP);
   }

   // initialize the State Matrix
   for (int r=0; r<p6_nrow; r++) {
      for (int c=0; c<p6_ncol; c++) {
         p6_state[r][c] = new Bounce(p6_rowpins[r], 10);
      }
   }


   // DUMP  setup ///////////////////////////////////////////
   pinMode(dump_pin, INPUT_PULLUP);
   dump_state = new Bounce(dump_pin, 10);
   release_dump = false;

   // MOTCON setup ////////////////////////////////////////
   pinMode(motcon_pins[0], OUTPUT);
   pinMode(motcon_pins[1], OUTPUT);
   digitalWrite(motcon_pins[0], LOW);
   digitalWrite(motcon_pins[1], LOW);
   
   down_throttle = false;

}


/////////////////////////////////////////////////
// LOOP                                        //
/////////////////////////////////////////////////
void loop() {
   // p1 SCAN //////////////////////////////////////////////
   for (int c=0; c<p1_ncol; c++) {
      // ground the col pin
      digitalWrite(p1_colpins[c], LOW);
      delay(10);

      // scan the rows
      for (int r=0; r<p1_nrow; r++) {
         p1_state[r][c]->update();
         
         if (p1_state[r][c]->fallingEdge()) {
            Keyboard.press(p1_codes[r][c]);
         } else if (p1_state[r][c]->risingEdge()) {
            Keyboard.release(p1_codes[r][c]);
         }
      }
      // set pin high again when finished
      digitalWrite(p1_colpins[c], HIGH);
   }

   // p2 SCAN //////////////////////////////////////////////
   for (int c=0; c<p2_ncol; c++) {
      // ground the col pin
      digitalWrite(p2_colpins[c], LOW);
      delay(10);

      // scan the rows
      for (int r=0; r<p2_nrow; r++) {
         p2_state[r][c]->update();
         
         if (p2_state[r][c]->fallingEdge()) {
            Keyboard.press(p2_codes[r][c]);
         } else if (p2_state[r][c]->risingEdge()) {
            Keyboard.release(p2_codes[r][c]);
         }
      }
      // set pin high again when finished
      digitalWrite(p2_colpins[c], HIGH);
   }

   // p3 SCAN //////////////////////////////////////////////
   for (int r=0; r<p3_nrow; r++) {
      // scan the rows
      p3_state[r]->update();

      if (p3_state[r]->fallingEdge()) {
         Keyboard.press(p3_codes[r]);
      } else if (p3_state[r]->risingEdge()) {
         Keyboard.release(p3_codes[r]);
      }
   }

   // P4 SCAN //////////////////////////////////////////////
   for (int c=0; c<p4_ncol; c++) {
      // ground the col pin
      digitalWrite(p4_colpins[c], LOW);
      delay(10);

      // scan the rows
      for (int r=0; r<p4_nrow; r++) {
         p4_state[r][c]->update();
         
         if (p4_state[r][c]->fallingEdge()) {
            Keyboard.press(p4_codes[r][c]);
         } else if (p4_state[r][c]->risingEdge()) {
            Keyboard.release(p4_codes[r][c]);
         }
      }
      // set pin high again when finished
      digitalWrite(p4_colpins[c], HIGH);
   }

   // p5 SCAN //////////////////////////////////////////////
   for (int c=0; c<p5_ncol; c++) {
      // ground the col pin
      digitalWrite(p5_colpins[c], LOW);
      delay(10);

      // scan the rows
      for (int r=0; r<p5_nrow; r++) {
         p5_state[r][c]->update();
         
         if (p5_state[r][c]->fallingEdge()) {
            Keyboard.press(p5_codes[r][c]);
         } else if (p5_state[r][c]->risingEdge()) {
            Keyboard.release(p5_codes[r][c]);
         }
      }
      // set pin high again when finished
      digitalWrite(p5_colpins[c], HIGH);
   }

   // p6b SCAN /////////////////////////////////////////////
   for (int c=0; c<p6_ncol; c++) {
      // ground the col pin
      digitalWrite(p6_colpins[c], LOW);
      delay(10);

      // scan the rows
      for (int r=0; r<p6_nrow; r++) {
         p6_state[r][c]->update();
         
         if (p6_state[r][c]->fallingEdge()) {
            Keyboard.press(p6_codes[r][c]);
         } else if (p6_state[r][c]->risingEdge()) {
            Keyboard.release(p6_codes[r][c]);
         }
      }
      // set pin high again when finished
      digitalWrite(p6_colpins[c], HIGH);
   }

   // p6a JOYSTICK /////////////////////////////////////////
   // read in axis values
   // uncalibrated values
   axisVals[0] = 1024 - analogRead(p6_xyztpins[0]); 
   axisVals[1] = 1024 - analogRead(p6_xyztpins[1]);
   axisVals[2] =        analogRead(p6_xyztpins[2]);

 
   // remap to calibrate 
   // map(value, fromLow, fromHigh, toLow, toHigh)
   axisVals[0] = map(axisVals[0],  350,  680, 0, 1023);
   axisVals[1] = map(axisVals[1],  355,  675, 0, 1023);
   axisVals[2] = map(axisVals[2],  295,  735, 0, 1023);
  
   // send 
   Joystick.X(axisVals[0]);
   Joystick.Y(axisVals[1]);
   Joystick.Z(axisVals[2]);


   // THROTTLE ////////////////////////////////////////////
   
   // detect targeting condition ALL_STOP
   if (p6_state[2][0]->fallingEdge()) {
      down_throttle = true;
      Serial.println("THROTTLE DOWN");
   }

   // detect targeting condition FULL_REVERSE
   if (p6_state[1][0]->fallingEdge()) {
      down_throttle = true;
      Serial.println("THROTTLE DOWN");
   }

   // if targeting, move don't sense
   throttle_val = analogRead(p6_xyztpins[3]);

   // targeting and close -> block and clear
   if (down_throttle && (throttle_val < throttle_near)) { 
      //Serial.println("BLOCK");
      digitalWrite(motcon_pins[0], LOW);
      digitalWrite(motcon_pins[1], HIGH);
      while(throttle_val > throttle_tol) {
         throttle_val = analogRead(p6_xyztpins[3]);
         //Serial.println(throttle_val);
      }
      digitalWrite(motcon_pins[0], LOW);
      digitalWrite(motcon_pins[1], LOW);
      down_throttle = false;
      //Serial.println("RESUME");

   // targeting and far -> just set direction
   } else if (down_throttle) { 
      //Serial.println("case 2");
      digitalWrite(motcon_pins[0], LOW);
      digitalWrite(motcon_pins[1], HIGH);

   // not targeting -> just send the val
   } else {
      Joystick.sliderLeft(map(throttle_val, 5, 1010, 0, 1023));
   }

   //Serial.println("X[" + String(axisVals[0]) + "]  Y[" + String(axisVals[1]) \
      //+ "]  Z[" + String(axisVals[2]) + "]  TH[" + throttle_val + "]");


   // DUMP SCAN ////////////////////////////////////////////
   // 
   
   dump_state->update();
   if (dump_state->fallingEdge()){
      Keyboard.press(C_DUMP);
      release_dump = true;     // request release
   } else if (release_dump) {  // on next iteration
      Keyboard.release(C_DUMP);
      release_dump = false;
   }


   // BASE LOOP SPEED
   delay(40);
}

