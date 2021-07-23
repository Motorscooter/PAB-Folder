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

long doubleTapPeriod = 300;

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
int tClampDist1 = 0;
int bClampDist1 = 0;
int hDist1 = 0;
int hDist2 = 0;
int hDist3 = 0;
  
/////GUI Relations
int activeScreen = 0; //0 = home screen
double inputVal = 0;
String profiles[10];

/////BUTTON STATUS VARIABLES
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

boolean pBackBtnFlag = false;
boolean pCfmBtnFlag = false;
boolean p1BtnFlag = false;
boolean p2BtnFlag = false;
boolean p3BtnFlag = false;
boolean p4BtnFlag = false;
boolean p5BtnFlag = false;
boolean p6BtnFlag = false;
boolean p7BtnFlag = false;
boolean p8BtnFlag = false;
boolean p9BtnFlag = false;
boolean p10BtnFlag = false;

boolean pnNameBtnFlag = false;
boolean pnTClampBtnFlag = false;
boolean pnBClampBtnFlag = false;
boolean pnFChBtnFlag = false;
boolean pnStp1BtnFlag = false;
boolean pnStp2BtnFlag = false;
boolean pnStp3BtnFlag = false;
boolean pnStp4BtnFlag = false;
boolean pnStp5BtnFlag = false;
boolean pnBckBtnFlag = false;
boolean pnSaveBtnFlag = false;


boolean delBtnFlag = false;
boolean lettBtnFlag = false;
boolean n1BtnFlag = false;
boolean n2BtnFlag = false;
boolean n3BtnFlag = false;
boolean n4BtnFlag = false;
boolean n5BtnFlag = false;
boolean n6BtnFlag = false;
boolean n7BtnFlag = false;
boolean n8BtnFlag = false;
boolean n9BtnFlag = false;
boolean backBtnFlag = false;
boolean decimalBtnFlag = false;
boolean okBtnFlag = false;

/*--------------Home Screen------------*/
//Button Sizes
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

int mStepBtnX = 40;
int mStepBtnY = 40;

int mEnBtnX = 100;
int mEnBtnY = 245;

int manualBtnX = 40;
int manualBtnY = 40;

float moveAmount = 0.1;

//Button Positioning
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
int prev_Btn_Start_y = bot_clamp_Btn_Stop_y - mStepBtnY;
int prev_Btn_Stop_x = prev_Btn_Start_x + mStepBtnX;
int prev_Btn_Stop_y = prev_Btn_Start_y + mStepBtnY;

int next_Btn_Start_x = 355 - mEnBtnX - mStepBtnX;
int next_Btn_Start_y = bot_clamp_Btn_Stop_y - mStepBtnY;
int next_Btn_Stop_x = next_Btn_Start_x + mStepBtnX;
int next_Btn_Stop_y = next_Btn_Start_y + mStepBtnY;

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
/*------------------------------*/

/*--------Profile Load Screen-----------*/
//Button Sizes
int backBtnX = 40;
int backBtnY = 40;

int confirmBtnX = 40;
int confirmBtnY = 40;

int profileBtnX = 40;
int profileBtnY = 80;

//Button Positioning
int pBack_Btn_Start_x = xPadding;
int pBack_Btn_Start_y = yPadding;
int pBack_Btn_Stop_x = pBack_Btn_Start_x + backBtnX;
int pBack_Btn_Stop_y = pBack_Btn_Start_y + backBtnY;

int pCfm_Btn_Start_x = 320 - xPadding - confirmBtnX;
int pCfm_Btn_Start_y = 480 - xPadding - confirmBtnY;
int pCfm_Btn_Stop_x = pCfm_Btn_Start_x + confirmBtnX;
int pCfm_Btn_Stop_y = pCfm_Btn_Start_y + confirmBtnY;

int p1_Btn_Start_x = xPadding + backBtnX + xPadding;
int p1_Btn_Start_y = yPadding + 25;
int p1_Btn_Stop_x = p1_Btn_Start_x + profileBtnX;
int p1_Btn_Stop_y = p1_Btn_Start_y + profileBtnY;

int p2_Btn_Start_x = p1_Btn_Stop_x + xPadding;
int p2_Btn_Start_y = p1_Btn_Start_y;
int p2_Btn_Stop_x = p2_Btn_Start_x + profileBtnX;
int p2_Btn_Stop_y = p2_Btn_Start_y + profileBtnY;

int p3_Btn_Start_x = p2_Btn_Start_x + xPadding;
int p3_Btn_Start_y = p1_Btn_Start_y;
int p3_Btn_Stop_x = p3_Btn_Start_x + profileBtnX;
int p3_Btn_Stop_y = p3_Btn_Start_y + profileBtnY;

int p4_Btn_Start_x = p3_Btn_Start_x + xPadding;
int p4_Btn_Start_y = p1_Btn_Start_y;
int p4_Btn_Stop_x = p4_Btn_Start_x + profileBtnX;
int p4_Btn_Stop_y = p4_Btn_Start_y + profileBtnY;

int p5_Btn_Start_x = p4_Btn_Start_x + xPadding;
int p5_Btn_Start_y = p1_Btn_Start_y;
int p5_Btn_Stop_x = p5_Btn_Start_x + profileBtnX;
int p5_Btn_Stop_y = p5_Btn_Start_y + profileBtnY;

int p6_Btn_Start_x = p1_Btn_Start_x;
int p6_Btn_Start_y = p1_Btn_Stop_y + yPadding;
int p6_Btn_Stop_x = p6_Btn_Start_x + profileBtnX;
int p6_Btn_Stop_y = p6_Btn_Start_y + profileBtnY;

int p7_Btn_Start_x = p1_Btn_Stop_x + xPadding;
int p7_Btn_Start_y = p1_Btn_Stop_y + yPadding;
int p7_Btn_Stop_x = p7_Btn_Start_x + profileBtnX;
int p7_Btn_Stop_y = p7_Btn_Start_y + profileBtnY;

int p8_Btn_Start_x = p2_Btn_Start_x + xPadding;
int p8_Btn_Start_y = p1_Btn_Stop_y + yPadding;
int p8_Btn_Stop_x = p8_Btn_Start_x + profileBtnX;
int p8_Btn_Stop_y = p8_Btn_Start_y + profileBtnY;

int p9_Btn_Start_x = p3_Btn_Start_x + xPadding;
int p9_Btn_Start_y = p1_Btn_Stop_y + yPadding;
int p9_Btn_Stop_x = p9_Btn_Start_x + profileBtnX;
int p9_Btn_Stop_y = p9_Btn_Start_y + profileBtnY;

int p10_Btn_Start_x = p4_Btn_Start_x + xPadding;
int p10_Btn_Start_y = p1_Btn_Stop_y + yPadding;
int p10_Btn_Stop_x = p10_Btn_Start_x + profileBtnX;
int p10_Btn_Stop_y = p10_Btn_Start_y + profileBtnY;

/*--------Data Entry Screen-----------*/
//Button Sizes
int fieldBtnX = 100;
int fieldBtnY = 40;

int stepBtnX = 25;
int stepBtnY = 25;

int saveBtnX = 80;
int saveBtnY = 40;

//Button Positioning
int pnName_Btn_Start_x = xPadding + 50;
int pnName_Btn_Start_y = 50;
int pnName_Btn_Stop_x = pnName_Btn_Start_x + fieldBtnX;
int pnName_Btn_Stop_y = pnName_Btn_Start_y + fieldBtnY;

int pnTClamp_Btn_Start_x = xPadding + 50;
int pnTClamp_Btn_Start_y = pnName_Btn_Stop_y + yPadding + 15;
int pnTClamp_Btn_Stop_x = pnTClamp_Btn_Start_x + fieldBtnX;
int pnTClamp_Btn_Stop_y = pnTClamp_Btn_Start_y + fieldBtnY;

int pnBClamp_Btn_Start_x = xPadding + 50;
int pnBClamp_Btn_Start_y = pnTClamp_Btn_Stop_y + yPadding + 15;
int pnBClamp_Btn_Stop_x = pnBClamp_Btn_Start_x + fieldBtnX;
int pnBClamp_Btn_Stop_y = pnBClamp_Btn_Start_y + fieldBtnY;

int pnFCh_Btn_Start_x = xPadding + 50;
int pnFCh_Btn_Start_y = pnBClamp_Btn_Stop_y + yPadding + 15;
int pnFCh_Btn_Stop_x = pnFCh_Btn_Start_x + fieldBtnX;
int pnFCh_Btn_Stop_y = pnFCh_Btn_Start_y + fieldBtnY;

int pnStp1_Btn_Start_x = xPadding + 100;
int pnStp1_Btn_Start_y = pnFCh_Btn_Stop_y + yPadding + 30;
int pnStp1_Btn_Stop_x = pnStp1_Btn_Start_x + stepBtnX;
int pnStp1_Btn_Stop_y = pnStp1_Btn_Start_y + stepBtnY;

int pnStp2_Btn_Start_x = xPadding + pnStp1_Btn_Stop_x;
int pnStp2_Btn_Start_y = pnStp1_Btn_Start_y;
int pnStp2_Btn_Stop_x = pnStp2_Btn_Start_x + stepBtnX;
int pnStp2_Btn_Stop_y = pnStp2_Btn_Start_y + stepBtnY;

int pnStp3_Btn_Start_x = xPadding + pnStp2_Btn_Start_x;
int pnStp3_Btn_Start_y = pnStp1_Btn_Start_y;
int pnStp3_Btn_Stop_x = pnStp3_Btn_Start_x + stepBtnX;
int pnStp3_Btn_Stop_y = pnStp3_Btn_Start_y + stepBtnY;

int pnStp4_Btn_Start_x = xPadding + pnStp3_Btn_Start_x;
int pnStp4_Btn_Start_y = pnStp1_Btn_Start_y;
int pnStp4_Btn_Stop_x = pnStp4_Btn_Start_x + stepBtnX;
int pnStp4_Btn_Stop_y = pnStp4_Btn_Start_y + stepBtnY;

