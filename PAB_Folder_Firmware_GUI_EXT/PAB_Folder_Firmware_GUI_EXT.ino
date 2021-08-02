/*****LIBRARIES*****/
#include <EEPROM.h>
#include <GUI.h>
/******************/

/*****DEFINITIONS*****/
//Stepper Motor Connections
#define pulseA  23
#define pulseB  25
#define pulseC  27
#define dirA    22
#define dirB    24
#define dirC    26

//Stepper Motor Enabling
#define ENABLE_MOTORS 53

//Stepper Motor Settings
  /*LOW LOW LOW   - Full Step
    HIGH LOW LOW  - Half Step
    LOW HIGH LOW  - Quarter Step
    HIGH HIGH LOW - Eighth Step
    HIGH HIGH HIGH - Sixteenth Step*/
#define MS1A 38
#define MS2A 40
#define MS3A 42     
#define MS1B 30
#define MS2B 32
#define MS3B 34
#define MS1C 39
#define MS2C 41
#define MS3C 43
#define MS1D 31
#define MS2D 33
#define MS3D 35

//Endstops
#define xEndstop 45
#define yEndstop 47
#define zEndStop 49

/*********************/

/*****CONSTANTS*****/

/*******************/

/*****VARIABLES*****/

/////MACHINE SETTINGS
//Homing Offsets
float homeOffsetT = 0;
float homeOffsetB = 0;
float homeOffsetClampT = 0;
float homeOffsetClampB = 0;

/////MOTOR SETTINGS
//Motor Steps/mm
float mStepsPerA = 80;
float mStepsPerB = 80;
float mStepsPerC = 80;

//Motor Step Tracking
int mStepsA = 0;
int mStepsB = 0;
int mStepsC = 0;

GUI gui = GUI(mStepsA, mStepsB, mStepsC, mStepsPerA, mStepsPerB, mStepsPerC);

//Motor Enabled
boolean enM = true;
//Motor Direction
boolean mDirA = 0;
boolean mDirB = 0;
boolean mDirC = 0;

//Motor Direction Inversion
boolean mInvDirA = false;
boolean mInvDirB = false;
boolean mInvDirC = false;

//Microstepping Value
//0=1, 1=1/2, 2=1/4, 3=1/8, 4=1/16
int mUStepA = 4;
int mUStepB = 4;
int mUStepC = 4;

//Max Velocity (mm/s)
int mVelocityA = 50;
int mVelocityB = 50;
int mVelocityC = 50;

//Cur Velocity (mm/s)
int mCurVelocityA = 50;
int mCurVelocityB = 50;
int mCurVelocityC = 50;

//Max Acceleration (mm/s^2)
int mAccelA = 25;
int mAccelB = 25;
int mAccelC = 25;

//Max Acceleration (mm/s^2)
int mCurAccelA = 25;
int mCurAccelB = 25;
int mCurAccelC = 25;

//Max Jerk (mm/s^3)
int mJerkA = 7;
int mJerkB = 7;
int mJerkC = 7;

/////CLAMP STATUS
boolean tClampEngaged = false;
boolean bClampEngaged = false;

/////PROFILE VALUES
String profile = "     ";
int numProfSteps = 1;
int tClampDist = 0;
int bClampDist = 0;
int hDist1 = 0;
int hDist2 = 0;
int hDist3 = 0;
/*******************/

void setup() {  
  pinMode(pulseA, OUTPUT);
  pinMode(pulseB, OUTPUT);
  pinMode(pulseC, OUTPUT);
  pinMode(dirA, OUTPUT);
  pinMode(dirB, OUTPUT);
  pinMode(dirC, OUTPUT);
  
  pinMode(ENABLE_MOTORS, OUTPUT);
  
  pinMode(MS1A, OUTPUT);
  pinMode(MS2A, OUTPUT);
  pinMode(MS3A, OUTPUT);
  pinMode(MS1B, OUTPUT);
  pinMode(MS2B, OUTPUT);
  pinMode(MS3B, OUTPUT);
  pinMode(MS1C, OUTPUT);
  pinMode(MS2C, OUTPUT);
  pinMode(MS3C, OUTPUT);
  pinMode(MS1D, OUTPUT);
  pinMode(MS2D, OUTPUT);
  pinMode(MS3D, OUTPUT);
  
  /***Motor Setup***/  
  setMicroStepping('A', 4);
  setMicroStepping('B', 4);
  setMicroStepping('C', 4);
  setMotorDirection('A', 0);
  setMotorDirection('B', 0);
  setMotorDirection('C', 0);
  enableMotors();
}

