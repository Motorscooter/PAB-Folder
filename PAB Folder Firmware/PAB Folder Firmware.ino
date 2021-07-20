/*****LIBRARIES*****/
#include <LCDWIKI_GUI.h> //LCD core graphics library
#include <LCDWIKI_KBV.h> //LCD hardware-specific library
#include <TouchScreen.h> //LCD touch library
#include <EEPROM.h>      //Memory storage library
/******************/

/*****DEFINITIONS*****/
//LCD Pin Connections
#define LCD_RESET A4
#define LCD_CS    A3 
#define LCD_CD    A2
#define LCD_WR    A1  
#define LCD_RD    A0
   
//Touch Screen Connections
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

#define MINPRESSURE 10
#define MAXPRESSURE 1000

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
LCDWIKI_KBV tft(ILI9486,LCD_CD,LCD_CD,LCD_WR,LCD_RD,LCD_RESET); //model,cs,cd,wr,rd,reset

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//Touch parameter calibration
int TS_LEFT = 70;
int TS_RIGHT = 940;
int TS_TOP = 130;
int TS_BOT = 900;

/////FIRMWARE VARIABLES
boolean loadBtnFlag = false;
boolean newBtnFlag = false;
boolean editBtnFlag = false;
boolean setBtnFlag = false;
boolean tClampBtnFlag = false;
boolean bClampBtnFlag = false;
boolean homeBtnFlag = false;
boolean prevBtnFlag = false;
boolean nextBtnFlag = false;
boolean togMotBtnFlag = false;
boolean upTBtnFlag = false;
boolean downTBtnFlag = false;
boolean upBBtnFlag = false;
boolean downBBtnFlag = false;
boolean inBtnFlag = false;
boolean outBtnFlag = false;
boolean amountBtnFlag = false;

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
int mStepsA;
int mStepsB;
int mStepsC;

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
String profile = "";
int tClampDist = 0;
int bClampDist = 0;
int hDist = 0;

/////GUI Button Sizes
int xPadding = 5;
int yPadding = 5;

int configBtnX = 137;
int configBtnY = 40;

int settingBtnX = 40;
int settingBtnY = 40;

int clampBtnX = 105;
int clampBtnY = 120;

int homeBtnX = 40;
int homeBtnY = 40;

int stepBtnX = 40;
int stepBtnY = 40;

int mEnBtnX = 100;
int mEnBtnY = 245;

int manualBtnX = 40;
int manualBtnY = 40;

float moveAmount = 0.1;
  
/////GUI Relations
int load_Btn_Start_x = xPadding;
int load_Btn_Start_y = yPadding;
int load_Btn_Stop_x = load_Btn_Start_x + configBtnX;
int load_Btn_Stop_y = load_Btn_Start_y + configBtnY;  

int new_Btn_Start_x = (xPadding*2) + (configBtnX);
int new_Btn_Start_y = yPadding;
int new_Btn_Stop_x = new_Btn_Start_x + configBtnX;
int new_Btn_Stop_y = new_Btn_Start_y + configBtnY;

int edit_Btn_Start_x = (xPadding*3) + (configBtnX*2);
int edit_Btn_Start_y = yPadding;
int edit_Btn_Stop_x = edit_Btn_Start_x + configBtnX;
int edit_Btn_Stop_y = edit_Btn_Start_y + configBtnY;

int set_Btn_Start_x = (xPadding*4) + (configBtnX*3);
int set_Btn_Start_y = yPadding;
int set_Btn_Stop_x = set_Btn_Start_x + settingBtnX;
int set_Btn_Stop_y = set_Btn_Start_y + settingBtnY;

int top_clamp_Btn_Start_x = 365;
int top_clamp_Btn_Start_y = (yPadding*3) + settingBtnY + 15;
int top_clamp_Btn_Stop_x = top_clamp_Btn_Start_x + clampBtnX;
int top_clamp_Btn_Stop_y =top_clamp_Btn_Start_y + clampBtnY;

int bot_clamp_Btn_Start_x = 365;
int bot_clamp_Btn_Start_y = (yPadding*4) + settingBtnY + clampBtnY + 15;
int bot_clamp_Btn_Stop_x = bot_clamp_Btn_Start_x + clampBtnX;
int bot_clamp_Btn_Stop_y = bot_clamp_Btn_Start_y + clampBtnY;

int home_Btn_Start_x = xPadding;
int home_Btn_Start_y = bot_clamp_Btn_Stop_y - homeBtnY;
int home_Btn_Stop_x = home_Btn_Start_x + homeBtnX;
int home_Btn_Stop_y = home_Btn_Start_y + homeBtnY;

int prev_Btn_Start_x = (xPadding*2) + homeBtnX + 25;
int prev_Btn_Start_y = bot_clamp_Btn_Stop_y - stepBtnY;
int prev_Btn_Stop_x = prev_Btn_Start_x + stepBtnX;
int prev_Btn_Stop_y = prev_Btn_Start_y + stepBtnY;

