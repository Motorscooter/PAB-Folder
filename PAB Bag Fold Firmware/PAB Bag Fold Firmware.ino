/*****LIBRARIES*****/
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
/******************/

/*****DEFINITIONS*****/
//LCD Pin Connections
#define LCD_RD   A0
#define LCD_WR   A1     
#define LCD_RS   A2        
#define LCD_CS   A3       
#define LCD_REST A4

//LCD Color Values
#define BLACK   0x0000
#define GRAY    0x8410
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//Stepper Motor Connections
#define pulseA  2
#define pulseB  3
#define pulseC  4
#define pulseD  4
#define dirA    5
#define dirB    6
#define dirC    7
#define dirD    7

//Stepper Motor Enabling
#define ENABLE_MOTORS 8

//Stepper Motor Settings
  /*LOW LOW LOW   - Full Step
    HIGH LOW LOW  - Half Step
    LOW HIGH LOW  - Quarter Step
    HIGH HIGH LOW - Eighth Step
    HIGH HIGH HIGH - Sixteenth Step*/
#define MS1A 20
#define MS2A 21
#define MS3A 22     
#define MS1B 23
#define MS2B 24
#define MS3B 25
#define MS1C 26
#define MS2C 27
#define MS3C 28
#define MS1D 29
#define MS2D 30
#define MS3D 31

/*********************/

/*****CONSTANTS*****/

/*******************/