void loop() {
  //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
  if(gui.activeScreen == 0) {
    gui.updateMainScreen();
  }
  else if(gui.activeScreen == 1) {
    gui.updateProfileLoadScreen();
  }
  else if(gui.activeScreen == 2) {
    gui.updateNewProfileScreen();
  }
  else if(gui.activeScreen == 3) {
    gui.updateEditProfileScreen();
  }
  else if(gui.activeScreen == 4) {
    gui.updateMachineSettingsScreen();
  }
  else if(gui.activeScreen == 5){ 
    gui.updateInputScreen();
  }
  else if(gui.activeScreen == 6) {
    gui.updateVelScreen();
  }
  else if(gui.activeScreen == 7) {
    gui.updateAccelScreen();
  }
  else if(gui.activeScreen == 8) {
    gui.updateJerkScreen();
  }
  else if(gui.activeScreen = 9) {
    gui.updateMicrostepScreen();
  }
  else if(gui.activeScreen == 10) {
    gui.updateStepsMMScreen();
  }
}

/**Move the specified motor to a specified location**/
void moveTo(char motor, float mmPos) {
  if(motor == 'A') {
    int toSteps = (mmPos * mStepsPerA) - mStepsA;   //Get the distance needing to be closed from current position in step number
    long startTime = millis();                      //Record when move began
    long prevPulseTime = startTime;                 //Initialize the previous pulse timer

    int i = 0;
    while(i < toSteps) {            //While there are still steps to be made
      long curTime = millis();      //Update the current time

      if(i < toSteps / 2) {         //If in the first half of the movement
        mCurVelocityA = (mAccelA * (startTime - curTime)) + (mJerkA * (startTime - curTime)^2);        //Calculate the current velocity from elapsed time
        
        if(mCurVelocityA > mVelocityA) {                                                                //If the value would go beyond the max allowed
          mCurVelocityA = mVelocityA;                                                                   //Set it to the max allowed
        }
      }
      else if(i >= toSteps / 2) {   //If in the second half of the movement
        mCurVelocityA -= (mAccelA * (startTime - curTime)) + (mJerkA * (startTime - curTime)^2);

        if(mCurVelocityA < mVelocityA) {                                                                //If the value would go beyond the max allowed
          mCurVelocityA = 0;                                                                            //Set it to the max allowed
        }
      }

      if(mCurVelocityA != 0) {
        long delayPeriod = 1000 / mCurVelocityA;
    
        if(wait(delayPeriod, prevPulseTime)) {
          prevPulseTime = millis();
          i++;
          doStep('A');
        }
      }
    }
  }
  else if(motor == 'B') {
    int toSteps = (mmPos * mStepsPerB) - mStepsB;   //Get the distance needing to be closed from current position in step number
    long startTime = millis();                      //Record when move began
    long prevPulseTime = startTime;                 //Initialize the previous pulse timer

    int i = 0;
    while(i < toSteps) {            //While there are still steps to be made
      long curTime = millis();      //Update the current time

      if(i < toSteps / 2) {         //If in the first half of the movement
        mCurVelocityB = (mAccelB * (startTime - curTime)) + (mJerkB * (startTime - curTime)^2);        //Calculate the current velocity from elapsed time
        
        if(mCurVelocityB > mVelocityB) {                                                                //If the value would go beyond the max allowed
          mCurVelocityB = mVelocityB;                                                                   //Set it to the max allowed
        }
      }
      else if(i >= toSteps / 2) {   //If in the second half of the movement
        mCurVelocityB -= (mAccelB * (startTime - curTime)) + (mJerkB * (startTime - curTime)^2);

        if(mCurVelocityB < mVelocityB) {                                                                //If the value would go beyond the max allowed
          mCurVelocityB = 0;                                                                            //Set it to the max allowed
        }
      }

      if(mCurVelocityB != 0) {
        long delayPeriod = 1000 / mCurVelocityB;
    
        if(wait(delayPeriod, prevPulseTime)) {
          prevPulseTime = millis();
          i++;
          doStep('B');
        }
      }
    }
  }
  else if(motor == 'C') {
    int toSteps = (mmPos * mStepsPerC) - mStepsC;   //Get the distance needing to be closed from current position in step number
    long startTime = millis();                      //Record when move began
    long prevPulseTime = startTime;                 //Initialize the previous pulse timer

    int i = 0;
    while(i < toSteps) {            //While there are still steps to be made
      long curTime = millis();      //Update the current time

      if(i < toSteps / 2) {         //If in the first half of the movement
        mCurVelocityC = (mAccelC * (startTime - curTime)) + (mJerkC * (startTime - curTime)^2);        //Calculate the current velocity from elapsed time
        
        if(mCurVelocityC > mVelocityC) {                                                                //If the value would go beyond the max allowed
          mCurVelocityC = mVelocityC;                                                                   //Set it to the max allowed
        }
      }
      else if(i >= toSteps / 2) {   //If in the second half of the movement
        mCurVelocityC -= (mAccelC * (startTime - curTime)) + (mJerkC * (startTime - curTime)^2);

        if(mCurVelocityC < mVelocityC) {                                                                //If the value would go beyond the max allowed
          mCurVelocityC = 0;                                                                            //Set it to the max allowed
        }
      }

      if(mCurVelocityC != 0) {
        long delayPeriod = 1000 / mCurVelocityC;
    
        if(wait(delayPeriod, prevPulseTime)) {
          prevPulseTime = millis();
          i++;
          doStep('C');
        }
      }
    }
  }
}