int pnStp5_Btn_Start_x = xPadding + pnStp4_Btn_Start_x;
int pnStp5_Btn_Start_y = pnStp1_Btn_Start_y;
int pnStp5_Btn_Stop_x = pnStp5_Btn_Start_x + stepBtnX;
int pnStp5_Btn_Stop_y = pnStp5_Btn_Start_y + stepBtnY;

int pnBck_Btn_Start_x = xPadding;
int pnBck_Btn_Start_y = 320 - yPadding - backBtnY;
int pnBck_Btn_Stop_x = pnBck_Btn_Start_x + backBtnX;
int pnBck_Btn_Stop_y = pnBck_Btn_Start_y + backBtnY;

int pnSave_Btn_Start_x = 480 - xPadding - saveBtnX;
int pnSave_Btn_Start_y = 320 - yPadding - backBtnY;
int pnSave_Btn_Stop_x = pnSave_Btn_Start_x + saveBtnX;
int pnSave_Btn_Stop_y = pnSave_Btn_Start_y + saveBtnY;

/*--------Data Entry Screen-----------*/
//Button Sizes
int numBtnX = 80;
int numBtnY = 40;

//Button Positioning
int numDel_Btn_Start_x = 320 - xPadding - numBtnX;
int numDel_Btn_Start_y = yPadding;
int numDel_Btn_Stop_x = numDel_Btn_Start_x + numBtnX;
int numDel_Btn_Stop_y = numDel_Btn_Start_y + numBtnY;

int numLett_Btn_Start_x = xPadding;
int numLett_Btn_Start_y = yPadding;
int numLett_Btn_Stop_x = numLett_Btn_Start_x + numBtnX;
int numLett_Btn_Stop_y = numLett_Btn_Start_y + numBtnY;

int num1_Btn_Start_x = xPadding;
int num1_Btn_Start_y = yPadding + numBtnY + yPadding;
int num1_Btn_Stop_x = num1_Btn_Start_x + numBtnX;
int num1_Btn_Stop_y = num1_Btn_Start_y + numBtnY;

int num2_Btn_Start_x = xPadding + numBtnX + xPadding;
int num2_Btn_Start_y = yPadding + numBtnY + yPadding;
int num2_Btn_Stop_x = num2_Btn_Start_x + numBtnX;
int num2_Btn_Stop_y = num2_Btn_Start_y + numBtnY;

int num3_Btn_Start_x = xPadding + numBtnX + xPadding + numBtnX + xPadding;
int num3_Btn_Start_y = yPadding + numBtnY + yPadding;
int num3_Btn_Stop_x = num3_Btn_Start_x + numBtnX;
int num3_Btn_Stop_y = num3_Btn_Start_y + numBtnY;

int num4_Btn_Start_x = xPadding;
int num4_Btn_Start_y = yPadding + numBtnY + yPadding + numBtnY + yPadding;
int num4_Btn_Stop_x = num4_Btn_Start_x + numBtnX;
int num4_Btn_Stop_y = num4_Btn_Start_y + numBtnY;

int num5_Btn_Start_x = xPadding + numBtnX + xPadding;
int num5_Btn_Start_y = yPadding + numBtnY + yPadding + numBtnY + yPadding;
int num5_Btn_Stop_x = num5_Btn_Start_x + numBtnX;
int num5_Btn_Stop_y = num5_Btn_Start_y + numBtnY;

int num6_Btn_Start_x = xPadding + numBtnX + xPadding + numBtnX + xPadding;
int num6_Btn_Start_y = yPadding + numBtnY + yPadding + numBtnY + yPadding;
int num6_Btn_Stop_x = num6_Btn_Start_x + numBtnX;
int num6_Btn_Stop_y = num6_Btn_Start_y + numBtnY;

int num7_Btn_Start_x = xPadding;
int num7_Btn_Start_y = yPadding + numBtnY + yPadding + numBtnY + yPadding + numBtnY + yPadding;
int num7_Btn_Stop_x = num7_Btn_Start_x + numBtnX;
int num7_Btn_Stop_y = num7_Btn_Start_y + numBtnY;

int num8_Btn_Start_x = xPadding + numBtnX + xPadding;
int num8_Btn_Start_y = yPadding + numBtnY + yPadding + numBtnY + yPadding + numBtnY + yPadding;
int num8_Btn_Stop_x = num8_Btn_Start_x + numBtnX;
int num8_Btn_Stop_y = num8_Btn_Start_y + numBtnY;

int num9_Btn_Start_x = xPadding + numBtnX + xPadding + numBtnX + xPadding;
int num9_Btn_Start_y = yPadding + numBtnY + yPadding + numBtnY + yPadding + numBtnY + yPadding;
int num9_Btn_Stop_x = num9_Btn_Start_x + numBtnX;
int num9_Btn_Stop_y = num9_Btn_Start_y + numBtnY;

int numBck_Btn_Start_x = xPadding;
int numBck_Btn_Start_y = yPadding + numBtnY + yPadding + numBtnY + yPadding + numBtnY + yPadding + numBtnY + yPadding;
int numBck_Btn_Stop_x = numBck_Btn_Start_x + numBtnX;
int numBck_Btn_Stop_y = numBck_Btn_Start_y + numBtnY;

int numPd_Btn_Start_x = xPadding + numBtnX + xPadding;
int numPd_Btn_Start_y = yPadding + numBtnY + yPadding + numBtnY + yPadding + numBtnY + yPadding + numBtnY + yPadding;
int numPd_Btn_Stop_x = numPd_Btn_Start_x + numBtnX;
int numPd_Btn_Stop_y = numPd_Btn_Start_y + numBtnY;

int numOk_Btn_Start_x = xPadding + numBtnX + xPadding + numBtnX + xPadding;
int numOk_Btn_Start_y = yPadding + numBtnY + yPadding + numBtnY + yPadding + numBtnY + yPadding + numBtnY + yPadding;
int numOk_Btn_Stop_x = numOk_Btn_Start_x + numBtnX;
int numOk_Btn_Stop_y = numOk_Btn_Start_y + numBtnY;
/*------------------------------------*/
/*******************/

void setup() {
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  pinMode(pulseA, OUTPUT);
  pinMode(pulseB, OUTPUT);
  pinMode(pulseC, OUTPUT);
  pinMode(dirA, OUTPUT);
  pinMode(dirB, OUTPUT);
  pinMode(dirC, OUTPUT);
  
  pinMode(enableMotors, OUTPUT);
  
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
  //drawMainScreen();
  drawInputScreen();
  /***************/
  
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
  moveTo('A', 200);
  
  if(activeScreen == 0) {
      updateMainScreen();
  }
  else if(activeScreen == 1) {
    
  }
}

/**Draw the main home screen**/
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