/*****VARIABLES*****/
//Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
LCDWIKI_KBV tft(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

/////Screen Management
//Main(0), Load(1), New(2), Edit(3), Machine(4), Velocity-Acceleration-Jerk(5), Microstepping(6), Steps/mm(7)
int activeScreen = 0;

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
float mStepsPerD = 80;

//Motor Step Tracking
int mStepsA;
int mStepsB;
int mStepsC;
int mStepsD;

//Motor Enabled
boolean enM = false;

//Motor Direction
boolean mDirA = 0;
boolean mDirB = 0;
boolean mDirC = 0;
boolean mDirD = 0;

//Motor Direction Inversion
boolean mInvDirA = false;
boolean mInvDirB = false;
boolean mInvDirC = false;
boolean mInvDirD = false;

//Microstepping Value
//0=1, 1=1/2, 2=1/4, 3=1/8, 4=1/16
int mUStepA = 4;
int mUStepB = 4;
int mUStepC = 4;
int mUStepD = 4;

//Max Velocity (mm/s)
int mVelocityA = 50;
int mVelocityB = 50;
int mVelocityC = 50;
int mVelocityD = 50;

//Cur Velocity (mm/s)
int mCurVelocityA = 50;
int mCurVelocityB = 50;
int mCurVelocityC = 50;
int mCurVelocityD = 50;

//Max Acceleration (mm/s^2)
int mAccelA = 25;
int mAccelB = 25;
int mAccelC = 25;
int mAccelD = 25;

//Max Acceleration (mm/s^2)
int mCurAccelA = 25;
int mCurAccelB = 25;
int mCurAccelC = 25;
int mCurAccelD = 25;

//Max Jerk (mm/s^3)
int mJerkA = 7;
int mJerkB = 7;
int mJerkC = 7;
int mJerkD = 7;

/////CLAMP STATUS
boolean tClampEngaged = false;
boolean bClampEngaged = false;

/////DISPLAY VALUES
int topDist = 0;
int botDist = 0;
int topClampDist = 0;
int botClampDist = 0;
/*******************/

void setup() {
  /***Screen Setup***/
  tft.Init_LCD();
  tft.Set_Rotation(1);
  tft.Fill_Screen(BLACK);    //Change the screen to solid white
  tft.Set_Text_Mode(0);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(2);
  tft.Print_String("Autoliv", 0, 0);
  delay(200);
  /***************/
  
  /***Motor Setup***/  
  setMicroStepping('A', 4);
  setMicroStepping('B', 4);
  setMicroStepping('C', 4);
  setMicroStepping('D', 4);
  setMotorDirection('A', 0);
  setMotorDirection('B', 0);
  setMotorDirection('C', 0);
  setMotorDirection('D', 0);
  enableMotors();
}

void loop() {
  updateMainScreen();
}

/**Redraw the main home screen**/
void updateMainScreen() {

  /***Button Sizes***/
  int xPadding = 5;
  int yPadding = 5;
  
  int configBtnX = 135;
  int configBtnY = 40;
  
  int settingBtnX = 40;
  int settingBtnY = 40;
  
  int clampBtnX = 110;
  int clampBtnY = 110;
  
  int homeBtnX = 40;
  int homeBtnY = 40;
  
  int stepBtnX = 40;
  int stepBtnY = 40;
  
  int mEnBtnX = 100;
  int mEnBtnY = 100;
  
  /***Screen Update***/
  //Load Configuration Button
  int load_Btn_Start_x = xPadding;
  int load_Btn_Start_y = yPadding;
  int load_Btn_Stop_x = load_Btn_Start_x + configBtnX;
  int load_Btn_Stop_y = load_Btn_Start_y + configBtnY;  
  tft.Fill_Screen(BLACK);    //Change the screen to solid white
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(load_Btn_Start_x, load_Btn_Start_y, load_Btn_Stop_x, load_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(load_Btn_Start_x, load_Btn_Start_y, load_Btn_Stop_x, load_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Load Config", load_Btn_Start_x + 35, load_Btn_Start_y + 17);
  
  //New Configuration Button
  int new_Btn_Start_x = (xPadding*2) + (configBtnX);
  int new_Btn_Start_y = yPadding;
  int new_Btn_Stop_x = new_Btn_Start_x + configBtnX;
  int new_Btn_Stop_y = new_Btn_Start_y + configBtnY;
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(new_Btn_Start_x, new_Btn_Start_y, new_Btn_Stop_x, new_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(new_Btn_Start_x, new_Btn_Start_y, new_Btn_Stop_x, new_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("New Config", new_Btn_Start_x + 37, new_Btn_Start_y + 17);

  //Edit Configuration Button
  int edit_Btn_Start_x = (xPadding*3) + (configBtnX*2);
  int edit_Btn_Start_y = yPadding;
  int edit_Btn_Stop_x = edit_Btn_Start_x + configBtnX;
  int edit_Btn_Stop_y = edit_Btn_Start_y + configBtnY;
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(edit_Btn_Start_x, edit_Btn_Start_y, edit_Btn_Stop_x, edit_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(edit_Btn_Start_x, edit_Btn_Start_y, edit_Btn_Stop_x, edit_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Edit Config", edit_Btn_Start_x + 35, edit_Btn_Start_y + 17);

  //Machine Settings Button
  int set_Btn_Start_x = (xPadding*4) + (configBtnX*3);
  int set_Btn_Start_y = yPadding;
  int set_Btn_Stop_x = set_Btn_Start_x + settingBtnX;
  int set_Btn_Stop_y = set_Btn_Start_y + settingBtnY;
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(set_Btn_Start_x, set_Btn_Start_y, set_Btn_Stop_x, set_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(set_Btn_Start_y, set_Btn_Start_y, set_Btn_Stop_x, set_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Set", set_Btn_Start_x+ 11, set_Btn_Start_y + 17);

  //Clamp Toggle Buttons Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Clamp Toggles", 385, (yPadding*2) + settingBtnY + 5);

  //Clamp Toggle Button Top
  int top_clamp_Btn_Start_x = 365;
  int top_clamp_Btn_Start_y = (yPadding*3) + settingBtnY + 15;
  int top_clamp_Btn_Stop_x = top_clamp_Btn_Start_x + clampBtnX;
  int top_clamp_Btn_Stop_y =top_clamp_Btn_Start_y + clampBtnY;
  tft.Set_Draw_color(RED);
  tft.Fill_Rectangle(top_clamp_Btn_Start_x, top_clamp_Btn_Start_y, top_clamp_Btn_Stop_x, top_clamp_Btn_Stop_y);
  tft.Set_Draw_color(MAGENTA);
  tft.Draw_Rectangle(top_clamp_Btn_Start_x, top_clamp_Btn_Start_y, top_clamp_Btn_Stop_x, top_clamp_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(RED);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("TOP", top_clamp_Btn_Start_x + 37, top_clamp_Btn_Start_y + 40);
  tft.Set_Text_Size(1);
  tft.Print_String("(Disengaged)", top_clamp_Btn_Start_x + 20, top_clamp_Btn_Start_y + 65);

  //Clamp Toggle Button Bottom
  int bot_clamp_Btn_Start_x = 365;
  int bot_clamp_Btn_Start_y = (yPadding*4) + settingBtnY + clampBtnY + 15;
  int bot_clamp_Btn_Stop_x = bot_clamp_Btn_Start_x + clampBtnX;
  int bot_clamp_Btn_Stop_y = bot_clamp_Btn_Start_y + clampBtnY;
  tft.Set_Draw_color(RED);
  tft.Fill_Rectangle(bot_clamp_Btn_Start_x, bot_clamp_Btn_Start_y, bot_clamp_Btn_Stop_x, bot_clamp_Btn_Stop_y);
  tft.Set_Draw_color(MAGENTA);
  tft.Draw_Rectangle(bot_clamp_Btn_Start_x, bot_clamp_Btn_Start_y, bot_clamp_Btn_Stop_x, bot_clamp_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(RED);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("BOT", bot_clamp_Btn_Start_x + 37, bot_clamp_Btn_Start_y + 40);
  tft.Set_Text_Size(1);
  tft.Print_String("(Disengaged)", bot_clamp_Btn_Start_x + 20, bot_clamp_Btn_Start_y + 65);
  delay(10000);
/*
  //Home Button
  int home_Btn_Start_x = xPadding;
  int home_Btn_Start_y = 288;
  int home_Btn_Stop_x = home_Btn_Start_x + homeBtnX;
  int home_Btn_Stop_y = home_Btn_Start_y + homeBtnY;
  tft.fillRect(home_Btn_Start_x, home_Btn_Start_y, homeBtnX, homeBtnY, WHITE);
  tft.drawRect(home_Btn_Start_x, home_Btn_Start_y, homeBtnX, homeBtnY, GRAY);
  tft.setCursor(home_Btn_Start_x, home_Btn_Start_y);
  tft.setTextColor(BLACK);
  tft.setTextSize(6);
  tft.print("Home");

  //Prev Step Button
  int prev_Btn_Start_x = (xPadding*2) + homeBtnX;
  int prev_Btn_Start_y = 288;
  int prev_Btn_Stop_x = prev_Btn_Start_x + stepBtnX;
  int prev_Btn_Stop_y = prev_Btn_Start_y + stepBtnY;
  tft.fillRect(prev_Btn_Start_x, prev_Btn_Start_y, stepBtnX, stepBtnY, WHITE);
  tft.drawRect(prev_Btn_Start_x, prev_Btn_Start_y, stepBtnX, stepBtnY, GRAY);
  tft.setCursor(prev_Btn_Start_x, prev_Btn_Start_y);
  tft.setTextColor(BLACK);
  tft.setTextSize(6);
  tft.print("Back");

  //Next Step Button
  int next_Btn_Start_x = (xPadding*3) + homeBtnX + stepBtnX;
  int next_Btn_Start_y = 288;
  int next_Btn_Stop_x = next_Btn_Start_x + stepBtnX;
  int next_Btn_Stop_y = next_Btn_Start_y + stepBtnY;
  tft.fillRect(next_Btn_Start_x, next_Btn_Start_y, stepBtnX, stepBtnY, WHITE);
  tft.drawRect(next_Btn_Start_x, next_Btn_Start_y, stepBtnX, stepBtnY, GRAY);
  tft.setCursor(next_Btn_Start_x, next_Btn_Start_y);
  tft.setTextColor(BLACK);
  tft.setTextSize(6);
  tft.print("Next");

  //Motor Enable Toggle Button
  int mEN_Btn_Start_x = (xPadding*4) + homeBtnX + (stepBtnX*2);
  int mEN_Btn_Start_y = 263;
  int mEN_Btn_Stop_x = mEN_Btn_Start_x + mEnBtnX;
  int mEN_Btn_Stop_y = mEN_Btn_Start_y + mEnBtnY;
  tft.fillRect(mEN_Btn_Start_x, mEN_Btn_Start_y, mEnBtnX, mEnBtnY, WHITE);
  tft.drawRect(mEN_Btn_Start_x, mEN_Btn_Start_y, mEnBtnX, mEnBtnY, GRAY);
  tft.setCursor(mEN_Btn_Start_x, mEN_Btn_Start_y);
  tft.setTextColor(BLACK);
  tft.setTextSize(6);
  tft.print("Toggle Motors");

  //Configuration Profile Title
  tft.setCursor(xPadding, 39);
  tft.setTextColor(YELLOW);
  tft.setTextSize(6);
  tft.print("Configuration Profile: ");

  //Configuration Profile Text
  tft.setCursor(160, 39);
  tft.setTextColor(CYAN);
  tft.setTextSize(5);
  //tft.print();
  
  //Top Clamp Position Title
  tft.setCursor(xPadding, 52);
  tft.setTextColor(YELLOW);
  tft.setTextSize(6);
  tft.print("Top Clamp Position: ");

  //Top Clamp Position Text
  tft.setCursor(160, 52);
  tft.setTextColor(CYAN);
  tft.setTextSize(6);
  tft.print((String)convertStepsToDistance(mStepsB, mStepsPerB, mUStepB) + "mm");

  //Bottom Clamp Position Title
  tft.setCursor(xPadding, 65);
  tft.setTextColor(YELLOW);
  tft.setTextSize(6);
  tft.print("Bottom Clamp Position: ");

  //Bottom Clamp Position Text
  tft.setCursor(160, 65);
  tft.setTextColor(CYAN);
  tft.setTextSize(6);
  tft.print((String)convertStepsToDistance(mStepsC, mStepsPerC, mUStepC) + "mm");

  //Vertical Chassis Position Title
  tft.setCursor(xPadding, 78);
  tft.setTextColor(YELLOW);
  tft.setTextSize(6);
  tft.print("Blade Position: ");

  //Vertical Chassis Position Text
  tft.setCursor(160, 78);
  tft.setTextColor(CYAN);
  tft.setTextSize(6);
  tft.print((String)convertStepsToDistance(mStepsA, mStepsPerA, mUStepA) + "mm");
  
  /*******************/

  /***Touch Update***/
  /*
  TSPoint p = ts.getPoint();

  if(p.z > ts.pressureThreshhold) {
    //Load Configuration Button
    if(p.x > load_Btn_Start_x && p.x < load_Btn_Stop_x && p.y > load_Btn_Start_y && p.y < load_Btn_Stop_y) {
      
    }
    
    //New Configuration Button
    if(p.x > new_Btn_Start_x && p.x < new_Btn_Stop_x && p.y > new_Btn_Start_y && p.y < new_Btn_Stop_y) {        
      
    }
    
    //Edit Configuration Button
    if(p.x > edit_Btn_Start_x && p.x < edit_Btn_Stop_x && p.y > edit_Btn_Start_y && p.y < edit_Btn_Stop_y) {    
      
    }

    //Machine Settings Button
    if(p.x > set_Btn_Start_x && p.x < set_Btn_Stop_x && p.y > set_Btn_Start_y && p.y < set_Btn_Stop_y) {        
      
    }

    //Top Clamp Button
    if(p.x > top_clamp_Btn_Start_x && p.x < top_clamp_Btn_Stop_x && p.y > top_clamp_Btn_Start_y && p.y < top_clamp_Btn_Stop_y) {        
      
    }

    //Bottom Clamp Button
    if(p.x > bot_clamp_Btn_Start_x && p.x < bot_clamp_Btn_Stop_x && p.y > bot_clamp_Btn_Start_y && p.y < bot_clamp_Btn_Stop_y) {        
      
    }

    //Home Axis' Button
    if(p.x > home_Btn_Start_x && p.x < home_Btn_Stop_x && p.y > home_Btn_Start_y && p.y < home_Btn_Stop_y) {        
      
    }

    //Previous Step Button
    if(p.x > prev_Btn_Start_x && p.x < prev_Btn_Stop_x && p.y > prev_Btn_Start_y && p.y < prev_Btn_Stop_y) {        
      
    }

    //Next Step Button
    if(p.x > next_Btn_Start_x && p.x < next_Btn_Stop_x && p.y > next_Btn_Start_y && p.y < next_Btn_Stop_y) {        
      
    }

    //Motor Enable/Disable Button
    if(p.x > mEN_Btn_Start_x && p.x < mEN_Btn_Stop_x && p.y > mEN_Btn_Start_y && p.y < mEN_Btn_Stop_y) {            
      enM = !enM;               //Toggle on-off variable
      digitalWrite(ENM, !enM);   //Un-invert the vcc logic (on != high, on == low --> on = high)
    }
  }
  delay(100);   //Debounce delay
  */
  /*****************/  
}

void displayProfileLoadScreen() {
  
}

void displayNewProfileScreen() {
  
}

void displayEditProfileScreen() {
  
}

void displayMachineSettingsScreen() {
  /*tft.fillRect(load_Btn_Start_x, load_Btn_Start_y, configBtnX, configBtnY, WHITE);
  tft.drawRect(load_Btn_Start_x, load_Btn_Start_y, configBtnX, configBtnY, GRAY);
  tft.setCursor(load_Btn_Start_x, load_Btn_Start_y);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.print("Load Config");
*/
}

void displayVelAccelJerkScreen() {
  
}

void displayMicrostepScreen() {

}


void displayStepsMMScreen() {
  
}

/**Move the specified motor to a specified location 
   TODO: get steps to target point 
         add deceleration**/
void moveTo(char motor, float mmPos) {
  if(motor == 'A') {
    int toSteps = (mmPos * mStepsPerA) - mStepsA;   //Get the distance needing to be closed from current position in step number
    long startTime = millis();                      //Record when move began
    long prevPulseTime = startTime;                 //Initialize the previous pulse timer

    int i = 0;
    while(i < toSteps) {      //While there are still steps to be made
      long curTime = millis();    //Update the current time
      
      mCurVelocityA = (mAccelA * (startTime - curTime)) + (mJerkA * (startTime - curTime)^2);         //Calculate the current velocity from elapsed time
      if(mCurVelocityA > mVelocityA) {                                                                //If the value would go beyond the max allowed
        mCurVelocityA = mVelocityA;                                                                   //Set it to the max allowed
      }
      
      long delayPeriod = 1000 / mCurVelocityA;
  
      if(wait(delayPeriod, prevPulseTime)) {
        prevPulseTime = millis();
        i++;
        doStep('A');
      }
    }
  }
  else if(motor == 'B') {
    int toSteps = (mmPos * mStepsPerB) - mStepsB;   //Get the distance needing to be closed from current position in step number
    long startTime = millis();                      //Record when move began
    long prevPulseTime = startTime;                 //Initialize the previous pulse timer

    int i = 0;
    while(i < toSteps) {      //While there are still steps to be made
      long curTime = millis();    //Update the current time
      
      float mCurVelocityB = (mAccelB * (startTime - curTime)) + (mJerkB * (startTime - curTime)^2);   //Calculate the current velocity from elapsed time
      if(mCurVelocityB > mVelocityB) {                                                                //If the value would go beyond the max allowed
        mCurVelocityB = mVelocityB;                                                                   //Set it to the max allowed
      }
      
      long delayPeriod = 1000 / mCurVelocityB;
  
      if(wait(delayPeriod, prevPulseTime)) {
        prevPulseTime = millis();
        i++;
        doStep('B');
      }
    }
  }
  else if(motor == 'C') {
    int toSteps = (mmPos * mStepsPerC) - mStepsC;   //Get the distance needing to be closed from current position in step number
    long startTime = millis();                      //Record when move began
    long prevPulseTime = startTime;                 //Initialize the previous pulse timer

    int i = 0;
    while(i < toSteps) {      //While there are still steps to be made
      long curTime = millis();    //Update the current time
      
      float mCurVelocityC = (mAccelC * (startTime - curTime)) + (mJerkC * (startTime - curTime)^2);   //Calculate the current velocity from elapsed time
      if(mCurVelocityC > mVelocityC) {                                                                //If the value would go beyond the max allowed
        mCurVelocityC = mVelocityC;                                                                   //Set it to the max allowed
      }
      
      long delayPeriod = 1000 / mCurVelocityC;
  
      if(wait(delayPeriod, prevPulseTime)) {
        prevPulseTime = millis();
        i++;
        doStep('C');
      }
    }
  }
  else if(motor == 'D') {
    int toSteps = (mmPos * mStepsPerD) - mStepsD;   //Get the distance needing to be closed from current position in step number
    long startTime = millis();                      //Record when move began
    long prevPulseTime = startTime;                 //Initialize the previous pulse timer

    int i = 0;
    while(i < toSteps) {      //While there are still steps to be made
      long curTime = millis();    //Update the current time
      
      float mCurVelocityD = (mAccelD * (startTime - curTime)) + (mJerkD * (startTime - curTime)^2);   //Calculate the current velocity from elapsed time
      if(mCurVelocityD > mVelocityD) {                                                                //If the value would go beyond the max allowed
        mCurVelocityD = mVelocityD;                                                                   //Set it to the max allowed
      }
      
      long delayPeriod = 1000 / mCurVelocityD;
  
      if(wait(delayPeriod, prevPulseTime)) {
        prevPulseTime = millis();
        i++;
        doStep('D');
      }
    }
  }
}

/**Move the specified motor by one step**/
void doStep(char motor) {
  if(motor == 'A') {
    if(mDirA == 0) {
      mStepsA++;
    }
    else if(mDirA == 1) {
      mStepsA--;
    }
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
  else if(motor == 'D') {
    if(mDirD == 0) {
      mStepsD++;
    }
    else if(mDirD == 1) {
      mStepsD--;
    }
    digitalWrite(pulseD, HIGH);
    delayMicroseconds(25);
    digitalWrite(pulseD, LOW);
    delayMicroseconds(25);
  }
  else {
    
  }
}

/**Turn on the specified motor**/
void enableMotors() {
    enM = true;
    digitalWrite(ENABLE_MOTORS, LOW);
}

/**Turn off the secified motor**/
void disableMotors(char motor) {
    enM = false;
    digitalWrite(ENABLE_MOTORS, HIGH);
}

/**Change the microstepping of the specified motor (A, B, C, D)**/
void setMicroStepping(char motor, int uStep) {
  if(motor == 'A') {
    mUStepA = uStep;
    
    switch(uStep) {
      case 0:                       //Full Step
        digitalWrite(MS1A, LOW);
        digitalWrite(MS2A, LOW);
        digitalWrite(MS3A, LOW);
        break;
      case 1:                       //1/2 Step
        digitalWrite(MS1A, HIGH);
        digitalWrite(MS2A, LOW);
        digitalWrite(MS3A, LOW);
        break;
      case 2:                       //1/4 Step
        digitalWrite(MS1A, LOW);
        digitalWrite(MS2A, HIGH);
        digitalWrite(MS3A, LOW);
        break;
      case 3:                       //1/8 Step
        digitalWrite(MS1A, HIGH);
        digitalWrite(MS2A, HIGH);
        digitalWrite(MS3A, LOW);
        break;
      case 4:                       //1/16 Step
        digitalWrite(MS1A, HIGH);
        digitalWrite(MS2A, HIGH);
        digitalWrite(MS3A, HIGH);
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
  else if(motor == 'D') {
    mUStepD = uStep;

    switch(uStep) {
      case 0:                       //Full Step
        digitalWrite(MS1D, LOW);
        digitalWrite(MS2D, LOW);
        digitalWrite(MS3D, LOW);
        break;
      case 1:                       //1/2 Step
        digitalWrite(MS1D, HIGH);
        digitalWrite(MS2D, LOW);
        digitalWrite(MS3D, LOW);
        break;
      case 2:                       //1/4 Step
        digitalWrite(MS1D, LOW);
        digitalWrite(MS2D, HIGH);
        digitalWrite(MS3D, LOW);
        break;
      case 3:                       //1/8 Step
        digitalWrite(MS1D, HIGH);
        digitalWrite(MS2D, HIGH);
        digitalWrite(MS3D, LOW);
        break;
      case 4:                       //1/16 Step
        digitalWrite(MS1D, HIGH);
        digitalWrite(MS2D, HIGH);
        digitalWrite(MS3D, HIGH);
        break;
    }
  }
  else {
    
  }
}

/**Sets the direction of the specified motor, inverts it if defined**/
void setMotorDirection(char motor, boolean dir) {
  if(motor == 'A') {
    if(mInvDirA) {
      dir = !dir;
    }
    mDirA = dir;
    digitalWrite(dirA, dir);
  }
  else if(motor == 'B') {
    if(mInvDirB) {
      dir = !dir;
    }
    mDirB = dir;
    digitalWrite(dirB, dir);
  }
  else if(motor == 'C') {
    if(mInvDirC) {
      dir = !dir;
    }
    mDirC = dir;
    digitalWrite(dirC, dir);
  }
  else if(motor == 'D') {
    if(mInvDirD) {
      dir = !dir;
    }
    mDirD = dir;
    digitalWrite(dirD, dir);
  }
  else {
    
  }
}

/**Calculate a distance(mm) from the step count**/
float convertStepsToDistance(int mSteps, float mStepsPer, int mUStep) {
  //float distance = (mSteps / mStepsPer) * ;
  //return distance;
}

/**Wait for elapsed time without suspending program**/
boolean wait(int waitTime, long timeStarted) {
  if(millis() - timeStarted >= waitTime) {
    return true;
  }
  else {
    return false;
  }
}