/**Move the specified motor by one step**/
void doStep(char motor) {
  if(motor == 'A') {               //If the motor specified is motor A
    if(mDirA == 0) {               //If the direction of movement is 0
      mStepsA++;                   //Increase the counted steps
    }
    else if(mDirA == 1) {          //If the direction of movement is 1
      mStepsA--;                   //Decrease the counted steps
    }
    //Send step pulse command
    digitalWrite(pulseA, HIGH);
    delayMicroseconds(25);
    digitalWrite(pulseA, LOW);
    delayMicroseconds(25);
  }
  else if(motor == 'B') {
    if(mDirB == 0) {
      mStepsB++;
    }
    else if(mDirB == 1) {
      mStepsB--;
    }
    digitalWrite(pulseB, HIGH);
    delayMicroseconds(25);
    digitalWrite(pulseB, LOW);
    delayMicroseconds(25);
  }
  else if(motor == 'C') {
    if(mDirC == 0) {
      mStepsC++;
    }
    else if(mDirC == 1) {
      mStepsC--;
    }
    digitalWrite(pulseC, HIGH);
    delayMicroseconds(25);
    digitalWrite(pulseC, LOW);
    delayMicroseconds(25);
  }
}

/**Turn on the specified motor**/
void enableMotors() {
    enM = true;
    digitalWrite(ENABLE_MOTORS, LOW);
}

/**Turn off the secified motor**/
void disableMotors() {
    enM = false;
    digitalWrite(ENABLE_MOTORS, HIGH);
}