/**Draw the input screen **/
void drawInputScreen() {
  tft.Fill_Screen(BLACK);   //Change the screen to solid black

  //Value Field Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding + numBtnX + xPadding, yPadding , 320 - numBtnX, numBtnY + yPadding);

  //Value Text
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_Number_Float(inputVal, 2, 0, 56, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
  
  //Button 'Delete'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("DEL", numDel_Btn_Start_x + 17, numDel_Btn_Start_y + 17);

  //Button 'Letter'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(numLett_Btn_Start_x, numLett_Btn_Start_y, numLett_Btn_Stop_x, numLett_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(numLett_Btn_Start_x, numLett_Btn_Start_y, numLett_Btn_Stop_x, numLett_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("ABC", numLett_Btn_Start_x + 17, numLett_Btn_Start_y + 17);

  //Button '1'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num1_Btn_Start_x, num1_Btn_Start_y, num1_Btn_Stop_x, num1_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num1_Btn_Start_x, num1_Btn_Start_y, num1_Btn_Stop_x, num1_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("1", num1_Btn_Start_x + 17, num1_Btn_Start_y + 17);

  //Button '2'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num2_Btn_Start_x, num2_Btn_Start_y, num2_Btn_Stop_x, num2_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num2_Btn_Start_x, num2_Btn_Start_y, num2_Btn_Stop_x, num2_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("2", num2_Btn_Start_x + 17, num2_Btn_Start_y + 17);

  //Button '3'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num3_Btn_Start_x, num3_Btn_Start_y, num3_Btn_Stop_x, num3_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num3_Btn_Start_x, num3_Btn_Start_y, num3_Btn_Stop_x, num3_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("3", num3_Btn_Start_x + 17, num3_Btn_Start_y + 17);

  //Button '4'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num4_Btn_Start_x, num4_Btn_Start_y, num4_Btn_Stop_x, num4_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num4_Btn_Start_x, num4_Btn_Start_y, num4_Btn_Stop_x, num4_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("4", num4_Btn_Start_x + 17, num4_Btn_Start_y + 17);
    
  //Button '5'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num5_Btn_Start_x, num5_Btn_Start_y, num5_Btn_Stop_x, num5_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num5_Btn_Start_x, num5_Btn_Start_y, num5_Btn_Stop_x, num5_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("5", num5_Btn_Start_x + 17, num5_Btn_Start_y + 17);

  //Button '6'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num6_Btn_Start_x, num6_Btn_Start_y, num6_Btn_Stop_x, num6_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num6_Btn_Start_x, num6_Btn_Start_y, num6_Btn_Stop_x, num6_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("6", num6_Btn_Start_x + 17, num6_Btn_Start_y + 17);

  //Button '7'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num7_Btn_Start_x, num7_Btn_Start_y, num7_Btn_Stop_x, num7_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num7_Btn_Start_x, num7_Btn_Start_y, num7_Btn_Stop_x, num7_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("7", num7_Btn_Start_x + 17, num7_Btn_Start_y + 17);

  //Button '8'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num8_Btn_Start_x, num8_Btn_Start_y, num8_Btn_Stop_x, num8_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num8_Btn_Start_x, num8_Btn_Start_y, num8_Btn_Stop_x, num8_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("8", num8_Btn_Start_x + 17, num8_Btn_Start_y + 17);

  //Button '9'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num9_Btn_Start_x, num9_Btn_Start_y, num9_Btn_Stop_x, num9_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num9_Btn_Start_x, num9_Btn_Start_y, num9_Btn_Stop_x, num9_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("9", num9_Btn_Start_x + 17, num9_Btn_Start_y + 17);

  //Button 'Back'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(numBck_Btn_Start_x, numBck_Btn_Start_y, numBck_Btn_Stop_x, numBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(numBck_Btn_Start_x, numBck_Btn_Start_y, numBck_Btn_Stop_x, numBck_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", numBck_Btn_Start_x + 17, numBck_Btn_Start_y + 17);

  //Button '.'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String(".", numPd_Btn_Start_x + 17, numPd_Btn_Start_y + 17);

  //Button 'Ok'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("OK", numOk_Btn_Start_x + 17, numOk_Btn_Start_y + 17);
}

/**Get touch readings and redraw changes**/
void updateInputScreen() {
  /***Touch Update***/
  TSPoint p = ts.getPoint();  //Leaves pins in input mode. Shared by lcd so must revert to output after reading.
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int x = map(p.x, TS_LEFT, TS_RIGHT, 0, 480);
  int y = map(p.y, TS_TOP, TS_BOT, 0, 320);

  //'Delete' Button
  //Most efficient drawing usage used. If a spot where a button is located is touched it will change the button
  //appearance and do its task while setting a flag. When the process runs the next cycle, it will see the flag but no touch
  //and then return the button to original. This way things are redrawn only when they need to be instead of refreshed each iteration.
  if(x > numDel_Btn_Start_x && x < numDel_Btn_Stop_x && y > numDel_Btn_Start_y && y < numDel_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Delete'
    tft.Set_Draw_color(RED);
    tft.Fill_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(RED);
    tft.Set_Text_colour(WHITE);
    tft.Print_String("DEL", numDel_Btn_Start_x + 17, numDel_Btn_Start_y + 17);
      
    //Flag the delete button as pressed
    delBtnFlag = true;
    delay(50);

    inputVal /= 10;           //Move decimal one point left
    if(inputVal < 0.005) {    //If the new value is less than the display precision (0.00)
      inputVal = 0;           //Set the input value to 0
    } 
  }
  else if(delBtnFlag) {
    //Button 'Delete'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("DEL", numDel_Btn_Start_x + 17, numDel_Btn_Start_y + 17);

    delBtnFlag = false;
  }

  //'Letters' Button
  if(x > numLett_Btn_Start_x && x < numLett_Btn_Stop_x && y > numLett_Btn_Start_y && y < numLett_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    lettBtnFlag = !lettBtnFlag;
    
    if(lettBtnFlag) {
      //Button 'Letter'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(numLett_Btn_Start_x, numLett_Btn_Start_y, numLett_Btn_Stop_x, numLett_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(numLett_Btn_Start_x, numLett_Btn_Start_y, numLett_Btn_Stop_x, numLett_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("ABC", numLett_Btn_Start_x + 17, numLett_Btn_Start_y + 17);
      delay(50);
    }
    else {
      //Button 'Letter'
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(numLett_Btn_Start_x, numLett_Btn_Start_y, numLett_Btn_Stop_x, numLett_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(numLett_Btn_Start_x, numLett_Btn_Start_y, numLett_Btn_Stop_x, numLett_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("ABC", numLett_Btn_Start_x + 17, numLett_Btn_Start_y + 17);
      delay(50);
    }
  }

  //'1' Button
  if(x > num1_Btn_Start_x && x < num1_Btn_Stop_x && y > num1_Btn_Start_y && y < num1_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE || lettBtnFlag) {
    if(lettBtnFlag) {
      //Button '1'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num1_Btn_Start_x, num1_Btn_Start_y, num1_Btn_Stop_x, num1_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num1_Btn_Start_x, num1_Btn_Start_y, num1_Btn_Stop_x, num1_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("ABC", num1_Btn_Start_x + 17, num1_Btn_Start_y + 17);

      //TODO: Add 'A' to the input
      int taps = 1;               //Variable to track how many taps were detecting in the double/tripple touch period
      long tapTime = millis();    //Note the time the tap occured
      while(millis() - tapTime < doubleTapPeriod) {         //While still within the alloted multi tap period
        //If another touch is detected
        if(x > num1_Btn_Start_x && x < num1_Btn_Stop_x && y > num1_Btn_Start_y && y < num1_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
          int taps = 2;           //Increase the touch count to 2
          //TODO: Replace with 'B' in input
          
          while(millis() - tapTime < doubleTapPeriod*2) {     //While still within the alloted multi tap period
            if(x > num1_Btn_Start_x && x < num1_Btn_Stop_x && y > num1_Btn_Start_y && y < num1_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
              int taps = 3;       //Increase the touch count to 3
              //TODO: Replace with 'C' in input
            }
          }
        }
      }
    }
    else {
      //Button '1'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num1_Btn_Start_x, num1_Btn_Start_y, num1_Btn_Stop_x, num1_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num1_Btn_Start_x, num1_Btn_Start_y, num1_Btn_Stop_x, num1_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("1", num1_Btn_Start_x + 17, num1_Btn_Start_y + 17);

      //TODO: Add '1' to the input

    }
    
    //Flag the delete button as pressed
    n1BtnFlag = true;
    delay(50);
  }
  else if(n1BtnFlag) {
    //Button '1'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(num1_Btn_Start_x, num1_Btn_Start_y, num1_Btn_Stop_x, num1_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(num1_Btn_Start_x, num1_Btn_Start_y, num1_Btn_Stop_x, num1_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("2", num1_Btn_Start_x + 17, num1_Btn_Start_y + 17);

    n1BtnFlag = false;
  }

  //'2' Button
  if(x > num2_Btn_Start_x && x < num2_Btn_Stop_x && y > num2_Btn_Start_y && y < num2_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE || lettBtnFlag) {
    if(lettBtnFlag) {
      //Button '2'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num2_Btn_Start_x, num2_Btn_Start_y, num2_Btn_Stop_x, num2_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num2_Btn_Start_x, num2_Btn_Start_y, num2_Btn_Stop_x, num2_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("DEF", num2_Btn_Start_x + 17, num2_Btn_Start_y + 17);

      //TODO: Add 'D' to the input
      int taps = 1;               //Variable to track how many taps were detecting in the double/tripple touch period
      long tapTime = millis();    //Note the time the tap occured
      while(millis() - tapTime < doubleTapPeriod) {         //While still within the alloted multi tap period
        //If another touch is detected
        if(x > num2_Btn_Start_x && x < num2_Btn_Stop_x && y > num2_Btn_Start_y && y < num2_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
          int taps = 2;           //Increase the touch count to 2
          //TODO: Replace with 'E' in input
          
          while(millis() - tapTime < doubleTapPeriod*2) {     //While still within the alloted multi tap period
            if(x > num2_Btn_Start_x && x < num2_Btn_Stop_x && y > num2_Btn_Start_y && y < num2_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
              int taps = 3;       //Increase the touch count to 3
              //TODO: Replace with 'F' in input
            }
          }
        }
      }
    }
    else {
      //Button '2'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num2_Btn_Start_x, num2_Btn_Start_y, num2_Btn_Stop_x, num2_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num2_Btn_Start_x, num2_Btn_Start_y, num2_Btn_Stop_x, num2_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("2", num2_Btn_Start_x + 17, num2_Btn_Start_y + 17);

      //TODO: Add '2' to the input

    }
    
    //Flag the delete button as pressed
    n2BtnFlag = true;
    delay(50);
  }
  else if(n2BtnFlag) {
    //Button '2'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(num2_Btn_Start_x, num2_Btn_Start_y, num2_Btn_Stop_x, num2_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(num2_Btn_Start_x, num2_Btn_Start_y, num2_Btn_Stop_x, num2_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("2", num2_Btn_Start_x + 17, num2_Btn_Start_y + 17);

    n2BtnFlag = false;
  }

  //'3' Button
  if(x > num3_Btn_Start_x && x < num3_Btn_Stop_x && y > num3_Btn_Start_y && y < num3_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE || lettBtnFlag) {
     if(lettBtnFlag) {
      //Button '3'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num3_Btn_Start_x, num3_Btn_Start_y, num3_Btn_Stop_x, num3_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num3_Btn_Start_x, num3_Btn_Start_y, num3_Btn_Stop_x, num3_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("GHI", num3_Btn_Start_x + 17, num3_Btn_Start_y + 17);

      //TODO: Add 'G' to the input
      int taps = 1;               //Variable to track how many taps were detecting in the double/tripple touch period
      long tapTime = millis();    //Note the time the tap occured
      while(millis() - tapTime < doubleTapPeriod) {         //While still within the alloted multi tap period
        //If another touch is detected
        if(x > num3_Btn_Start_x && x < num3_Btn_Stop_x && y > num3_Btn_Start_y && y < num3_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
          int taps = 2;           //Increase the touch count to 2
          //TODO: Replace with 'H' in input
          
          while(millis() - tapTime < doubleTapPeriod*2) {     //While still within the alloted multi tap period
            if(x > num3_Btn_Start_x && x < num3_Btn_Stop_x && y > num3_Btn_Start_y && y < num3_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
              int taps = 3;       //Increase the touch count to 3
              //TODO: Replace with 'I' in input
            }
          }
        }
      }
    }
    else {
      //Button '3'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num3_Btn_Start_x, num3_Btn_Start_y, num3_Btn_Stop_x, num3_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num3_Btn_Start_x, num3_Btn_Start_y, num3_Btn_Stop_x, num3_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("3", num3_Btn_Start_x + 17, num3_Btn_Start_y + 17);

      //TODO: Add '3' to the input

    }
    
    //Flag the delete button as pressed
    n3BtnFlag = true;
    delay(50);
  }
  else if(n3BtnFlag) {
    //Button '2'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(num3_Btn_Start_x, num3_Btn_Start_y, num3_Btn_Stop_x, num3_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(num3_Btn_Start_x, num3_Btn_Start_y, num3_Btn_Stop_x, num3_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("3", num3_Btn_Start_x + 17, num3_Btn_Start_y + 17);

    n3BtnFlag = false;
  }

  //'4' Button
  if(x > num4_Btn_Start_x && x < num4_Btn_Stop_x && y > num4_Btn_Start_y && y < num4_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE || lettBtnFlag) {
    if(lettBtnFlag) {
      //Button '4'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num4_Btn_Start_x, num4_Btn_Start_y, num4_Btn_Stop_x, num4_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num4_Btn_Start_x, num4_Btn_Start_y, num4_Btn_Stop_x, num4_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("JKL", num4_Btn_Start_x + 17, num4_Btn_Start_y + 17);

      //TODO: Add 'J' to the input
      int taps = 1;               //Variable to track how many taps were detecting in the double/tripple touch period
      long tapTime = millis();    //Note the time the tap occured
      while(millis() - tapTime < doubleTapPeriod) {         //While still within the alloted multi tap period
        //If another touch is detected
        if(x > num4_Btn_Start_x && x < num4_Btn_Stop_x && y > num4_Btn_Start_y && y < num4_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
          int taps = 2;           //Increase the touch count to 2
          //TODO: Replace with 'K' in input
          
          while(millis() - tapTime < doubleTapPeriod*2) {     //While still within the alloted multi tap period
            if(x > num4_Btn_Start_x && x < num4_Btn_Stop_x && y > num4_Btn_Start_y && y < num4_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
              int taps = 3;       //Increase the touch count to 3
              //TODO: Replace with 'L' in input
            }
          }
        }
      }
    }
    else {
      //Button '4'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num4_Btn_Start_x, num4_Btn_Start_y, num4_Btn_Stop_x, num4_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num4_Btn_Start_x, num4_Btn_Start_y, num4_Btn_Stop_x, num4_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("4", num4_Btn_Start_x + 17, num4_Btn_Start_y + 17);

      //TODO: Add '4' to the input

    }
    
    //Flag the delete button as pressed
    n4BtnFlag = true;
    delay(50);
  }
  else if(n4BtnFlag) {
    //Button '4'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(num4_Btn_Start_x, num4_Btn_Start_y, num4_Btn_Stop_x, num4_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(num4_Btn_Start_x, num4_Btn_Start_y, num4_Btn_Stop_x, num4_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("4", num4_Btn_Start_x + 17, num4_Btn_Start_y + 17);

    n4BtnFlag = false;
  }

  //'5' Button
  if(x > num5_Btn_Start_x && x < num5_Btn_Stop_x && y > num5_Btn_Start_y && y < num5_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE || lettBtnFlag) {
    if(lettBtnFlag) {
      //Button '5'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num5_Btn_Start_x, num5_Btn_Start_y, num5_Btn_Stop_x, num5_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num5_Btn_Start_x, num5_Btn_Start_y, num5_Btn_Stop_x, num5_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("MNO", num5_Btn_Start_x + 17, num5_Btn_Start_y + 17);

      //TODO: Add 'M' to the input
      int taps = 1;               //Variable to track how many taps were detecting in the double/tripple touch period
      long tapTime = millis();    //Note the time the tap occured
      while(millis() - tapTime < doubleTapPeriod) {         //While still within the alloted multi tap period
        //If another touch is detected
        if(x > num5_Btn_Start_x && x < num5_Btn_Stop_x && y > num5_Btn_Start_y && y < num5_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
          int taps = 2;           //Increase the touch count to 2
          //TODO: Replace with 'N' in input
          
          while(millis() - tapTime < doubleTapPeriod*2) {     //While still within the alloted multi tap period
            if(x > num5_Btn_Start_x && x < num5_Btn_Stop_x && y > num5_Btn_Start_y && y < num5_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
              int taps = 3;       //Increase the touch count to 3
              //TODO: Replace with 'O' in input
            }
          }
        }
      }
    }
    else {
      //Button '5'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num5_Btn_Start_x, num5_Btn_Start_y, num5_Btn_Stop_x, num5_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num5_Btn_Start_x, num5_Btn_Start_y, num5_Btn_Stop_x, num5_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("5", num5_Btn_Start_x + 17, num5_Btn_Start_y + 17);

      //TODO: Add '5' to the input

    }
    
    //Flag the delete button as pressed
    n5BtnFlag = true;
    delay(50);
  }
  else if(n5BtnFlag) {
    //Button '5'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(num5_Btn_Start_x, num5_Btn_Start_y, num5_Btn_Stop_x, num5_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(num5_Btn_Start_x, num5_Btn_Start_y, num5_Btn_Stop_x, num5_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("5", num5_Btn_Start_x + 17, num5_Btn_Start_y + 17);

    n5BtnFlag = false;
  }

  //'6' Button
  if(x > num6_Btn_Start_x && x < num6_Btn_Stop_x && y > num6_Btn_Start_y && y < num6_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE || lettBtnFlag) {
    if(lettBtnFlag) {
      //Button '6'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num6_Btn_Start_x, num6_Btn_Start_y, num6_Btn_Stop_x, num6_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num6_Btn_Start_x, num6_Btn_Start_y, num6_Btn_Stop_x, num6_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("PQR", num6_Btn_Start_x + 17, num6_Btn_Start_y + 17);

      //TODO: Add 'P' to the input
      int taps = 1;               //Variable to track how many taps were detecting in the double/tripple touch period
      long tapTime = millis();    //Note the time the tap occured
      while(millis() - tapTime < doubleTapPeriod) {         //While still within the alloted multi tap period
        //If another touch is detected
        if(x > num6_Btn_Start_x && x < num6_Btn_Stop_x && y > num6_Btn_Start_y && y < num6_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
          int taps = 2;           //Increase the touch count to 2
          //TODO: Replace with 'Q' in input
          
          while(millis() - tapTime < doubleTapPeriod*2) {     //While still within the alloted multi tap period
            if(x > num6_Btn_Start_x && x < num6_Btn_Stop_x && y > num6_Btn_Start_y && y < num6_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
              int taps = 3;       //Increase the touch count to 3
              //TODO: Replace with 'R' in input
            }
          }
        }
      }
    }
    else {
      //Button '6'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num6_Btn_Start_x, num6_Btn_Start_y, num6_Btn_Stop_x, num6_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num6_Btn_Start_x, num6_Btn_Start_y, num6_Btn_Stop_x, num6_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("6", num6_Btn_Start_x + 17, num6_Btn_Start_y + 17);

      //TODO: Add '6' to the input

    }
    
    //Flag the delete button as pressed
    n6BtnFlag = true;
    delay(50);
  }
  else if(n6BtnFlag) {
    //Button '6'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(num6_Btn_Start_x, num6_Btn_Start_y, num6_Btn_Stop_x, num6_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(num6_Btn_Start_x, num6_Btn_Start_y, num6_Btn_Stop_x, num6_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("6", num6_Btn_Start_x + 17, num6_Btn_Start_y + 17);

    n6BtnFlag = false;
  }

  //'7' Button
  if(x > num7_Btn_Start_x && x < num7_Btn_Stop_x && y > num7_Btn_Start_y && y < num7_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE || lettBtnFlag) {
    if(lettBtnFlag) {
      //Button '7'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num7_Btn_Start_x, num7_Btn_Start_y, num7_Btn_Stop_x, num7_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num7_Btn_Start_x, num7_Btn_Start_y, num7_Btn_Stop_x, num7_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("STU", num7_Btn_Start_x + 17, num7_Btn_Start_y + 17);

      //TODO: Add 'S' to the input
      int taps = 1;               //Variable to track how many taps were detecting in the double/tripple touch period
      long tapTime = millis();    //Note the time the tap occured
      while(millis() - tapTime < doubleTapPeriod) {         //While still within the alloted multi tap period
        //If another touch is detected
        if(x > num7_Btn_Start_x && x < num7_Btn_Stop_x && y > num7_Btn_Start_y && y < num7_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
          int taps = 2;           //Increase the touch count to 2
          //TODO: Replace with 'T' in input
          
          while(millis() - tapTime < doubleTapPeriod*2) {     //While still within the alloted multi tap period
            if(x > num7_Btn_Start_x && x < num7_Btn_Stop_x && y > num7_Btn_Start_y && y < num7_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
              int taps = 3;       //Increase the touch count to 3
              //TODO: Replace with 'U' in input
            }
          }
        }
      }
    }
    else {
      //Button '7'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num7_Btn_Start_x, num7_Btn_Start_y, num7_Btn_Stop_x, num7_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num7_Btn_Start_x, num7_Btn_Start_y, num7_Btn_Stop_x, num7_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("7", num7_Btn_Start_x + 17, num7_Btn_Start_y + 17);

      //TODO: Add '7' to the input

    }
    
    //Flag the delete button as pressed
    n7BtnFlag = true;
    delay(50);
  }
  else if(n7BtnFlag) {
    //Button '7'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(num7_Btn_Start_x, num7_Btn_Start_y, num7_Btn_Stop_x, num7_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(num7_Btn_Start_x, num7_Btn_Start_y, num7_Btn_Stop_x, num7_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("7", num7_Btn_Start_x + 17, num7_Btn_Start_y + 17);

    n7BtnFlag = false;
  }

  //'8' Button
  if(x > num8_Btn_Start_x && x < num8_Btn_Stop_x && y > num8_Btn_Start_y && y < num8_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE || lettBtnFlag) {
      if(lettBtnFlag) {
      //Button '8'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num8_Btn_Start_x, num8_Btn_Start_y, num8_Btn_Stop_x, num8_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num8_Btn_Start_x, num8_Btn_Start_y, num8_Btn_Stop_x, num8_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("VWX", num8_Btn_Start_x + 17, num8_Btn_Start_y + 17);

      //TODO: Add 'V' to the input
      int taps = 1;               //Variable to track how many taps were detecting in the double/tripple touch period
      long tapTime = millis();    //Note the time the tap occured
      while(millis() - tapTime < doubleTapPeriod) {         //While still within the alloted multi tap period
        //If another touch is detected
        if(x > num8_Btn_Start_x && x < num8_Btn_Stop_x && y > num8_Btn_Start_y && y < num8_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
          int taps = 2;           //Increase the touch count to 2
          //TODO: Replace with 'W' in input
          
          while(millis() - tapTime < doubleTapPeriod*2) {     //While still within the alloted multi tap period
            if(x > num8_Btn_Start_x && x < num8_Btn_Stop_x && y > num8_Btn_Start_y && y < num8_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
              int taps = 3;       //Increase the touch count to 3
              //TODO: Replace with 'X' in input
            }
          }
        }
      }
    }
    else {
      //Button '8'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num8_Btn_Start_x, num8_Btn_Start_y, num8_Btn_Stop_x, num8_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num8_Btn_Start_x, num8_Btn_Start_y, num8_Btn_Stop_x, num8_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("8", num8_Btn_Start_x + 17, num8_Btn_Start_y + 17);

      //TODO: Add '8' to the input

    }
    
    //Flag the delete button as pressed
    n8BtnFlag = true;
    delay(50);
  }
  else if(n8BtnFlag) {
    //Button '8'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(num8_Btn_Start_x, num8_Btn_Start_y, num8_Btn_Stop_x, num8_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(num8_Btn_Start_x, num8_Btn_Start_y, num8_Btn_Stop_x, num8_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("8", num8_Btn_Start_x + 17, num8_Btn_Start_y + 17);

    n8BtnFlag = false;
  }

  //'9' Button
  if(x > num9_Btn_Start_x && x < num9_Btn_Stop_x && y > num9_Btn_Start_y && y < num9_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE || lettBtnFlag) {
    if(lettBtnFlag) {
      //Button '9'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num9_Btn_Start_x, num9_Btn_Start_y, num9_Btn_Stop_x, num9_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num9_Btn_Start_x, num9_Btn_Start_y, num9_Btn_Stop_x, num9_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("YZ", num9_Btn_Start_x + 17, num9_Btn_Start_y + 17);

      //TODO: Add 'Y' to the input
      int taps = 1;               //Variable to track how many taps were detecting in the double/tripple touch period
      long tapTime = millis();    //Note the time the tap occured
      while(millis() - tapTime < doubleTapPeriod) {         //While still within the alloted multi tap period
        //If another touch is detected
        if(x > num9_Btn_Start_x && x < num9_Btn_Stop_x && y > num9_Btn_Start_y && y < num9_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
          int taps = 2;           //Increase the touch count to 2
          //TODO: Replace with 'Z' in input
        }
      }
    }
    else {
      //Button '9'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(num9_Btn_Start_x, num9_Btn_Start_y, num9_Btn_Stop_x, num9_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(num9_Btn_Start_x, num9_Btn_Start_y, num9_Btn_Stop_x, num9_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("9", num9_Btn_Start_x + 17, num9_Btn_Start_y + 17);

      //TODO: Add '9' to the input

    }
    
    //Flag the delete button as pressed
    n9BtnFlag = true;
    delay(50);
  }
  else if(n9BtnFlag) {
    //Button '9'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(num9_Btn_Start_x, num9_Btn_Start_y, num9_Btn_Stop_x, num9_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(num9_Btn_Start_x, num9_Btn_Start_y, num9_Btn_Stop_x, num9_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("9", num9_Btn_Start_x + 17, num9_Btn_Start_y + 17);

    n9BtnFlag = false;
  }

  //Back Button
  if(x > numBck_Btn_Start_x && x < numBck_Btn_Stop_x && y > numBck_Btn_Start_y && y < numBck_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Back'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(numBck_Btn_Start_x, numBck_Btn_Start_y, numBck_Btn_Stop_x, numBck_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(numBck_Btn_Start_x, numBck_Btn_Start_y, numBck_Btn_Stop_x, numBck_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Back", numBck_Btn_Start_x + 17, numBck_Btn_Start_y + 17);

    drawMainScreen();
    activeScreen = 0;
    delay(50);
  }
  
  //'Decimal' ('.') Button
  if(x > numPd_Btn_Start_x && x < numPd_Btn_Stop_x && y > numPd_Btn_Start_y && y < numPd_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button '.'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(".", numPd_Btn_Start_x + 17, numPd_Btn_Start_y + 17);

    //Flag the decimal button as pressed
    decimalBtnFlag = true;
    delay(50);
    //TODO: add a decimal point
  }
  else if(decimalBtnFlag) {
    //Button '.'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(".", numPd_Btn_Start_x + 17, numPd_Btn_Start_y + 17);

    decimalBtnFlag = false;
  }
  
  //'Ok' Button
  if(x > numOk_Btn_Start_x && x < numOk_Btn_Stop_x && y > numOk_Btn_Start_y && y < numOk_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Ok'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("OK", numOk_Btn_Start_x + 17, numOk_Btn_Start_y + 17);
    
    //Flag the delete button as pressed
    okBtnFlag = true;
    delay(50);
    //TODO: 
  }
  else if(okBtnFlag) {
    //Button 'Ok'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("OK", numOk_Btn_Start_x + 17, numOk_Btn_Start_y + 17);

    okBtnFlag = false;
  }
}

/**Draw the profile loading screen**/
void drawProfileLoadScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black

  int numSavedProfiles = 0;
  int eeAddr = 150;
  byte readData = 0xff;

  //Look through all profile address indexes for data
  for(int i = 0; i < 10; i++) {               
      EEPROM.get(eeAddr, readData);
      if(readData != 0xff) {
        numSavedProfiles++;
        EEPROM.get(eeAddr, profile[numSavedProfiles]);
      }
      eeAddr += 50;
  }
  
  //Button 'Back'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pBack_Btn_Start_x, pBack_Btn_Start_y, pBack_Btn_Stop_x, pBack_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pBack_Btn_Start_x, pBack_Btn_Start_y, pBack_Btn_Stop_x, pBack_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pBack_Btn_Start_x + 17, pBack_Btn_Start_y + 17);

  //Button 'Confirm'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pCfm_Btn_Start_x, pCfm_Btn_Start_y, pCfm_Btn_Stop_x, pCfm_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pCfm_Btn_Start_x, pCfm_Btn_Start_y, pCfm_Btn_Stop_x, pCfm_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Confirm", pCfm_Btn_Start_x + 17, pCfm_Btn_Start_y + 17);

  if(numSavedProfiles == 1) {
    //Button 'Profile 1'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p1_Btn_Start_x, p1_Btn_Start_y, p1_Btn_Stop_x, p1_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p1_Btn_Start_x, p1_Btn_Start_y, p1_Btn_Stop_x, p1_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[0], p1_Btn_Start_x + 17, p1_Btn_Start_y + 17);
  }

  if(numSavedProfiles == 2) {
    //Button 'Profile 2'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p2_Btn_Start_x, p2_Btn_Start_y, p2_Btn_Stop_x, p2_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p2_Btn_Start_x, p2_Btn_Start_y, p2_Btn_Stop_x, p2_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[1], p2_Btn_Start_x + 17, p2_Btn_Start_y + 17);
  }

  if(numSavedProfiles == 3) {
    //Button 'Profile 3'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p3_Btn_Start_x, p3_Btn_Start_y, p3_Btn_Stop_x, p2_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p3_Btn_Start_x, p3_Btn_Start_y, p3_Btn_Stop_x, p2_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[2], p3_Btn_Start_x + 17, p3_Btn_Start_y + 17);
  }

  if(numSavedProfiles == 4) {
    //Button 'Profile 4'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p4_Btn_Start_x, p4_Btn_Start_y, p4_Btn_Stop_x, p4_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p4_Btn_Start_x, p4_Btn_Start_y, p4_Btn_Stop_x, p4_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[3], p4_Btn_Start_x + 17, p4_Btn_Start_y + 17);
  }

  if(numSavedProfiles == 5) {
    //Button 'Profile 5'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p5_Btn_Start_x, p5_Btn_Start_y, p5_Btn_Stop_x, p5_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p5_Btn_Start_x, p5_Btn_Start_y, p5_Btn_Stop_x, p5_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[4], p5_Btn_Start_x + 17, p5_Btn_Start_y + 17);
  }

  if(numSavedProfiles == 6) {
    //Button 'Profile 6'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p6_Btn_Start_x, p6_Btn_Start_y, p6_Btn_Stop_x, p6_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p6_Btn_Start_x, p6_Btn_Start_y, p6_Btn_Stop_x, p6_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[5], p6_Btn_Start_x + 17, p6_Btn_Start_y + 17);
  }

  if(numSavedProfiles == 7) {
    //Button 'Profile 7'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p7_Btn_Start_x, p7_Btn_Start_y, p7_Btn_Stop_x, p7_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p7_Btn_Start_x, p7_Btn_Start_y, p7_Btn_Stop_x, p7_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[6], p7_Btn_Start_x + 17, p7_Btn_Start_y + 17);
  }

  if(numSavedProfiles == 8) {
    //Button 'Profile 8'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p8_Btn_Start_x, p8_Btn_Start_y, p8_Btn_Stop_x, p8_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p8_Btn_Start_x, p8_Btn_Start_y, p8_Btn_Stop_x, p8_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[7], p8_Btn_Start_x + 17, p8_Btn_Start_y + 17);
  }

  if(numSavedProfiles == 9) {
  //Button 'Profile 9'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(p9_Btn_Start_x, p9_Btn_Start_y, p9_Btn_Stop_x, p9_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(p9_Btn_Start_x, p9_Btn_Start_y, p9_Btn_Stop_x, p9_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String(profiles[8], p9_Btn_Start_x + 17, p9_Btn_Start_y + 17);
  }

  if(numSavedProfiles == 10) {
    //Button 'Profile 10'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p10_Btn_Start_x, p10_Btn_Start_y, p10_Btn_Stop_x, p10_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p10_Btn_Start_x, p10_Btn_Start_y, p10_Btn_Stop_x, p10_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[9], p10_Btn_Start_x + 17, p10_Btn_Start_y + 17);
  }
}

/**Get touch readings and redraw changes**/
void updateProfileLoadScreen() {
    /***Touch Update***/
  TSPoint p = ts.getPoint();  //Leaves pins in input mode. Shared by lcd so must revert to output after reading.
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int x = map(p.x, TS_LEFT, TS_RIGHT, 0, 480);
  int y = map(p.y, TS_TOP, TS_BOT, 0, 320);
  
  //'Back' Button
  if(x > pBack_Btn_Start_x && x < pBack_Btn_Stop_x && y > pBack_Btn_Start_y && y < pBack_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Back'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(pBack_Btn_Start_x, pBack_Btn_Start_y, pBack_Btn_Stop_x, pBack_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(pBack_Btn_Start_x, pBack_Btn_Start_y, pBack_Btn_Stop_x, pBack_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String("Back", pBack_Btn_Start_x + 17, pBack_Btn_Start_y + 17);
      
    //Flag the delete button as pressed
    pBackBtnFlag = true;
    
    drawMainScreen();
    activeScreen = 0;
    delay(50);
  }
  else if(pBackBtnFlag) {
    //Button 'Back'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(pBack_Btn_Start_x, pBack_Btn_Start_y, pBack_Btn_Stop_x, pBack_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(pBack_Btn_Start_x, pBack_Btn_Start_y, pBack_Btn_Stop_x, pBack_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Back", pBack_Btn_Start_x + 17, pBack_Btn_Start_y + 17);

    pBackBtnFlag = false;
  }

  //'Confirm' Button
  if(x > pCfm_Btn_Start_x && x < pCfm_Btn_Stop_x && y > pCfm_Btn_Start_y && y < pCfm_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Confirm'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(pCfm_Btn_Start_x, pCfm_Btn_Start_y, pCfm_Btn_Stop_x, pCfm_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(pCfm_Btn_Start_x, pCfm_Btn_Start_y, pCfm_Btn_Stop_x, pCfm_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String("Confirm", pCfm_Btn_Start_x + 17, pCfm_Btn_Start_y + 17);
      
    //Flag the delete button as pressed
    pCfmBtnFlag = true;
    delay(50);
    //TODO: Load selected profile and move to position
  }
  else if(pCfmBtnFlag) {
    //Button 'Confirm'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(pCfm_Btn_Start_x, pCfm_Btn_Start_y, pCfm_Btn_Stop_x, pCfm_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(pCfm_Btn_Start_x, pCfm_Btn_Start_y, pCfm_Btn_Stop_x, pCfm_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Confirm", pCfm_Btn_Start_x + 17, pCfm_Btn_Start_y + 17);

    pCfmBtnFlag = false;
  }

  //'Profile 1' Button
  if(x > p1_Btn_Start_x && x < p1_Btn_Stop_x && y > p1_Btn_Start_y && y < p1_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p1BtnFlag = !p1BtnFlag;

    if(p1BtnFlag) {
      //Button 'Profile 1'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(p1_Btn_Start_x, p1_Btn_Start_y, p1_Btn_Stop_x, p1_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(p1_Btn_Start_x, p1_Btn_Start_y, p1_Btn_Stop_x, p1_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(WHITE);
      tft.Print_String(profiles[0], p1_Btn_Start_x + 17, p1_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      p1BtnFlag = true;
      delay(50);
      //TODO: Load selected profile and move to position
    }
  }
  else {
    //Button 'Profile 1'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p1_Btn_Start_x, p1_Btn_Start_y, p1_Btn_Stop_x, p1_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p1_Btn_Start_x, p1_Btn_Start_y, p1_Btn_Stop_x, p1_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[0], p1_Btn_Start_x + 17, p1_Btn_Start_y + 17);

    //Flag the delete button as pressed
    p1BtnFlag = false;
    delay(50);
  }

  //'Profile 2' Button
  if(x > p2_Btn_Start_x && x < p2_Btn_Stop_x && y > p2_Btn_Start_y && y < p2_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p2BtnFlag = !p2BtnFlag;

    if(p2BtnFlag) {
      //Button 'Profile 2'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(p2_Btn_Start_x, p2_Btn_Start_y, p2_Btn_Stop_x, p2_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(p2_Btn_Start_x, p2_Btn_Start_y, p2_Btn_Stop_x, p2_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(WHITE);
      tft.Print_String(profiles[1], p2_Btn_Start_x + 17, p2_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      p2BtnFlag = true;
      delay(50);
      //TODO: Load selected profile and move to position
    }
  }
  else {
    //Button 'Profile 2'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p2_Btn_Start_x, p2_Btn_Start_y, p2_Btn_Stop_x, p2_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p2_Btn_Start_x, p2_Btn_Start_y, p2_Btn_Stop_x, p2_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[1], p2_Btn_Start_x + 17, p2_Btn_Start_y + 17);

    //Flag the delete button as pressed
    p2BtnFlag = false;
    delay(50);
  }

  //'Profile 3' Button
  if(x > p3_Btn_Start_x && x < p3_Btn_Stop_x && y > p3_Btn_Start_y && y < p3_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p3BtnFlag = !p3BtnFlag;

    if(p3BtnFlag) {
      //Button 'Profile 23
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(p3_Btn_Start_x, p3_Btn_Start_y, p3_Btn_Stop_x, p3_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(p3_Btn_Start_x, p3_Btn_Start_y, p3_Btn_Stop_x, p3_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(WHITE);
      tft.Print_String(profiles[2], p3_Btn_Start_x + 17, p3_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      p3BtnFlag = true;
      delay(50);
      //TODO: Load selected profile and move to position
    }
  }
  else {
    //Button 'Profile 3'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p3_Btn_Start_x, p3_Btn_Start_y, p3_Btn_Stop_x, p3_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p3_Btn_Start_x, p3_Btn_Start_y, p3_Btn_Stop_x, p3_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[2], p3_Btn_Start_x + 17, p3_Btn_Start_y + 17);

    //Flag the delete button as pressed
    p3BtnFlag = false;
    delay(50);
  }

  //'Profile 4' Button
  if(x > p4_Btn_Start_x && x < p4_Btn_Stop_x && y > p4_Btn_Start_y && y < p4_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p4BtnFlag = !p4BtnFlag;

    if(p4BtnFlag) {
      //Button 'Profile 4'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(p4_Btn_Start_x, p4_Btn_Start_y, p4_Btn_Stop_x, p4_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(p4_Btn_Start_x, p4_Btn_Start_y, p4_Btn_Stop_x, p4_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(WHITE);
      tft.Print_String(profiles[3], p4_Btn_Start_x + 17, p4_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      p4BtnFlag = true;
      delay(50);
      //TODO: Load selected profile and move to position
    }
  }
  else {
    //Button 'Profile 4'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p4_Btn_Start_x, p4_Btn_Start_y, p4_Btn_Stop_x, p4_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p4_Btn_Start_x, p4_Btn_Start_y, p4_Btn_Stop_x, p4_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[3], p4_Btn_Start_x + 17, p4_Btn_Start_y + 17);

    //Flag the delete button as pressed
    p4BtnFlag = false;
    delay(50);
  }

  //'Profile 5' Button
  if(x > p5_Btn_Start_x && x < p5_Btn_Stop_x && y > p5_Btn_Start_y && y < p5_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p5BtnFlag = !p5BtnFlag;

    if(p5BtnFlag) {
      //Button 'Profile 5'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(p5_Btn_Start_x, p5_Btn_Start_y, p5_Btn_Stop_x, p5_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(p5_Btn_Start_x, p5_Btn_Start_y, p5_Btn_Stop_x, p5_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(WHITE);
      tft.Print_String(profiles[4], p5_Btn_Start_x + 17, p5_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      p5BtnFlag = true;
      delay(50);
      //TODO: Load selected profile and move to position
    }
  }
  else {
    //Button 'Profile 5'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p5_Btn_Start_x, p5_Btn_Start_y, p5_Btn_Stop_x, p5_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p5_Btn_Start_x, p5_Btn_Start_y, p5_Btn_Stop_x, p5_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[4], p5_Btn_Start_x + 17, p5_Btn_Start_y + 17);

    //Flag the delete button as pressed
    p5BtnFlag = false;
    delay(50);
  }

  //'Profile 6' Button
  if(x > p6_Btn_Start_x && x < p6_Btn_Stop_x && y > p6_Btn_Start_y && y < p6_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p6BtnFlag = !p6BtnFlag;

    if(p6BtnFlag) {
      //Button 'Profile 6'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(p6_Btn_Start_x, p6_Btn_Start_y, p6_Btn_Stop_x, p6_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(p6_Btn_Start_x, p6_Btn_Start_y, p6_Btn_Stop_x, p6_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(WHITE);
      tft.Print_String(profiles[5], p6_Btn_Start_x + 17, p6_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      p6BtnFlag = true;
      delay(50);
      //TODO: Load selected profile and move to position
    }
  }
  else {
    //Button 'Profile 6'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p6_Btn_Start_x, p6_Btn_Start_y, p6_Btn_Stop_x, p6_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p6_Btn_Start_x, p6_Btn_Start_y, p6_Btn_Stop_x, p6_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[5], p6_Btn_Start_x + 17, p6_Btn_Start_y + 17);

    //Flag the delete button as pressed
    p6BtnFlag = false;
    delay(50);
  }

  //'Profile 7' Button
  if(x > p7_Btn_Start_x && x < p7_Btn_Stop_x && y > p7_Btn_Start_y && y < p7_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p7BtnFlag = !p7BtnFlag;

    if(p7BtnFlag) {
      //Button 'Profile 7'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(p7_Btn_Start_x, p7_Btn_Start_y, p7_Btn_Stop_x, p7_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(p7_Btn_Start_x, p7_Btn_Start_y, p7_Btn_Stop_x, p7_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(WHITE);
      tft.Print_String(profiles[6], p7_Btn_Start_x + 17, p7_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      p7BtnFlag = true;
      delay(50);
      //TODO: Load selected profile and move to position
    }
  }
  else {
    //Button 'Profile 7'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p7_Btn_Start_x, p7_Btn_Start_y, p7_Btn_Stop_x, p7_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p7_Btn_Start_x, p7_Btn_Start_y, p7_Btn_Stop_x, p7_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[6], p7_Btn_Start_x + 17, p7_Btn_Start_y + 17);

    //Flag the delete button as pressed
    p7BtnFlag = false;
    delay(50);
  }

  //'Profile 8' Button
  if(x > p8_Btn_Start_x && x < p8_Btn_Stop_x && y > p8_Btn_Start_y && y < p8_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p8BtnFlag = !p8BtnFlag;

    if(p8BtnFlag) {
      //Button 'Profile 8'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(p8_Btn_Start_x, p8_Btn_Start_y, p8_Btn_Stop_x, p8_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(p8_Btn_Start_x, p8_Btn_Start_y, p8_Btn_Stop_x, p8_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(WHITE);
      tft.Print_String(profiles[7], p8_Btn_Start_x + 17, p8_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      p8BtnFlag = true;
      delay(50);
      //TODO: Load selected profile and move to position
    }
  }
  else {
    //Button 'Profile 8'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p8_Btn_Start_x, p8_Btn_Start_y, p8_Btn_Stop_x, p8_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p8_Btn_Start_x, p8_Btn_Start_y, p8_Btn_Stop_x, p8_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[7], p8_Btn_Start_x + 17, p8_Btn_Start_y + 17);

    //Flag the delete button as pressed
    p8BtnFlag = false;
    delay(50);
  }

  //'Profile 9' Button
  if(x > p9_Btn_Start_x && x < p9_Btn_Stop_x && y > p9_Btn_Start_y && y < p9_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p9BtnFlag = !p9BtnFlag;

    if(p9BtnFlag) {
      //Button 'Profile 9'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(p9_Btn_Start_x, p9_Btn_Start_y, p9_Btn_Stop_x, p9_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(p9_Btn_Start_x, p9_Btn_Start_y, p9_Btn_Stop_x, p9_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(WHITE);
      tft.Print_String(profiles[8], p9_Btn_Start_x + 17, p9_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      p9BtnFlag = true;
      delay(50);
      //TODO: Load selected profile and move to position
    }
  }
  else {
    //Button 'Profile 9'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p9_Btn_Start_x, p9_Btn_Start_y, p9_Btn_Stop_x, p9_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p9_Btn_Start_x, p9_Btn_Start_y, p9_Btn_Stop_x, p9_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[8], p9_Btn_Start_x + 17, p9_Btn_Start_y + 17);

    //Flag the delete button as pressed
    p9BtnFlag = false;
    delay(50);
  }

  //'Profile 10' Button
  if(x > p10_Btn_Start_x && x < p10_Btn_Stop_x && y > p10_Btn_Start_y && y < p10_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p10BtnFlag = !p10BtnFlag;

    if(p10BtnFlag) {
      //Button 'Profile 10'
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(p10_Btn_Start_x, p10_Btn_Start_y, p10_Btn_Stop_x, p10_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(p10_Btn_Start_x, p10_Btn_Start_y, p10_Btn_Stop_x, p10_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(WHITE);
      tft.Print_String(profiles[9], p10_Btn_Start_x + 17, p10_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      p10BtnFlag = true;
      delay(50);
      //TODO: Load selected profile and move to position
    }
  }
  else {
    //Button 'Profile 10'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p10_Btn_Start_x, p10_Btn_Start_y, p10_Btn_Stop_x, p10_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p10_Btn_Start_x, p10_Btn_Start_y, p10_Btn_Stop_x, p10_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[9], p10_Btn_Start_x + 17, p10_Btn_Start_y + 17);

    //Flag the delete button as pressed
    p10BtnFlag = false;
    delay(50);
  }
}

/**Draw the new profile creation screen **/
void drawNewProfileScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black

  //Profile Configuration Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 40);

  //Profile Configuration Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(GREEN);
  tft.Print_String("Profile Configuration", 30, pnName_Btn_Start_y);

  //Profile Name Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(GREEN);
  tft.Print_String("Profile Name: ", 30, pnName_Btn_Start_y);

  //Profile Name Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnName_Btn_Start_x, pnName_Btn_Start_y, pnName_Btn_Stop_x, pnName_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnName_Btn_Start_x, pnName_Btn_Start_y, pnName_Btn_Stop_x, pnName_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("(tap to input)", pnName_Btn_Start_x + 17, pnName_Btn_Start_y + 17);

  //Top Clamp Position Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Top Clamp Position: ", 30, pnTClamp_Btn_Start_y);

  //Top Clamp Position Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnTClamp_Btn_Start_x, pnTClamp_Btn_Start_y, pnTClamp_Btn_Stop_x, pnTClamp_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnTClamp_Btn_Start_x, pnTClamp_Btn_Start_y, pnTClamp_Btn_Stop_x, pnTClamp_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("(tap to input)", pnTClamp_Btn_Start_x + 17, pnTClamp_Btn_Start_y + 17);

  //Bottom Clamp Position Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Bottom Clamp Position", 30, pnBClamp_Btn_Start_y);

  //Bottom Clamp Position Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("(tap to input)", pnBClamp_Btn_Start_x + 17, pnBClamp_Btn_Start_y + 17);

  //Frame Chassis Position Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Frame Chassis Position", 30, pnFCh_Btn_Start_y);

  //Frame Chassis Position Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnFCh_Btn_Start_x, pnFCh_Btn_Start_y, pnFCh_Btn_Stop_x, pnFCh_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnFCh_Btn_Start_x, pnFCh_Btn_Start_y, pnFCh_Btn_Stop_x, pnFCh_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("(tap to input)", pnFCh_Btn_Start_x + 17, pnFCh_Btn_Start_y + 17);

  //Number of Steps Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Bottom Clamp Position", 30, pnStp1_Btn_Start_y);

  //Number of Steps Button 1
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp1_Btn_Start_x, pnStp1_Btn_Start_y, pnStp1_Btn_Stop_x, pnStp1_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp1_Btn_Start_x, pnStp1_Btn_Start_y, pnStp1_Btn_Stop_x, pnStp1_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("1", pnStp1_Btn_Start_x + 17, pnStp1_Btn_Start_y + 17);

  //Number of Steps Button 2
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp2_Btn_Start_x, pnStp2_Btn_Start_y, pnStp2_Btn_Stop_x, pnStp2_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp2_Btn_Start_x, pnStp2_Btn_Start_y, pnStp2_Btn_Stop_x, pnStp2_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("2", pnStp2_Btn_Start_x + 17, pnStp2_Btn_Start_y + 17);

  //Number of Steps Button 3
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp3_Btn_Start_x, pnStp3_Btn_Start_y, pnStp3_Btn_Stop_x, pnStp3_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp3_Btn_Start_x, pnStp3_Btn_Start_y, pnStp3_Btn_Stop_x, pnStp3_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("3", pnStp3_Btn_Start_x + 17, pnStp3_Btn_Start_y + 17);

  //Number of Steps Button 4
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp4_Btn_Start_x, pnStp4_Btn_Start_y, pnStp4_Btn_Stop_x, pnStp4_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp4_Btn_Start_x, pnStp4_Btn_Start_y, pnStp4_Btn_Stop_x, pnStp4_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("4", pnStp4_Btn_Start_x + 17, pnStp4_Btn_Start_y + 17);

  //Number of Steps Button 5
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp5_Btn_Start_x, pnStp5_Btn_Start_y, pnStp5_Btn_Stop_x, pnStp5_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp5_Btn_Start_x, pnStp5_Btn_Start_y, pnStp5_Btn_Stop_x, pnStp5_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("5", pnStp5_Btn_Start_x + 17, pnStp5_Btn_Start_y + 17);

  //Back Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 17);

  //Save Button
  tft.Set_Draw_color(GREEN);
  tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(GREEN);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Save", pnSave_Btn_Start_x + 17, pnSave_Btn_Start_y + 17);
}

/**Get thouch readings and redraw changes**/
void updateNewProfileScreen() {
    /***Touch Update***/
  TSPoint p = ts.getPoint();  //Leaves pins in input mode. Shared by lcd so must revert to output after reading.
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int x = map(p.x, TS_LEFT, TS_RIGHT, 0, 480);
  int y = map(p.y, TS_TOP, TS_BOT, 0, 320);
  
  //TODO: Change from toggle to click btn on: pnName, pnTClamp, pnBClamp, pnFch 
  //'Profile Name' Button
  if(x > pnName_Btn_Start_x && x < pnName_Btn_Stop_x && y > pnName_Btn_Start_y && y < pnName_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    pnNameBtnFlag = !pnNameBtnFlag;

    if(pnNameBtnFlag) {
      //Profile Name Field
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnName_Btn_Start_x, pnName_Btn_Start_y, pnName_Btn_Stop_x, pnName_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnName_Btn_Start_x, pnName_Btn_Start_y, pnName_Btn_Stop_x, pnName_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("(tap to input)", pnName_Btn_Start_x + 17, pnName_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnNameBtnFlag = true;
      delay(50);
      //TODO: Load input screen and display data
    }
    else {
      //Profile Name Field
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnName_Btn_Start_x, pnName_Btn_Start_y, pnName_Btn_Stop_x, pnName_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnName_Btn_Start_x, pnName_Btn_Start_y, pnName_Btn_Stop_x, pnName_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("(tap to input)", pnName_Btn_Start_x + 17, pnName_Btn_Start_y + 17);
  
  
      //Flag the delete button as pressed
      pnNameBtnFlag = false;
      delay(50);
      //TODO: If profile name is given display it instead
    }
  }

  //Top Clamp Position Button
  if(x > pnTClamp_Btn_Start_x && x < pnTClamp_Btn_Stop_x && y > pnTClamp_Btn_Start_y && y < pnTClamp_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    pnTClampBtnFlag = !pnTClampBtnFlag;

    if(pnTClampBtnFlag) {
      //Top Clamp Position Field
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnTClamp_Btn_Start_x, pnTClamp_Btn_Start_y, pnTClamp_Btn_Stop_x, pnTClamp_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnTClamp_Btn_Start_x, pnTClamp_Btn_Start_y, pnTClamp_Btn_Stop_x, pnTClamp_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("(tap to input)", pnTClamp_Btn_Start_x + 17, pnTClamp_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnTClampBtnFlag = true;
      delay(50);
      //TODO: Load input screen and display data
    }
    else {
      //Top Clamp Position Field
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnTClamp_Btn_Start_x, pnTClamp_Btn_Start_y, pnTClamp_Btn_Stop_x, pnTClamp_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnTClamp_Btn_Start_x, pnTClamp_Btn_Start_y, pnTClamp_Btn_Stop_x, pnTClamp_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("(tap to input)", pnTClamp_Btn_Start_x + 17, pnTClamp_Btn_Start_y + 17);
  
  
      //Flag the delete button as pressed
      pnTClampBtnFlag = false;
      delay(50);
      //TODO: If value is given display it instead
    }
  }

  //Bottom Clamp Position Button
  if(x > pnBClamp_Btn_Start_x && x < pnBClamp_Btn_Stop_x && y > pnBClamp_Btn_Start_y && y < pnBClamp_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    pnBClampBtnFlag = !pnBClampBtnFlag;

    if(pnBClampBtnFlag) {
      //Bottom Clamp Position Field
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("(tap to input)", pnBClamp_Btn_Start_x + 17, pnBClamp_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnBClampBtnFlag = true;
      delay(50);
      //TODO: Load input screen and display data
    }
    else {
      //Bottom Clamp Position Field
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("(tap to input)", pnBClamp_Btn_Start_x + 17, pnBClamp_Btn_Start_y + 17);
  
  
      //Flag the delete button as pressed
      pnBClampBtnFlag = false;
      delay(50);
      //TODO: If value is given display it instead
    }
  }

  //Frame Chassis Position Button
  if(x > pnFCh_Btn_Start_x && x < pnFCh_Btn_Stop_x && y > pnFCh_Btn_Start_y && y < pnFCh_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    pnFChBtnFlag = !pnFChBtnFlag;

    if(pnFChBtnFlag) {
      //Frame Chassis Position Field
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnFCh_Btn_Start_x, pnFCh_Btn_Start_y, pnFCh_Btn_Stop_x, pnFCh_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnFCh_Btn_Start_x, pnFCh_Btn_Start_y, pnFCh_Btn_Stop_x, pnFCh_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("(tap to input)", pnFCh_Btn_Start_x + 17, pnFCh_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnFChBtnFlag = true;
      delay(50);
      //TODO: Load input screen and display data
    
    }
    else {
      //Frame Chassis Position Field
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnFCh_Btn_Start_x, pnFCh_Btn_Start_y, pnFCh_Btn_Stop_x, pnFCh_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnFCh_Btn_Start_x, pnFCh_Btn_Start_y, pnFCh_Btn_Stop_x, pnFCh_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("(tap to input)", pnFCh_Btn_Start_x + 17, pnFCh_Btn_Start_y + 17);
  
      //Flag the delete button as pressed
      pnFChBtnFlag = false;
      delay(50);
      //TODO: If value is given display it instead
    }
  }

  //'Step 1' Button
  if(x > pnStp1_Btn_Start_x && x < pnStp1_Btn_Stop_x && y > pnStp1_Btn_Start_y && y < pnStp1_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    pnStp1BtnFlag = !pnStp1BtnFlag;

    if(pnStp1BtnFlag) {
      //'Step 1' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnStp1_Btn_Start_x, pnStp1_Btn_Start_y, pnStp1_Btn_Stop_x, pnStp1_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnStp1_Btn_Start_x, pnStp1_Btn_Start_y, pnStp1_Btn_Stop_x, pnStp1_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("1", pnStp1_Btn_Start_x + 17, pnStp1_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnStp1BtnFlag = true;
      delay(50);
      //TODO: Toggle this step on and the others off
    
    }
    else {
      //'Step 1' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnStp1_Btn_Start_x, pnStp1_Btn_Start_y, pnStp1_Btn_Stop_x, pnStp1_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnStp1_Btn_Start_x, pnStp1_Btn_Start_y, pnStp1_Btn_Stop_x, pnStp1_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("1", pnStp1_Btn_Start_x + 17, pnStp1_Btn_Start_y + 17);
  
      //Flag the delete button as pressed
      pnStp1BtnFlag = false;
      delay(50);
    }
  }

  //'Step 2' Button
  if(x > pnStp2_Btn_Start_x && x < pnStp2_Btn_Stop_x && y > pnStp2_Btn_Start_y && y < pnStp2_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    pnStp2BtnFlag = !pnStp2BtnFlag;

    if(pnStp2BtnFlag) {
      //'Step 2' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnStp2_Btn_Start_x, pnStp2_Btn_Start_y, pnStp2_Btn_Stop_x, pnStp2_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnStp2_Btn_Start_x, pnStp2_Btn_Start_y, pnStp2_Btn_Stop_x, pnStp2_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("2", pnStp2_Btn_Start_x + 17, pnStp2_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnStp2BtnFlag = true;
      delay(50);
      //TODO: Toggle this step on and the others off
    }
    else {
      //'Step 2' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnStp2_Btn_Start_x, pnStp2_Btn_Start_y, pnStp2_Btn_Stop_x, pnStp2_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnStp2_Btn_Start_x, pnStp2_Btn_Start_y, pnStp2_Btn_Stop_x, pnStp2_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("2", pnStp2_Btn_Start_x + 17, pnStp2_Btn_Start_y + 17);
  
      //Flag the delete button as pressed
      pnStp2BtnFlag = false;
      delay(50);
    }
  }

  //'Step 3' Button
  if(x > pnStp3_Btn_Start_x && x < pnStp3_Btn_Stop_x && y > pnStp3_Btn_Start_y && y < pnStp3_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    pnStp3BtnFlag = !pnStp3BtnFlag;

    if(pnStp3BtnFlag) {
      //'Step 3' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnStp3_Btn_Start_x, pnStp3_Btn_Start_y, pnStp3_Btn_Stop_x, pnStp3_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnStp3_Btn_Start_x, pnStp3_Btn_Start_y, pnStp3_Btn_Stop_x, pnStp3_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("3", pnStp3_Btn_Start_x + 17, pnStp3_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnStp3BtnFlag = true;
      delay(50);
      //TODO: Toggle this step on and the others off
    }
    else {
      //'Step 3' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnStp3_Btn_Start_x, pnStp3_Btn_Start_y, pnStp3_Btn_Stop_x, pnStp3_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnStp3_Btn_Start_x, pnStp3_Btn_Start_y, pnStp3_Btn_Stop_x, pnStp3_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("3", pnStp3_Btn_Start_x + 17, pnStp3_Btn_Start_y + 17);
  
      //Flag the delete button as pressed
      pnStp3BtnFlag = false;
      delay(50);
    }
  }

  //'Step 4' Button
  if(x > pnStp4_Btn_Start_x && x < pnStp4_Btn_Stop_x && y > pnStp4_Btn_Start_y && y < pnStp4_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    pnStp4BtnFlag = !pnStp4BtnFlag;

    if(pnStp4BtnFlag) {
      //'Step 4' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnStp4_Btn_Start_x, pnStp4_Btn_Start_y, pnStp4_Btn_Stop_x, pnStp4_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnStp4_Btn_Start_x, pnStp4_Btn_Start_y, pnStp4_Btn_Stop_x, pnStp4_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("4", pnStp4_Btn_Start_x + 17, pnStp4_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnStp4BtnFlag = true;
      delay(50);
      //TODO: Toggle this step on and the others off
    
    }
    else {
      //'Step 4' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnStp4_Btn_Start_x, pnStp4_Btn_Start_y, pnStp4_Btn_Stop_x, pnStp4_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnStp4_Btn_Start_x, pnStp4_Btn_Start_y, pnStp4_Btn_Stop_x, pnStp4_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("4", pnStp4_Btn_Start_x + 17, pnStp4_Btn_Start_y + 17);
  
      //Flag the delete button as pressed
      pnStp4BtnFlag = false;
      delay(50);
    }
  }
  
  //'Step 5' Button
  if(x > pnStp5_Btn_Start_x && x < pnStp5_Btn_Stop_x && y > pnStp5_Btn_Start_y && y < pnStp5_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    pnStp5BtnFlag = !pnStp5BtnFlag;

    if(pnStp5BtnFlag) {
      //'Step 5' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnStp5_Btn_Start_x, pnStp5_Btn_Start_y, pnStp5_Btn_Stop_x, pnStp5_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnStp5_Btn_Start_x, pnStp5_Btn_Start_y, pnStp5_Btn_Stop_x, pnStp5_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("5", pnStp5_Btn_Start_x + 17, pnStp5_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnStp5BtnFlag = true;
      delay(50);
      //TODO: Toggle this step on and the others off
    }
    else {
      //'Step 5' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(pnStp5_Btn_Start_x, pnStp5_Btn_Start_y, pnStp5_Btn_Stop_x, pnStp5_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(pnStp5_Btn_Start_x, pnStp5_Btn_Start_y, pnStp5_Btn_Stop_x, pnStp5_Btn_Stop_y);
      tft.Set_Text_Size(1);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("5", pnStp5_Btn_Start_x + 17, pnStp5_Btn_Start_y + 17);
  
      //Flag the delete button as pressed
      pnStp5BtnFlag = false;
      delay(50);
    }
  }

  //'Back' Button
  if(x > pnBck_Btn_Start_x && x < pnBck_Btn_Stop_x && y > pnBck_Btn_Start_y && y < pnBck_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Back'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 17);
      
    //Flag the delete button as pressed
    pnBckBtnFlag = true;
    drawMainScreen();
    activeScreen = 0;
    delay(50);
  }
  else if(pnBckBtnFlag) {
    //Button 'Back'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 17);

    pnBckBtnFlag = false;
  }

  //'Save' Button
  if(x > pnSave_Btn_Start_x && x < pnSave_Btn_Stop_x && y > pnSave_Btn_Start_y && y < pnSave_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Save'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String("Save", pnSave_Btn_Start_x + 17, pnSave_Btn_Start_y + 17);
      
    //Flag the delete button as pressed
    pnSaveBtnFlag = true;
    delay(50);
    //TODO: Save profile and move to position
  }
  else if(pnSaveBtnFlag) {
    //Button 'Save'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Save", pnSave_Btn_Start_x + 17, pnSave_Btn_Start_y + 17);

    pnSaveBtnFlag = false;
  }
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
  EEPROM.put(eeAddr, tClampDist1);    //Save the value of the top clamp position
  eeAddr += sizeof(int);              
  EEPROM.put(eeAddr, bClampDist1);    //Save the value of the bottom clamp position
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
  EEPROM.get(eeAddr, tClampDist1);    //Load the value of the top clamp position
  eeAddr += sizeof(int);
  EEPROM.get(eeAddr, bClampDist1);    //Load the value of the bottom clamp position
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