int next_Btn_Start_x = 355 - mEnBtnX - stepBtnX;
int next_Btn_Start_y = bot_clamp_Btn_Stop_y - stepBtnY;
int next_Btn_Stop_x = next_Btn_Start_x + stepBtnX;
int next_Btn_Stop_y = next_Btn_Start_y + stepBtnY;

int mEN_Btn_Start_x = 360 - mEnBtnX;
int mEN_Btn_Start_y = (yPadding*3) + settingBtnY + 15;
int mEN_Btn_Stop_x = mEN_Btn_Start_x + mEnBtnX;
int mEN_Btn_Stop_y = mEN_Btn_Start_y + mEnBtnY;

int upT_Btn_Start_x = (xPadding * 3) + manualBtnX;
int upT_Btn_Start_y = (yPadding*8) + settingBtnY + 100;
int upT_Btn_Stop_x = upT_Btn_Start_x + manualBtnX;
int upT_Btn_Stop_y = upT_Btn_Start_y + manualBtnY;

int downT_Btn_Start_x = upT_Btn_Start_x;
int downT_Btn_Start_y = upT_Btn_Stop_y + yPadding;
int downT_Btn_Stop_x = downT_Btn_Start_x + manualBtnX;
int downT_Btn_Stop_y = downT_Btn_Start_y + manualBtnY;

int upB_Btn_Start_x = (xPadding * 4) + (manualBtnX * 2);
int upB_Btn_Start_y = (yPadding*8) + settingBtnY + 100;
int upB_Btn_Stop_x = upB_Btn_Start_x + manualBtnX;
int upB_Btn_Stop_y = upB_Btn_Start_y + manualBtnY;

int downB_Btn_Start_x = upB_Btn_Start_x;
int downB_Btn_Start_y = upB_Btn_Stop_y + yPadding;
int downB_Btn_Stop_x = downB_Btn_Start_x + manualBtnX;
int downB_Btn_Stop_y = downB_Btn_Start_y + manualBtnY;

int in_Btn_Start_x = upT_Btn_Start_x - manualBtnX - xPadding;
int in_Btn_Start_y = upT_Btn_Start_y + (manualBtnY/2) + yPadding;
int in_Btn_Stop_x = in_Btn_Start_x + manualBtnX;
int in_Btn_Stop_y = in_Btn_Start_y + manualBtnY;

int out_Btn_Start_x = upB_Btn_Stop_x + xPadding;
int out_Btn_Start_y = upB_Btn_Start_y + (manualBtnY/2) + yPadding;
int out_Btn_Stop_x = out_Btn_Start_x + manualBtnX;
int out_Btn_Stop_y = out_Btn_Start_y + manualBtnY;

int amount_Btn_Start_x = mEN_Btn_Start_x - (xPadding * 2) - manualBtnX;
int amount_Btn_Start_y = out_Btn_Start_y;
int amount_Btn_Stop_x = amount_Btn_Start_x + manualBtnX;
int amount_Btn_Stop_y = amount_Btn_Start_y + manualBtnY;

/*******************/