/**Change the microstepping of the specified motor**/
/*uStep 0 = 1, 1 = 1/2, 2 = 1/4, 3 = 1/8, 4 = 1/168*/
void setMicroStepping(char motor, int uStep) {
  if(motor == 'A') {  //If the specified motor is motor A or D (top/bot carriages)
    mUStepA = uStep;                  //Update the motor A microstepping variable

    //Motor D is cloned from A with few exceptions such as inversion and direction settings
    switch(uStep) {
      case 0:                       //Full Step
        digitalWrite(MS1A, LOW);
        digitalWrite(MS2A, LOW);
        digitalWrite(MS3A, LOW);
        digitalWrite(MS1D, LOW);
        digitalWrite(MS2D, LOW);
        digitalWrite(MS3D, LOW);
        break;
      case 1:                       //1/2 Step
        digitalWrite(MS1A, HIGH);
        digitalWrite(MS2A, LOW);
        digitalWrite(MS3A, LOW);
        digitalWrite(MS1D, HIGH);
        digitalWrite(MS2D, LOW);
        digitalWrite(MS3D, LOW);
        break;
      case 2:                       //1/4 Step
        digitalWrite(MS1A, LOW);
        digitalWrite(MS2A, HIGH);
        digitalWrite(MS3A, LOW);
        digitalWrite(MS1D, LOW);
        digitalWrite(MS2D, HIGH);
        digitalWrite(MS3D, LOW);
        break;
      case 3:                       //1/8 Step
        digitalWrite(MS1A, HIGH);
        digitalWrite(MS2A, HIGH);
        digitalWrite(MS3A, LOW);
        digitalWrite(MS1D, HIGH);
        digitalWrite(MS2D, HIGH);
        digitalWrite(MS3D, LOW);
        break;
      case 4:                       //1/16 Step
        digitalWrite(MS1A, HIGH);
        digitalWrite(MS2A, HIGH);
        digitalWrite(MS3A, HIGH);
        digitalWrite(MS1D, HIGH);
        digitalWrite(MS2D, HIGH);
        digitalWrite(MS3D, HIGH);
        break;  
    }
  }
  else if(motor == 'B') {
    mUStepB = uStep;

    switch(uStep) {
      case 0:                       //Full Step
        digitalWrite(MS1B, LOW);
        digitalWrite(MS2B, LOW);
        digitalWrite(MS3B, LOW);
        break;
      case 1:                       //1/2 Step
        digitalWrite(MS1B, HIGH);
        digitalWrite(MS2B, LOW);
        digitalWrite(MS3B, LOW);
        break;
      case 2:                       //1/4 Step
        digitalWrite(MS1B, LOW);
        digitalWrite(MS2B, HIGH);
        digitalWrite(MS3B, LOW);
        break;
      case 3:                       //1/8 Step
        digitalWrite(MS1B, HIGH);
        digitalWrite(MS2B, HIGH);
        digitalWrite(MS3B, LOW);
        break;
      case 4:                       //1/16 Step
        digitalWrite(MS1B, HIGH);
        digitalWrite(MS2B, HIGH);
        digitalWrite(MS3B, HIGH);
        break;  
    }
  }
  else if(motor == 'C') {
    mUStepC = uStep;
    
    switch(uStep) {
      case 0:                       //Full Step
        digitalWrite(MS1C, LOW);
        digitalWrite(MS2C, LOW);
        digitalWrite(MS3C, LOW);
        break;
      case 1:                       //1/2 Step
        digitalWrite(MS1C, HIGH);
        digitalWrite(MS2C, LOW);
        digitalWrite(MS3C, LOW);
        break;
      case 2:                       //1/4 Step
        digitalWrite(MS1C, LOW);
        digitalWrite(MS2C, HIGH);
        digitalWrite(MS3C, LOW);
        break;
      case 3:                       //1/8 Step
        digitalWrite(MS1C, HIGH);
        digitalWrite(MS2C, HIGH);
        digitalWrite(MS3C, LOW);
        break;
      case 4:                       //1/16 Step
        digitalWrite(MS1C, HIGH);
        digitalWrite(MS2C, HIGH);
        digitalWrite(MS3C, HIGH);
        break;  
    }
  }
}

/**Sets the direction of the specified motor, inverts it if defined**/
void setMotorDirection(char motor, boolean dir) {
  if(motor == 'A') {            //If the specified motor is motor A (top carriages)
    if(mInvDirA) {              //If the motor is marked as inverted
      dir = !dir;               //Change the direction to the opposite of the given direction
    }
    mDirA = dir;                //Update the current motor direction variable
    digitalWrite(dirA, dir);    //Set the direction of motor A
  }
  else if(motor == 'B') {       //If the specified motor is motor B (top clamp)
    if(mInvDirB) {              //If the motor is marked as inverted
      dir = !dir;               //Change the direction to the opposite of the given direction
    }
    mDirB = dir;                //Update the current motor direction variable
    digitalWrite(dirB, dir);    //Set the direction of motor B
  }
  else if(motor == 'C') {       //If the specified motor is motor C (bottom clamp)
    if(mInvDirC) {              //If the motor is marked as inverted
      dir = !dir;               //Change the direction to the opposite of the given direction
    }
    mDirC = dir;                //Update the current motor direction variable
    digitalWrite(dirC, dir);    //Set the direction of motor C
  }
}

/**Saves the specified profile**/
void saveProfile(int profileNum) {
  int eeAddr;
  
  //Change the starting memory address of the initial data value depending on which profile is being saved
  switch(profileNum) {
    case(0):
      eeAddr = 150;
      break;
    case(1):
      eeAddr = 200;
      break;
    case(2):
      eeAddr = 250;
      break;
    case(3):
      eeAddr = 300;
      break;
    case(4):
      eeAddr = 350;
      break;
    case(5):
      eeAddr = 400;
      break;
    case(6):
      eeAddr = 450;
      break;
    case(7):
      eeAddr = 500;
      break;
    case(8):
      eeAddr = 550;
      break;
    case(9):
      eeAddr = 600;
      break;
  }

  EEPROM.put(eeAddr, profile);        //Save the profile name (5byte/char limit)
  eeAddr += sizeof(profile);          //Increase the address location to the next free spot
  EEPROM.put(eeAddr, numProfSteps);   //Save the number of steps used in the profile
  eeAddr += sizeof(int);              
  EEPROM.put(eeAddr, tClampDist);    //Save the value of the top clamp position
  eeAddr += sizeof(int);              
  EEPROM.put(eeAddr, bClampDist);    //Save the value of the bottom clamp position
  eeAddr += sizeof(int);    
  EEPROM.put(eeAddr, hDist1);         //Save the value of the top/bottom carriage position
  eeAddr += sizeof(int);
  
  if(numProfSteps == 2) {             //If there are 2 steps in the profile
    EEPROM.put(eeAddr, hDist2);       //Save the second top/bottom carriage position
    eeAddr += sizeof(int);
  }
  else if(numProfSteps == 3) {        //If there are 3 steps in the profile
    EEPROM.put(eeAddr, hDist3);       //Save the third top/bottom carriage position
    eeAddr += sizeof(int);
  }
}

/**Loads the specified profile**/
void loadProfile(int profileNum) {
  int eeAddr;
  
  //Change the starting memory address of the initial data value depending on which profile is being loaded
  switch(profileNum) {
    case(0):
      eeAddr = 150;
      break;
    case(1):
      eeAddr = 200;
      break;
    case(2):
      eeAddr = 250;
      break;
    case(3):
      eeAddr = 300;
      break;
    case(4):
      eeAddr = 350;
      break;
    case(5):
      eeAddr = 400;
      break;
    case(6):
      eeAddr = 450;
      break;
    case(7):
      eeAddr = 500;
      break;
    case(8):
      eeAddr = 550;
      break;
    case(9):
      eeAddr = 600;
      break;
  }

  EEPROM.get(eeAddr, profile);        //Load the profile name (5byte/char limit)
  eeAddr += sizeof(profile);          //Increase the address location to the next free spot
  EEPROM.get(eeAddr, numProfSteps);   //Load the number of steps used in the profile
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, tClampDist);    //Load the value of the top clamp position
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, bClampDist);    //Load the value of the bottom clamp position
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, hDist1);         //Load the value of the top/bottom carriage position
  eeAddr += sizeof(int);
  
  if(numProfSteps == 2) {             //If there are 2 steps in the profile
    EEPROM.get(eeAddr, hDist2);       //Load the second top/bottom carriage position
    eeAddr += sizeof(int);
  }
  else if(numProfSteps == 3) {        //If there are 3 steps in the profile
    EEPROM.get(eeAddr, hDist3);       //Load the third top/bottom carriage position
    eeAddr += sizeof(int);
  }
}

/**Saves the current values to the EEPROM **/
void saveMachineSettings() {
  int eeAddr = 0;

  //Store touch calibration data
  EEPROM.put(eeAddr, gui.TS_LEFT);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, gui.TS_RIGHT);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, gui.TS_TOP);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, gui.TS_BOT);
  eeAddr += sizeof(int);

  //Store homing offsets data
  EEPROM.put(eeAddr, homeOffsetT);
  eeAddr += sizeof(float);
  EEPROM.put(eeAddr, homeOffsetB);
  eeAddr += sizeof(float);
  EEPROM.put(eeAddr, homeOffsetClampT);
  eeAddr += sizeof(float);
  EEPROM.put(eeAddr, homeOffsetClampB);
  eeAddr += sizeof(float);

  //Store steps/mm data
  EEPROM.put(eeAddr, mStepsPerA);
  eeAddr += sizeof(float);
  EEPROM.put(eeAddr, mStepsPerB);
  eeAddr += sizeof(float);
  EEPROM.put(eeAddr, mStepsPerC);
  eeAddr += sizeof(float);

  //Store motor inversion settings
  EEPROM.put(eeAddr, mInvDirA);
  eeAddr += sizeof(boolean);
  EEPROM.put(eeAddr, mInvDirB);
  eeAddr += sizeof(boolean);
  EEPROM.put(eeAddr, mInvDirC);
  eeAddr += sizeof(boolean);

  //Store microstepping data
  EEPROM.put(eeAddr, mUStepA);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, mUStepB);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, mUStepC);
  eeAddr += sizeof(int);

  //Store velocity settings
  EEPROM.put(eeAddr, mVelocityA);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, mVelocityB);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, mVelocityC);
  eeAddr += sizeof(int);

  //Store acceleration settings
  EEPROM.put(eeAddr, mAccelA);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, mAccelB);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, mAccelC);
  eeAddr += sizeof(int);
  
  //Store jerk settings
  EEPROM.put(eeAddr, mJerkA);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, mJerkB);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, mJerkC);
  eeAddr += sizeof(int);
}