void setup() {
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  /***Screen Setup***/
  tft.Init_LCD();
  tft.Set_Rotation(1);
  tft.Fill_Screen(BLACK);    //Change the screen to solid white
  tft.Set_Text_Mode(0);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(4);
  tft.Print_String("Autoliv", 160, 230);
  delay(500);
  drawMainScreen();
  /***************/
  
  /***Motor Setup***/  
  setMicroStepping('A', 4);
  setMicroStepping('B', 4);
  setMicroStepping('C', 4);
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
void drawMainScreen() { 
  /***Screen Update***/
  tft.Fill_Screen(BLACK);    //Change the screen to solid black

  //Load Configuration Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(load_Btn_Start_x, load_Btn_Start_y, load_Btn_Stop_x, load_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(load_Btn_Start_x, load_Btn_Start_y, load_Btn_Stop_x, load_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Load Config", load_Btn_Start_x + 35, load_Btn_Start_y + 17);
  
  //New Configuration Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(new_Btn_Start_x, new_Btn_Start_y, new_Btn_Stop_x, new_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(new_Btn_Start_x, new_Btn_Start_y, new_Btn_Stop_x, new_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("New Config", new_Btn_Start_x + 37, new_Btn_Start_y + 17);

  //Edit Configuration Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(edit_Btn_Start_x, edit_Btn_Start_y, edit_Btn_Stop_x, edit_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(edit_Btn_Start_x, edit_Btn_Start_y, edit_Btn_Stop_x, edit_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Edit Config", edit_Btn_Start_x + 35, edit_Btn_Start_y + 17);

  //Machine Settings Button
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
  tft.Print_String("Toggle Clamps", 380, (yPadding*2) + settingBtnY + 5);

  //Clamp Toggle Button Top
  tft.Set_Draw_color(RED);
  tft.Fill_Rectangle(top_clamp_Btn_Start_x, top_clamp_Btn_Start_y, top_clamp_Btn_Stop_x, top_clamp_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(top_clamp_Btn_Start_x, top_clamp_Btn_Start_y, top_clamp_Btn_Stop_x, top_clamp_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(RED);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Top", top_clamp_Btn_Start_x + 33, top_clamp_Btn_Start_y + 40);
  tft.Set_Text_Size(1);
  tft.Print_String("(DISENGAGED)", top_clamp_Btn_Start_x + 15, top_clamp_Btn_Start_y + 65);

  //Clamp Toggle Button Bottom
  tft.Set_Draw_color(RED);
  tft.Fill_Rectangle(bot_clamp_Btn_Start_x, bot_clamp_Btn_Start_y, bot_clamp_Btn_Stop_x, bot_clamp_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(bot_clamp_Btn_Start_x, bot_clamp_Btn_Start_y, bot_clamp_Btn_Stop_x, bot_clamp_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(RED);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Bottom", bot_clamp_Btn_Start_x + 17, bot_clamp_Btn_Start_y + 40);
  tft.Set_Text_Size(1);
  tft.Print_String("(DISENGAGED)", bot_clamp_Btn_Start_x + 15, bot_clamp_Btn_Start_y + 65);

  //Home Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(home_Btn_Start_x, home_Btn_Start_y, home_Btn_Stop_x, home_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(home_Btn_Start_x, home_Btn_Start_y, home_Btn_Stop_x, home_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Home", home_Btn_Start_x + 9, home_Btn_Start_y + 17);

  //Prev Step Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(prev_Btn_Start_x, prev_Btn_Start_y, prev_Btn_Stop_x, prev_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(prev_Btn_Start_x, prev_Btn_Start_y, prev_Btn_Stop_x, prev_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Prev", prev_Btn_Start_x + 9, prev_Btn_Start_y + 17);

  //Step Value
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Step: 1/2", prev_Btn_Stop_x + 23, bot_clamp_Btn_Stop_y - homeBtnY + 17);

  //Next Step Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(next_Btn_Start_x, next_Btn_Start_y, next_Btn_Stop_x, next_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(next_Btn_Start_x, next_Btn_Start_y, next_Btn_Stop_x, next_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Next", next_Btn_Start_x + 9, next_Btn_Start_y + 17);

  //Motor Toggle Button Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Toggle Motors", 275, (yPadding*2) + settingBtnY + 5);

  //Motor Enable Toggle Button
  tft.Set_Draw_color(GREEN);
  tft.Fill_Rectangle(mEN_Btn_Start_x, mEN_Btn_Start_y, mEN_Btn_Stop_x, mEN_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mEN_Btn_Start_x, mEN_Btn_Start_y, mEN_Btn_Stop_x, mEN_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(GREEN);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Motors", mEN_Btn_Start_x + 15, mEN_Btn_Start_y + 100);
  tft.Set_Text_Size(1);
  tft.Print_String("(ENABLED)", mEN_Btn_Start_x + 24, mEN_Btn_Start_y + 125);

  //Toggle Field Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(mEN_Btn_Start_x, (yPadding*2) + configBtnY, top_clamp_Btn_Stop_x, mEN_Btn_Start_y - yPadding);
  
  //Data Field Border
  tft.Set_Draw_color(CYAN);
  tft.Draw_Rectangle(xPadding, (yPadding*2) + configBtnY, mEN_Btn_Start_x - xPadding, (yPadding*8) + settingBtnY + 75);

  //Manual Control Field Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, (yPadding*10) + settingBtnY + 70, mEN_Btn_Start_x - xPadding, home_Btn_Start_y - yPadding);

  //Manual Control Title
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Set_Text_Size(1);
  tft.Print_String("Manual Control", (xPadding * 2) + 80, (yPadding*10) + settingBtnY + 75);

  //Up Button top clamp
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(upT_Btn_Start_x, upT_Btn_Start_y, upT_Btn_Stop_x, upT_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(upT_Btn_Start_x, upT_Btn_Start_y, upT_Btn_Stop_x, upT_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("T_UP", upT_Btn_Start_x + 10, upT_Btn_Start_y + 15);

  //Down Button top clamp
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(downT_Btn_Start_x, downT_Btn_Start_y, downT_Btn_Stop_x, downT_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(downT_Btn_Start_x, downT_Btn_Start_y, downT_Btn_Stop_x, downT_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("T_DWN", downT_Btn_Start_x + 7, downT_Btn_Start_y + 15);

  //Up Button bottom clamp
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(upB_Btn_Start_x, upB_Btn_Start_y, upB_Btn_Stop_x, upB_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(upB_Btn_Start_x, upB_Btn_Start_y, upB_Btn_Stop_x, upB_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("B_UP", upB_Btn_Start_x + 10, upB_Btn_Start_y + 15);

  //Down Button bottom clamp
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(downB_Btn_Start_x, downB_Btn_Start_y, downB_Btn_Stop_x, downB_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(downB_Btn_Start_x, downB_Btn_Start_y, downB_Btn_Stop_x, downB_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("B_DWN", downB_Btn_Start_x + 7, downB_Btn_Start_y + 15);
  
  //Inward Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(in_Btn_Start_x, in_Btn_Start_y, in_Btn_Stop_x, in_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(in_Btn_Start_x, in_Btn_Start_y, in_Btn_Stop_x, in_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("IN", in_Btn_Start_x + 15, in_Btn_Start_y + 15);

  //Outword Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(out_Btn_Start_x, out_Btn_Start_y, out_Btn_Stop_x, out_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(out_Btn_Start_x, out_Btn_Start_y, out_Btn_Stop_x, out_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("OUT", out_Btn_Start_x + 12, out_Btn_Start_y + 15);

  //Move Distance Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(amount_Btn_Start_x, amount_Btn_Start_y, amount_Btn_Stop_x, amount_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(amount_Btn_Start_x, amount_Btn_Start_y, amount_Btn_Stop_x, amount_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1mm", amount_Btn_Start_x + 7, amount_Btn_Start_y + 15);
  
  //Configuration Profile Title
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(GREEN);
  tft.Set_Text_Size(2);
  tft.Print_String("Configuration: ", xPadding * 2, (yPadding*3) + settingBtnY);

  //Configuration Profile Text
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(BLUE);
  tft.Set_Text_Size(2);
  tft.Print_String("780B", xPadding + 180, (yPadding*3) + settingBtnY);

  //Top Clamp Position Title
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String("Top Clamp Position: ", xPadding * 2, (yPadding*4) + settingBtnY + 25);

  //Top Clamp Position Text
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String((String)convertStepsToDistance(mStepsB, mStepsPerB, mUStepB) + "mm", xPadding + 180, (yPadding*4) + settingBtnY + 25);

  //Bottom Clamp Position Title
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String("Bottom Clamp Position: ", xPadding * 2, (yPadding*5) + settingBtnY + 50);
 
  //Bottom Clamp Position Text
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String((String)convertStepsToDistance(mStepsC, mStepsPerC, mUStepC) + "mm", xPadding + 180, (yPadding*5) + settingBtnY + 50);

  //Vertical Chassis Position Title
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String("Blade Position: ", xPadding * 2, (yPadding*6) + settingBtnY + 75);

  //Vertical Chassis Position Text
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String((String)convertStepsToDistance(mStepsA, mStepsPerA, mUStepA) + "mm", xPadding + 180, (yPadding*6) + settingBtnY + 75);
}

/**Get touch readings and redraw changes**/
void updateMainScreen() {
  /***Touch Update***/
  TSPoint p = ts.getPoint();  //Leaves pins in input mode. Shared by lcd so must revert to output after reading.
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int x = map(p.x, TS_LEFT, TS_RIGHT, 0, 480);
  int y = map(p.y, TS_TOP, TS_BOT, 0, 320);

//Load Configuration Button
  if(x > load_Btn_Start_x && x < load_Btn_Stop_x && y > load_Btn_Start_y && y < load_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(load_Btn_Start_x, load_Btn_Start_y, load_Btn_Stop_x, load_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(load_Btn_Start_x, load_Btn_Start_y, load_Btn_Stop_x, load_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Load Config", load_Btn_Start_x + 35, load_Btn_Start_y + 17);
    
    //Flag the load button as pressed
    loadBtnFlag = true;
    delay(50);
    //TODO: Open the load profile screen
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(loadBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(load_Btn_Start_x, load_Btn_Start_y, load_Btn_Stop_x, load_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(load_Btn_Start_x, load_Btn_Start_y, load_Btn_Stop_x, load_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Load Config", load_Btn_Start_x + 35, load_Btn_Start_y + 17);

    //Reset the load button flag to unpressed
    loadBtnFlag = false;
  }
  
//New Configuration Button
  if(x > new_Btn_Start_x && x < new_Btn_Stop_x && y > new_Btn_Start_y && y < new_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {        
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(new_Btn_Start_x, new_Btn_Start_y, new_Btn_Stop_x, new_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(new_Btn_Start_x, new_Btn_Start_y, new_Btn_Stop_x, new_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("New Config", new_Btn_Start_x + 37, new_Btn_Start_y + 17);

    newBtnFlag = true;
    delay(50);
    //TODO: Open new configuration screen
  }
  else if(newBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(new_Btn_Start_x, new_Btn_Start_y, new_Btn_Stop_x, new_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(new_Btn_Start_x, new_Btn_Start_y, new_Btn_Stop_x, new_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("New Config", new_Btn_Start_x + 37, new_Btn_Start_y + 17);

    newBtnFlag = false;
  }
//Edit Configuration Button
  if(x > edit_Btn_Start_x && x < edit_Btn_Stop_x && y > edit_Btn_Start_y && y < edit_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {    
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(edit_Btn_Start_x, edit_Btn_Start_y, edit_Btn_Stop_x, edit_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(edit_Btn_Start_x, edit_Btn_Start_y, edit_Btn_Stop_x, edit_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Edit Config", edit_Btn_Start_x + 35, edit_Btn_Start_y + 17);

    editBtnFlag = true;
    delay(50);
    //TODO: Open configuration edit screen
  }
  else if(editBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(edit_Btn_Start_x, edit_Btn_Start_y, edit_Btn_Stop_x, edit_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(edit_Btn_Start_x, edit_Btn_Start_y, edit_Btn_Stop_x, edit_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Edit Config", edit_Btn_Start_x + 35, edit_Btn_Start_y + 17);

    editBtnFlag = false;
  }

//Machine Settings Button
  if(x > set_Btn_Start_x && x < set_Btn_Stop_x && y > set_Btn_Start_y && y < set_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {        
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(set_Btn_Start_x, set_Btn_Start_y, set_Btn_Stop_x, set_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(set_Btn_Start_y, set_Btn_Start_y, set_Btn_Stop_x, set_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Set", set_Btn_Start_x+ 11, set_Btn_Start_y + 17);

    setBtnFlag = true;
    delay(50);
    //TODO: Open machine settings screen
  }
  else if(setBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(set_Btn_Start_x, set_Btn_Start_y, set_Btn_Stop_x, set_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(set_Btn_Start_y, set_Btn_Start_y, set_Btn_Stop_x, set_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Set", set_Btn_Start_x+ 11, set_Btn_Start_y + 17);

    setBtnFlag = false;
  }

//Top Clamp Button
  if(x > top_clamp_Btn_Start_x && x < top_clamp_Btn_Stop_x && y > top_clamp_Btn_Start_y && y < top_clamp_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    tClampBtnFlag = !tClampBtnFlag;

    if(tClampBtnFlag) {
      tft.Set_Draw_color(GREEN);
      tft.Fill_Rectangle(top_clamp_Btn_Start_x, top_clamp_Btn_Start_y, top_clamp_Btn_Stop_x, top_clamp_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(top_clamp_Btn_Start_x, top_clamp_Btn_Start_y, top_clamp_Btn_Stop_x, top_clamp_Btn_Stop_y);
      tft.Set_Text_Size(2);
      tft.Set_Text_Back_colour(GREEN);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("Top", top_clamp_Btn_Start_x + 33, top_clamp_Btn_Start_y + 40);
      tft.Set_Text_Size(1);
      tft.Print_String("(ENGAGED)", top_clamp_Btn_Start_x + 24, top_clamp_Btn_Start_y + 65);

      tClampEngaged = true;
      delay(50);
      //TODO: Engange top clamp
    }
    else {      
      tft.Set_Draw_color(RED);
      tft.Fill_Rectangle(top_clamp_Btn_Start_x, top_clamp_Btn_Start_y, top_clamp_Btn_Stop_x, top_clamp_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(top_clamp_Btn_Start_x, top_clamp_Btn_Start_y, top_clamp_Btn_Stop_x, top_clamp_Btn_Stop_y);
      tft.Set_Text_Size(2);
      tft.Set_Text_Back_colour(RED);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("Top", top_clamp_Btn_Start_x + 33, top_clamp_Btn_Start_y + 40);
      tft.Set_Text_Size(1);
      tft.Print_String("(DISENGAGED)", top_clamp_Btn_Start_x + 15, top_clamp_Btn_Start_y + 65);

      tClampEngaged = false;
      //TODO: Disengage top clamp
    }
  }

//Bottom Clamp Button
  if(x > bot_clamp_Btn_Start_x && x < bot_clamp_Btn_Stop_x && y > bot_clamp_Btn_Start_y && y < bot_clamp_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {    
    bClampBtnFlag = !bClampBtnFlag;
    
    if(bClampBtnFlag) {    
      tft.Set_Draw_color(GREEN);
      tft.Fill_Rectangle(bot_clamp_Btn_Start_x, bot_clamp_Btn_Start_y, bot_clamp_Btn_Stop_x, bot_clamp_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(bot_clamp_Btn_Start_x, bot_clamp_Btn_Start_y, bot_clamp_Btn_Stop_x, bot_clamp_Btn_Stop_y);
      tft.Set_Text_Size(2);
      tft.Set_Text_Back_colour(GREEN);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("Bottom", bot_clamp_Btn_Start_x + 17, bot_clamp_Btn_Start_y + 40);
      tft.Set_Text_Size(1);
      tft.Print_String("(ENGAGED)", bot_clamp_Btn_Start_x + 24, bot_clamp_Btn_Start_y + 65);

      tClampEngaged = true;
      delay(50);
      //TODO: Engage bottom clamp
    }
    else {
      tft.Set_Draw_color(RED);
      tft.Fill_Rectangle(bot_clamp_Btn_Start_x, bot_clamp_Btn_Start_y, bot_clamp_Btn_Stop_x, bot_clamp_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(bot_clamp_Btn_Start_x, bot_clamp_Btn_Start_y, bot_clamp_Btn_Stop_x, bot_clamp_Btn_Stop_y);
      tft.Set_Text_Size(2);
      tft.Set_Text_Back_colour(RED);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("Bottom", bot_clamp_Btn_Start_x + 17, bot_clamp_Btn_Start_y + 40);
      tft.Set_Text_Size(1);
      tft.Print_String("(DISENGAGED)", bot_clamp_Btn_Start_x + 15, bot_clamp_Btn_Start_y + 65);

      tClampEngaged = false;
      //TODO: Disengage bottom clamp
    }
  }

//Home Axis' Button
  if(x > home_Btn_Start_x && x < home_Btn_Stop_x && y > home_Btn_Start_y && y < home_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {        
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(home_Btn_Start_x, home_Btn_Start_y, home_Btn_Stop_x, home_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(home_Btn_Start_x, home_Btn_Start_y, home_Btn_Stop_x, home_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Home", home_Btn_Start_x + 9, home_Btn_Start_y + 17);

    homeBtnFlag = true;
    delay(50);
    //TODO: Home all axis'
  }
  else if(homeBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(home_Btn_Start_x, home_Btn_Start_y, home_Btn_Stop_x, home_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(home_Btn_Start_x, home_Btn_Start_y, home_Btn_Stop_x, home_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Home", home_Btn_Start_x + 9, home_Btn_Start_y + 17);

    homeBtnFlag = false;
  }

//Previous Step Button
  if(x > prev_Btn_Start_x && x < prev_Btn_Stop_x && y > prev_Btn_Start_y && y < prev_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {        
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(prev_Btn_Start_x, prev_Btn_Start_y, prev_Btn_Stop_x, prev_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(prev_Btn_Start_x, prev_Btn_Start_y, prev_Btn_Stop_x, prev_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Prev", prev_Btn_Start_x + 9, prev_Btn_Start_y + 17);

    prevBtnFlag = true;
    delay(50);
    //TODO: Move carriages to previous step location
  }
  else if(prevBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(prev_Btn_Start_x, prev_Btn_Start_y, prev_Btn_Stop_x, prev_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(prev_Btn_Start_x, prev_Btn_Start_y, prev_Btn_Stop_x, prev_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Prev", prev_Btn_Start_x + 10, prev_Btn_Start_y + 17);

    prevBtnFlag = false;
  }

//Next Step Button
  if(x > next_Btn_Start_x && x < next_Btn_Stop_x && y > next_Btn_Start_y && y < next_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {        
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(next_Btn_Start_x, next_Btn_Start_y, next_Btn_Stop_x, next_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(next_Btn_Start_x, next_Btn_Start_y, next_Btn_Stop_x, next_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Next", next_Btn_Start_x + 9, next_Btn_Start_y + 17);

    nextBtnFlag = true;
    delay(50);
    //TODO: Move carriages to the next step location
  }
  else if(nextBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(next_Btn_Start_x, next_Btn_Start_y, next_Btn_Stop_x, next_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(next_Btn_Start_x, next_Btn_Start_y, next_Btn_Stop_x, next_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Next", next_Btn_Start_x + 9, next_Btn_Start_y + 17);

    nextBtnFlag = false;
  }

//Motor Enable/Disable Button
  if(x > mEN_Btn_Start_x && x < mEN_Btn_Stop_x && y > mEN_Btn_Start_y && y < mEN_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    togMotBtnFlag = !togMotBtnFlag;

    if(togMotBtnFlag) {
      tft.Set_Draw_color(GREEN);
      tft.Fill_Rectangle(mEN_Btn_Start_x, mEN_Btn_Start_y, mEN_Btn_Stop_x, mEN_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(mEN_Btn_Start_x, mEN_Btn_Start_y, mEN_Btn_Stop_x, mEN_Btn_Stop_y);
      tft.Set_Text_Size(2);
      tft.Set_Text_Back_colour(GREEN);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("Motors", mEN_Btn_Start_x + 15, mEN_Btn_Start_y + 100);
      tft.Set_Text_Size(1);
      tft.Print_String("(ENABLED)", mEN_Btn_Start_x + 24, mEN_Btn_Start_y + 125);
      enableMotors();
      delay(50);
    }
    else {
      tft.Set_Draw_color(RED);
      tft.Fill_Rectangle(mEN_Btn_Start_x, mEN_Btn_Start_y, mEN_Btn_Stop_x, mEN_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(mEN_Btn_Start_x, mEN_Btn_Start_y, mEN_Btn_Stop_x, mEN_Btn_Stop_y);
      tft.Set_Text_Size(2);
      tft.Set_Text_Back_colour(RED);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("Motors", mEN_Btn_Start_x + 15, mEN_Btn_Start_y + 100);
      tft.Set_Text_Size(1);
      tft.Print_String("(DISABLED)", mEN_Btn_Start_x + 22, mEN_Btn_Start_y + 125);
      disableMotors();
      delay(50);      
    }
  }

//Up Button Top Clamp
  if(x > upT_Btn_Start_x && x < upT_Btn_Stop_x && y > upT_Btn_Start_y && y < upT_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(upT_Btn_Start_x, upT_Btn_Start_y, upT_Btn_Stop_x, upT_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(upT_Btn_Start_x, upT_Btn_Start_y, upT_Btn_Stop_x, upT_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("T_UP", upT_Btn_Start_x + 10, upT_Btn_Start_y + 16);

    upTBtnFlag = true;
    delay(50);
    //TODO: Move by specified amount upwards on specified motor
  }
  else if(upTBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(upT_Btn_Start_x, upT_Btn_Start_y, upT_Btn_Stop_x, upT_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(upT_Btn_Start_x, upT_Btn_Start_y, upT_Btn_Stop_x, upT_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("T_UP", upT_Btn_Start_x + 10, upT_Btn_Start_y + 16);

    upTBtnFlag = false;
  }

//Down Button Top Clamp
  if(x > downT_Btn_Start_x && x < downT_Btn_Stop_x && y > downT_Btn_Start_y && y < downT_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(downT_Btn_Start_x, downT_Btn_Start_y, downT_Btn_Stop_x, downT_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(downT_Btn_Start_x, downT_Btn_Start_y, downT_Btn_Stop_x, downT_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("T_DWN", downT_Btn_Start_x + 7, downT_Btn_Start_y + 16);

    downTBtnFlag = true;
    delay(50);
    //TODO: Move specified clamp down by specified amount
  }
  else if(downTBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(downT_Btn_Start_x, downT_Btn_Start_y, downT_Btn_Stop_x, downT_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(downT_Btn_Start_x, downT_Btn_Start_y, downT_Btn_Stop_x, downT_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("T_DWN", downT_Btn_Start_x + 7, downT_Btn_Start_y + 16);

    downTBtnFlag = false;
  }

  //Up Button Bottom Clamp
  if(x > upB_Btn_Start_x && x < upB_Btn_Stop_x && y > upB_Btn_Start_y && y < upB_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(upB_Btn_Start_x, upB_Btn_Start_y, upB_Btn_Stop_x, upB_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(upB_Btn_Start_x, upB_Btn_Start_y, upB_Btn_Stop_x, upB_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("B_UP", upB_Btn_Start_x + 10, upB_Btn_Start_y + 16);

    upBBtnFlag = true;
    delay(50);
    //TODO: Move by specified amount upwards on specified motor
  }
  else if(upBBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(upB_Btn_Start_x, upB_Btn_Start_y, upB_Btn_Stop_x, upB_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(upB_Btn_Start_x, upB_Btn_Start_y, upB_Btn_Stop_x, upB_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("B_UP", upB_Btn_Start_x + 10, upB_Btn_Start_y + 16);

    upBBtnFlag = false;
  }

//Down Button Bottom Clamp
  if(x > downB_Btn_Start_x && x < downB_Btn_Stop_x && y > downB_Btn_Start_y && y < downB_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(downB_Btn_Start_x, downB_Btn_Start_y, downB_Btn_Stop_x, downB_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(downB_Btn_Start_x, downB_Btn_Start_y, downB_Btn_Stop_x, downB_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("B_DWN", downB_Btn_Start_x + 7, downB_Btn_Start_y + 16);

    downBBtnFlag = true;
    delay(50);
    //TODO: Move specified clamp down by specified amount
  }
  else if(downBBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(downB_Btn_Start_x, downB_Btn_Start_y, downB_Btn_Stop_x, downB_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(downB_Btn_Start_x, downB_Btn_Start_y, downB_Btn_Stop_x, downB_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("B_DWN", downB_Btn_Start_x + 7, downB_Btn_Start_y + 16);

    downBBtnFlag = false;
  }

//In Button
  if(x > in_Btn_Start_x && x < in_Btn_Stop_x && y > in_Btn_Start_y && y < in_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(in_Btn_Start_x, in_Btn_Start_y, in_Btn_Stop_x, in_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(in_Btn_Start_x, in_Btn_Start_y, in_Btn_Stop_x, in_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("IN", in_Btn_Start_x + 15, in_Btn_Start_y + 16);

    inBtnFlag = true;
    delay(50);
    //TODO: Move top/bottom frame carriages by specified amount from center
  }
  else if(inBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(in_Btn_Start_x, in_Btn_Start_y, in_Btn_Stop_x, in_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(in_Btn_Start_x, in_Btn_Start_y, in_Btn_Stop_x, in_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("IN", in_Btn_Start_x + 15, in_Btn_Start_y + 16);

    inBtnFlag = false;
  }

//Out Button
  if(x > out_Btn_Start_x && x < out_Btn_Stop_x && y > out_Btn_Start_y && y < out_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(out_Btn_Start_x, out_Btn_Start_y, out_Btn_Stop_x, out_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(out_Btn_Start_x, out_Btn_Start_y, out_Btn_Stop_x, out_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("OUT", out_Btn_Start_x + 12, out_Btn_Start_y + 16);

    outBtnFlag = true;
    delay(50);
    //TODO: Move top/bottom frame carriage by specified amount outwards from center
  }
  else if(outBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(out_Btn_Start_x, out_Btn_Start_y, out_Btn_Stop_x, out_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(out_Btn_Start_x, out_Btn_Start_y, out_Btn_Stop_x, out_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("OUT", out_Btn_Start_x + 12, out_Btn_Start_y + 16);

    outBtnFlag = false;
  }

//Distance Amount Button
  if(x > amount_Btn_Start_x && x < amount_Btn_Stop_x && y > amount_Btn_Start_y && y < amount_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(amount_Btn_Start_x, amount_Btn_Start_y, amount_Btn_Stop_x, amount_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(amount_Btn_Start_x, amount_Btn_Start_y, amount_Btn_Stop_x, amount_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(moveAmount == 0.1) {
      moveAmount = 1;
      tft.Print_String("1mm", amount_Btn_Start_x + 10, amount_Btn_Start_y + 16);
    }
    else if(moveAmount == 1) {
      moveAmount = 10;
      tft.Print_String("10mm", amount_Btn_Start_x + 8, amount_Btn_Start_y + 16);
    }
    else if(moveAmount == 10) {
      moveAmount = 50;
      tft.Print_String("50mm", amount_Btn_Start_x + 8, amount_Btn_Start_y + 16);
    }
    else if(moveAmount == 50) {
      moveAmount = 0.1;
      tft.Print_String("0.1mm", amount_Btn_Start_x + 7, amount_Btn_Start_y + 16);
    }

    amountBtnFlag = true;
    delay(50);
  }
  else if(amountBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(amount_Btn_Start_x, amount_Btn_Start_y, amount_Btn_Stop_x, amount_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(amount_Btn_Start_x, amount_Btn_Start_y, amount_Btn_Stop_x, amount_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(moveAmount == 0.1) {
      tft.Print_String("0.1mm", amount_Btn_Start_x + 7, amount_Btn_Start_y + 16);
    }
    else if(moveAmount == 1) {
      tft.Print_String("1mm", amount_Btn_Start_x + 10, amount_Btn_Start_y + 16);
    }
    else if(moveAmount == 10) {
      tft.Print_String("10mm", amount_Btn_Start_x + 8, amount_Btn_Start_y + 16);
    }
    else if(moveAmount == 50) {
      tft.Print_String("50mm", amount_Btn_Start_x + 8, amount_Btn_Start_y + 16);
    }

    amountBtnFlag = false;
  }
}

void drawProfileLoadScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black
}

void updateProfileLoadScreen() {

}

void drawNewProfileScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black 
}

void updateNewProfileScreen() {

}

void drawEditProfileScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black  
}

void updateEditProfileScreen() {

}

void drawMachineSettingsScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black
}

void updateMachineSettingsScreen() {

}

void drawVelAccelJerkScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black  
}

void updateVelAccelJerkScreen() {

}

void drawMicrostepScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black
}

void updateMicrostepScreen() {

}

void drawStepsMMScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black  
}

void updateStepsMMScreen() {

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
    delayMicroseconds(5);
    digitalWrite(pulseA, LOW);
    delayMicroseconds(5);
  }
  else if(motor == 'B') {
    if(mDirB == 0) {
      mStepsB++;
    }
    else if(mDirB == 1) {
      mStepsB--;
    }
    digitalWrite(pulseB, HIGH);
    delayMicroseconds(5);
    digitalWrite(pulseB, LOW);
    delayMicroseconds(5);
  }
  else if(motor == 'C') {
    if(mDirC == 0) {
      mStepsC++;
    }
    else if(mDirC == 1) {
      mStepsC--;
    }
    digitalWrite(pulseC, HIGH);
    delayMicroseconds(5);
    digitalWrite(pulseC, LOW);
    delayMicroseconds(5);
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

/**Change the microstepping of the specified motor (A, B, C, D)**/
void setMicroStepping(char motor, int uStep) {
  if(motor == 'A') {
    mUStepA = uStep;
    
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

/** **/
void saveProfile(char profileID) {
  int eeAddr = 100;

}

/** **/
void loadProfile(char profileID) {
  int eeAddr = 100;

}

/**Saves the current values to the EEPROM **/
void saveMachineSettings() {
  int eeAddr = 0;

  //Store touch calibration data
  EEPROM.put(eeAddr, TS_LEFT);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, TS_RIGHT);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, TS_TOP);
  eeAddr += sizeof(int);
  EEPROM.put(eeAddr, TS_BOT);
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
  EEPROM.put(eeAddr, mInvDirD);
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
  EEPROM.get(eeAddr, TS_LEFT);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, TS_RIGHT);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, TS_TOP);
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, TS_BOT);
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
  EEPROM.get(eeAddr, mInvDirD);
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