/**Reads in the saved values to current objects**/
void loadMachineSettings() {
  int eeAddr = 0;

  //Load touch calibration data
  EEPROM.get(eeAddr, gui.TS_LEFT);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, gui.TS_RIGHT);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, gui.TS_TOP);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, gui.TS_BOT);
  eeAddr += sizeof(int);

  //Load homing offsets data
  EEPROM.get(eeAddr, homeOffsetT);
  eeAddr += sizeof(float);
  EEPROM.get(eeAddr, homeOffsetB);
  eeAddr += sizeof(float);
  EEPROM.get(eeAddr, homeOffsetClampT);
  eeAddr += sizeof(float);
  EEPROM.get(eeAddr, homeOffsetClampB);
  eeAddr += sizeof(float);

  //Load steps/mm data
  EEPROM.get(eeAddr, mStepsPerA);
  eeAddr += sizeof(float);
  EEPROM.get(eeAddr, mStepsPerB);
  eeAddr += sizeof(float);
  EEPROM.get(eeAddr, mStepsPerC);
  eeAddr += sizeof(float);

  //Load motor inversion settings
  EEPROM.get(eeAddr, mInvDirA);
  eeAddr += sizeof(boolean);
  EEPROM.get(eeAddr, mInvDirB);
  eeAddr += sizeof(boolean);
  EEPROM.get(eeAddr, mInvDirC);
  eeAddr += sizeof(boolean);

  //Load microstepping data
  EEPROM.get(eeAddr, mUStepA);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, mUStepB);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, mUStepC);
  eeAddr += sizeof(int);

  //Load velocity settings
  EEPROM.get(eeAddr, mVelocityA);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, mVelocityB);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, mVelocityC);
  eeAddr += sizeof(int);

  //Load acceleration settings
  EEPROM.get(eeAddr, mAccelA);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, mAccelB);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, mAccelC);
  eeAddr += sizeof(int);
  
  //Load jerk settings
  EEPROM.get(eeAddr, mJerkA);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, mJerkB);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, mJerkC);
  eeAddr += sizeof(int);
}

/**Calculate a distance(mm) from the step count**/
float convertStepsToDistance(int mSteps, float mStepsPer, int mUStep) {
  //0=1, 1=1/2, 2=1/4, 3=1/8, 4=1/16
  float uStepF = 0;
  switch(mUStep) {
    case(0):
      uStepF = 1;
      break;
    case(1):
      uStepF = 1/2;
      break;
    case(2):
      uStepF = 1/4;
      break;
    case(3):
      uStepF = 1/8;
      break;
    case(4):
      uStepF = 1/16;
      break;
  }
  float distance = (mSteps / mStepsPer) * uStepF;
  return distance;
}

/**Calculates the number of steps to meet a distane**/
int convertDistanceToSteps(float distance, float mStepsPer, int mUStep) {
  //0=1, 1=1/2, 2=1/4, 3=1/8, 4=1/16
  float uStepF = 0;
  switch(mUStep) {
    case(0):
      uStepF = 1;
      break;
    case(1):
      uStepF = 1/2;
      break;
    case(2):
      uStepF = 1/4;
      break;
    case(3):
      uStepF = 1/8;
      break;
    case(4):
      uStepF = 1/16;
      break;
  }
  float steps = (distance * mStepsPer) / uStepF;
  return steps;
}

/**Wait for elapsed time without suspending program**/
boolean wait(int waitTime, long timeStarted) {
  if(millis() - timeStarted >= waitTime) {    //If the right amount of time has passed
    return true;                              //Return 1 (true)
  }
  else {                                      //Otherwise
    return false;                             //Return 0 (false)
  }
}
