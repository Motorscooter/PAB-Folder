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
#define pulseA  22
#define pulseB  24
#define pulseC  26
#define dirA    28
#define dirB    30
#define dirC    27

//Stepper Motor Enabling
#define ENABLE_MOTORS 29

//Stepper Motor Settings
  /*LOW LOW LOW   - Full Step
    HIGH LOW LOW  - Half Step
    LOW HIGH LOW  - Quarter Step
    HIGH HIGH LOW - Eighth Step
    HIGH HIGH HIGH - Sixteenth Step*/
#define MS1A 32
#define MS2A 34
#define MS3A 36     
#define MS1B 38
#define MS2B 40
#define MS3B 42
#define MS1C 33
#define MS2C 35
#define MS3C 37
#define MS1D 39
#define MS2D 41
#define MS3D 43

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

int numSavedProfiles = 0;

//0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
//7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
//13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
//19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
//24=homeOffsetClampT, 25=homeOffsetClampB
int varInputRegister = 0;

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

/////PROFILE VALUES
String profile = "     ";
int numProfSteps = 1;
int tClampDist = 0;
int bClampDist = 0;
int hDist1 = 0;
int hDist2 = 0;
int hDist3 = 0;
int stepNum = 1;

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

//Microstepping Value
//0=1, 1=1/2, 2=1/4, 3=1/8, 4=1/16
int mUStepA = 4;
int mUStepB = 4;
int mUStepC = 4;

/////MOTOR SETTINGS
//Motor Steps/mm
float mStepsPerA = 80;
float mStepsPerB = 80;
float mStepsPerC = 80;

/////MACHINE SETTINGS
//Homing Offsets
float homeOffsetT = 0;
float homeOffsetB = 0;
float homeOffsetClampT = 0;
float homeOffsetClampB = 0;

/////CLAMP STATUS
boolean tClampEngaged = false;
boolean bClampEngaged = false;

/////GUI Relations
int activeScreen = 0; //0 = home screen
float inputVal = 0;
String inputBuilder = "";
String profiles[10];

int prevActiveScreen;

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

boolean mAuSBtnFlag = false;
boolean mBuSBtnFlag = false;
boolean mCuSBtnFlag = false;
boolean mAinvBtnFlag = false;
boolean mBinvBtnFlag = false;
boolean mCinvBtnFlag = false;

boolean sVelBtnFlag = false;
boolean sAccelBtnFlag = false;
boolean sJerkBtnFlag = false;
boolean sUSBtnFlag = false;
boolean sSmmBtnFlag = false;
boolean sRstBtnFlag = false;

boolean svAValBtnFlag = false;
boolean svBValBtnFlag = false;
boolean svCValBtnFlag = false;
boolean svADecBtnFlag = false;
boolean svBDecBtnFlag = false;
boolean svCDecBtnFlag = false;
boolean svAIncBtnFlag = false;
boolean svBIncBtnFlag = false;
boolean svCIncBtnFlag = false;
boolean svAamtBtnFlag = false;
boolean svBamtBtnFlag = false;
boolean svCamtBtnFlag = false;

boolean saAValBtnFlag = false;
boolean saBValBtnFlag = false;
boolean saCValBtnFlag = false;
boolean saADecBtnFlag = false;
boolean saBDecBtnFlag = false;
boolean saCDecBtnFlag = false;
boolean saAIncBtnFlag = false;
boolean saBIncBtnFlag = false;
boolean saCIncBtnFlag = false;
boolean saAamtBtnFlag = false;
boolean saBamtBtnFlag = false;
boolean saCamtBtnFlag = false;

boolean sjAValBtnFlag = false;
boolean sjBValBtnFlag = false;
boolean sjCValBtnFlag = false;
boolean sjADecBtnFlag = false;
boolean sjBDecBtnFlag = false;
boolean sjCDecBtnFlag = false;
boolean sjAIncBtnFlag = false;
boolean sjBIncBtnFlag = false;
boolean sjCIncBtnFlag = false;
boolean sjAamtBtnFlag = false;
boolean sjBamtBtnFlag = false;
boolean sjCamtBtnFlag = false;

boolean ssmmAValBtnFlag = false;
boolean ssmmBValBtnFlag = false;
boolean ssmmCValBtnFlag = false;
boolean ssmmADecBtnFlag = false;
boolean ssmmBDecBtnFlag = false;
boolean ssmmCDecBtnFlag = false;
boolean ssmmAIncBtnFlag = false;
boolean ssmmBIncBtnFlag = false;
boolean ssmmCIncBtnFlag = false;
boolean ssmmAamtBtnFlag = false;
boolean ssmmBamtBtnFlag = false;
boolean ssmmCamtBtnFlag = false;

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
int backBtnX = 80;
int backBtnY = 40;

int confirmBtnX = 80;
int confirmBtnY = 40;

int profileBtnX = 40;
int profileBtnY = 80;

//Button Positioning
int pBack_Btn_Start_x = xPadding;
int pBack_Btn_Start_y = 320 - yPadding - backBtnX;
int pBack_Btn_Stop_x = pBack_Btn_Start_x + backBtnX;
int pBack_Btn_Stop_y = pBack_Btn_Start_y + backBtnY;

int pCfm_Btn_Start_x = 480 - xPadding - confirmBtnX;
int pCfm_Btn_Start_y = 320 - yPadding - confirmBtnY;
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

/*--------New Profile Screen-----------*/
//Button Sizes
int fieldBtnX = 100;
int fieldBtnY = 40;

int stepBtnX = 30;
int stepBtnY = 30;

int saveBtnX = 80;
int saveBtnY = 40;

int chgValBtnX = 40;
int chgValBtnY = 40;

float aAmt = 1;
float bAmt = 1;
float cAmt = 1;

//Button Positioning
int pnName_Btn_Start_x = xPadding + 175;
int pnName_Btn_Start_y = 50;
int pnName_Btn_Stop_x = pnName_Btn_Start_x + fieldBtnX * 2;
int pnName_Btn_Stop_y = pnName_Btn_Start_y + fieldBtnY;

int pnTClamp_Btn_Start_x = xPadding + 200;
int pnTClamp_Btn_Start_y = pnName_Btn_Stop_y + yPadding + 10;
int pnTClamp_Btn_Stop_x = pnTClamp_Btn_Start_x + fieldBtnX;
int pnTClamp_Btn_Stop_y = pnTClamp_Btn_Start_y + fieldBtnY;

int pnBClamp_Btn_Start_x = xPadding + 200;
int pnBClamp_Btn_Start_y = pnTClamp_Btn_Stop_y + yPadding + 10;
int pnBClamp_Btn_Stop_x = pnBClamp_Btn_Start_x + fieldBtnX;
int pnBClamp_Btn_Stop_y = pnBClamp_Btn_Start_y + fieldBtnY;

int pnFCh_Btn_Start_x = xPadding + 200;
int pnFCh_Btn_Start_y = pnBClamp_Btn_Stop_y + yPadding + 10;
int pnFCh_Btn_Stop_x = pnFCh_Btn_Start_x + fieldBtnX;
int pnFCh_Btn_Stop_y = pnFCh_Btn_Start_y + fieldBtnY;

int pnTDec_Btn_Start_x = pnTClamp_Btn_Stop_x + 25;
int pnTDec_Btn_Start_y = pnTClamp_Btn_Start_y;
int pnTDec_Btn_Stop_x = pnTDec_Btn_Start_x + chgValBtnX;
int pnTDec_Btn_Stop_y = pnTDec_Btn_Start_y + chgValBtnY;

int pnBDec_Btn_Start_x = pnBClamp_Btn_Stop_x + 25;
int pnBDec_Btn_Start_y = pnBClamp_Btn_Start_y;
int pnBDec_Btn_Stop_x = pnBDec_Btn_Start_x + chgValBtnX;
int pnBDec_Btn_Stop_y = pnBDec_Btn_Start_y + chgValBtnY;

int pnFDec_Btn_Start_x = pnFCh_Btn_Stop_x + 25;
int pnFDec_Btn_Start_y = pnFCh_Btn_Start_y;
int pnFDec_Btn_Stop_x = pnFDec_Btn_Start_x + chgValBtnX;
int pnFDec_Btn_Stop_y = pnFDec_Btn_Start_y + chgValBtnY;

int pnTInc_Btn_Start_x = pnTDec_Btn_Stop_x + 5;
int pnTInc_Btn_Start_y = pnTClamp_Btn_Start_y;
int pnTInc_Btn_Stop_x = pnTInc_Btn_Start_x + chgValBtnX;
int pnTInc_Btn_Stop_y = pnTInc_Btn_Start_y + chgValBtnY;

int pnBInc_Btn_Start_x = pnBDec_Btn_Stop_x + 5;
int pnBInc_Btn_Start_y = pnBClamp_Btn_Start_y;
int pnBInc_Btn_Stop_x = pnBInc_Btn_Start_x + chgValBtnX;
int pnBInc_Btn_Stop_y = pnBInc_Btn_Start_y + chgValBtnY;

int pnFInc_Btn_Start_x = pnFDec_Btn_Stop_x + 5;
int pnFInc_Btn_Start_y = pnFCh_Btn_Start_y;
int pnFInc_Btn_Stop_x = pnFInc_Btn_Start_x + chgValBtnX;
int pnFInc_Btn_Stop_y = pnFInc_Btn_Start_y + chgValBtnY;

int pnTamt_Btn_Start_x = pnTInc_Btn_Stop_x + 25;
int pnTamt_Btn_Start_y = pnTClamp_Btn_Start_y;
int pnTamt_Btn_Stop_x = pnTamt_Btn_Start_x + chgValBtnX;
int pnTamt_Btn_Stop_y = pnTamt_Btn_Start_y + chgValBtnY;

int pnBamt_Btn_Start_x = pnBInc_Btn_Stop_x + 25;
int pnBamt_Btn_Start_y = pnBClamp_Btn_Start_y;
int pnBamt_Btn_Stop_x = pnBamt_Btn_Start_x + chgValBtnX;
int pnBamt_Btn_Stop_y = pnBamt_Btn_Start_y + chgValBtnY;

int pnFamt_Btn_Start_x = pnFInc_Btn_Stop_x + 25;
int pnFamt_Btn_Start_y = pnFCh_Btn_Start_y;
int pnFamt_Btn_Stop_x = pnFamt_Btn_Start_x + chgValBtnX;
int pnFamt_Btn_Stop_y = pnFamt_Btn_Start_y + chgValBtnY;

int pnStp1_Btn_Start_x = xPadding + 140;
int pnStp1_Btn_Start_y = pnFCh_Btn_Stop_y + yPadding + 20;
int pnStp1_Btn_Stop_x = pnStp1_Btn_Start_x + stepBtnX;
int pnStp1_Btn_Stop_y = pnStp1_Btn_Start_y + stepBtnY;

int pnStp2_Btn_Start_x = pnStp1_Btn_Stop_x + 10;
int pnStp2_Btn_Start_y = pnStp1_Btn_Start_y;
int pnStp2_Btn_Stop_x = pnStp2_Btn_Start_x + stepBtnX;
int pnStp2_Btn_Stop_y = pnStp2_Btn_Start_y + stepBtnY;

int pnStp3_Btn_Start_x = pnStp2_Btn_Stop_x + 10;
int pnStp3_Btn_Start_y = pnStp1_Btn_Start_y;
int pnStp3_Btn_Stop_x = pnStp3_Btn_Start_x + stepBtnX;
int pnStp3_Btn_Stop_y = pnStp3_Btn_Start_y + stepBtnY;

int pnStp4_Btn_Start_x = pnStp3_Btn_Stop_x + 10;
int pnStp4_Btn_Start_y = pnStp1_Btn_Start_y;
int pnStp4_Btn_Stop_x = pnStp4_Btn_Start_x + stepBtnX;
int pnStp4_Btn_Stop_y = pnStp4_Btn_Start_y + stepBtnY;

int pnStp5_Btn_Start_x = pnStp4_Btn_Stop_x + 10;
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

/*--------Machine Settings Screen-----------*/
//Button Sizes
int setgBtnX = 152;
int setgBtnY = 110;

//Button Positioning
int sVel_Btn_Start_x = xPadding;
int sVel_Btn_Start_y = 45;
int sVel_Btn_Stop_x = sVel_Btn_Start_x + setgBtnX;
int sVel_Btn_Stop_y = sVel_Btn_Start_y + setgBtnY;

int sAccel_Btn_Start_x = (480 / 2) - (setgBtnX / 2);
int sAccel_Btn_Start_y = 45;
int sAccel_Btn_Stop_x = sAccel_Btn_Start_x + setgBtnX;
int sAccel_Btn_Stop_y = sAccel_Btn_Start_y + setgBtnY;

int sJerk_Btn_Start_x = 480 - xPadding - setgBtnX;
int sJerk_Btn_Start_y = 45;
int sJerk_Btn_Stop_x = sJerk_Btn_Start_x + setgBtnX;
int sJerk_Btn_Stop_y = sJerk_Btn_Start_y + setgBtnY;

int sUS_Btn_Start_x = sVel_Btn_Start_x;
int sUS_Btn_Start_y = sVel_Btn_Stop_y + 5;
int sUS_Btn_Stop_x = sUS_Btn_Start_x + setgBtnX;
int sUS_Btn_Stop_y = sUS_Btn_Start_y + setgBtnY;

int sSmm_Btn_Start_x = sAccel_Btn_Start_x;
int sSmm_Btn_Start_y = sVel_Btn_Stop_y + 5;
int sSmm_Btn_Stop_x = sSmm_Btn_Start_x + setgBtnX;
int sSmm_Btn_Stop_y = sSmm_Btn_Start_y + setgBtnY;

int sRst_Btn_Start_x = sJerk_Btn_Start_x;
int sRst_Btn_Start_y = sVel_Btn_Stop_y + 5;
int sRst_Btn_Stop_x = sRst_Btn_Start_x + setgBtnX;
int sRst_Btn_Stop_y = sRst_Btn_Start_y + setgBtnY;

/*--------Microstepping Setting Screen-----------*/
//Button Sizes
int uSBtnX = 100;
int uSBtnY = 100;

int invBtnX = 25;
int invBtnY = 25;

//Button Positioning
int mAuS_Btn_Start_x = 30;
int mAuS_Btn_Start_y = 80;
int mAuS_Btn_Stop_x = mAuS_Btn_Start_x + uSBtnX;
int mAuS_Btn_Stop_y = mAuS_Btn_Start_y + uSBtnY;

int mBuS_Btn_Start_x = (480/2) - (uSBtnX / 2);
int mBuS_Btn_Start_y = 80;
int mBuS_Btn_Stop_x = mBuS_Btn_Start_x + uSBtnX;
int mBuS_Btn_Stop_y = mBuS_Btn_Start_y + uSBtnY;

int mCuS_Btn_Start_x = 480 - uSBtnX - 30;
int mCuS_Btn_Start_y = 80;
int mCuS_Btn_Stop_x = mCuS_Btn_Start_x + uSBtnX;
int mCuS_Btn_Stop_y = mCuS_Btn_Start_y + uSBtnY;

int mAinv_Btn_Start_x = mAuS_Btn_Start_x + 15;
int mAinv_Btn_Start_y = 215;
int mAinv_Btn_Stop_x = mAinv_Btn_Start_x + invBtnX;
int mAinv_Btn_Stop_y = mAinv_Btn_Start_y + invBtnY;

int mBinv_Btn_Start_x = mBuS_Btn_Start_x + 15;
int mBinv_Btn_Start_y = 215;
int mBinv_Btn_Stop_x = mBinv_Btn_Start_x + invBtnX;
int mBinv_Btn_Stop_y = mBinv_Btn_Start_y + invBtnY;

int mCinv_Btn_Start_x = mCuS_Btn_Start_x + 15;
int mCinv_Btn_Start_y = 215;
int mCinv_Btn_Stop_x = mCinv_Btn_Start_x + invBtnX;
int mCinv_Btn_Stop_y = mCinv_Btn_Start_y + invBtnY;

/*--------Velocity Settings Screen-----------*/
//Button Sizes
int field2BtnX = 100;
int field2BtnY = 40;

//Button Positioning
int svAVal_Btn_Start_x = 150;
int svAVal_Btn_Start_y = 65;
int svAVal_Btn_Stop_x = svAVal_Btn_Start_x + fieldBtnX;
int svAVal_Btn_Stop_y = svAVal_Btn_Start_y + fieldBtnY;

int svBVal_Btn_Start_x = svAVal_Btn_Start_x;
int svBVal_Btn_Start_y = svAVal_Btn_Stop_y + 40;
int svBVal_Btn_Stop_x = svBVal_Btn_Start_x + fieldBtnX;
int svBVal_Btn_Stop_y = svBVal_Btn_Start_y + fieldBtnY;

int svCVal_Btn_Start_x = svAVal_Btn_Start_x;
int svCVal_Btn_Start_y = svBVal_Btn_Stop_y + 40;
int svCVal_Btn_Stop_x = svCVal_Btn_Start_x + fieldBtnX;
int svCVal_Btn_Stop_y = svCVal_Btn_Start_y + fieldBtnY;

int svADec_Btn_Start_x = svAVal_Btn_Stop_x + 65;
int svADec_Btn_Start_y = svAVal_Btn_Start_y;
int svADec_Btn_Stop_x = svADec_Btn_Start_x + chgValBtnX;
int svADec_Btn_Stop_y = svADec_Btn_Start_y + chgValBtnY;

int svBDec_Btn_Start_x = svBVal_Btn_Stop_x + 65;
int svBDec_Btn_Start_y = svBVal_Btn_Start_y;
int svBDec_Btn_Stop_x = svBDec_Btn_Start_x + chgValBtnX;
int svBDec_Btn_Stop_y = svBDec_Btn_Start_y + chgValBtnY;

int svCDec_Btn_Start_x = svCVal_Btn_Stop_x + 65;
int svCDec_Btn_Start_y = svCVal_Btn_Start_y;
int svCDec_Btn_Stop_x = svCDec_Btn_Start_x + chgValBtnX;
int svCDec_Btn_Stop_y = svCDec_Btn_Start_y + chgValBtnY;

int svAInc_Btn_Start_x = svADec_Btn_Stop_x + 5;
int svAInc_Btn_Start_y = svAVal_Btn_Start_y;
int svAInc_Btn_Stop_x = svAInc_Btn_Start_x + chgValBtnX;
int svAInc_Btn_Stop_y = svAInc_Btn_Start_y + chgValBtnY;

int svBInc_Btn_Start_x = svBDec_Btn_Stop_x + 5;
int svBInc_Btn_Start_y = svBVal_Btn_Start_y;
int svBInc_Btn_Stop_x = svBInc_Btn_Start_x + chgValBtnX;
int svBInc_Btn_Stop_y = svBInc_Btn_Start_y + chgValBtnY;

int svCInc_Btn_Start_x = svCDec_Btn_Stop_x + 5;
int svCInc_Btn_Start_y = svCVal_Btn_Start_y;
int svCInc_Btn_Stop_x = svCInc_Btn_Start_x + chgValBtnX;
int svCInc_Btn_Stop_y = svCInc_Btn_Start_y + chgValBtnY;

int svAamt_Btn_Start_x = svAInc_Btn_Stop_x + 25;
int svAamt_Btn_Start_y = svAVal_Btn_Start_y;
int svAamt_Btn_Stop_x = svAamt_Btn_Start_x + chgValBtnX;
int svAamt_Btn_Stop_y = svAamt_Btn_Start_y + chgValBtnY;

int svBamt_Btn_Start_x = svBInc_Btn_Stop_x + 25;
int svBamt_Btn_Start_y = svBVal_Btn_Start_y;
int svBamt_Btn_Stop_x = svBamt_Btn_Start_x + chgValBtnX;
int svBamt_Btn_Stop_y = svBamt_Btn_Start_y + chgValBtnY;

int svCamt_Btn_Start_x = svCInc_Btn_Stop_x + 25;
int svCamt_Btn_Start_y = svCVal_Btn_Start_y;
int svCamt_Btn_Stop_x = svCamt_Btn_Start_x + chgValBtnX;
int svCamt_Btn_Stop_y = svCamt_Btn_Start_y + chgValBtnY;

/*--------Acceleration Settings Screen-----------*/
//Button Positioning
int saAVal_Btn_Start_x = 150;
int saAVal_Btn_Start_y = 65;
int saAVal_Btn_Stop_x = saAVal_Btn_Start_x + fieldBtnX;
int saAVal_Btn_Stop_y = saAVal_Btn_Start_y + fieldBtnY;

int saBVal_Btn_Start_x = saAVal_Btn_Start_x;
int saBVal_Btn_Start_y = saAVal_Btn_Stop_y + 40;
int saBVal_Btn_Stop_x = saBVal_Btn_Start_x + fieldBtnX;
int saBVal_Btn_Stop_y = saBVal_Btn_Start_y + fieldBtnY;

int saCVal_Btn_Start_x = saAVal_Btn_Start_x;
int saCVal_Btn_Start_y = saBVal_Btn_Stop_y + 40;
int saCVal_Btn_Stop_x = saCVal_Btn_Start_x + fieldBtnX;
int saCVal_Btn_Stop_y = saCVal_Btn_Start_y + fieldBtnY;

int saADec_Btn_Start_x = saAVal_Btn_Stop_x + 65;
int saADec_Btn_Start_y = saAVal_Btn_Start_y;
int saADec_Btn_Stop_x = saADec_Btn_Start_x + chgValBtnX;
int saADec_Btn_Stop_y = saADec_Btn_Start_y + chgValBtnY;

int saBDec_Btn_Start_x = saBVal_Btn_Stop_x + 65;
int saBDec_Btn_Start_y = saBVal_Btn_Start_y;
int saBDec_Btn_Stop_x = saBDec_Btn_Start_x + chgValBtnX;
int saBDec_Btn_Stop_y = saBDec_Btn_Start_y + chgValBtnY;

int saCDec_Btn_Start_x = saCVal_Btn_Stop_x + 65;
int saCDec_Btn_Start_y = saCVal_Btn_Start_y;
int saCDec_Btn_Stop_x = saCDec_Btn_Start_x + chgValBtnX;
int saCDec_Btn_Stop_y = saCDec_Btn_Start_y + chgValBtnY;

int saAInc_Btn_Start_x = saADec_Btn_Stop_x + 5;
int saAInc_Btn_Start_y = saAVal_Btn_Start_y;
int saAInc_Btn_Stop_x = saAInc_Btn_Start_x + chgValBtnX;
int saAInc_Btn_Stop_y = saAInc_Btn_Start_y + chgValBtnY;

int saBInc_Btn_Start_x = saBDec_Btn_Stop_x + 5;
int saBInc_Btn_Start_y = saBVal_Btn_Start_y;
int saBInc_Btn_Stop_x = saBInc_Btn_Start_x + chgValBtnX;
int saBInc_Btn_Stop_y = saBInc_Btn_Start_y + chgValBtnY;

int saCInc_Btn_Start_x = saCDec_Btn_Stop_x + 5;
int saCInc_Btn_Start_y = saCVal_Btn_Start_y;
int saCInc_Btn_Stop_x = saCInc_Btn_Start_x + chgValBtnX;
int saCInc_Btn_Stop_y = saCInc_Btn_Start_y + chgValBtnY;

int saAamt_Btn_Start_x = saAInc_Btn_Stop_x + 25;
int saAamt_Btn_Start_y = saAVal_Btn_Start_y;
int saAamt_Btn_Stop_x = saAamt_Btn_Start_x + chgValBtnX;
int saAamt_Btn_Stop_y = saAamt_Btn_Start_y + chgValBtnY;

int saBamt_Btn_Start_x = saBInc_Btn_Stop_x + 25;
int saBamt_Btn_Start_y = saBVal_Btn_Start_y;
int saBamt_Btn_Stop_x = saBamt_Btn_Start_x + chgValBtnX;
int saBamt_Btn_Stop_y = saBamt_Btn_Start_y + chgValBtnY;

int saCamt_Btn_Start_x = saCInc_Btn_Stop_x + 25;
int saCamt_Btn_Start_y = saCVal_Btn_Start_y;
int saCamt_Btn_Stop_x = saCamt_Btn_Start_x + chgValBtnX;
int saCamt_Btn_Stop_y = saCamt_Btn_Start_y + chgValBtnY;

/*--------Jerk Settings Screen-----------*/
//Button Positioning
int sjAVal_Btn_Start_x = 150;
int sjAVal_Btn_Start_y = 65;
int sjAVal_Btn_Stop_x = sjAVal_Btn_Start_x + fieldBtnX;
int sjAVal_Btn_Stop_y = sjAVal_Btn_Start_y + fieldBtnY;

int sjBVal_Btn_Start_x = sjAVal_Btn_Start_x;
int sjBVal_Btn_Start_y = sjAVal_Btn_Stop_y + 40;
int sjBVal_Btn_Stop_x = sjBVal_Btn_Start_x + fieldBtnX;
int sjBVal_Btn_Stop_y = sjBVal_Btn_Start_y + fieldBtnY;

int sjCVal_Btn_Start_x = sjAVal_Btn_Start_x;
int sjCVal_Btn_Start_y = sjBVal_Btn_Stop_y + 40;
int sjCVal_Btn_Stop_x = sjCVal_Btn_Start_x + fieldBtnX;
int sjCVal_Btn_Stop_y = sjCVal_Btn_Start_y + fieldBtnY;

int sjADec_Btn_Start_x = sjAVal_Btn_Stop_x + 65;
int sjADec_Btn_Start_y = sjAVal_Btn_Start_y;
int sjADec_Btn_Stop_x = sjADec_Btn_Start_x + chgValBtnX;
int sjADec_Btn_Stop_y = sjADec_Btn_Start_y + chgValBtnY;

int sjBDec_Btn_Start_x = sjBVal_Btn_Stop_x + 65;
int sjBDec_Btn_Start_y = sjBVal_Btn_Start_y;
int sjBDec_Btn_Stop_x = sjBDec_Btn_Start_x + chgValBtnX;
int sjBDec_Btn_Stop_y = sjBDec_Btn_Start_y + chgValBtnY;

int sjCDec_Btn_Start_x = sjCVal_Btn_Stop_x + 65;
int sjCDec_Btn_Start_y = sjCVal_Btn_Start_y;
int sjCDec_Btn_Stop_x = sjCDec_Btn_Start_x + chgValBtnX;
int sjCDec_Btn_Stop_y = sjCDec_Btn_Start_y + chgValBtnY;

int sjAInc_Btn_Start_x = sjADec_Btn_Stop_x + 5;
int sjAInc_Btn_Start_y = sjAVal_Btn_Start_y;
int sjAInc_Btn_Stop_x = sjAInc_Btn_Start_x + chgValBtnX;
int sjAInc_Btn_Stop_y = sjAInc_Btn_Start_y + chgValBtnY;

int sjBInc_Btn_Start_x = sjBDec_Btn_Stop_x + 5;
int sjBInc_Btn_Start_y = sjBVal_Btn_Start_y;
int sjBInc_Btn_Stop_x = sjBInc_Btn_Start_x + chgValBtnX;
int sjBInc_Btn_Stop_y = sjBInc_Btn_Start_y + chgValBtnY;

int sjCInc_Btn_Start_x = sjCDec_Btn_Stop_x + 5;
int sjCInc_Btn_Start_y = sjCVal_Btn_Start_y;
int sjCInc_Btn_Stop_x = sjCInc_Btn_Start_x + chgValBtnX;
int sjCInc_Btn_Stop_y = sjCInc_Btn_Start_y + chgValBtnY;

int sjAamt_Btn_Start_x = sjAInc_Btn_Stop_x + 25;
int sjAamt_Btn_Start_y = sjAVal_Btn_Start_y;
int sjAamt_Btn_Stop_x = sjAamt_Btn_Start_x + chgValBtnX;
int sjAamt_Btn_Stop_y = sjAamt_Btn_Start_y + chgValBtnY;

int sjBamt_Btn_Start_x = sjBInc_Btn_Stop_x + 25;
int sjBamt_Btn_Start_y = sjBVal_Btn_Start_y;
int sjBamt_Btn_Stop_x = sjBamt_Btn_Start_x + chgValBtnX;
int sjBamt_Btn_Stop_y = sjBamt_Btn_Start_y + chgValBtnY;

int sjCamt_Btn_Start_x = sjCInc_Btn_Stop_x + 25;
int sjCamt_Btn_Start_y = sjCVal_Btn_Start_y;
int sjCamt_Btn_Stop_x = sjCamt_Btn_Start_x + chgValBtnX;
int sjCamt_Btn_Stop_y = sjCamt_Btn_Start_y + chgValBtnY;

/*--------Steps/mm Settings Screen-----------*/
//Button Positioning
int ssmmAVal_Btn_Start_x = 150;
int ssmmAVal_Btn_Start_y = 65;
int ssmmAVal_Btn_Stop_x = ssmmAVal_Btn_Start_x + fieldBtnX;
int ssmmAVal_Btn_Stop_y = ssmmAVal_Btn_Start_y + fieldBtnY;

int ssmmBVal_Btn_Start_x = ssmmAVal_Btn_Start_x;
int ssmmBVal_Btn_Start_y = ssmmAVal_Btn_Stop_y + 40;
int ssmmBVal_Btn_Stop_x = ssmmBVal_Btn_Start_x + fieldBtnX;
int ssmmBVal_Btn_Stop_y = ssmmBVal_Btn_Start_y + fieldBtnY;

int ssmmCVal_Btn_Start_x = ssmmAVal_Btn_Start_x;
int ssmmCVal_Btn_Start_y = ssmmBVal_Btn_Stop_y + 40;
int ssmmCVal_Btn_Stop_x = ssmmCVal_Btn_Start_x + fieldBtnX;
int ssmmCVal_Btn_Stop_y = ssmmCVal_Btn_Start_y + fieldBtnY;

int ssmmADec_Btn_Start_x = ssmmAVal_Btn_Stop_x + 65;
int ssmmADec_Btn_Start_y = ssmmAVal_Btn_Start_y;
int ssmmADec_Btn_Stop_x = ssmmADec_Btn_Start_x + chgValBtnX;
int ssmmADec_Btn_Stop_y = ssmmADec_Btn_Start_y + chgValBtnY;

int ssmmBDec_Btn_Start_x = ssmmBVal_Btn_Stop_x + 65;
int ssmmBDec_Btn_Start_y = ssmmBVal_Btn_Start_y;
int ssmmBDec_Btn_Stop_x = ssmmBDec_Btn_Start_x + chgValBtnX;
int ssmmBDec_Btn_Stop_y = ssmmBDec_Btn_Start_y + chgValBtnY;

int ssmmCDec_Btn_Start_x = ssmmCVal_Btn_Stop_x + 65;
int ssmmCDec_Btn_Start_y = ssmmCVal_Btn_Start_y;
int ssmmCDec_Btn_Stop_x = ssmmCDec_Btn_Start_x + chgValBtnX;
int ssmmCDec_Btn_Stop_y = ssmmCDec_Btn_Start_y + chgValBtnY;

int ssmmAInc_Btn_Start_x = ssmmADec_Btn_Stop_x + 5;
int ssmmAInc_Btn_Start_y = ssmmAVal_Btn_Start_y;
int ssmmAInc_Btn_Stop_x = ssmmAInc_Btn_Start_x + chgValBtnX;
int ssmmAInc_Btn_Stop_y = ssmmAInc_Btn_Start_y + chgValBtnY;

int ssmmBInc_Btn_Start_x = ssmmBDec_Btn_Stop_x + 5;
int ssmmBInc_Btn_Start_y = ssmmBVal_Btn_Start_y;
int ssmmBInc_Btn_Stop_x = ssmmBInc_Btn_Start_x + chgValBtnX;
int ssmmBInc_Btn_Stop_y = ssmmBInc_Btn_Start_y + chgValBtnY;

int ssmmCInc_Btn_Start_x = ssmmCDec_Btn_Stop_x + 5;
int ssmmCInc_Btn_Start_y = ssmmCVal_Btn_Start_y;
int ssmmCInc_Btn_Stop_x = ssmmCInc_Btn_Start_x + chgValBtnX;
int ssmmCInc_Btn_Stop_y = ssmmCInc_Btn_Start_y + chgValBtnY;

int ssmmAamt_Btn_Start_x = ssmmAInc_Btn_Stop_x + 25;
int ssmmAamt_Btn_Start_y = ssmmAVal_Btn_Start_y;
int ssmmAamt_Btn_Stop_x = ssmmAamt_Btn_Start_x + chgValBtnX;
int ssmmAamt_Btn_Stop_y = ssmmAamt_Btn_Start_y + chgValBtnY;

int ssmmBamt_Btn_Start_x = ssmmBInc_Btn_Stop_x + 25;
int ssmmBamt_Btn_Start_y = ssmmBVal_Btn_Start_y;
int ssmmBamt_Btn_Stop_x = ssmmBamt_Btn_Start_x + chgValBtnX;
int ssmmBamt_Btn_Stop_y = ssmmBamt_Btn_Start_y + chgValBtnY;

int ssmmCamt_Btn_Start_x = ssmmCInc_Btn_Stop_x + 25;
int ssmmCamt_Btn_Start_y = ssmmCVal_Btn_Start_y;
int ssmmCamt_Btn_Stop_x = ssmmCamt_Btn_Start_x + chgValBtnX;
int ssmmCamt_Btn_Stop_y = ssmmCamt_Btn_Start_y + chgValBtnY;

/*--------Data Entry Screen-----------*/
//Button Sizes
int numBtnX = 153;
int numBtnY = 58;

//Button Positioning
int numDel_Btn_Start_x = 480 - xPadding - (numBtnX / 2);
int numDel_Btn_Start_y = yPadding;
int numDel_Btn_Stop_x = numDel_Btn_Start_x + (numBtnX / 2);
int numDel_Btn_Stop_y = numDel_Btn_Start_y + numBtnY;

int numLett_Btn_Start_x = xPadding;
int numLett_Btn_Start_y = yPadding;
int numLett_Btn_Stop_x = numLett_Btn_Start_x + (numBtnX / 2);
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
  Serial.begin(9600);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

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
  
  /***Screen Setup***/
  tft.Init_LCD();
  tft.Set_Rotation(1);
  tft.Fill_Screen(BLACK);    //Change the screen to solid white
  tft.Set_Text_Mode(0);

  //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
  activeScreen = 0;
  drawMainScreen();
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
  //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
  if(activeScreen == 0) {
    updateMainScreen();
  }
  else if(activeScreen == 1) {
    updateProfileLoadScreen();
  }
  else if(activeScreen == 2) {
    updateNewProfileScreen();
  }
  else if(activeScreen == 3) {
    updateEditProfileScreen();
  }
  else if(activeScreen == 4) {
    updateMachineSettingsScreen();
  }
  else if(activeScreen == 5){ 
    updateInputScreen();
  }
  else if(activeScreen == 6) {
    updateVelScreen();
  }
  else if(activeScreen == 7) {
    updateAccelScreen();
  }
  else if(activeScreen == 8) {
    updateJerkScreen();
  }
  else if(activeScreen = 9) {
    updateMicrostepScreen();
  }
  else if(activeScreen == 10) {
    updateStepsMMScreen();
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
  tft.Print_String("Top Clamp Position: ", 25, (yPadding*4) + settingBtnY + 20);

  //Top Clamp Position Text
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String((String)convertStepsToDistance(mStepsB, mStepsPerB, mUStepB) + "mm", xPadding + 180, (yPadding*4) + settingBtnY + 20);

  //Bottom Clamp Position Title
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String("Bottom Clamp Position: ", 25, (yPadding*5) + settingBtnY + 40);
 
  //Bottom Clamp Position Text
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String((String)convertStepsToDistance(mStepsC, mStepsPerC, mUStepC) + "mm", xPadding + 180, (yPadding*5) + settingBtnY + 40);

  //Vertical Chassis Position Title
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String("Blade Position: ", 25, (yPadding*6) + settingBtnY + 60);

  //Vertical Chassis Position Text
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Set_Text_Size(1);
  tft.Print_String((String)convertStepsToDistance(mStepsA, mStepsPerA, mUStepA) + "mm", xPadding + 180, (yPadding*6) + settingBtnY + 60);
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
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
    activeScreen = 1;
    drawProfileLoadScreen();
    delay(50);
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
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
    activeScreen = 2;
    drawNewProfileScreen();
    delay(50);
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
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
    activeScreen = 3;
    drawEditProfileScreen();
    delay(50);
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
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
    activeScreen = 4;
    drawMachineSettingsScreen();
    delay(50);
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
  tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);

  //Value Text
  tft.Set_Text_Size(4);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
  
  //Button 'Delete'
  tft.Set_Draw_color(RED);
  tft.Fill_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(RED);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("DEL", numDel_Btn_Start_x + 15, numDel_Btn_Start_y + 18);

  //Button 'Letter'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(numLett_Btn_Start_x, numLett_Btn_Start_y, numLett_Btn_Stop_x, numLett_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(numLett_Btn_Start_x, numLett_Btn_Start_y, numLett_Btn_Stop_x, numLett_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("ABC", numLett_Btn_Start_x + 13, numLett_Btn_Start_y + 18);

  //Button '1'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num1_Btn_Start_x, num1_Btn_Start_y, num1_Btn_Stop_x, num1_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num1_Btn_Start_x, num1_Btn_Start_y, num1_Btn_Stop_x, num1_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("1", num1_Btn_Start_x + 70, num1_Btn_Start_y + 18);

  //Button '2'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num2_Btn_Start_x, num2_Btn_Start_y, num2_Btn_Stop_x, num2_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num2_Btn_Start_x, num2_Btn_Start_y, num2_Btn_Stop_x, num2_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("2", num2_Btn_Start_x + 70, num2_Btn_Start_y + 18);

  //Button '3'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num3_Btn_Start_x, num3_Btn_Start_y, num3_Btn_Stop_x, num3_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num3_Btn_Start_x, num3_Btn_Start_y, num3_Btn_Stop_x, num3_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("3", num3_Btn_Start_x + 70, num3_Btn_Start_y + 18);

  //Button '4'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num4_Btn_Start_x, num4_Btn_Start_y, num4_Btn_Stop_x, num4_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num4_Btn_Start_x, num4_Btn_Start_y, num4_Btn_Stop_x, num4_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("4", num4_Btn_Start_x + 70, num4_Btn_Start_y + 18);
    
  //Button '5'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num5_Btn_Start_x, num5_Btn_Start_y, num5_Btn_Stop_x, num5_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num5_Btn_Start_x, num5_Btn_Start_y, num5_Btn_Stop_x, num5_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("5", num5_Btn_Start_x + 70, num5_Btn_Start_y + 18);

  //Button '6'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num6_Btn_Start_x, num6_Btn_Start_y, num6_Btn_Stop_x, num6_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num6_Btn_Start_x, num6_Btn_Start_y, num6_Btn_Stop_x, num6_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("6", num6_Btn_Start_x + 70, num6_Btn_Start_y + 18);

  //Button '7'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num7_Btn_Start_x, num7_Btn_Start_y, num7_Btn_Stop_x, num7_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num7_Btn_Start_x, num7_Btn_Start_y, num7_Btn_Stop_x, num7_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("7", num7_Btn_Start_x + 70, num7_Btn_Start_y + 18);

  //Button '8'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num8_Btn_Start_x, num8_Btn_Start_y, num8_Btn_Stop_x, num8_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num8_Btn_Start_x, num8_Btn_Start_y, num8_Btn_Stop_x, num8_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("8", num8_Btn_Start_x + 70, num8_Btn_Start_y + 18);

  //Button '9'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(num9_Btn_Start_x, num9_Btn_Start_y, num9_Btn_Stop_x, num9_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(num9_Btn_Start_x, num9_Btn_Start_y, num9_Btn_Stop_x, num9_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("9", num9_Btn_Start_x + 70, num9_Btn_Start_y + 18);

  //Button 'Back'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(numBck_Btn_Start_x, numBck_Btn_Start_y, numBck_Btn_Stop_x, numBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(numBck_Btn_Start_x, numBck_Btn_Start_y, numBck_Btn_Stop_x, numBck_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", numBck_Btn_Start_x + 45, numBck_Btn_Start_y + 18);

  //Button '.'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String(".", numPd_Btn_Start_x + 68, numPd_Btn_Start_y + 18);

  //Button 'Ok'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
  tft.Set_Text_Size(3);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("OK", numOk_Btn_Start_x + 60, numOk_Btn_Start_y + 18);
}

/**Get touch readings and redraw changes**/
//TODO: Adjust input and delete operation to work from right end instead of the decimal point seperating int and float
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
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
    tft.Set_Draw_color(RED);
    tft.Draw_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("DEL", numDel_Btn_Start_x + 15, numDel_Btn_Start_y + 18);
    
    //Flag the delete button as pressed
    delBtnFlag = true;
    
    unsigned int len = inputBuilder.length();
    inputBuilder.remove(len-1);
    inputBuilder.trim();

    tft.Set_Draw_color(BLACK);
    tft.Fill_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
    tft.Set_Draw_color(YELLOW);
    tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
    tft.Set_Text_Size(4);
    tft.Set_Text_Back_colour(BLACK);
    tft.Set_Text_colour(YELLOW);
    tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
    delay(50);
  }
  else if(delBtnFlag) {
    //Button 'Delete'
    tft.Set_Draw_color(RED);
    tft.Fill_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(numDel_Btn_Start_x, numDel_Btn_Start_y, numDel_Btn_Stop_x, numDel_Btn_Stop_y);
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(RED);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("DEL", numDel_Btn_Start_x + 15, numDel_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("ABC", numLett_Btn_Start_x + 13, numLett_Btn_Start_y + 18);
      delay(50);
    }
    else {
      //Button 'Letter'
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(numLett_Btn_Start_x, numLett_Btn_Start_y, numLett_Btn_Stop_x, numLett_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(numLett_Btn_Start_x, numLett_Btn_Start_y, numLett_Btn_Stop_x, numLett_Btn_Stop_y);
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("ABC", numLett_Btn_Start_x + 13, numLett_Btn_Start_y + 18);
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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("ABC", num1_Btn_Start_x + 70, num1_Btn_Start_y + 18);

      //TODO: Replace while loops with wait function implementation
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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("1", num1_Btn_Start_x + 70, num1_Btn_Start_y + 18);

      inputBuilder += "1";
     
      tft.Set_Draw_color(YELLOW);
      tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
      tft.Set_Text_Size(4);
      tft.Set_Text_Back_colour(BLACK);
      tft.Set_Text_colour(YELLOW);
      //tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
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
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("1", num1_Btn_Start_x + 70, num1_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("DEF", num2_Btn_Start_x + 70, num2_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("2", num2_Btn_Start_x + 70, num2_Btn_Start_y + 18);
      
      inputBuilder += "2";

      tft.Set_Draw_color(YELLOW);
      tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
      tft.Set_Text_Size(4);
      tft.Set_Text_Back_colour(BLACK);
      tft.Set_Text_colour(YELLOW);
      //tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
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
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("2", num2_Btn_Start_x + 70, num2_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("GHI", num3_Btn_Start_x + 70, num3_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("3", num3_Btn_Start_x + 70, num3_Btn_Start_y + 18);

      inputBuilder += "3";
      
      tft.Set_Draw_color(YELLOW);
      tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
      tft.Set_Text_Size(4);
      tft.Set_Text_Back_colour(BLACK);
      tft.Set_Text_colour(YELLOW);
      //tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
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
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("3", num3_Btn_Start_x + 70, num3_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("JKL", num4_Btn_Start_x + 70, num4_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("4", num4_Btn_Start_x + 70, num4_Btn_Start_y + 18);

      inputBuilder += "4";
      
      tft.Set_Draw_color(YELLOW);
      tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
      tft.Set_Text_Size(4);
      tft.Set_Text_Back_colour(BLACK);
      tft.Set_Text_colour(YELLOW);
      //tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
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
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("4", num4_Btn_Start_x + 70, num4_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("MNO", num5_Btn_Start_x + 70, num5_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("5", num5_Btn_Start_x + 70, num5_Btn_Start_y + 18);
     
      inputBuilder += "5";

      tft.Set_Draw_color(YELLOW);
      tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
      tft.Set_Text_Size(4);
      tft.Set_Text_Back_colour(BLACK);
      tft.Set_Text_colour(YELLOW);
      //tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
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
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("5", num5_Btn_Start_x + 70, num5_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("PQR", num6_Btn_Start_x + 70, num6_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("6", num6_Btn_Start_x + 70, num6_Btn_Start_y + 18);
  
      inputBuilder += "6";

      tft.Set_Draw_color(YELLOW);
      tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
      tft.Set_Text_Size(4);
      tft.Set_Text_Back_colour(BLACK);
      tft.Set_Text_colour(YELLOW);
      //tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
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
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("6", num6_Btn_Start_x + 70, num6_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("STU", num7_Btn_Start_x + 705, num7_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("7", num7_Btn_Start_x + 70, num7_Btn_Start_y + 18);
    
      inputBuilder += "7";
      
      tft.Set_Draw_color(YELLOW);
      tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
      tft.Set_Text_Size(4);
      tft.Set_Text_Back_colour(BLACK);
      tft.Set_Text_colour(YELLOW);
      //tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
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
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("7", num7_Btn_Start_x + 70, num7_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("VWX", num8_Btn_Start_x + 70, num8_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("8", num8_Btn_Start_x + 70, num8_Btn_Start_y + 18);

      inputBuilder += "8";
      
      tft.Set_Draw_color(YELLOW);
      tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
      tft.Set_Text_Size(4);
      tft.Set_Text_Back_colour(BLACK);
      tft.Set_Text_colour(YELLOW);
      //tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
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
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("8", num8_Btn_Start_x + 70, num8_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("YZ", num9_Btn_Start_x + 70, num9_Btn_Start_y + 18);

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
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String("9", num9_Btn_Start_x + 70, num9_Btn_Start_y + 18);
      
      inputBuilder += "9";
 
      tft.Set_Draw_color(YELLOW);
      tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
      tft.Set_Text_Size(4);
      tft.Set_Text_Back_colour(BLACK);
      tft.Set_Text_colour(YELLOW);
      //tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
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
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("9", num9_Btn_Start_x + 45, num9_Btn_Start_y + 18);

    n9BtnFlag = false;
  }

  //Back Button
  if(x > numBck_Btn_Start_x && x < numBck_Btn_Stop_x && y > numBck_Btn_Start_y && y < numBck_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Back'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(numBck_Btn_Start_x, numBck_Btn_Start_y, numBck_Btn_Stop_x, numBck_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(numBck_Btn_Start_x, numBck_Btn_Start_y, numBck_Btn_Stop_x, numBck_Btn_Stop_y);
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Back", numBck_Btn_Start_x + 45, numBck_Btn_Start_y + 18);

    if(prevActiveScreen == 0) {
      activeScreen = prevActiveScreen;
      drawMainScreen();
    }
    else if(prevActiveScreen == 1) {
      activeScreen = prevActiveScreen;
      drawProfileLoadScreen();
    }
    else if(prevActiveScreen == 2) {
      activeScreen = prevActiveScreen;
      drawNewProfileScreen();
    }
    else if(prevActiveScreen == 3) {
      activeScreen = prevActiveScreen;
      drawEditProfileScreen();
    }
    else if(prevActiveScreen == 4) {
      activeScreen = prevActiveScreen;
      drawMachineSettingsScreen();
    }
    else if(prevActiveScreen == 5){ 
      activeScreen = prevActiveScreen;
      drawInputScreen();
    }
    else if(prevActiveScreen == 6) {
      activeScreen = prevActiveScreen;
      drawVelScreen();
    }
    else if(prevActiveScreen == 7) {
      activeScreen = prevActiveScreen;
      drawAccelScreen();
    }
    else if(prevActiveScreen == 8) {
      activeScreen = prevActiveScreen;
      drawJerkScreen();
    }
    else if(prevActiveScreen = 9) {
      activeScreen = prevActiveScreen;
      drawMicrostepScreen();
    }
    else if(prevActiveScreen == 10) {
      activeScreen = prevActiveScreen;
      drawStepsMMScreen();
    }
    delay(50);
  }

  //'Decimal' Button
  if(x > numPd_Btn_Start_x && x < numPd_Btn_Stop_x && y > numPd_Btn_Start_y && y < numPd_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    decimalBtnFlag = !decimalBtnFlag;
    
    if(decimalBtnFlag) {
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(GRAY);
      tft.Set_Text_colour(BLACK);
      tft.Print_String(".", numPd_Btn_Start_x + 68, numPd_Btn_Start_y + 18);

      inputBuilder += ".";
            
      //Flag the decimal button as pressed
      decimalBtnFlag = true;  
      
      tft.Set_Draw_color(YELLOW);
      tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
      tft.Set_Text_Size(4);
      tft.Set_Text_Back_colour(BLACK);
      tft.Set_Text_colour(YELLOW);
      //tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      tft.Print_String(inputBuilder, numBtnX + 45, yPadding + 18);
      
      delay(50);
    }
    else {
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(numPd_Btn_Start_x, numPd_Btn_Start_y, numPd_Btn_Stop_x, numPd_Btn_Stop_y);
      tft.Set_Text_Size(3);
      tft.Set_Text_Back_colour(WHITE);
      tft.Set_Text_colour(BLACK);
      tft.Print_String(".", numPd_Btn_Start_x + 68, numPd_Btn_Start_y + 18);
  
      decimalBtnFlag = false;
      delay(50);
    }
  }
  
  //'Ok' Button
  if(x > numOk_Btn_Start_x && x < numOk_Btn_Stop_x && y > numOk_Btn_Start_y && y < numOk_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Ok'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("OK", numOk_Btn_Start_x + 60, numOk_Btn_Start_y + 18);
    
    //Flag the delete button as pressed
    okBtnFlag = true;

    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    switch(varInputRegister) {
      case(0):
        break;
      case(1):
        profile = inputBuilder;
        break;
      case(2):
        tClampDist = inputBuilder.toFloat();
        break;
      case(3):
        bClampDist = inputBuilder.toFloat();
        break;
      case(4):
        hDist1 = inputBuilder.toFloat();
        break;
      case(5):
        hDist2 = inputBuilder.toFloat();
        break;
      case(6):
        hDist3 = inputBuilder.toFloat();
        break;
      case(7):
        mVelocityA = inputBuilder.toFloat();
        break;
      case(8):
        mVelocityB = inputBuilder.toFloat();
        break;
      case(9):
        mVelocityC = inputBuilder.toFloat();
        break;
      case(10):
        mAccelA = inputBuilder.toFloat();
        break;
      case(11):
        mAccelB = inputBuilder.toFloat();
        break;
      case(12):
        mAccelC = inputBuilder.toFloat();
        break;
      case(13):
        mJerkA = inputBuilder.toFloat();
        break;
      case(14):
        mJerkB = inputBuilder.toFloat();
        break;
      case(15):
        mJerkC = inputBuilder.toFloat();
        break;
      case(16):
        //mUStepA = inputBuilder.toFloat();
        break;
      case(17):
        //mUStepB = inputBuilder.toFloat();
        break;
      case(18):
        //mUStepC = inputBuilder.toFloat();
        break;
      case(19):
        mStepsPerA = inputBuilder.toFloat();
        break;
      case(20):
        mStepsPerB = inputBuilder.toFloat();
        break;
      case(21):
        mStepsPerC = inputBuilder.toFloat();
        break;
      case(22):
        homeOffsetT = inputBuilder.toFloat();
        break;
      case(23):
        homeOffsetB = inputBuilder.toFloat();
        break;
      case(24):
        homeOffsetClampT = inputBuilder.toFloat();
        break;
      case(25):
        homeOffsetClampB = inputBuilder.toFloat();
        break;
    }

    switch(prevActiveScreen) {
      case(0):
        activeScreen = prevActiveScreen;
        drawMainScreen();
        break;
      case(1):
        activeScreen = prevActiveScreen;
        drawProfileLoadScreen();      
        break;
      case(2):
        activeScreen = prevActiveScreen;
        drawNewProfileScreen();     
        break;
      case(3):
        activeScreen = prevActiveScreen;
        drawEditProfileScreen();
        break;
      case(4):
        activeScreen = prevActiveScreen;
        drawMachineSettingsScreen();
        break;
      case(5):
        activeScreen = prevActiveScreen;
        drawInputScreen();     
        break;
      case(6):
        activeScreen = prevActiveScreen;
        drawVelScreen();
        break;
      case(7):
        activeScreen = prevActiveScreen;
        drawAccelScreen();
        break;
      case(8):
        activeScreen = prevActiveScreen;
        drawJerkScreen();
        break;
      case(9):
        activeScreen = prevActiveScreen;
        drawMicrostepScreen();
        break;
      case(10):
        activeScreen = prevActiveScreen;
        drawStepsMMScreen();
        break;
    }
    delay(50);
  }
  else if(okBtnFlag) {
    //Button 'Ok'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(numOk_Btn_Start_x, numOk_Btn_Start_y, numOk_Btn_Stop_x, numOk_Btn_Stop_y);
    tft.Set_Text_Size(3);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("OK", numOk_Btn_Start_x + 60, numOk_Btn_Start_y + 18);

    okBtnFlag = false;
  }
}

/**Draw the profile loading screen**/
void drawProfileLoadScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black

  //Machine Settings Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 35);

  //Machine Settings Title
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Load Profile", 200, yPadding + 12);
  
  numSavedProfiles = 0;
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
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pBack_Btn_Start_x + 25, pBack_Btn_Start_y + 17);

  //Button 'Confirm'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pCfm_Btn_Start_x, pCfm_Btn_Start_y, pCfm_Btn_Stop_x, pCfm_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pCfm_Btn_Start_x, pCfm_Btn_Start_y, pCfm_Btn_Stop_x, pCfm_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Confirm", pCfm_Btn_Start_x + 25, pCfm_Btn_Start_y + 17);

  if(numSavedProfiles >= 1) {
    //Button 'Profile 1'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p1_Btn_Start_x, p1_Btn_Start_y, p1_Btn_Stop_x, p1_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p1_Btn_Start_x, p1_Btn_Start_y, p1_Btn_Stop_x, p1_Btn_Stop_y);
    tft.Set_Text_Size(2);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[0], p1_Btn_Start_x + 17, p1_Btn_Start_y + 17);
  }

  if(numSavedProfiles >= 2) {
    //Button 'Profile 2'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p2_Btn_Start_x, p2_Btn_Start_y, p2_Btn_Stop_x, p2_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p2_Btn_Start_x, p2_Btn_Start_y, p2_Btn_Stop_x, p2_Btn_Stop_y);
    tft.Set_Text_Size(2);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[1], p2_Btn_Start_x + 17, p2_Btn_Start_y + 17);
  }

  if(numSavedProfiles >= 3) {
    //Button 'Profile 3'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p3_Btn_Start_x, p3_Btn_Start_y, p3_Btn_Stop_x, p2_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p3_Btn_Start_x, p3_Btn_Start_y, p3_Btn_Stop_x, p2_Btn_Stop_y);
    tft.Set_Text_Size(2);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[2], p3_Btn_Start_x + 17, p3_Btn_Start_y + 17);
  }

  if(numSavedProfiles >= 4) {
    //Button 'Profile 4'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p4_Btn_Start_x, p4_Btn_Start_y, p4_Btn_Stop_x, p4_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p4_Btn_Start_x, p4_Btn_Start_y, p4_Btn_Stop_x, p4_Btn_Stop_y);
    tft.Set_Text_Size(2);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[3], p4_Btn_Start_x + 17, p4_Btn_Start_y + 17);
  }

  if(numSavedProfiles >= 5) {
    //Button 'Profile 5'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p5_Btn_Start_x, p5_Btn_Start_y, p5_Btn_Stop_x, p5_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p5_Btn_Start_x, p5_Btn_Start_y, p5_Btn_Stop_x, p5_Btn_Stop_y);
    tft.Set_Text_Size(2);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[4], p5_Btn_Start_x + 17, p5_Btn_Start_y + 17);
  }

  if(numSavedProfiles >= 6) {
    //Button 'Profile 6'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p6_Btn_Start_x, p6_Btn_Start_y, p6_Btn_Stop_x, p6_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p6_Btn_Start_x, p6_Btn_Start_y, p6_Btn_Stop_x, p6_Btn_Stop_y);
    tft.Set_Text_Size(2);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[5], p6_Btn_Start_x + 17, p6_Btn_Start_y + 17);
  }

  if(numSavedProfiles >= 7) {
    //Button 'Profile 7'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p7_Btn_Start_x, p7_Btn_Start_y, p7_Btn_Stop_x, p7_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p7_Btn_Start_x, p7_Btn_Start_y, p7_Btn_Stop_x, p7_Btn_Stop_y);
    tft.Set_Text_Size(2);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[6], p7_Btn_Start_x + 17, p7_Btn_Start_y + 17);
  }

  if(numSavedProfiles >= 8) {
    //Button 'Profile 8'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(p8_Btn_Start_x, p8_Btn_Start_y, p8_Btn_Stop_x, p8_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(p8_Btn_Start_x, p8_Btn_Start_y, p8_Btn_Stop_x, p8_Btn_Stop_y);
    tft.Set_Text_Size(2);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(profiles[7], p8_Btn_Start_x + 17, p8_Btn_Start_y + 17);
  }

  if(numSavedProfiles >= 9) {
  //Button 'Profile 9'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(p9_Btn_Start_x, p9_Btn_Start_y, p9_Btn_Stop_x, p9_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(p9_Btn_Start_x, p9_Btn_Start_y, p9_Btn_Stop_x, p9_Btn_Stop_y);
  tft.Set_Text_Size(2);
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
    tft.Set_Text_Size(2);
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

    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
    activeScreen = 0;
    drawMainScreen();
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
    
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
    activeScreen = 0;
    drawMainScreen();
    //TODO: Load selected profile and move to position
    delay(50);
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
  if(numSavedProfiles >= 1 && x > p1_Btn_Start_x && x < p1_Btn_Stop_x && y > p1_Btn_Start_y && y < p1_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
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
      //TODO: 
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
  }

  //'Profile 2' Button
  if(numSavedProfiles >= 2 && x > p2_Btn_Start_x && x < p2_Btn_Stop_x && y > p2_Btn_Start_y && y < p2_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
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
      //TODO:
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
  }

  //'Profile 3' Button
  if(numSavedProfiles >= 3 && x > p3_Btn_Start_x && x < p3_Btn_Stop_x && y > p3_Btn_Start_y && y < p3_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
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
      //TODO:
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
  }

  //'Profile 4' Button
  if(numSavedProfiles >= 4 && x > p4_Btn_Start_x && x < p4_Btn_Stop_x && y > p4_Btn_Start_y && y < p4_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
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
      //TODO:

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
  }

  //'Profile 5' Button
  if(numSavedProfiles >= 5 && x > p5_Btn_Start_x && x < p5_Btn_Stop_x && y > p5_Btn_Start_y && y < p5_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
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
      //TODO:
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
  }
  
  //'Profile 6' Button
  if(numSavedProfiles >= 6 && x > p6_Btn_Start_x && x < p6_Btn_Stop_x && y > p6_Btn_Start_y && y < p6_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
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
      //TODO:
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
  }

  //'Profile 7' Button
  if(numSavedProfiles >= 7 && x > p7_Btn_Start_x && x < p7_Btn_Stop_x && y > p7_Btn_Start_y && y < p7_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
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
      //TODO:
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
  }

  //'Profile 8' Button
  if(numSavedProfiles >= 8 && x > p8_Btn_Start_x && x < p8_Btn_Stop_x && y > p8_Btn_Start_y && y < p8_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
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
      //TODO:
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
  }

  //'Profile 9' Button
  if(numSavedProfiles >= 9 && x > p9_Btn_Start_x && x < p9_Btn_Stop_x && y > p9_Btn_Start_y && y < p9_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
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
      //TODO:
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
  }

  //'Profile 10' Button
  if(numSavedProfiles == 10 && x > p10_Btn_Start_x && x < p10_Btn_Stop_x && y > p10_Btn_Start_y && y < p10_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
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
      //TODO:
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
}

/**Draw the new profile creation screen **/
void drawNewProfileScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black

  //Profile Configuration Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 35);

  //Profile Configuration Title
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Profile Configuration", 115, yPadding + 12);

  //Profile Name Text
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(GREEN);
  tft.Print_String("Profile Name: ", 10, pnName_Btn_Start_y + 12);

  //Profile Name Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnName_Btn_Start_x, pnName_Btn_Start_y, pnName_Btn_Stop_x, pnName_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnName_Btn_Start_x, pnName_Btn_Start_y, pnName_Btn_Stop_x, pnName_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("(tap to input)", pnName_Btn_Start_x + 17, pnName_Btn_Start_y + 13);

  //Top Clamp Position Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Top Clamp Position: ", 30, pnTClamp_Btn_Start_y + 17);

  //Top Clamp Position Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnTClamp_Btn_Start_x, pnTClamp_Btn_Start_y, pnTClamp_Btn_Stop_x, pnTClamp_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnTClamp_Btn_Start_x, pnTClamp_Btn_Start_y, pnTClamp_Btn_Stop_x, pnTClamp_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.00", pnTClamp_Btn_Start_x + 27, pnTClamp_Btn_Start_y + 14);

  //Bottom Clamp Position Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Bottom Clamp Position: ", 30, pnBClamp_Btn_Start_y + 17);

  //Bottom Clamp Position Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.00", pnBClamp_Btn_Start_x + 27, pnBClamp_Btn_Start_y + 14);

  //Frame Chassis Position Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Frame Chassis Position: ", 30, pnFCh_Btn_Start_y + 17);

  //Frame Chassis Position Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnFCh_Btn_Start_x, pnFCh_Btn_Start_y, pnFCh_Btn_Stop_x, pnFCh_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnFCh_Btn_Start_x, pnFCh_Btn_Start_y, pnFCh_Btn_Stop_x, pnFCh_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.00", pnFCh_Btn_Start_x + 27, pnFCh_Btn_Start_y + 14);

  //Motor A Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm", pnTClamp_Btn_Stop_x + 4, pnTClamp_Btn_Start_y + 17);

  //Motor B Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm", pnBClamp_Btn_Stop_x + 4, pnBClamp_Btn_Start_y + 17);
  
  //Motor C Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm", pnFCh_Btn_Stop_x + 4, pnFCh_Btn_Start_y + 17);

  //Decrease A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnTDec_Btn_Start_x, pnTDec_Btn_Start_y, pnTDec_Btn_Stop_x, pnTDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnTDec_Btn_Start_x, pnTDec_Btn_Start_y, pnTDec_Btn_Stop_x, pnTDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", pnTDec_Btn_Start_x + 16, pnTDec_Btn_Start_y + 14);

  //Increase A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnTInc_Btn_Start_x, pnTInc_Btn_Start_y, pnTInc_Btn_Stop_x, pnTInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnTInc_Btn_Start_x, pnTInc_Btn_Start_y, pnTInc_Btn_Stop_x, pnTInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", pnTInc_Btn_Start_x + 15, pnTInc_Btn_Start_y + 14);

  //Amount Button A
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnTamt_Btn_Start_x, pnTamt_Btn_Start_y, pnTamt_Btn_Stop_x, pnTamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnTamt_Btn_Start_x, pnTamt_Btn_Start_y, pnTamt_Btn_Stop_x, pnTamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", pnTamt_Btn_Start_x + 11, pnTamt_Btn_Start_y + 18);

  //Decrease B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBDec_Btn_Start_x, pnBDec_Btn_Start_y, pnBDec_Btn_Stop_x, pnBDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBDec_Btn_Start_x, pnBDec_Btn_Start_y, pnBDec_Btn_Stop_x, pnBDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", pnBDec_Btn_Start_x + 16, pnBDec_Btn_Start_y + 14);

  //Increase B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBInc_Btn_Start_x, pnBInc_Btn_Start_y, pnBInc_Btn_Stop_x, pnBInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBInc_Btn_Start_x, pnBInc_Btn_Start_y, pnBInc_Btn_Stop_x, pnBInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", pnBInc_Btn_Start_x + 15, pnBInc_Btn_Start_y + 14);

  //Amount Button B
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBamt_Btn_Start_x, pnBamt_Btn_Start_y, pnBamt_Btn_Stop_x, pnBamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBamt_Btn_Start_x, pnBamt_Btn_Start_y, pnBamt_Btn_Stop_x, pnBamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", pnBamt_Btn_Start_x + 11, pnBamt_Btn_Start_y + 18);

  //Decrease C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnFDec_Btn_Start_x, pnFDec_Btn_Start_y, pnFDec_Btn_Stop_x, pnFDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnFDec_Btn_Start_x, pnFDec_Btn_Start_y, pnFDec_Btn_Stop_x, pnFDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", pnFDec_Btn_Start_x + 16, pnFDec_Btn_Start_y + 14);

  //Increase C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnFInc_Btn_Start_x, pnFInc_Btn_Start_y, pnFInc_Btn_Stop_x, pnFInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnFInc_Btn_Start_x, pnFInc_Btn_Start_y, pnFInc_Btn_Stop_x, pnFInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", pnFInc_Btn_Start_x + 15, pnFInc_Btn_Start_y + 14);

  //Amount Button C
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnFamt_Btn_Start_x, pnFamt_Btn_Start_y, pnFamt_Btn_Stop_x, pnFamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnFamt_Btn_Start_x, pnFamt_Btn_Start_y, pnFamt_Btn_Stop_x, pnFamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", pnFamt_Btn_Start_x + 11, pnFamt_Btn_Start_y + 18);
  
  //Number of Steps Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Fold Process Steps", 195, pnStp1_Btn_Start_y - 15);

  //Number of Steps Button 1
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp1_Btn_Start_x, pnStp1_Btn_Start_y, pnStp1_Btn_Stop_x, pnStp1_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp1_Btn_Start_x, pnStp1_Btn_Start_y, pnStp1_Btn_Stop_x, pnStp1_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("1", pnStp1_Btn_Start_x + 10, pnStp1_Btn_Start_y + 10);

  //Number of Steps Button 2
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp2_Btn_Start_x, pnStp2_Btn_Start_y, pnStp2_Btn_Stop_x, pnStp2_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp2_Btn_Start_x, pnStp2_Btn_Start_y, pnStp2_Btn_Stop_x, pnStp2_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("2", pnStp2_Btn_Start_x + 10, pnStp2_Btn_Start_y + 10);

  //Number of Steps Button 3
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp3_Btn_Start_x, pnStp3_Btn_Start_y, pnStp3_Btn_Stop_x, pnStp3_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp3_Btn_Start_x, pnStp3_Btn_Start_y, pnStp3_Btn_Stop_x, pnStp3_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("3", pnStp3_Btn_Start_x + 10, pnStp3_Btn_Start_y + 10);

  //Number of Steps Button 4
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp4_Btn_Start_x, pnStp4_Btn_Start_y, pnStp4_Btn_Stop_x, pnStp4_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp4_Btn_Start_x, pnStp4_Btn_Start_y, pnStp4_Btn_Stop_x, pnStp4_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("4", pnStp4_Btn_Start_x + 10, pnStp4_Btn_Start_y + 10);

  //Number of Steps Button 5
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp5_Btn_Start_x, pnStp5_Btn_Start_y, pnStp5_Btn_Stop_x, pnStp5_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp5_Btn_Start_x, pnStp5_Btn_Start_y, pnStp5_Btn_Stop_x, pnStp5_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("5", pnStp5_Btn_Start_x + 10, pnStp5_Btn_Start_y + 10);

  //Back Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 14);

  //Save Button
  tft.Set_Draw_color(GREEN);
  tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(GREEN);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Save", pnSave_Btn_Start_x + 17, pnSave_Btn_Start_y + 14);
}

/**Get touch readings and redraw changes**/
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

    //Profile Name Field
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
      
      //TODO: Load input screen and display data
      //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
      prevActiveScreen = 2;
      activeScreen = 5;
      //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
      //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
      //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
      //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
      //24=homeOffsetClampT, 25=homeOffsetClampB
      varInputRegister = 1;
      inputBuilder = "";
      drawInputScreen();
      delay(50);
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

      //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
      prevActiveScreen = 2;
      activeScreen = 5;
      //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
      //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
      //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
      //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
      //24=homeOffsetClampT, 25=homeOffsetClampB
      varInputRegister = 2;
      inputBuilder = "";
      drawInputScreen();
      delay(50);
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

      //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
      prevActiveScreen = 2;
      activeScreen = 5;
      //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
      //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
      //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
      //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
      //24=homeOffsetClampT, 25=homeOffsetClampB
      varInputRegister = 3;
      inputBuilder = "";
      drawInputScreen();
      delay(50);
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

      //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
      prevActiveScreen = 2;
      activeScreen = 5;      
      //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
      //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
      //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
      //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
      //24=homeOffsetClampT, 25=homeOffsetClampB
      if(stepNum == 1) {
        varInputRegister = 4;
      }
      else if(stepNum == 2) {
        varInputRegister = 5;
      }
      else if(stepNum == 3) {
        varInputRegister = 6;
      }
      inputBuilder = "";
      drawInputScreen();
      delay(50);    
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
  if(x > pnStp1_Btn_Start_x && x < pnStp1_Btn_Stop_x && y > pnStp1_Btn_Start_y && y < pnStp1_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE
     && !pnStp2BtnFlag && !pnStp3BtnFlag && !pnStp4BtnFlag && !pnStp5BtnFlag) {
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
        
      //Flag the button as pressed
      pnStp1BtnFlag = true;
      stepNum = 1;
      delay(50);
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
  if(x > pnStp2_Btn_Start_x && x < pnStp2_Btn_Stop_x && y > pnStp2_Btn_Start_y && y < pnStp2_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE
     && !pnStp1BtnFlag && !pnStp3BtnFlag && !pnStp4BtnFlag && !pnStp5BtnFlag) {
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
        
      //Flag the button as pressed
      pnStp2BtnFlag = true;  
      stepNum = 2;
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
  if(x > pnStp3_Btn_Start_x && x < pnStp3_Btn_Stop_x && y > pnStp3_Btn_Start_y && y < pnStp3_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE
     && !pnStp1BtnFlag && !pnStp2BtnFlag && !pnStp4BtnFlag && !pnStp5BtnFlag) {
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
        
      //Flag the button as pressed
      pnStp3BtnFlag = true;
      stepNum = 3;
      delay(50);
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
  if(x > pnStp4_Btn_Start_x && x < pnStp4_Btn_Stop_x && y > pnStp4_Btn_Start_y && y < pnStp4_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE
     && !pnStp1BtnFlag && !pnStp2BtnFlag && !pnStp3BtnFlag && !pnStp5BtnFlag) {
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
     
      //Flag the button as pressed
      pnStp4BtnFlag = true;   
      stepNum = 4;
      delay(50);    
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
  if(x > pnStp5_Btn_Start_x && x < pnStp5_Btn_Stop_x && y > pnStp5_Btn_Start_y && y < pnStp5_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE
     && !pnStp1BtnFlag && !pnStp1BtnFlag && !pnStp3BtnFlag && !pnStp4BtnFlag) {
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
        
      //Flag the button as pressed
      pnStp5BtnFlag = true;
      stepNum = 5;
      delay(50);
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
    varInputRegister = 0;
    inputBuilder = "";
    activeScreen = 0;
    drawMainScreen();
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
    varInputRegister = 0;
    inputBuilder = "";
    //TODO: Save profile and move to position
    saveProfile(numSavedProfiles);
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
    activeScreen = 0;
    drawMainScreen();
    delay(50);
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

/**Draw the edit profile screen**/
void drawEditProfileScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black

  //Profile Configuration Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 35);

  //Profile Configuration Title
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Profile Configuration", 115, yPadding + 12);

  //Profile Name Text
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(GREEN);
  tft.Print_String("Profile Name: ", 10, pnName_Btn_Start_y + 12);

  //Profile Name Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnName_Btn_Start_x, pnName_Btn_Start_y, pnName_Btn_Stop_x, pnName_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnName_Btn_Start_x, pnName_Btn_Start_y, pnName_Btn_Stop_x, pnName_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)profile, pnName_Btn_Start_x + 17, pnName_Btn_Start_y + 13);

  //Top Clamp Position Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Top Clamp Position: ", 30, pnTClamp_Btn_Start_y + 17);

  //Top Clamp Position Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnTClamp_Btn_Start_x, pnTClamp_Btn_Start_y, pnTClamp_Btn_Stop_x, pnTClamp_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnTClamp_Btn_Start_x, pnTClamp_Btn_Start_y, pnTClamp_Btn_Stop_x, pnTClamp_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)tClampDist, pnTClamp_Btn_Start_x + 27, pnTClamp_Btn_Start_y + 14);

  //Bottom Clamp Position Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Bottom Clamp Position: ", 30, pnBClamp_Btn_Start_y + 17);

  //Bottom Clamp Position Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)bClampDist, pnBClamp_Btn_Start_x + 27, pnBClamp_Btn_Start_y + 14);

  //Frame Chassis Position Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Frame Chassis Position", 30, pnFCh_Btn_Start_y + 17);

  //Frame Chassis Position Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnFCh_Btn_Start_x, pnFCh_Btn_Start_y, pnFCh_Btn_Stop_x, pnFCh_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnFCh_Btn_Start_x, pnFCh_Btn_Start_y, pnFCh_Btn_Stop_x, pnFCh_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)hDist1, pnFCh_Btn_Start_x + 27, pnFCh_Btn_Start_y + 14);

  //Motor A Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm", pnTClamp_Btn_Stop_x + 4, pnTClamp_Btn_Start_y + 17);

  //Motor B Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm", pnBClamp_Btn_Stop_x + 4, pnBClamp_Btn_Start_y + 17);
  
  //Motor C Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm", pnFCh_Btn_Stop_x + 4, pnFCh_Btn_Start_y + 17);

  //Decrease A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnTDec_Btn_Start_x, pnTDec_Btn_Start_y, pnTDec_Btn_Stop_x, pnTDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnTDec_Btn_Start_x, pnTDec_Btn_Start_y, pnTDec_Btn_Stop_x, pnTDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", pnTDec_Btn_Start_x + 16, pnTDec_Btn_Start_y + 14);

  //Increase A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnTInc_Btn_Start_x, pnTInc_Btn_Start_y, pnTInc_Btn_Stop_x, pnTInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnTInc_Btn_Start_x, pnTInc_Btn_Start_y, pnTInc_Btn_Stop_x, pnTInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", pnTInc_Btn_Start_x + 15, pnTInc_Btn_Start_y + 14);

  //Amount Button A
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnTamt_Btn_Start_x, pnTamt_Btn_Start_y, pnTamt_Btn_Stop_x, pnTamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnTamt_Btn_Start_x, pnTamt_Btn_Start_y, pnTamt_Btn_Stop_x, pnTamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", pnTamt_Btn_Start_x + 11, pnTamt_Btn_Start_y + 18);

  //Decrease B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBDec_Btn_Start_x, pnBDec_Btn_Start_y, pnBDec_Btn_Stop_x, pnBDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBDec_Btn_Start_x, pnBDec_Btn_Start_y, pnBDec_Btn_Stop_x, pnBDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", pnBDec_Btn_Start_x + 16, pnBDec_Btn_Start_y + 14);

  //Increase B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBInc_Btn_Start_x, pnBInc_Btn_Start_y, pnBInc_Btn_Stop_x, pnBInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBInc_Btn_Start_x, pnBInc_Btn_Start_y, pnBInc_Btn_Stop_x, pnBInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", pnBInc_Btn_Start_x + 15, pnBInc_Btn_Start_y + 14);

  //Amount Button B
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBamt_Btn_Start_x, pnBamt_Btn_Start_y, pnBamt_Btn_Stop_x, pnBamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBamt_Btn_Start_x, pnBamt_Btn_Start_y, pnBamt_Btn_Stop_x, pnBamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", pnBamt_Btn_Start_x + 11, pnBamt_Btn_Start_y + 18);

  //Decrease C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnFDec_Btn_Start_x, pnFDec_Btn_Start_y, pnFDec_Btn_Stop_x, pnFDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnFDec_Btn_Start_x, pnFDec_Btn_Start_y, pnFDec_Btn_Stop_x, pnFDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", pnFDec_Btn_Start_x + 16, pnFDec_Btn_Start_y + 14);

  //Increase C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnFInc_Btn_Start_x, pnFInc_Btn_Start_y, pnFInc_Btn_Stop_x, pnFInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnFInc_Btn_Start_x, pnFInc_Btn_Start_y, pnFInc_Btn_Stop_x, pnFInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", pnFInc_Btn_Start_x + 15, pnFInc_Btn_Start_y + 14);

  //Amount Button C
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnFamt_Btn_Start_x, pnFamt_Btn_Start_y, pnFamt_Btn_Stop_x, pnFamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnFamt_Btn_Start_x, pnFamt_Btn_Start_y, pnFamt_Btn_Stop_x, pnFamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", pnFamt_Btn_Start_x + 11, pnFamt_Btn_Start_y + 18);
  
  //Number of Steps Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Fold Process Steps", 195, pnStp1_Btn_Start_y - 15);

  //Number of Steps Button 1
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp1_Btn_Start_x, pnStp1_Btn_Start_y, pnStp1_Btn_Stop_x, pnStp1_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp1_Btn_Start_x, pnStp1_Btn_Start_y, pnStp1_Btn_Stop_x, pnStp1_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("1", pnStp1_Btn_Start_x + 10, pnStp1_Btn_Start_y + 10);

  //Number of Steps Button 2
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp2_Btn_Start_x, pnStp2_Btn_Start_y, pnStp2_Btn_Stop_x, pnStp2_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp2_Btn_Start_x, pnStp2_Btn_Start_y, pnStp2_Btn_Stop_x, pnStp2_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("2", pnStp2_Btn_Start_x + 10, pnStp2_Btn_Start_y + 10);

  //Number of Steps Button 3
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp3_Btn_Start_x, pnStp3_Btn_Start_y, pnStp3_Btn_Stop_x, pnStp3_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp3_Btn_Start_x, pnStp3_Btn_Start_y, pnStp3_Btn_Stop_x, pnStp3_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("3", pnStp3_Btn_Start_x + 10, pnStp3_Btn_Start_y + 10);

  //Number of Steps Button 4
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp4_Btn_Start_x, pnStp4_Btn_Start_y, pnStp4_Btn_Stop_x, pnStp4_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp4_Btn_Start_x, pnStp4_Btn_Start_y, pnStp4_Btn_Stop_x, pnStp4_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("4", pnStp4_Btn_Start_x + 10, pnStp4_Btn_Start_y + 10);

  //Number of Steps Button 5
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnStp5_Btn_Start_x, pnStp5_Btn_Start_y, pnStp5_Btn_Stop_x, pnStp5_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnStp5_Btn_Start_x, pnStp5_Btn_Start_y, pnStp5_Btn_Stop_x, pnStp5_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("5", pnStp5_Btn_Start_x + 10, pnStp5_Btn_Start_y + 10);

  //Back Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 14);

  //Save Button
  tft.Set_Draw_color(GREEN);
  tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(GREEN);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Save", pnSave_Btn_Start_x + 17, pnSave_Btn_Start_y + 14);
}

/**Get touch readings and redraw changes**/
void updateEditProfileScreen() {
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
      tft.Print_String(profile, pnName_Btn_Start_x + 17, pnName_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnNameBtnFlag = true;
      
      //TODO: Load input screen and display data
      //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
      prevActiveScreen = 3;
      activeScreen = 5;
      //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
      //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
      //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
      //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
      //24=homeOffsetClampT, 25=homeOffsetClampB
      varInputRegister = 1;
      inputBuilder = profile;
      drawInputScreen();
      delay(50);
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
      tft.Print_String((String)profile, pnName_Btn_Start_x + 17, pnName_Btn_Start_y + 17);
  
  
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
      tft.Print_String((String)tClampDist, pnTClamp_Btn_Start_x + 17, pnTClamp_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnTClampBtnFlag = true;

      //TODO: Load input screen and display data
      //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
      prevActiveScreen = 3;
      activeScreen = 5;
      //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
      //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
      //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
      //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
      //24=homeOffsetClampT, 25=homeOffsetClampB
      varInputRegister = 2;
      inputBuilder = tClampDist;
      drawInputScreen();
      delay(50);
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
      tft.Print_String((String)tClampDist, pnTClamp_Btn_Start_x + 17, pnTClamp_Btn_Start_y + 17);
  
  
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
      tft.Print_String((String)bClampDist, pnBClamp_Btn_Start_x + 17, pnBClamp_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnBClampBtnFlag = true;

      //TODO: Load input screen and display data
      //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
      prevActiveScreen = 3;
      activeScreen = 5;
      //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
      //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
      //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
      //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
      //24=homeOffsetClampT, 25=homeOffsetClampB
      varInputRegister = 3;
      inputBuilder = bClampDist;
      drawInputScreen();
      delay(50);
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
      tft.Print_String((String)bClampDist, pnBClamp_Btn_Start_x + 17, pnBClamp_Btn_Start_y + 17);
  
  
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
      tft.Print_String((String)hDist1, pnFCh_Btn_Start_x + 17, pnFCh_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnFChBtnFlag = true;

      //TODO: Load input screen and display data
      //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
      prevActiveScreen = 3;
      activeScreen = 5;      
      //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
      //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
      //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
      //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
      //24=homeOffsetClampT, 25=homeOffsetClampB
      if(stepNum == 1) {
        varInputRegister = 4;
        inputBuilder = hDist1;
      }
      else if(stepNum == 2) {
        varInputRegister = 5;
        inputBuilder = hDist1;
      }
      else if(stepNum == 3) {
        varInputRegister = 6;
        inputBuilder = hDist1;
      }
      drawInputScreen();
      delay(50);    
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
      tft.Print_String((String)hDist1, pnFCh_Btn_Start_x + 17, pnFCh_Btn_Start_y + 17);
  
      //Flag the delete button as pressed
      pnFChBtnFlag = false;
      delay(50);
      //TODO: If value is given display it instead
    }
  }

  //'Step 1' Button
  if(x > pnStp1_Btn_Start_x && x < pnStp1_Btn_Stop_x && y > pnStp1_Btn_Start_y && y < pnStp1_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE
     && !pnStp2BtnFlag && !pnStp3BtnFlag && !pnStp4BtnFlag && !pnStp5BtnFlag) {
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
      stepNum = 1;
      delay(50);
      //TODO:
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
  if(x > pnStp2_Btn_Start_x && x < pnStp2_Btn_Stop_x && y > pnStp2_Btn_Start_y && y < pnStp2_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE
     && !pnStp1BtnFlag && !pnStp3BtnFlag && !pnStp4BtnFlag && !pnStp5BtnFlag) {
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
      stepNum = 2;
      delay(50);
      //TODO:
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
  if(x > pnStp3_Btn_Start_x && x < pnStp3_Btn_Stop_x && y > pnStp3_Btn_Start_y && y < pnStp3_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE
     && !pnStp1BtnFlag && !pnStp2BtnFlag && !pnStp4BtnFlag && !pnStp5BtnFlag) {
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
      stepNum = 3;
      delay(50);
      //TODO:
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
  if(x > pnStp4_Btn_Start_x && x < pnStp4_Btn_Stop_x && y > pnStp4_Btn_Start_y && y < pnStp4_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE
     && !pnStp1BtnFlag && !pnStp2BtnFlag && !pnStp3BtnFlag && !pnStp5BtnFlag) {
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
      stepNum = 4;
      delay(50);
      //TODO:
    
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
  if(x > pnStp5_Btn_Start_x && x < pnStp5_Btn_Stop_x && y > pnStp5_Btn_Start_y && y < pnStp5_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE
     && !pnStp1BtnFlag && !pnStp2BtnFlag && !pnStp3BtnFlag && !pnStp4BtnFlag) {
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
      stepNum = 5;
      delay(50);
      //TODO:
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

    varInputRegister = 0;
    inputBuilder = "";
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
    activeScreen = 0;
    drawMainScreen();
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
    
    //TODO: Save profile and move to position
    varInputRegister = 0;
    inputBuilder = "";
    saveProfile(numSavedProfiles);
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
    activeScreen = 0;
    drawMainScreen();
    delay(50);
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

/**Draw the machine settings screen**/
void drawMachineSettingsScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black

  //Machine Settings Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 35);

  //Machine Settings Title
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Machine Settings", 150, yPadding + 10);

  //Velocity Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sVel_Btn_Start_x, sVel_Btn_Start_y, sVel_Btn_Stop_x, sVel_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sVel_Btn_Start_x, sVel_Btn_Start_y, sVel_Btn_Stop_x, sVel_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Velocity", sVel_Btn_Start_x + 27, sVel_Btn_Start_y + 47);

  //Acceleration Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sAccel_Btn_Start_x, sAccel_Btn_Start_y, sAccel_Btn_Stop_x, sAccel_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sAccel_Btn_Start_x, sAccel_Btn_Start_y, sAccel_Btn_Stop_x, sAccel_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Acceleration", sAccel_Btn_Start_x + 6, sAccel_Btn_Start_y + 47);

  //Jerk Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sJerk_Btn_Start_x, sJerk_Btn_Start_y, sJerk_Btn_Stop_x, sJerk_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sJerk_Btn_Start_x, sJerk_Btn_Start_y, sJerk_Btn_Stop_x, sJerk_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Jerk", sJerk_Btn_Start_x + 50, sJerk_Btn_Start_y + 47);

  //uStep Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sUS_Btn_Start_x, sUS_Btn_Start_y, sUS_Btn_Stop_x, sUS_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sUS_Btn_Start_x, sUS_Btn_Start_y, sUS_Btn_Stop_x, sUS_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Microstep", sUS_Btn_Start_x + 23, sUS_Btn_Start_y + 46);

  //Step/mm Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sSmm_Btn_Start_x, sSmm_Btn_Start_y, sSmm_Btn_Stop_x, sSmm_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sSmm_Btn_Start_x, sSmm_Btn_Start_y, sSmm_Btn_Stop_x, sSmm_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Steps/mm", sSmm_Btn_Start_x + 27, sSmm_Btn_Start_y + 46);

  //Reset Button
  tft.Set_Draw_color(RED);
  tft.Fill_Rectangle(sRst_Btn_Start_x, sRst_Btn_Start_y, sRst_Btn_Stop_x, sRst_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sRst_Btn_Start_x, sRst_Btn_Start_y, sRst_Btn_Stop_x, sRst_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(RED);
  tft.Set_Text_colour(BLACK);
  tft.Print_String(" Reset", sRst_Btn_Start_x + 35, sRst_Btn_Start_y + 35);
  tft.Print_String("Default", sRst_Btn_Start_x + 35, sRst_Btn_Start_y + 55);
  
  //Back Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 13);

  //Save Button
  tft.Set_Draw_color(GREEN);
  tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(GREEN);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Save", pnSave_Btn_Start_x + 18, pnSave_Btn_Start_y + 13);
}

/**Get touch readings and redraw changes**/
void updateMachineSettingsScreen() {
  /***Touch Update***/
  TSPoint p = ts.getPoint();  //Leaves pins in input mode. Shared by lcd so must revert to output after reading.
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int x = map(p.x, TS_LEFT, TS_RIGHT, 0, 480);
  int y = map(p.y, TS_TOP, TS_BOT, 0, 320);
  
  //'Velocity' Button
  if(x > sVel_Btn_Start_x && x < sVel_Btn_Stop_x && y > sVel_Btn_Start_y && y < sVel_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Velocity'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sVel_Btn_Start_x, sVel_Btn_Start_y, sVel_Btn_Stop_x, sVel_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sVel_Btn_Start_x, sVel_Btn_Start_y, sVel_Btn_Stop_x, sVel_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String("Velocity", sVel_Btn_Start_x + 17, sVel_Btn_Start_y + 17);
      
    //Flag the delete button as pressed
    sVelBtnFlag = true;
    
    //TODO:
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 6;
    drawVelScreen();
    delay(50);
  }
  else if(sVelBtnFlag) {
    //Button 'Back'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sVel_Btn_Start_x, sVel_Btn_Start_y, sVel_Btn_Stop_x, sVel_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sVel_Btn_Start_x, sVel_Btn_Start_y, sVel_Btn_Stop_x, sVel_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Velocity", sVel_Btn_Start_x + 17, sVel_Btn_Start_y + 17);

    sVelBtnFlag = false;
  }

  //'Acceleration' Button
  if(x > sAccel_Btn_Start_x && x < sAccel_Btn_Stop_x && y > sAccel_Btn_Start_y && y < sAccel_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Acceleration'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sAccel_Btn_Start_x, sAccel_Btn_Start_y, sAccel_Btn_Stop_x, sAccel_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sAccel_Btn_Start_x, sAccel_Btn_Start_y, sAccel_Btn_Stop_x, sAccel_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String("Acceleration", sAccel_Btn_Start_x + 17, sAccel_Btn_Start_y + 17);
      
    //Flag the button as pressed
    sAccelBtnFlag = true;
    
    //TODO:
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 7;
    drawAccelScreen();
    delay(50);
  }
  else if(sAccelBtnFlag) {
    //Button 'Acceleration'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sAccel_Btn_Start_x, sAccel_Btn_Start_y, sAccel_Btn_Stop_x, sAccel_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sAccel_Btn_Start_x, sAccel_Btn_Start_y, sAccel_Btn_Stop_x, sAccel_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Acceleration", sAccel_Btn_Start_x + 17, sAccel_Btn_Start_y + 17);

    sAccelBtnFlag = false;
  }

  //'Jerk' Button
  if(x > sJerk_Btn_Start_x && x < sJerk_Btn_Stop_x && y > sJerk_Btn_Start_y && y < sJerk_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Jerk'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sJerk_Btn_Start_x, sJerk_Btn_Start_y, sJerk_Btn_Stop_x, sJerk_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sJerk_Btn_Start_x, sJerk_Btn_Start_y, sJerk_Btn_Stop_x, sJerk_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String("Jerk", sJerk_Btn_Start_x + 17, sJerk_Btn_Start_y + 17);
      
    //Flag the button as pressed
    sJerkBtnFlag = true;
    
    //TODO:
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 8;
    drawJerkScreen();
    delay(50);
  }
  else if(sJerkBtnFlag) {
    //Button 'Jerk'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sJerk_Btn_Start_x, sJerk_Btn_Start_y, sJerk_Btn_Stop_x, sJerk_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sJerk_Btn_Start_x, sJerk_Btn_Start_y, sJerk_Btn_Stop_x, sJerk_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Jerk", sJerk_Btn_Start_x + 17, sJerk_Btn_Start_y + 17);

    sAccelBtnFlag = false;
  }

  //'Microstep' Button
  if(x > sUS_Btn_Start_x && x < sUS_Btn_Stop_x && y > sUS_Btn_Start_y && y < sUS_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Microstep'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sUS_Btn_Start_x, sUS_Btn_Start_y, sUS_Btn_Stop_x, sUS_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sUS_Btn_Start_x, sUS_Btn_Start_y, sUS_Btn_Stop_x, sUS_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String("uStep", sUS_Btn_Start_x + 17, sUS_Btn_Start_y + 17);
      
    //Flag the button as pressed
    sUSBtnFlag = true;
    
    //TODO:
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 9;
    drawMicrostepScreen();
    delay(50);
  }
  else if(sUSBtnFlag) {
    //Button 'Microstep'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sUS_Btn_Start_x, sUS_Btn_Start_y, sUS_Btn_Stop_x, sUS_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sUS_Btn_Start_x, sUS_Btn_Start_y, sUS_Btn_Stop_x, sUS_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("uStep", sUS_Btn_Start_x + 17, sUS_Btn_Start_y + 17);

    sUSBtnFlag = false;
  }

  //'Steps/mm' Button
  if(x > sSmm_Btn_Start_x && x < sSmm_Btn_Stop_x && y > sSmm_Btn_Start_y && y < sSmm_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Button 'Steps/mm'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sSmm_Btn_Start_x, sSmm_Btn_Start_y, sSmm_Btn_Stop_x, sSmm_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sSmm_Btn_Start_x, sSmm_Btn_Start_y, sSmm_Btn_Stop_x, sSmm_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String("Steps/mm", sSmm_Btn_Start_x + 17, sSmm_Btn_Start_y + 17);
      
    //Flag the button as pressed
    sSmmBtnFlag = true;
    
    //TODO:
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 10;
    drawStepsMMScreen();
    delay(50);
  }
  else if(sSmmBtnFlag) {
    //Button 'Steps/mm'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sSmm_Btn_Start_x, sSmm_Btn_Start_y, sSmm_Btn_Stop_x, sSmm_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sSmm_Btn_Start_x, sSmm_Btn_Start_y, sSmm_Btn_Stop_x, sSmm_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("Steps/mm", sSmm_Btn_Start_x + 17, sSmm_Btn_Start_y + 17);

    sSmmBtnFlag = false;
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
    
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
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

    //TODO: Save
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    saveMachineSettings();
    activeScreen = 0;
    drawMainScreen();
    delay(50);
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

/**Draw the velocity setting screen**/
void drawVelScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black  

  //Machine Settings Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 35);

  //Machine Settings Title
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Velocity Settings", 145, yPadding + 12);

  //Motor A Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Top Clamp Motor (A): ", xPadding, svAVal_Btn_Start_y + 17);

  //Motor B Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Bottom Clamp Motor (B): ", xPadding, svBVal_Btn_Start_y + 17);
  
  //Motor C Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Carriage Motors (C): ", xPadding, svCVal_Btn_Start_y + 17);

  //Top Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svAVal_Btn_Start_x, svAVal_Btn_Start_y, svAVal_Btn_Stop_x, svAVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svAVal_Btn_Start_x, svAVal_Btn_Start_y, svAVal_Btn_Stop_x, svAVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mVelocityA, svAVal_Btn_Start_x + 37, svAVal_Btn_Start_y + 14);

  //Bottom Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svBVal_Btn_Start_x, svBVal_Btn_Start_y, svBVal_Btn_Stop_x, svBVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svBVal_Btn_Start_x, svBVal_Btn_Start_y, svBVal_Btn_Stop_x, svBVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mVelocityB, svBVal_Btn_Start_x + 37, svBVal_Btn_Start_y + 14);
  
  //Frame Carriage Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svCVal_Btn_Start_x, svCVal_Btn_Start_y, svCVal_Btn_Stop_x, svCVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svCVal_Btn_Start_x, svCVal_Btn_Start_y, svCVal_Btn_Stop_x, svCVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mVelocityC, svCVal_Btn_Start_x + 37, svCVal_Btn_Start_y + 14);

  //Motor A Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s", svAVal_Btn_Stop_x + 5, svAVal_Btn_Start_y + 17);

  //Motor B Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s", svBVal_Btn_Stop_x + 5, svBVal_Btn_Start_y + 17);
  
  //Motor C Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s", svCVal_Btn_Stop_x + 5, svCVal_Btn_Start_y + 17);

  //Decrease A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svADec_Btn_Start_x, svADec_Btn_Start_y, svADec_Btn_Stop_x, svADec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svADec_Btn_Start_x, svADec_Btn_Start_y, svADec_Btn_Stop_x, svADec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", svADec_Btn_Start_x + 16, svADec_Btn_Start_y + 14);

  //Increase A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svAInc_Btn_Start_x, svAInc_Btn_Start_y, svAInc_Btn_Stop_x, svAInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svAInc_Btn_Start_x, svAInc_Btn_Start_y, svAInc_Btn_Stop_x, svAInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", svAInc_Btn_Start_x + 15, svAInc_Btn_Start_y + 14);

  //Amount Button A
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svAamt_Btn_Start_x, svAamt_Btn_Start_y, svAamt_Btn_Stop_x, svAamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svAamt_Btn_Start_x, svAamt_Btn_Start_y, svAamt_Btn_Stop_x, svAamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", svAamt_Btn_Start_x + 9, svAamt_Btn_Start_y + 17);

  //Decrease B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svBDec_Btn_Start_x, svBDec_Btn_Start_y, svBDec_Btn_Stop_x, svBDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svBDec_Btn_Start_x, svBDec_Btn_Start_y, svBDec_Btn_Stop_x, svBDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", svBDec_Btn_Start_x + 16, svBDec_Btn_Start_y + 14);

  //Increase B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svBInc_Btn_Start_x, svBInc_Btn_Start_y, svBInc_Btn_Stop_x, svBInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svBInc_Btn_Start_x, svBInc_Btn_Start_y, svBInc_Btn_Stop_x, svBInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", svBInc_Btn_Start_x + 15, svBInc_Btn_Start_y + 14);

  //Amount Button B
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svBamt_Btn_Start_x, svBamt_Btn_Start_y, svBamt_Btn_Stop_x, svBamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svBamt_Btn_Start_x, svBamt_Btn_Start_y, svBamt_Btn_Stop_x, svBamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", svBamt_Btn_Start_x + 9, svBamt_Btn_Start_y + 17);

  //Decrease C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svCDec_Btn_Start_x, svCDec_Btn_Start_y, svCDec_Btn_Stop_x, svCDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svCDec_Btn_Start_x, svCDec_Btn_Start_y, svCDec_Btn_Stop_x, svCDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", svCDec_Btn_Start_x + 16, svCDec_Btn_Start_y + 14);

  //Increase C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svCInc_Btn_Start_x, svCInc_Btn_Start_y, svCInc_Btn_Stop_x, svCInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svCInc_Btn_Start_x, svCInc_Btn_Start_y, svCInc_Btn_Stop_x, svCInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", svCInc_Btn_Start_x + 15, svCInc_Btn_Start_y + 14);

  //Amount Button C
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svCamt_Btn_Start_x, svCamt_Btn_Start_y, svCamt_Btn_Stop_x, svCamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svCamt_Btn_Start_x, svCamt_Btn_Start_y, svCamt_Btn_Stop_x, svCamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", svCamt_Btn_Start_x + 9, svCamt_Btn_Start_y + 17);

  //Back Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 14);

  //Save Button
  tft.Set_Draw_color(GREEN);
  tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(GREEN);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Save", pnSave_Btn_Start_x + 17, pnSave_Btn_Start_y + 14);
}

/**Get touch readings and redraw changes**/
void updateVelScreen() {
  /***Touch Update***/
  TSPoint p = ts.getPoint();  //Leaves pins in input mode. Shared by lcd so must revert to output after reading.
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int x = map(p.x, TS_LEFT, TS_RIGHT, 0, 480);
  int y = map(p.y, TS_TOP, TS_BOT, 0, 320);

  //Velocity A Input Field
  if(x > svAVal_Btn_Start_x && x < svAVal_Btn_Stop_x && y > svAVal_Btn_Start_y && y < svAVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svAVal_Btn_Start_x, svAVal_Btn_Start_y, svAVal_Btn_Stop_x, svAVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svAVal_Btn_Start_x, svAVal_Btn_Start_y, svAVal_Btn_Stop_x, svAVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mVelocityA, svAVal_Btn_Start_x + 35, svAVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    svAValBtnFlag = true;

    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 6;
    activeScreen = 5;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 7;
    inputBuilder = mVelocityA;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(svAValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svAVal_Btn_Start_x, svAVal_Btn_Start_y, svAVal_Btn_Stop_x, svAVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svAVal_Btn_Start_x, svAVal_Btn_Start_y, svAVal_Btn_Stop_x, svAVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mVelocityA, svAVal_Btn_Start_x + 35, svAVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    svAValBtnFlag = false;
  }

  //Velocity B Input Field
  if(x > svBVal_Btn_Start_x && x < svBVal_Btn_Stop_x && y > svBVal_Btn_Start_y && y < svBVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svBVal_Btn_Start_x, svBVal_Btn_Start_y, svBVal_Btn_Stop_x, svBVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svBVal_Btn_Start_x, svBVal_Btn_Start_y, svBVal_Btn_Stop_x, svBVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mVelocityB, svBVal_Btn_Start_x + 35, svBVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    svBValBtnFlag = true;
    
    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 6;
    activeScreen = 5;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 8;
    inputBuilder = mVelocityB;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(svBValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svBVal_Btn_Start_x, svBVal_Btn_Start_y, svBVal_Btn_Stop_x, svBVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svBVal_Btn_Start_x, svBVal_Btn_Start_y, svBVal_Btn_Stop_x, svBVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mVelocityB, svBVal_Btn_Start_x + 35, svBVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    svBValBtnFlag = false;
  }

  //Velocity C Input Field
  if(x > svCVal_Btn_Start_x && x < svCVal_Btn_Stop_x && y > svCVal_Btn_Start_y && y < svCVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svCVal_Btn_Start_x, svCVal_Btn_Start_y, svCVal_Btn_Stop_x, svCVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svCVal_Btn_Start_x, svCVal_Btn_Start_y, svCVal_Btn_Stop_x, svCVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mVelocityC, svCVal_Btn_Start_x + 35, svCVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    svCValBtnFlag = true;
    
    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 6;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 9;
    inputBuilder = mVelocityC;
    activeScreen = 5;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(svCValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svCVal_Btn_Start_x, svCVal_Btn_Start_y, svCVal_Btn_Stop_x, svCVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svCVal_Btn_Start_x, svCVal_Btn_Start_y, svCVal_Btn_Stop_x, svCVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mVelocityC, svCVal_Btn_Start_x + 35, svCVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    svCValBtnFlag = false;
  }

  //Motor A Decrease Button
  if(x > svADec_Btn_Start_x && x < svADec_Btn_Stop_x && y > svADec_Btn_Start_y && y < svADec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svADec_Btn_Start_x, svADec_Btn_Start_y, svADec_Btn_Stop_x, svADec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svADec_Btn_Start_x, svADec_Btn_Start_y, svADec_Btn_Stop_x, svADec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", svADec_Btn_Start_x + 35, svADec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    svADecBtnFlag = true;
    mVelocityA -= aAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(svADecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svADec_Btn_Start_x, svADec_Btn_Start_y, svADec_Btn_Stop_x, svADec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svADec_Btn_Start_x, svADec_Btn_Start_y, svADec_Btn_Stop_x, svADec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", svADec_Btn_Start_x + 35, svADec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    svADecBtnFlag = false;
  }

  //Motor B Decrease Button
  if(x > svBDec_Btn_Start_x && x < svBDec_Btn_Stop_x && y > svBDec_Btn_Start_y && y < svBDec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svBDec_Btn_Start_x, svBDec_Btn_Start_y, svBDec_Btn_Stop_x, svBDec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svBDec_Btn_Start_x, svBDec_Btn_Start_y, svBDec_Btn_Stop_x, svBDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", svBDec_Btn_Start_x + 35, svBDec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    svBDecBtnFlag = true;
    mVelocityB -= bAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(svBDecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svBDec_Btn_Start_x, svBDec_Btn_Start_y, svBDec_Btn_Stop_x, svBDec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svBDec_Btn_Start_x, svBDec_Btn_Start_y, svBDec_Btn_Stop_x, svBDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", svBDec_Btn_Start_x + 35, svBDec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    svBDecBtnFlag = false;
  }

  //Motor C Decrease Button
  if(x > svCDec_Btn_Start_x && x < svCDec_Btn_Stop_x && y > svCDec_Btn_Start_y && y < svCDec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svCDec_Btn_Start_x, svCDec_Btn_Start_y, svCDec_Btn_Stop_x, svCDec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svCDec_Btn_Start_x, svCDec_Btn_Start_y, svCDec_Btn_Stop_x, svCDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", svCDec_Btn_Start_x + 35, svCDec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    svCDecBtnFlag = true;
    mVelocityC -= cAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(svCDecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svCDec_Btn_Start_x, svCDec_Btn_Start_y, svCDec_Btn_Stop_x, svCDec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svCDec_Btn_Start_x, svCDec_Btn_Start_y, svCDec_Btn_Stop_x, svCDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", svCDec_Btn_Start_x + 35, svCDec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    svCDecBtnFlag = false;
  }

  //Motor A Increase Button
  if(x > svAInc_Btn_Start_x && x < svAInc_Btn_Stop_x && y > svAInc_Btn_Start_y && y < svAInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svAInc_Btn_Start_x, svAInc_Btn_Start_y, svAInc_Btn_Stop_x, svAInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svAInc_Btn_Start_x, svAInc_Btn_Start_y, svAInc_Btn_Stop_x, svAInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", svAInc_Btn_Start_x + 35, svAInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    svAIncBtnFlag = true;
    mVelocityA += aAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(svAIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svAInc_Btn_Start_x, svAInc_Btn_Start_y, svAInc_Btn_Stop_x, svAInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svAInc_Btn_Start_x, svAInc_Btn_Start_y, svAInc_Btn_Stop_x, svAInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", svAInc_Btn_Start_x + 35, svAInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    svAIncBtnFlag = false;
  }

  //Motor B Increase Button
  if(x > svBInc_Btn_Start_x && x < svBInc_Btn_Stop_x && y > svBInc_Btn_Start_y && y < svBInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svBInc_Btn_Start_x, svBInc_Btn_Start_y, svBInc_Btn_Stop_x, svBInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svBInc_Btn_Start_x, svBInc_Btn_Start_y, svBInc_Btn_Stop_x, svBInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", svBInc_Btn_Start_x + 35, svBInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    svBIncBtnFlag = true;
    mVelocityB += bAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(svBIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svBInc_Btn_Start_x, svBInc_Btn_Start_y, svBInc_Btn_Stop_x, svBInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svBInc_Btn_Start_x, svBInc_Btn_Start_y, svBInc_Btn_Stop_x, svBInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", svBInc_Btn_Start_x + 35, svBInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    svBIncBtnFlag = false;
  }

  //Motor C Increase Button
  if(x > svCInc_Btn_Start_x && x < svCInc_Btn_Stop_x && y > svCInc_Btn_Start_y && y < svCInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svCInc_Btn_Start_x, svCInc_Btn_Start_y, svCInc_Btn_Stop_x, svCInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svCInc_Btn_Start_x, svCInc_Btn_Start_y, svCInc_Btn_Stop_x, svCInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", svCInc_Btn_Start_x + 35, svCInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    svCIncBtnFlag = true;
    mVelocityC += cAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(svCIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svCInc_Btn_Start_x, svCInc_Btn_Start_y, svCInc_Btn_Stop_x, svCInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svCInc_Btn_Start_x, svCInc_Btn_Start_y, svCInc_Btn_Stop_x, svCInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", svCInc_Btn_Start_x + 35, svCInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    svCIncBtnFlag = false;
  }

  //Motor A Amount Toggle Button
  if(x > svAamt_Btn_Start_x && x < svAamt_Btn_Stop_x && y > svAamt_Btn_Start_y && y < svAamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svAamt_Btn_Start_x, svAamt_Btn_Start_y, svAamt_Btn_Stop_x, svAamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svAamt_Btn_Start_x, svAamt_Btn_Start_y, svAamt_Btn_Stop_x, svAamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(aAmt == 0.1) {
      aAmt = 1;
      tft.Print_String("1mm/s", svAamt_Btn_Start_x + 10, svAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 1) {
      aAmt = 10;
      tft.Print_String("10mm/s", svAamt_Btn_Start_x + 8, svAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 10) {
      aAmt = 50;
      tft.Print_String("50mm/s", svAamt_Btn_Start_x + 8, svAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 50) {
      aAmt = 0.1;
      tft.Print_String("0.1mm/s", svAamt_Btn_Start_x + 7, svAamt_Btn_Start_y + 16);
    }

    svAamtBtnFlag = true;
    delay(50);
  }
  else if(svAamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svAamt_Btn_Start_x, svAamt_Btn_Start_y, svAamt_Btn_Stop_x, svAamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svAamt_Btn_Start_x, svAamt_Btn_Start_y, svAamt_Btn_Stop_x, svAamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(aAmt == 0.1) {
      tft.Print_String("0.1mm/s", svAamt_Btn_Start_x + 7, svAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 1) {
      tft.Print_String("1mm/s", svAamt_Btn_Start_x + 10, svAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 10) {
      tft.Print_String("10mm/s", svAamt_Btn_Start_x + 8, svAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 50) {
      tft.Print_String("50mm/s", svAamt_Btn_Start_x + 8, svAamt_Btn_Start_y + 16);
    }
    svAamtBtnFlag = false;
  }
  
  //Motor B Amount Toggle Button
  if(x > svBamt_Btn_Start_x && x < svBamt_Btn_Stop_x && y > svBamt_Btn_Start_y && y < svBamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svBamt_Btn_Start_x, svBamt_Btn_Start_y, svBamt_Btn_Stop_x, svBamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svBamt_Btn_Start_x, svBamt_Btn_Start_y, svBamt_Btn_Stop_x, svBamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(bAmt == 0.1) {
      bAmt = 1;
      tft.Print_String("1mm/s", svBamt_Btn_Start_x + 10, svBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 1) {
      bAmt = 10;
      tft.Print_String("10mm/s", svBamt_Btn_Start_x + 8, svBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 10) {
      bAmt = 50;
      tft.Print_String("50mm/s", svBamt_Btn_Start_x + 8, svBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 50) {
      bAmt = 0.1;
      tft.Print_String("0.1mm/s", svBamt_Btn_Start_x + 7, svBamt_Btn_Start_y + 16);
    }

    svBamtBtnFlag = true;
    delay(50);
  }
  else if(svBamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svBamt_Btn_Start_x, svBamt_Btn_Start_y, svBamt_Btn_Stop_x, svBamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svBamt_Btn_Start_x, svBamt_Btn_Start_y, svBamt_Btn_Stop_x, svBamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(bAmt == 0.1) {
      tft.Print_String("0.1mm/s", svBamt_Btn_Start_x + 7, svBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 1) {
      tft.Print_String("1mm/s", svBamt_Btn_Start_x + 10, svBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 10) {
      tft.Print_String("10mm/s", svBamt_Btn_Start_x + 8, svBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 50) {
      tft.Print_String("50mm/s", svBamt_Btn_Start_x + 8, svBamt_Btn_Start_y + 16);
    }
    svBamtBtnFlag = false;
  }
  
  //Motor C Amount Toggle Button
  if(x > svCamt_Btn_Start_x && x < svCamt_Btn_Stop_x && y > svCamt_Btn_Start_y && y < svCamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(svCamt_Btn_Start_x, svCamt_Btn_Start_y, svCamt_Btn_Stop_x, svCamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(svCamt_Btn_Start_x, svCamt_Btn_Start_y, svCamt_Btn_Stop_x, svCamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(cAmt == 0.1) {
      cAmt = 1;
      tft.Print_String("1mm/s", svCamt_Btn_Start_x + 10, svCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 1) {
      cAmt = 10;
      tft.Print_String("10mm/s", svCamt_Btn_Start_x + 8, svCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 10) {
      cAmt = 50;
      tft.Print_String("50mm/s", svCamt_Btn_Start_x + 8, svCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 50) {
      cAmt = 0.1;
      tft.Print_String("0.1mm/s", svCamt_Btn_Start_x + 7, svCamt_Btn_Start_y + 16);
    }

    svCamtBtnFlag = true;
    delay(50);
  }
  else if(svCamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(svCamt_Btn_Start_x, svCamt_Btn_Start_y, svCamt_Btn_Stop_x, svCamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(svCamt_Btn_Start_x, svCamt_Btn_Start_y, svCamt_Btn_Stop_x, svCamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(cAmt == 0.1) {
      tft.Print_String("0.1mm/s", svCamt_Btn_Start_x + 7, svCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 1) {
      tft.Print_String("1mm/s", svCamt_Btn_Start_x + 10, svCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 10) {
      tft.Print_String("10mm/s", svCamt_Btn_Start_x + 8, svCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 50) {
      tft.Print_String("50mm/s", svCamt_Btn_Start_x + 8, svCamt_Btn_Start_y + 16);
    }
    svCamtBtnFlag = false;
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
      
    //Flag the button as pressed
    pnBckBtnFlag = true;
    
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 0;
    inputBuilder = "";
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 4;
    drawMachineSettingsScreen();
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
      
    //Flag the button as pressed
    pnSaveBtnFlag = true;
    
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 0;
    inputBuilder = "";    
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 5;
    drawMachineSettingsScreen();
    delay(50);
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

/**Draw the acceleration setting screen**/
void drawAccelScreen() {
   tft.Fill_Screen(BLACK);    //Change the screen to solid black  

  //Machine Settings Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 35);

  //Machine Settings Title
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Acceleration Settings", 115, yPadding + 12);

  //Motor A Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Top Clamp Motor (A): ", xPadding, saAVal_Btn_Start_y + 17);

  //Motor B Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Bottom Clamp Motor (B): ", xPadding, saBVal_Btn_Start_y + 17);
  
  //Motor C Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Carriage Motors (C): ", xPadding, saCVal_Btn_Start_y + 17);

  //Top Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saAVal_Btn_Start_x, saAVal_Btn_Start_y, saAVal_Btn_Stop_x, saAVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saAVal_Btn_Start_x, saAVal_Btn_Start_y, saAVal_Btn_Stop_x, saAVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mAccelA, saAVal_Btn_Start_x + 37, saAVal_Btn_Start_y + 14);

  //Bottom Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saBVal_Btn_Start_x, saBVal_Btn_Start_y, saBVal_Btn_Stop_x, saBVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saBVal_Btn_Start_x, saBVal_Btn_Start_y, saBVal_Btn_Stop_x, saBVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mAccelB, saBVal_Btn_Start_x + 37, saBVal_Btn_Start_y + 14);
  
  //Frame Carriage Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saCVal_Btn_Start_x, saCVal_Btn_Start_y, saCVal_Btn_Stop_x, saCVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saCVal_Btn_Start_x, saCVal_Btn_Start_y, saCVal_Btn_Stop_x, saCVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mAccelC, saCVal_Btn_Start_x + 37, saCVal_Btn_Start_y + 14);

  //Motor A Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^2", saAVal_Btn_Stop_x + 5, saAVal_Btn_Start_y + 17);

  //Motor B Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^2", saBVal_Btn_Stop_x + 5, saBVal_Btn_Start_y + 17);
  
  //Motor C Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^2", saCVal_Btn_Stop_x + 5, saCVal_Btn_Start_y + 17);

  //Decrease A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saADec_Btn_Start_x, saADec_Btn_Start_y, saADec_Btn_Stop_x, saADec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saADec_Btn_Start_x, saADec_Btn_Start_y, saADec_Btn_Stop_x, saADec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", saADec_Btn_Start_x + 16, saADec_Btn_Start_y + 14);

  //Increase A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saAInc_Btn_Start_x, saAInc_Btn_Start_y, saAInc_Btn_Stop_x, saAInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saAInc_Btn_Start_x, saAInc_Btn_Start_y, saAInc_Btn_Stop_x, saAInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", saAInc_Btn_Start_x + 15, saAInc_Btn_Start_y + 14);

  //Amount Button A
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saAamt_Btn_Start_x, saAamt_Btn_Start_y, saAamt_Btn_Stop_x, saAamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saAamt_Btn_Start_x, saAamt_Btn_Start_y, saAamt_Btn_Stop_x, saAamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", saAamt_Btn_Start_x + 9, saAamt_Btn_Start_y + 17);

  //Decrease B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saBDec_Btn_Start_x, saBDec_Btn_Start_y, saBDec_Btn_Stop_x, saBDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saBDec_Btn_Start_x, saBDec_Btn_Start_y, saBDec_Btn_Stop_x, saBDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", saBDec_Btn_Start_x + 16, saBDec_Btn_Start_y + 14);

  //Increase B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saBInc_Btn_Start_x, saBInc_Btn_Start_y, saBInc_Btn_Stop_x, saBInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saBInc_Btn_Start_x, saBInc_Btn_Start_y, saBInc_Btn_Stop_x, saBInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", saBInc_Btn_Start_x + 15, saBInc_Btn_Start_y + 14);

  //Amount Button B
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saBamt_Btn_Start_x, saBamt_Btn_Start_y, saBamt_Btn_Stop_x, saBamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saBamt_Btn_Start_x, saBamt_Btn_Start_y, saBamt_Btn_Stop_x, saBamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", saBamt_Btn_Start_x + 9, saBamt_Btn_Start_y + 17);

  //Decrease C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saCDec_Btn_Start_x, saCDec_Btn_Start_y, saCDec_Btn_Stop_x, saCDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saCDec_Btn_Start_x, saCDec_Btn_Start_y, saCDec_Btn_Stop_x, saCDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", saCDec_Btn_Start_x + 16, saCDec_Btn_Start_y + 14);

  //Increase C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saCInc_Btn_Start_x, saCInc_Btn_Start_y, saCInc_Btn_Stop_x, saCInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saCInc_Btn_Start_x, saCInc_Btn_Start_y, saCInc_Btn_Stop_x, saCInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", saCInc_Btn_Start_x + 15, saCInc_Btn_Start_y + 14);

  //Amount Button C
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saCamt_Btn_Start_x, saCamt_Btn_Start_y, saCamt_Btn_Stop_x, saCamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saCamt_Btn_Start_x, saCamt_Btn_Start_y, saCamt_Btn_Stop_x, saCamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", saCamt_Btn_Start_x + 9, saCamt_Btn_Start_y + 17);

  //Back Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 14);

  //Save Button
  tft.Set_Draw_color(GREEN);
  tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(GREEN);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Save", pnSave_Btn_Start_x + 17, pnSave_Btn_Start_y + 14);
}

/**Get touch readings and redraw changes**/
void updateAccelScreen() {
  /***Touch Update***/
  TSPoint p = ts.getPoint();  //Leaves pins in input mode. Shared by lcd so must revert to output after reading.
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int x = map(p.x, TS_LEFT, TS_RIGHT, 0, 480);
  int y = map(p.y, TS_TOP, TS_BOT, 0, 320);

  //Acceleration A Input Field
  if(x > saAVal_Btn_Start_x && x < saAVal_Btn_Stop_x && y > saAVal_Btn_Start_y && y < saAVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saAVal_Btn_Start_x, saAVal_Btn_Start_y, saAVal_Btn_Stop_x, saAVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saAVal_Btn_Start_x, saAVal_Btn_Start_y, saAVal_Btn_Stop_x, saAVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mAccelA, saAVal_Btn_Start_x + 35, saAVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    saAValBtnFlag = true;
    
    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 7;
    activeScreen = 5;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 10;
    inputBuilder = mAccelA;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(saAValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saAVal_Btn_Start_x, saAVal_Btn_Start_y, saAVal_Btn_Stop_x, saAVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saAVal_Btn_Start_x, saAVal_Btn_Start_y, saAVal_Btn_Stop_x, saAVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mAccelA, saAVal_Btn_Start_x + 35, saAVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    saAValBtnFlag = false;
  }

  //Velocity B Input Field
  if(x > saBVal_Btn_Start_x && x < saBVal_Btn_Stop_x && y > saBVal_Btn_Start_y && y < saBVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saBVal_Btn_Start_x, saBVal_Btn_Start_y, saBVal_Btn_Stop_x, saBVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saBVal_Btn_Start_x, saBVal_Btn_Start_y, saBVal_Btn_Stop_x, saBVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mAccelB, saBVal_Btn_Start_x + 35, saBVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    saBValBtnFlag = true;
    
    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 7;
    activeScreen = 5;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 11;
    inputBuilder = mAccelB;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(saBValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saBVal_Btn_Start_x, saBVal_Btn_Start_y, saBVal_Btn_Stop_x, saBVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saBVal_Btn_Start_x, saBVal_Btn_Start_y, saBVal_Btn_Stop_x, saBVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mAccelB, saBVal_Btn_Start_x + 35, saBVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    saBValBtnFlag = false;
  }

  //Velocity C Input Field
  if(x > saCVal_Btn_Start_x && x < saCVal_Btn_Stop_x && y > saCVal_Btn_Start_y && y < saCVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saCVal_Btn_Start_x, saCVal_Btn_Start_y, saCVal_Btn_Stop_x, saCVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saCVal_Btn_Start_x, saCVal_Btn_Start_y, saCVal_Btn_Stop_x, saCVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mAccelC, saCVal_Btn_Start_x + 35, saCVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    saCValBtnFlag = true;
    
    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 7;
    activeScreen = 5;    
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 12;
    inputBuilder = mAccelC;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(saCValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saCVal_Btn_Start_x, saCVal_Btn_Start_y, saCVal_Btn_Stop_x, saCVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saCVal_Btn_Start_x, saCVal_Btn_Start_y, saCVal_Btn_Stop_x, saCVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mAccelC, saCVal_Btn_Start_x + 35, saCVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    saCValBtnFlag = false;
  }

  //Motor A Decrease Button
  if(x > saADec_Btn_Start_x && x < saADec_Btn_Stop_x && y > saADec_Btn_Start_y && y < saADec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saADec_Btn_Start_x, saADec_Btn_Start_y, saADec_Btn_Stop_x, saADec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saADec_Btn_Start_x, saADec_Btn_Start_y, saADec_Btn_Stop_x, saADec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", saADec_Btn_Start_x + 35, saADec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    saADecBtnFlag = true;
    mAccelA -= aAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(saADecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saADec_Btn_Start_x, saADec_Btn_Start_y, saADec_Btn_Stop_x, saADec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saADec_Btn_Start_x, saADec_Btn_Start_y, saADec_Btn_Stop_x, saADec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", saADec_Btn_Start_x + 35, saADec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    saADecBtnFlag = false;
  }

  //Motor B Decrease Button
  if(x > saBDec_Btn_Start_x && x < saBDec_Btn_Stop_x && y > saBDec_Btn_Start_y && y < saBDec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saBDec_Btn_Start_x, saBDec_Btn_Start_y, saBDec_Btn_Stop_x, saBDec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saBDec_Btn_Start_x, saBDec_Btn_Start_y, saBDec_Btn_Stop_x, saBDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", saBDec_Btn_Start_x + 35, saBDec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    saBDecBtnFlag = true;
    mAccelB -= bAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(saBDecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saBDec_Btn_Start_x, saBDec_Btn_Start_y, saBDec_Btn_Stop_x, saBDec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saBDec_Btn_Start_x, saBDec_Btn_Start_y, saBDec_Btn_Stop_x, saBDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", saBDec_Btn_Start_x + 35, saBDec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    saBDecBtnFlag = false;
  }

  //Motor C Decrease Button
  if(x > saCDec_Btn_Start_x && x < saCDec_Btn_Stop_x && y > saCDec_Btn_Start_y && y < saCDec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saCDec_Btn_Start_x, saCDec_Btn_Start_y, saCDec_Btn_Stop_x, saCDec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saCDec_Btn_Start_x, saCDec_Btn_Start_y, saCDec_Btn_Stop_x, saCDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", saCDec_Btn_Start_x + 35, saCDec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    saCDecBtnFlag = true;
    mAccelC -= cAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(saCDecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saCDec_Btn_Start_x, saCDec_Btn_Start_y, saCDec_Btn_Stop_x, saCDec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saCDec_Btn_Start_x, saCDec_Btn_Start_y, saCDec_Btn_Stop_x, saCDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", saCDec_Btn_Start_x + 35, saCDec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    saCDecBtnFlag = false;
  }

  //Motor A Increase Button
  if(x > saAInc_Btn_Start_x && x < saAInc_Btn_Stop_x && y > saAInc_Btn_Start_y && y < saAInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saAInc_Btn_Start_x, saAInc_Btn_Start_y, saAInc_Btn_Stop_x, saAInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saAInc_Btn_Start_x, saAInc_Btn_Start_y, saAInc_Btn_Stop_x, saAInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", saAInc_Btn_Start_x + 35, saAInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    saAIncBtnFlag = true;
    mAccelA += aAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(saAIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saAInc_Btn_Start_x, saAInc_Btn_Start_y, saAInc_Btn_Stop_x, saAInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saAInc_Btn_Start_x, saAInc_Btn_Start_y, saAInc_Btn_Stop_x, saAInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", saAInc_Btn_Start_x + 35, saAInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    saAIncBtnFlag = false;
  }

  //Motor B Increase Button
  if(x > saBInc_Btn_Start_x && x < saBInc_Btn_Stop_x && y > saBInc_Btn_Start_y && y < saBInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saBInc_Btn_Start_x, saBInc_Btn_Start_y, saBInc_Btn_Stop_x, saBInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saBInc_Btn_Start_x, saBInc_Btn_Start_y, saBInc_Btn_Stop_x, saBInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", saBInc_Btn_Start_x + 35, saBInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    saBIncBtnFlag = true;
    mAccelB += bAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(saBIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saBInc_Btn_Start_x, saBInc_Btn_Start_y, saBInc_Btn_Stop_x, saBInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saBInc_Btn_Start_x, saBInc_Btn_Start_y, saBInc_Btn_Stop_x, saBInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", saBInc_Btn_Start_x + 35, saBInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    saBIncBtnFlag = false;
  }

  //Motor C Increase Button
  if(x > saCInc_Btn_Start_x && x < saCInc_Btn_Stop_x && y > saCInc_Btn_Start_y && y < saCInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saCInc_Btn_Start_x, saCInc_Btn_Start_y, saCInc_Btn_Stop_x, saCInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saCInc_Btn_Start_x, saCInc_Btn_Start_y, saCInc_Btn_Stop_x, saCInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", saCInc_Btn_Start_x + 35, saCInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    saCIncBtnFlag = true;
    mAccelC += cAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(saCIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saCInc_Btn_Start_x, saCInc_Btn_Start_y, saCInc_Btn_Stop_x, saCInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saCInc_Btn_Start_x, saCInc_Btn_Start_y, saCInc_Btn_Stop_x, saCInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", saCInc_Btn_Start_x + 35, saCInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    saCIncBtnFlag = false;
  }

  //Motor A Amount Toggle Button
  if(x > saAamt_Btn_Start_x && x < saAamt_Btn_Stop_x && y > saAamt_Btn_Start_y && y < saAamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saAamt_Btn_Start_x, saAamt_Btn_Start_y, saAamt_Btn_Stop_x, saAamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saAamt_Btn_Start_x, saAamt_Btn_Start_y, saAamt_Btn_Stop_x, saAamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(aAmt == 0.1) {
      aAmt = 1;
      tft.Print_String("1mm/s^2", saAamt_Btn_Start_x + 10, saAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 1) {
      aAmt = 10;
      tft.Print_String("10mm/s^2", saAamt_Btn_Start_x + 8, saAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 10) {
      aAmt = 50;
      tft.Print_String("50mm/s^2", saAamt_Btn_Start_x + 8, saAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 50) {
      aAmt = 0.1;
      tft.Print_String("0.1mm/s^2", saAamt_Btn_Start_x + 7, saAamt_Btn_Start_y + 16);
    }

    saAamtBtnFlag = true;
    delay(50);
  }
  else if(saAamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saAamt_Btn_Start_x, saAamt_Btn_Start_y, saAamt_Btn_Stop_x, saAamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saAamt_Btn_Start_x, saAamt_Btn_Start_y, saAamt_Btn_Stop_x, saAamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(aAmt == 0.1) {
      tft.Print_String("0.1mm/s^2", saAamt_Btn_Start_x + 7, saAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 1) {
      tft.Print_String("1mm/s^2", saAamt_Btn_Start_x + 10, saAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 10) {
      tft.Print_String("10mm/s^2", saAamt_Btn_Start_x + 8, saAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 50) {
      tft.Print_String("50mm/s^2", saAamt_Btn_Start_x + 8, saAamt_Btn_Start_y + 16);
    }
    saAamtBtnFlag = false;
  }
  
  //Motor B Amount Toggle Button
  if(x > saBamt_Btn_Start_x && x < saBamt_Btn_Stop_x && y > saBamt_Btn_Start_y && y < saBamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saBamt_Btn_Start_x, saBamt_Btn_Start_y, saBamt_Btn_Stop_x, saBamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saBamt_Btn_Start_x, saBamt_Btn_Start_y, saBamt_Btn_Stop_x, saBamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(bAmt == 0.1) {
      bAmt = 1;
      tft.Print_String("1mm/s^2", saBamt_Btn_Start_x + 10, saBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 1) {
      bAmt = 10;
      tft.Print_String("10mm/s^2", saBamt_Btn_Start_x + 8, saBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 10) {
      bAmt = 50;
      tft.Print_String("50mm/s^2", saBamt_Btn_Start_x + 8, saBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 50) {
      bAmt = 0.1;
      tft.Print_String("0.1mm/s^2", saBamt_Btn_Start_x + 7, saBamt_Btn_Start_y + 16);
    }

    saBamtBtnFlag = true;
    delay(50);
  }
  else if(saBamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saBamt_Btn_Start_x, saBamt_Btn_Start_y, saBamt_Btn_Stop_x, saBamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saBamt_Btn_Start_x, saBamt_Btn_Start_y, saBamt_Btn_Stop_x, saBamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(bAmt == 0.1) {
      tft.Print_String("0.1mm/s^2", saBamt_Btn_Start_x + 7, saBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 1) {
      tft.Print_String("1mm/s^2", saBamt_Btn_Start_x + 10, saBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 10) {
      tft.Print_String("10mm/s^2", saBamt_Btn_Start_x + 8, saBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 50) {
      tft.Print_String("50mm/s^2", saBamt_Btn_Start_x + 8, saBamt_Btn_Start_y + 16);
    }
    saBamtBtnFlag = false;
  }
  
  //Motor C Amount Toggle Button
  if(x > saCamt_Btn_Start_x && x < saCamt_Btn_Stop_x && y > saCamt_Btn_Start_y && y < saCamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(saCamt_Btn_Start_x, saCamt_Btn_Start_y, saCamt_Btn_Stop_x, saCamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(saCamt_Btn_Start_x, saCamt_Btn_Start_y, saCamt_Btn_Stop_x, saCamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(cAmt == 0.1) {
      cAmt = 1;
      tft.Print_String("1mm/s^2", saCamt_Btn_Start_x + 10, saCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 1) {
      cAmt = 10;
      tft.Print_String("10mm/s^2", saCamt_Btn_Start_x + 8, saCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 10) {
      cAmt = 50;
      tft.Print_String("50mm/s^2", saCamt_Btn_Start_x + 8, saCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 50) {
      cAmt = 0.1;
      tft.Print_String("0.1mm/s^2", saCamt_Btn_Start_x + 7, saCamt_Btn_Start_y + 16);
    }

    saCamtBtnFlag = true;
    delay(50);
  }
  else if(saCamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(saCamt_Btn_Start_x, saCamt_Btn_Start_y, saCamt_Btn_Stop_x, saCamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(saCamt_Btn_Start_x, saCamt_Btn_Start_y, saCamt_Btn_Stop_x, saCamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(cAmt == 0.1) {
      tft.Print_String("0.1mm/s^2", saCamt_Btn_Start_x + 7, saCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 1) {
      tft.Print_String("1mm/s^2", saCamt_Btn_Start_x + 10, saCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 10) {
      tft.Print_String("10mm/s^2", saCamt_Btn_Start_x + 8, saCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 50) {
      tft.Print_String("50mm/s^2", saCamt_Btn_Start_x + 8, saCamt_Btn_Start_y + 16);
    }
    saCamtBtnFlag = false;
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
      
    //Flag the button as pressed
    pnBckBtnFlag = true;

    //TODO: Open input screen and get input
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 0;
    inputBuilder = "";
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 4;
    drawMachineSettingsScreen();
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
      
    //Flag the button as pressed
    pnSaveBtnFlag = true;

    //TODO:
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 0;
    inputBuilder = "";
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 4;
    drawMachineSettingsScreen();
    delay(50);
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

/**Draw the jerk setting screen**/
void drawJerkScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black  

  //Machine Settings Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 35);

  //Machine Settings Title
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Jerk Settings", 170, yPadding + 12);

  //Motor A Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Top Clamp Motor (A): ", xPadding, sjAVal_Btn_Start_y + 17);

  //Motor B Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Bottom Clamp Motor (B): ", xPadding, sjBVal_Btn_Start_y + 17);
  
  //Motor C Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Carriage Motors (C): ", xPadding, sjCVal_Btn_Start_y + 17);

  //Top Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjAVal_Btn_Start_x, sjAVal_Btn_Start_y, sjAVal_Btn_Stop_x, sjAVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjAVal_Btn_Start_x, sjAVal_Btn_Start_y, sjAVal_Btn_Stop_x, sjAVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mJerkA, sjAVal_Btn_Start_x + 47, sjAVal_Btn_Start_y + 14);

  //Bottom Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjBVal_Btn_Start_x, sjBVal_Btn_Start_y, sjBVal_Btn_Stop_x, sjBVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjBVal_Btn_Start_x, sjBVal_Btn_Start_y, sjBVal_Btn_Stop_x, sjBVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mJerkB, sjBVal_Btn_Start_x + 47, sjBVal_Btn_Start_y + 14);
  
  //Frame Carriage Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjCVal_Btn_Start_x, sjCVal_Btn_Start_y, sjCVal_Btn_Stop_x, sjCVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjCVal_Btn_Start_x, sjCVal_Btn_Start_y, sjCVal_Btn_Stop_x, sjCVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mJerkC, sjCVal_Btn_Start_x + 47, sjCVal_Btn_Start_y + 14);

  //Motor A Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^3", sjAVal_Btn_Stop_x + 5, sjAVal_Btn_Start_y + 17);

  //Motor B Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^3", sjBVal_Btn_Stop_x + 5, sjBVal_Btn_Start_y + 17);
  
  //Motor C Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^3", sjCVal_Btn_Stop_x + 5, sjCVal_Btn_Start_y + 17);

  //Decrease A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjADec_Btn_Start_x, sjADec_Btn_Start_y, sjADec_Btn_Stop_x, sjADec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjADec_Btn_Start_x, sjADec_Btn_Start_y, sjADec_Btn_Stop_x, sjADec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", sjADec_Btn_Start_x + 16, sjADec_Btn_Start_y + 14);

  //Increase A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjAInc_Btn_Start_x, sjAInc_Btn_Start_y, sjAInc_Btn_Stop_x, sjAInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjAInc_Btn_Start_x, sjAInc_Btn_Start_y, sjAInc_Btn_Stop_x, sjAInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", sjAInc_Btn_Start_x + 15, sjAInc_Btn_Start_y + 14);

  //Amount Button A
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjAamt_Btn_Start_x, sjAamt_Btn_Start_y, sjAamt_Btn_Stop_x, sjAamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjAamt_Btn_Start_x, sjAamt_Btn_Start_y, sjAamt_Btn_Stop_x, sjAamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", sjAamt_Btn_Start_x + 9, sjAamt_Btn_Start_y + 17);

  //Decrease B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjBDec_Btn_Start_x, sjBDec_Btn_Start_y, sjBDec_Btn_Stop_x, sjBDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjBDec_Btn_Start_x, sjBDec_Btn_Start_y, sjBDec_Btn_Stop_x, sjBDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", sjBDec_Btn_Start_x + 16, sjBDec_Btn_Start_y + 14);

  //Increase B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjBInc_Btn_Start_x, sjBInc_Btn_Start_y, sjBInc_Btn_Stop_x, sjBInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjBInc_Btn_Start_x, sjBInc_Btn_Start_y, sjBInc_Btn_Stop_x, sjBInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", sjBInc_Btn_Start_x + 15, sjBInc_Btn_Start_y + 14);

  //Amount Button B
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjBamt_Btn_Start_x, sjBamt_Btn_Start_y, sjBamt_Btn_Stop_x, sjBamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjBamt_Btn_Start_x, sjBamt_Btn_Start_y, sjBamt_Btn_Stop_x, sjBamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", sjBamt_Btn_Start_x + 9, sjBamt_Btn_Start_y + 17);

  //Decrease C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjCDec_Btn_Start_x, sjCDec_Btn_Start_y, sjCDec_Btn_Stop_x, sjCDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjCDec_Btn_Start_x, sjCDec_Btn_Start_y, sjCDec_Btn_Stop_x, sjCDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", sjCDec_Btn_Start_x + 16, sjCDec_Btn_Start_y + 14);

  //Increase C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjCInc_Btn_Start_x, sjCInc_Btn_Start_y, sjCInc_Btn_Stop_x, sjCInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjCInc_Btn_Start_x, sjCInc_Btn_Start_y, sjCInc_Btn_Stop_x, sjCInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", sjCInc_Btn_Start_x + 15, sjCInc_Btn_Start_y + 14);

  //Amount Button C
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjCamt_Btn_Start_x, sjCamt_Btn_Start_y, sjCamt_Btn_Stop_x, sjCamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjCamt_Btn_Start_x, sjCamt_Btn_Start_y, sjCamt_Btn_Stop_x, sjCamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", sjCamt_Btn_Start_x + 9, sjCamt_Btn_Start_y + 17);

  //Back Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 14);

  //Save Button
  tft.Set_Draw_color(GREEN);
  tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(GREEN);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Save", pnSave_Btn_Start_x + 17, pnSave_Btn_Start_y + 14);
}

/**Get touch readings and redraw changes**/
void updateJerkScreen() {
    /***Touch Update***/
  TSPoint p = ts.getPoint();  //Leaves pins in input mode. Shared by lcd so must revert to output after reading.
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int x = map(p.x, TS_LEFT, TS_RIGHT, 0, 480);
  int y = map(p.y, TS_TOP, TS_BOT, 0, 320);

  //Jerk A Input Field
  if(x > sjAVal_Btn_Start_x && x < sjAVal_Btn_Stop_x && y > sjAVal_Btn_Start_y && y < sjAVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjAVal_Btn_Start_x, sjAVal_Btn_Start_y, sjAVal_Btn_Stop_x, sjAVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjAVal_Btn_Start_x, sjAVal_Btn_Start_y, sjAVal_Btn_Stop_x, sjAVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mJerkA, sjAVal_Btn_Start_x + 35, sjAVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    sjAValBtnFlag = true;

    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 8;
    activeScreen = 5;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 13;
    inputBuilder = mJerkA;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(sjAValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjAVal_Btn_Start_x, sjAVal_Btn_Start_y, sjAVal_Btn_Stop_x, sjAVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjAVal_Btn_Start_x, sjAVal_Btn_Start_y, sjAVal_Btn_Stop_x, sjAVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mJerkA, sjAVal_Btn_Start_x + 35, sjAVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    sjAValBtnFlag = false;
  }

  //Jerk B Input Field
  if(x > sjBVal_Btn_Start_x && x < sjBVal_Btn_Stop_x && y > sjBVal_Btn_Start_y && y < sjBVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjBVal_Btn_Start_x, sjBVal_Btn_Start_y, sjBVal_Btn_Stop_x, sjBVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjBVal_Btn_Start_x, sjBVal_Btn_Start_y, sjBVal_Btn_Stop_x, sjBVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mJerkB, sjBVal_Btn_Start_x + 35, sjBVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    sjBValBtnFlag = true;

    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 8;
    activeScreen = 5;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 14;
    inputBuilder = mJerkB;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(sjBValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjBVal_Btn_Start_x, sjBVal_Btn_Start_y, sjBVal_Btn_Stop_x, sjBVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjBVal_Btn_Start_x, sjBVal_Btn_Start_y, sjBVal_Btn_Stop_x, sjBVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mJerkB, sjBVal_Btn_Start_x + 35, sjBVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    sjBValBtnFlag = false;
  }

  //Velocity C Input Field
  if(x > sjCVal_Btn_Start_x && x < sjCVal_Btn_Stop_x && y > sjCVal_Btn_Start_y && y < sjCVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjCVal_Btn_Start_x, sjCVal_Btn_Start_y, sjCVal_Btn_Stop_x, sjCVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjCVal_Btn_Start_x, sjCVal_Btn_Start_y, sjCVal_Btn_Stop_x, sjCVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mJerkC, sjCVal_Btn_Start_x + 35, sjCVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    sjCValBtnFlag = true;

    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 8;
    activeScreen = 5;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 15;
    inputBuilder = mJerkC;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(sjCValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjCVal_Btn_Start_x, sjCVal_Btn_Start_y, sjCVal_Btn_Stop_x, sjCVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjCVal_Btn_Start_x, sjCVal_Btn_Start_y, sjCVal_Btn_Stop_x, sjCVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mJerkC, sjCVal_Btn_Start_x + 35, sjCVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    sjCValBtnFlag = false;
  }

  //Motor A Decrease Button
  if(x > sjADec_Btn_Start_x && x < sjADec_Btn_Stop_x && y > sjADec_Btn_Start_y && y < sjADec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjADec_Btn_Start_x, sjADec_Btn_Start_y, sjADec_Btn_Stop_x, sjADec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjADec_Btn_Start_x, sjADec_Btn_Start_y, sjADec_Btn_Stop_x, sjADec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", sjADec_Btn_Start_x + 35, sjADec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    sjADecBtnFlag = true;
    mJerkA -= aAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(sjADecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjADec_Btn_Start_x, sjADec_Btn_Start_y, sjADec_Btn_Stop_x, sjADec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjADec_Btn_Start_x, sjADec_Btn_Start_y, sjADec_Btn_Stop_x, sjADec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", sjADec_Btn_Start_x + 35, sjADec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    sjADecBtnFlag = false;
  }

  //Motor B Decrease Button
  if(x > sjBDec_Btn_Start_x && x < sjBDec_Btn_Stop_x && y > sjBDec_Btn_Start_y && y < sjBDec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjBDec_Btn_Start_x, sjBDec_Btn_Start_y, sjBDec_Btn_Stop_x, sjBDec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjBDec_Btn_Start_x, sjBDec_Btn_Start_y, sjBDec_Btn_Stop_x, sjBDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", sjBDec_Btn_Start_x + 35, sjBDec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    sjBDecBtnFlag = true;
    mJerkB -= bAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(sjBDecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjBDec_Btn_Start_x, sjBDec_Btn_Start_y, sjBDec_Btn_Stop_x, sjBDec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjBDec_Btn_Start_x, sjBDec_Btn_Start_y, sjBDec_Btn_Stop_x, sjBDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", sjBDec_Btn_Start_x + 35, sjBDec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    sjBDecBtnFlag = false;
  }

  //Motor C Decrease Button
  if(x > sjCDec_Btn_Start_x && x < sjCDec_Btn_Stop_x && y > sjCDec_Btn_Start_y && y < sjCDec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjCDec_Btn_Start_x, sjCDec_Btn_Start_y, sjCDec_Btn_Stop_x, sjCDec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjCDec_Btn_Start_x, sjCDec_Btn_Start_y, sjCDec_Btn_Stop_x, sjCDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", sjCDec_Btn_Start_x + 35, sjCDec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    sjCDecBtnFlag = true;
    mJerkC -= cAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(sjCDecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjCDec_Btn_Start_x, sjCDec_Btn_Start_y, sjCDec_Btn_Stop_x, sjCDec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjCDec_Btn_Start_x, sjCDec_Btn_Start_y, sjCDec_Btn_Stop_x, sjCDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", sjCDec_Btn_Start_x + 35, sjCDec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    sjCDecBtnFlag = false;
  }

  //Motor A Increase Button
  if(x > sjAInc_Btn_Start_x && x < sjAInc_Btn_Stop_x && y > sjAInc_Btn_Start_y && y < sjAInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjAInc_Btn_Start_x, sjAInc_Btn_Start_y, sjAInc_Btn_Stop_x, sjAInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjAInc_Btn_Start_x, sjAInc_Btn_Start_y, sjAInc_Btn_Stop_x, sjAInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", sjAInc_Btn_Start_x + 35, sjAInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    sjAIncBtnFlag = true;
    mJerkA += aAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(sjAIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjAInc_Btn_Start_x, sjAInc_Btn_Start_y, sjAInc_Btn_Stop_x, sjAInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjAInc_Btn_Start_x, sjAInc_Btn_Start_y, sjAInc_Btn_Stop_x, sjAInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", sjAInc_Btn_Start_x + 35, sjAInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    sjAIncBtnFlag = false;
  }

  //Motor B Increase Button
  if(x > sjBInc_Btn_Start_x && x < sjBInc_Btn_Stop_x && y > sjBInc_Btn_Start_y && y < sjBInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjBInc_Btn_Start_x, sjBInc_Btn_Start_y, sjBInc_Btn_Stop_x, sjBInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjBInc_Btn_Start_x, sjBInc_Btn_Start_y, sjBInc_Btn_Stop_x, sjBInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", sjBInc_Btn_Start_x + 35, sjBInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    sjBIncBtnFlag = true;
    mJerkB += bAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(sjBIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjBInc_Btn_Start_x, sjBInc_Btn_Start_y, sjBInc_Btn_Stop_x, sjBInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjBInc_Btn_Start_x, sjBInc_Btn_Start_y, sjBInc_Btn_Stop_x, sjBInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", sjBInc_Btn_Start_x + 35, sjBInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    sjBIncBtnFlag = false;
  }

  //Motor C Increase Button
  if(x > sjCInc_Btn_Start_x && x < sjCInc_Btn_Stop_x && y > sjCInc_Btn_Start_y && y < sjCInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjCInc_Btn_Start_x, sjCInc_Btn_Start_y, sjCInc_Btn_Stop_x, sjCInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjCInc_Btn_Start_x, sjCInc_Btn_Start_y, sjCInc_Btn_Stop_x, sjCInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", sjCInc_Btn_Start_x + 35, sjCInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    sjCIncBtnFlag = true;
    mJerkC += cAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(sjCIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjCInc_Btn_Start_x, sjCInc_Btn_Start_y, sjCInc_Btn_Stop_x, sjCInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjCInc_Btn_Start_x, sjCInc_Btn_Start_y, sjCInc_Btn_Stop_x, sjCInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", sjCInc_Btn_Start_x + 35, sjCInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    sjCIncBtnFlag = false;
  }

  //Motor A Amount Toggle Button
  if(x > sjAamt_Btn_Start_x && x < sjAamt_Btn_Stop_x && y > sjAamt_Btn_Start_y && y < sjAamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjAamt_Btn_Start_x, sjAamt_Btn_Start_y, sjAamt_Btn_Stop_x, sjAamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjAamt_Btn_Start_x, sjAamt_Btn_Start_y, sjAamt_Btn_Stop_x, sjAamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(aAmt == 0.1) {
      aAmt = 1;
      tft.Print_String("1mm/s^3", sjAamt_Btn_Start_x + 10, sjAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 1) {
      aAmt = 10;
      tft.Print_String("10mm/s^3", sjAamt_Btn_Start_x + 8, sjAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 10) {
      aAmt = 50;
      tft.Print_String("50mm/s^3", sjAamt_Btn_Start_x + 8, sjAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 50) {
      aAmt = 0.1;
      tft.Print_String("0.1mm/s^3", sjAamt_Btn_Start_x + 7, sjAamt_Btn_Start_y + 16);
    }

    sjAamtBtnFlag = true;
    delay(50);
  }
  else if(sjAamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjAamt_Btn_Start_x, sjAamt_Btn_Start_y, sjAamt_Btn_Stop_x, sjAamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjAamt_Btn_Start_x, sjAamt_Btn_Start_y, sjAamt_Btn_Stop_x, sjAamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(aAmt == 0.1) {
      tft.Print_String("0.1mm/s^3", sjAamt_Btn_Start_x + 7, sjAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 1) {
      tft.Print_String("1mm/s^3", sjAamt_Btn_Start_x + 10, sjAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 10) {
      tft.Print_String("10mm/s^3", sjAamt_Btn_Start_x + 8, sjAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 50) {
      tft.Print_String("50mm/s^3", sjAamt_Btn_Start_x + 8, sjAamt_Btn_Start_y + 16);
    }
    sjAamtBtnFlag = false;
  }
  
  //Motor B Amount Toggle Button
  if(x > sjBamt_Btn_Start_x && x < sjBamt_Btn_Stop_x && y > sjBamt_Btn_Start_y && y < sjBamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjBamt_Btn_Start_x, sjBamt_Btn_Start_y, sjBamt_Btn_Stop_x, sjBamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjBamt_Btn_Start_x, sjBamt_Btn_Start_y, sjBamt_Btn_Stop_x, sjBamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(bAmt == 0.1) {
      bAmt = 1;
      tft.Print_String("1mm/s^3", sjBamt_Btn_Start_x + 10, sjBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 1) {
      bAmt = 10;
      tft.Print_String("10mm/s^3", sjBamt_Btn_Start_x + 8, sjBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 10) {
      bAmt = 50;
      tft.Print_String("50mm/s^3", sjBamt_Btn_Start_x + 8, sjBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 50) {
      bAmt = 0.1;
      tft.Print_String("0.1mm/s^3", sjBamt_Btn_Start_x + 7, sjBamt_Btn_Start_y + 16);
    }

    sjBamtBtnFlag = true;
    delay(50);
  }
  else if(sjBamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjBamt_Btn_Start_x, sjBamt_Btn_Start_y, sjBamt_Btn_Stop_x, sjBamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjBamt_Btn_Start_x, sjBamt_Btn_Start_y, sjBamt_Btn_Stop_x, sjBamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(bAmt == 0.1) {
      tft.Print_String("0.1mm/s^3", sjBamt_Btn_Start_x + 7, sjBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 1) {
      tft.Print_String("1mm/s^3", sjBamt_Btn_Start_x + 10, sjBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 10) {
      tft.Print_String("10mm/s^3", sjBamt_Btn_Start_x + 8, sjBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 50) {
      tft.Print_String("50mm/s^3", sjBamt_Btn_Start_x + 8, sjBamt_Btn_Start_y + 16);
    }
    sjBamtBtnFlag = false;
  }
  
  //Motor C Amount Toggle Button
  if(x > sjCamt_Btn_Start_x && x < sjCamt_Btn_Stop_x && y > sjCamt_Btn_Start_y && y < sjCamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(sjCamt_Btn_Start_x, sjCamt_Btn_Start_y, sjCamt_Btn_Stop_x, sjCamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(sjCamt_Btn_Start_x, sjCamt_Btn_Start_y, sjCamt_Btn_Stop_x, sjCamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(cAmt == 0.1) {
      cAmt = 1;
      tft.Print_String("1mm/s^3", sjCamt_Btn_Start_x + 10, sjCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 1) {
      cAmt = 10;
      tft.Print_String("10mm/s^3", sjCamt_Btn_Start_x + 8, sjCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 10) {
      cAmt = 50;
      tft.Print_String("50mm/s^3", sjCamt_Btn_Start_x + 8, sjCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 50) {
      cAmt = 0.1;
      tft.Print_String("0.1mm/s^3", sjCamt_Btn_Start_x + 7, sjCamt_Btn_Start_y + 16);
    }

    sjCamtBtnFlag = true;
    delay(50);
  }
  else if(sjCamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(sjCamt_Btn_Start_x, sjCamt_Btn_Start_y, sjCamt_Btn_Stop_x, sjCamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(sjCamt_Btn_Start_x, sjCamt_Btn_Start_y, sjCamt_Btn_Stop_x, sjCamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(cAmt == 0.1) {
      tft.Print_String("0.1mm/s^3", sjCamt_Btn_Start_x + 7, sjCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 1) {
      tft.Print_String("1mm/s^3", sjCamt_Btn_Start_x + 10, sjCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 10) {
      tft.Print_String("10mm/s^3", sjCamt_Btn_Start_x + 8, sjCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 50) {
      tft.Print_String("50mm/s^3", sjCamt_Btn_Start_x + 8, sjCamt_Btn_Start_y + 16);
    }
    sjCamtBtnFlag = false;
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
      
    //Flag the button as pressed
    pnBckBtnFlag = true;

    //TODO: Open input screen and get input
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 0;
    inputBuilder = "";
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 4;
    drawMachineSettingsScreen();
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
      
    //Flag the button as pressed
    pnSaveBtnFlag = true;

    //TODO:
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 0;
    inputBuilder = "";
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 4;
    drawMachineSettingsScreen();
    delay(50);
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

/**Draw the microstep settings screen**/
void drawMicrostepScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black

  //Microstepping Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 35);

  //Microstepping Title
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Microstepping Settings", 115, yPadding + 12);

  //Motor A uStep Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Top Clamp Motor (A)", mAuS_Btn_Start_x - 7, mAuS_Btn_Start_y - 15);

  //Motor B uStep Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Bottom Clamp Motor (B)", mBuS_Btn_Start_x -10, mBuS_Btn_Start_y - 15);

  //Motor C uStep Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Carriage Motors (C)", mCuS_Btn_Start_x - 5, mCuS_Btn_Start_y - 15);

  //Microstep string values
  String mAuStep;
  String mBuStep;
  String mCuStep;
  
  //Motor A microstepping value display
  if(mUStepA == 0) {
    mAuStep = "1";
  }
  else if(mUStepA == 1) {
    mAuStep = "1/2";
  }
  else if(mUStepA == 2){ 
    mAuStep = "1/4";
  }
  else if(mUStepA == 3) {
    mAuStep = "1/8";
  }
  else if(mUStepA == 4) {
    mAuStep = "1/16";
  }

  //Motor B microstepping value display
  if(mUStepB == 0) {
    mBuStep = "1";
  }
  else if(mUStepB == 1) {
    mBuStep = "1/2";
  }
  else if(mUStepB == 2){ 
    mBuStep = "1/4";
  }
  else if(mUStepB == 3) {
    mBuStep = "1/8";
  }
  else if(mUStepB == 4) {
    mBuStep = "1/16";
  }

  //Motor C microstepping value display
  if(mUStepC == 0) {
    mCuStep = "1";
  }
  else if(mUStepC == 1) {
    mCuStep = "1/2";
  }
  else if(mUStepC == 2){ 
    mCuStep = "1/4";
  }
  else if(mUStepC == 3) {
    mCuStep = "1/8";
  }
  else if(mUStepC == 4) {
    mCuStep = "1/16";
  }
  
  //Motor A uStep Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(mAuS_Btn_Start_x, mAuS_Btn_Start_y, mAuS_Btn_Stop_x, mAuS_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mAuS_Btn_Start_x, mAuS_Btn_Start_y, mAuS_Btn_Stop_x, mAuS_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String(mAuStep, mAuS_Btn_Start_x + 27, mAuS_Btn_Start_y + 35);
  
  //Motor B uStep Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(mBuS_Btn_Start_x, mBuS_Btn_Start_y, mBuS_Btn_Stop_x, mBuS_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mBuS_Btn_Start_x, mBuS_Btn_Start_y, mBuS_Btn_Stop_x, mBuS_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String(mBuStep, mBuS_Btn_Start_x + 27, mBuS_Btn_Start_y + 35);

  //Motor C uStep Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(mCuS_Btn_Start_x, mCuS_Btn_Start_y, mCuS_Btn_Stop_x, mCuS_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mCuS_Btn_Start_x, mCuS_Btn_Start_y, mCuS_Btn_Stop_x, mCuS_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String(mCuStep, mCuS_Btn_Start_x + 27, mCuS_Btn_Start_y + 35);

  //Motor A Invert Button
  tft.Set_Draw_color(RED);
  tft.Fill_Rectangle(mAinv_Btn_Start_x, mAinv_Btn_Start_y, mAinv_Btn_Stop_x, mAinv_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mAinv_Btn_Start_x, mAinv_Btn_Start_y, mAinv_Btn_Stop_x, mAinv_Btn_Stop_y);

  //Motor B Invert Button
  tft.Set_Draw_color(RED);
  tft.Fill_Rectangle(mBinv_Btn_Start_x, mBinv_Btn_Start_y, mBinv_Btn_Stop_x, mBinv_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mBinv_Btn_Start_x, mBinv_Btn_Start_y, mBinv_Btn_Stop_x, mBinv_Btn_Stop_y);

  //Motor C Invert Button
  tft.Set_Draw_color(RED);
  tft.Fill_Rectangle(mCinv_Btn_Start_x, mCinv_Btn_Start_y, mCinv_Btn_Stop_x, mCinv_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mCinv_Btn_Start_x, mCinv_Btn_Start_y, mCinv_Btn_Stop_x, mCinv_Btn_Stop_y);

  //Motor A Invert Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Invert", mAinv_Btn_Stop_x + 5, mAinv_Btn_Start_y  + 9);

  //Motor B Invert Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Invert", mBinv_Btn_Stop_x + 5, mAinv_Btn_Start_y + 9);

  //Motor C Invert Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Invert", mCinv_Btn_Stop_x + 5, mCinv_Btn_Start_y + 9);

  //Back Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 14);

  //Save Button
  tft.Set_Draw_color(GREEN);
  tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(GREEN);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Save", pnSave_Btn_Start_x + 17, pnSave_Btn_Start_y + 14);
}

/**Get touch readings and update changes**/
void updateMicrostepScreen() {
  /***Touch Update***/
  TSPoint p = ts.getPoint();  //Leaves pins in input mode. Shared by lcd so must revert to output after reading.
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int x = map(p.x, TS_LEFT, TS_RIGHT, 0, 480);
  int y = map(p.y, TS_TOP, TS_BOT, 0, 320);

  //Microstep string values
  String mAuStep;
  String mBuStep;
  String mCuStep;
  
  //'Motor A uStep' Button
  if(x > mAuS_Btn_Start_x && x < mAuS_Btn_Stop_x && y > mAuS_Btn_Start_y && y < mAuS_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    mUStepA++;
    if(mUStepA > 4) {
      mUStepA = 0;
    }
    
    //Motor A microstepping value display
    if(mUStepA == 0) {
      mAuStep = "1";
    }
    else if(mUStepA == 1) {
      mAuStep = "1/2";
    }
    else if(mUStepA == 2){ 
      mAuStep = "1/4";
    }
    else if(mUStepA == 3) {
      mAuStep = "1/8";
    }
    else if(mUStepA == 4) {
      mAuStep = "1/16";
    }
    
    //Button 'Motor A uStep'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(mAuS_Btn_Start_x, mAuS_Btn_Start_y, mAuS_Btn_Stop_x, mAuS_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(mAuS_Btn_Start_x, mAuS_Btn_Start_y, mAuS_Btn_Stop_x, mAuS_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String(mAuStep, mAuS_Btn_Start_x + 17, mAuS_Btn_Start_y + 17);
        
    //Flag the button as pressed
    mAuSBtnFlag = true;
    setMicroStepping('A', mUStepA);
    delay(50);
  }
  else if(mAuSBtnFlag) {
    //Button 'Motor A uStep'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(mAuS_Btn_Start_x, mAuS_Btn_Start_y, mAuS_Btn_Stop_x, mAuS_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(mAuS_Btn_Start_x, mAuS_Btn_Start_y, mAuS_Btn_Stop_x, mAuS_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(mAuStep, mAuS_Btn_Start_x + 17, mAuS_Btn_Start_y + 17);

    mAuSBtnFlag = false;
  }

  //'Motor B uStep' Button
  if(x > mBuS_Btn_Start_x && x < mBuS_Btn_Stop_x && y > mBuS_Btn_Start_y && y < mBuS_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    mUStepB++;
    if(mUStepB > 4) {
      mUStepB = 0;
    }

    //Motor B microstepping value display
    if(mUStepB == 0) {
      mBuStep = "1";
    }
    else if(mUStepB == 1) {
      mBuStep = "1/2";
    }
    else if(mUStepB == 2){ 
      mBuStep = "1/4";
    }
    else if(mUStepB == 3) {
      mBuStep = "1/8";
    }
    else if(mUStepB == 4) {
      mBuStep = "1/6";
    }
  
    //Button 'Motor B uStep'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(mBuS_Btn_Start_x, mBuS_Btn_Start_y, mBuS_Btn_Stop_x, mBuS_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(mBuS_Btn_Start_x, mBuS_Btn_Start_y, mBuS_Btn_Stop_x, mBuS_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String(mBuStep, mBuS_Btn_Start_x + 17, mBuS_Btn_Start_y + 17);
      
    //Flag the button as pressed
    mBuSBtnFlag = true;
    setMicroStepping('B', mUStepB);
    delay(50);
  }
  else if(mBuSBtnFlag) {
    //Button 'Motor B uStep'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(mAuS_Btn_Start_x, mAuS_Btn_Start_y, mAuS_Btn_Stop_x, mAuS_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(mAuS_Btn_Start_x, mAuS_Btn_Start_y, mAuS_Btn_Stop_x, mAuS_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(mBuStep, mAuS_Btn_Start_x + 17, mAuS_Btn_Start_y + 17);

    mBuSBtnFlag = false;
  }

  //'Motor C uStep' Button
  if(x > mCuS_Btn_Start_x && x < mCuS_Btn_Stop_x && y > mCuS_Btn_Start_y && y < mCuS_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    mUStepC++;
    if(mUStepC > 4) {
      mUStepC = 0;
    }

    //Motor C microstepping value display
    if(mUStepC == 0) {
      mCuStep = "1";
    }
    else if(mUStepC == 1) {
      mCuStep = "1/2";
    }
    else if(mUStepC == 2){ 
      mCuStep = "1/4";
    }
    else if(mUStepC == 3) {
      mCuStep = "1/8";
    }
    else if(mUStepC == 4) {
      mCuStep = "1/16";
    }
    
    //Button 'Motor C uStep'
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(mCuS_Btn_Start_x, mCuS_Btn_Start_y, mCuS_Btn_Stop_x, mCuS_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(mCuS_Btn_Start_x, mCuS_Btn_Start_y, mCuS_Btn_Stop_x, mCuS_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(WHITE);
    tft.Print_String(mCuStep, mCuS_Btn_Start_x + 17, mCuS_Btn_Start_y + 17);
      
    //Flag the button as pressed
    mCuSBtnFlag = true;
    setMicroStepping('C', mUStepC);
    delay(50);
  }
  else if(mCuSBtnFlag) {
    //Button 'Motor C uStep'
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(mCuS_Btn_Start_x, mCuS_Btn_Start_y, mCuS_Btn_Stop_x, mCuS_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(mCuS_Btn_Start_x, mCuS_Btn_Start_y, mCuS_Btn_Stop_x, mCuS_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String(mCuStep, mCuS_Btn_Start_x + 17, mCuS_Btn_Start_y + 17);

    mCuSBtnFlag = false;
  }
  
  //'Motor A Invert' Button
  if(x > mAinv_Btn_Start_x && x < mAinv_Btn_Stop_x && y > mAinv_Btn_Start_y && y < mAinv_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    mAinvBtnFlag = !mAinvBtnFlag;

    if(mAinvBtnFlag) {
      //'Motor A Invert' Button
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(mAinv_Btn_Start_x, mAinv_Btn_Start_y, mAinv_Btn_Stop_x, mAinv_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(mAinv_Btn_Start_x, mAinv_Btn_Start_y, mAinv_Btn_Stop_x, mAinv_Btn_Stop_y);
        
      //Flag the delete button as pressed
      mAinvBtnFlag = true;
      mInvDirA = true;
      setMotorDirection('A', mDirA);
      delay(50);
    }
    else {
      //'Motor A Invert' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(mAinv_Btn_Start_x, mAinv_Btn_Start_y, mAinv_Btn_Stop_x, mAinv_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(mAinv_Btn_Start_x, mAinv_Btn_Start_y, mAinv_Btn_Stop_x, mAinv_Btn_Stop_y);  
  
      //Flag the delete button as pressed
      mAinvBtnFlag = false;
      mInvDirA = false;
      setMotorDirection('A', mDirA);
      delay(50);
    }
  }

  //'Motor B Invert' Button
  if(x > mBinv_Btn_Start_x && x < mBinv_Btn_Stop_x && y > mBinv_Btn_Start_y && y < mBinv_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    mBinvBtnFlag = !mBinvBtnFlag;

    if(mBinvBtnFlag) {
      //'Motor B Invert' Button
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(mBinv_Btn_Start_x, mBinv_Btn_Start_y, mBinv_Btn_Stop_x, mBinv_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(mBinv_Btn_Start_x, mBinv_Btn_Start_y, mBinv_Btn_Stop_x, mBinv_Btn_Stop_y);
        
      //Flag the delete button as pressed
      mBinvBtnFlag = true;
      mInvDirB = true;
      setMotorDirection('B', mDirB);
      delay(50);
    }
    else {
      //'Motor B Invert' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(mBinv_Btn_Start_x, mBinv_Btn_Start_y, mBinv_Btn_Stop_x, mBinv_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(mBinv_Btn_Start_x, mBinv_Btn_Start_y, mBinv_Btn_Stop_x, mBinv_Btn_Stop_y);  
  
      //Flag the delete button as pressed
      mBinvBtnFlag = false;
      mInvDirB = false;
      setMotorDirection('B', mDirB);
      delay(50);
    }
  }

  //'Motor C Invert' Button
  if(x > mCinv_Btn_Start_x && x < mCinv_Btn_Stop_x && y > mCinv_Btn_Start_y && y < mCinv_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    mCinvBtnFlag = !mCinvBtnFlag;

    if(mCinvBtnFlag) {
      //'Motor C Invert' Button
      tft.Set_Draw_color(GRAY);
      tft.Fill_Rectangle(mCinv_Btn_Start_x, mCinv_Btn_Start_y, mCinv_Btn_Stop_x, mCinv_Btn_Stop_y);
      tft.Set_Draw_color(WHITE);
      tft.Draw_Rectangle(mCinv_Btn_Start_x, mCinv_Btn_Start_y, mCinv_Btn_Stop_x, mCinv_Btn_Stop_y);
        
      //Flag the delete button as pressed
      mCinvBtnFlag = true;
      mInvDirC = true;
      setMotorDirection('C', mDirC);
      delay(50);
    }
    else {
      //'Motor C Invert' Button
      tft.Set_Draw_color(WHITE);
      tft.Fill_Rectangle(mCinv_Btn_Start_x, mCinv_Btn_Start_y, mCinv_Btn_Stop_x, mCinv_Btn_Stop_y);
      tft.Set_Draw_color(GRAY);
      tft.Draw_Rectangle(mCinv_Btn_Start_x, mCinv_Btn_Start_y, mCinv_Btn_Stop_x, mCinv_Btn_Stop_y);  
  
      //Flag the delete button as pressed
      mCinvBtnFlag = false;
      mInvDirC = false;
      setMotorDirection('C', mDirC);
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
    
    //TODO: Go back to machine settings screen without retaining changes
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 4;
    drawMachineSettingsScreen();
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

    //TODO: Change back to machine settings screen with retaining changes
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 4;
    drawMachineSettingsScreen();
    delay(50);
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

/**Draw the steps/mm setting screen**/
void drawStepsMMScreen() {
  tft.Fill_Screen(BLACK);    //Change the screen to solid black  

  //Machine Settings Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 35);

  //Machine Settings Title
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Steps/mm Settings", 150, yPadding + 12);

  //Motor A Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Top Clamp Motor (A): ", xPadding, ssmmAVal_Btn_Start_y + 17);

  //Motor B Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Bottom Clamp Motor (B): ", xPadding, ssmmBVal_Btn_Start_y + 17);
  
  //Motor C Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Carriage Motors (C): ", xPadding, ssmmCVal_Btn_Start_y + 17);

  //Top Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmAVal_Btn_Start_x, ssmmAVal_Btn_Start_y, ssmmAVal_Btn_Stop_x, ssmmAVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmAVal_Btn_Start_x, ssmmAVal_Btn_Start_y, ssmmAVal_Btn_Stop_x, ssmmAVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mStepsPerA, ssmmAVal_Btn_Start_x + 22, ssmmAVal_Btn_Start_y + 14);

  //Bottom Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmBVal_Btn_Start_x, ssmmBVal_Btn_Start_y, ssmmBVal_Btn_Stop_x, ssmmBVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmBVal_Btn_Start_x, ssmmBVal_Btn_Start_y, ssmmBVal_Btn_Stop_x, ssmmBVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mStepsPerB, ssmmBVal_Btn_Start_x + 22, ssmmBVal_Btn_Start_y + 14);
  
  //Frame Carriage Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmCVal_Btn_Start_x, ssmmCVal_Btn_Start_y, ssmmCVal_Btn_Stop_x, ssmmCVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmCVal_Btn_Start_x, ssmmCVal_Btn_Start_y, ssmmCVal_Btn_Stop_x, ssmmCVal_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mStepsPerC, ssmmCVal_Btn_Start_x + 22, ssmmCVal_Btn_Start_y + 14);

  //Motor A Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Steps/mm", ssmmAVal_Btn_Stop_x + 5, ssmmAVal_Btn_Start_y + 17);

  //Motor B Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Steps/mm", ssmmBVal_Btn_Stop_x + 5, ssmmBVal_Btn_Start_y + 17);
  
  //Motor C Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Steps/mm", ssmmCVal_Btn_Stop_x + 5, ssmmCVal_Btn_Start_y + 17);

  //Decrease A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmADec_Btn_Start_x, ssmmADec_Btn_Start_y, ssmmADec_Btn_Stop_x, ssmmADec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmADec_Btn_Start_x, ssmmADec_Btn_Start_y, ssmmADec_Btn_Stop_x, ssmmADec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", ssmmADec_Btn_Start_x + 16, ssmmADec_Btn_Start_y + 14);

  //Increase A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmAInc_Btn_Start_x, ssmmAInc_Btn_Start_y, ssmmAInc_Btn_Stop_x, ssmmAInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmAInc_Btn_Start_x, ssmmAInc_Btn_Start_y, ssmmAInc_Btn_Stop_x, ssmmAInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", ssmmAInc_Btn_Start_x + 15, ssmmAInc_Btn_Start_y + 14);

  //Amount Button A
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmAamt_Btn_Start_x, ssmmAamt_Btn_Start_y, ssmmAamt_Btn_Stop_x, ssmmAamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmAamt_Btn_Start_x, ssmmAamt_Btn_Start_y, ssmmAamt_Btn_Stop_x, ssmmAamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", ssmmAamt_Btn_Start_x + 9, ssmmAamt_Btn_Start_y + 17);

  //Decrease B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmBDec_Btn_Start_x, ssmmBDec_Btn_Start_y, ssmmBDec_Btn_Stop_x, ssmmBDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmBDec_Btn_Start_x, ssmmBDec_Btn_Start_y, ssmmBDec_Btn_Stop_x, ssmmBDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", ssmmBDec_Btn_Start_x + 16, ssmmBDec_Btn_Start_y + 14);

  //Increase B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmBInc_Btn_Start_x, ssmmBInc_Btn_Start_y, ssmmBInc_Btn_Stop_x, ssmmBInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmBInc_Btn_Start_x, ssmmBInc_Btn_Start_y, ssmmBInc_Btn_Stop_x, ssmmBInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", ssmmBInc_Btn_Start_x + 15, ssmmBInc_Btn_Start_y + 14);

  //Amount Button B
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmBamt_Btn_Start_x, ssmmBamt_Btn_Start_y, ssmmBamt_Btn_Stop_x, ssmmBamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmBamt_Btn_Start_x, ssmmBamt_Btn_Start_y, ssmmBamt_Btn_Stop_x, ssmmBamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", ssmmBamt_Btn_Start_x + 9, ssmmBamt_Btn_Start_y + 17);

  //Decrease C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmCDec_Btn_Start_x, ssmmCDec_Btn_Start_y, ssmmCDec_Btn_Stop_x, ssmmCDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmCDec_Btn_Start_x, ssmmCDec_Btn_Start_y, ssmmCDec_Btn_Stop_x, ssmmCDec_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", ssmmCDec_Btn_Start_x + 16, ssmmCDec_Btn_Start_y + 14);

  //Increase C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmCInc_Btn_Start_x, ssmmCInc_Btn_Start_y, ssmmCInc_Btn_Stop_x, ssmmCInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmCInc_Btn_Start_x, ssmmCInc_Btn_Start_y, ssmmCInc_Btn_Stop_x, ssmmCInc_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", ssmmCInc_Btn_Start_x + 15, ssmmCInc_Btn_Start_y + 14);

  //Amount Button C
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(ssmmCamt_Btn_Start_x, ssmmCamt_Btn_Start_y, ssmmCamt_Btn_Stop_x, ssmmCamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(ssmmCamt_Btn_Start_x, ssmmCamt_Btn_Start_y, ssmmCamt_Btn_Stop_x, ssmmCamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.1", ssmmCamt_Btn_Start_x + 9, ssmmCamt_Btn_Start_y + 17);

  //Back Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBck_Btn_Start_x, pnBck_Btn_Start_y, pnBck_Btn_Stop_x, pnBck_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Back", pnBck_Btn_Start_x + 17, pnBck_Btn_Start_y + 14);

  //Save Button
  tft.Set_Draw_color(GREEN);
  tft.Fill_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnSave_Btn_Start_x, pnSave_Btn_Start_y, pnSave_Btn_Stop_x, pnSave_Btn_Stop_y);
  tft.Set_Text_Size(2);
  tft.Set_Text_Back_colour(GREEN);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Save", pnSave_Btn_Start_x + 17, pnSave_Btn_Start_y + 14);
}

/**Get touch readings and redraw changes**/
void updateStepsMMScreen() {
  /***Touch Update***/
  TSPoint p = ts.getPoint();  //Leaves pins in input mode. Shared by lcd so must revert to output after reading.
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int x = map(p.x, TS_LEFT, TS_RIGHT, 0, 480);
  int y = map(p.y, TS_TOP, TS_BOT, 0, 320);

  //Steps/mm A Input Field
  if(x > ssmmAVal_Btn_Start_x && x < ssmmAVal_Btn_Stop_x && y > ssmmAVal_Btn_Start_y && y < ssmmAVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmAVal_Btn_Start_x, ssmmAVal_Btn_Start_y, ssmmAVal_Btn_Stop_x, ssmmAVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmAVal_Btn_Start_x, ssmmAVal_Btn_Start_y, ssmmAVal_Btn_Stop_x, ssmmAVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mStepsPerA, ssmmAVal_Btn_Start_x + 35, ssmmAVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    ssmmAValBtnFlag = true;

    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 10;
    activeScreen = 5;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 19;
    inputBuilder = mStepsPerA;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(ssmmAValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmAVal_Btn_Start_x, ssmmAVal_Btn_Start_y, ssmmAVal_Btn_Stop_x, ssmmAVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmAVal_Btn_Start_x, ssmmAVal_Btn_Start_y, ssmmAVal_Btn_Stop_x, ssmmAVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mStepsPerA, ssmmAVal_Btn_Start_x + 35, ssmmAVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    ssmmAValBtnFlag = false;
  }

  //Steps/mm B Input Field
  if(x > ssmmBVal_Btn_Start_x && x < ssmmBVal_Btn_Stop_x && y > ssmmBVal_Btn_Start_y && y < ssmmBVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmBVal_Btn_Start_x, ssmmBVal_Btn_Start_y, ssmmBVal_Btn_Stop_x, ssmmBVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmBVal_Btn_Start_x, ssmmBVal_Btn_Start_y, ssmmBVal_Btn_Stop_x, ssmmBVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mStepsPerB, ssmmBVal_Btn_Start_x + 35, ssmmBVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    ssmmBValBtnFlag = true;

    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 10;
    activeScreen = 5;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 20;
    inputBuilder = mStepsPerB;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(ssmmBValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmBVal_Btn_Start_x, ssmmBVal_Btn_Start_y, ssmmBVal_Btn_Stop_x, ssmmBVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmBVal_Btn_Start_x, ssmmBVal_Btn_Start_y, ssmmBVal_Btn_Stop_x, ssmmBVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mStepsPerB, ssmmBVal_Btn_Start_x + 35, ssmmBVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    ssmmBValBtnFlag = false;
  }

  //Steps/mm C Input Field
  if(x > ssmmCVal_Btn_Start_x && x < ssmmCVal_Btn_Stop_x && y > ssmmCVal_Btn_Start_y && y < ssmmCVal_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmCVal_Btn_Start_x, ssmmCVal_Btn_Start_y, ssmmCVal_Btn_Stop_x, ssmmCVal_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmCVal_Btn_Start_x, ssmmCVal_Btn_Start_y, ssmmCVal_Btn_Stop_x, ssmmCVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mStepsPerC, ssmmCVal_Btn_Start_x + 35, ssmmCVal_Btn_Start_y + 17);
    
    //Flag the button as pressed
    ssmmCValBtnFlag = true;

    //TODO: Open input screen and get input
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    prevActiveScreen = 10;
    activeScreen = 5;
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 21;
    inputBuilder = mStepsPerC;
    drawInputScreen();
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(ssmmCValBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmCVal_Btn_Start_x, ssmmCVal_Btn_Start_y, ssmmCVal_Btn_Stop_x, ssmmCVal_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmCVal_Btn_Start_x, ssmmCVal_Btn_Start_y, ssmmCVal_Btn_Stop_x, ssmmCVal_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String((String)mStepsPerC, ssmmCVal_Btn_Start_x + 35, ssmmCVal_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    ssmmCValBtnFlag = false;
  }

  //Motor A Decrease Button
  if(x > ssmmADec_Btn_Start_x && x < ssmmADec_Btn_Stop_x && y > ssmmADec_Btn_Start_y && y < ssmmADec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmADec_Btn_Start_x, ssmmADec_Btn_Start_y, ssmmADec_Btn_Stop_x, ssmmADec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmADec_Btn_Start_x, ssmmADec_Btn_Start_y, ssmmADec_Btn_Stop_x, ssmmADec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", ssmmADec_Btn_Start_x + 35, ssmmADec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    ssmmADecBtnFlag = true;
    mStepsPerA -= aAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(ssmmADecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmADec_Btn_Start_x, ssmmADec_Btn_Start_y, ssmmADec_Btn_Stop_x, ssmmADec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmADec_Btn_Start_x, ssmmADec_Btn_Start_y, ssmmADec_Btn_Stop_x, ssmmADec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", ssmmADec_Btn_Start_x + 35, ssmmADec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    ssmmADecBtnFlag = false;
  }

  //Motor B Decrease Button
  if(x > ssmmBDec_Btn_Start_x && x < ssmmBDec_Btn_Stop_x && y > ssmmBDec_Btn_Start_y && y < ssmmBDec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmBDec_Btn_Start_x, ssmmBDec_Btn_Start_y, ssmmBDec_Btn_Stop_x, ssmmBDec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmBDec_Btn_Start_x, ssmmBDec_Btn_Start_y, ssmmBDec_Btn_Stop_x, ssmmBDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", ssmmBDec_Btn_Start_x + 35, ssmmBDec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    ssmmBDecBtnFlag = true;
    mStepsPerB -= bAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(ssmmBDecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmBDec_Btn_Start_x, ssmmBDec_Btn_Start_y, ssmmBDec_Btn_Stop_x, ssmmBDec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmBDec_Btn_Start_x, ssmmBDec_Btn_Start_y, ssmmBDec_Btn_Stop_x, ssmmBDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", ssmmBDec_Btn_Start_x + 35, ssmmBDec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    ssmmBDecBtnFlag = false;
  }

  //Motor C Decrease Button
  if(x > ssmmCDec_Btn_Start_x && x < ssmmCDec_Btn_Stop_x && y > ssmmCDec_Btn_Start_y && y < ssmmCDec_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmCDec_Btn_Start_x, ssmmCDec_Btn_Start_y, ssmmCDec_Btn_Stop_x, ssmmCDec_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmCDec_Btn_Start_x, ssmmCDec_Btn_Start_y, ssmmCDec_Btn_Stop_x, ssmmCDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", ssmmCDec_Btn_Start_x + 35, ssmmCDec_Btn_Start_y + 17);
    
    //Flag the button as pressed
    ssmmCDecBtnFlag = true;
    mStepsPerC -= cAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(ssmmCDecBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmCDec_Btn_Start_x, ssmmCDec_Btn_Start_y, ssmmCDec_Btn_Stop_x, ssmmCDec_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmCDec_Btn_Start_x, ssmmCDec_Btn_Start_y, ssmmCDec_Btn_Stop_x, ssmmCDec_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("-", ssmmCDec_Btn_Start_x + 35, ssmmCDec_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    ssmmCDecBtnFlag = false;
  }

  //Motor A Increase Button
  if(x > ssmmAInc_Btn_Start_x && x < ssmmAInc_Btn_Stop_x && y > ssmmAInc_Btn_Start_y && y < ssmmAInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmAInc_Btn_Start_x, ssmmAInc_Btn_Start_y, ssmmAInc_Btn_Stop_x, ssmmAInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmAInc_Btn_Start_x, ssmmAInc_Btn_Start_y, ssmmAInc_Btn_Stop_x, ssmmAInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", ssmmAInc_Btn_Start_x + 35, ssmmAInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    ssmmAIncBtnFlag = true;
    mStepsPerA += aAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(ssmmAIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmAInc_Btn_Start_x, ssmmAInc_Btn_Start_y, ssmmAInc_Btn_Stop_x, ssmmAInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmAInc_Btn_Start_x, ssmmAInc_Btn_Start_y, ssmmAInc_Btn_Stop_x, ssmmAInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", ssmmAInc_Btn_Start_x + 35, ssmmAInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    ssmmAIncBtnFlag = false;
  }

  //Motor B Increase Button
  if(x > ssmmBInc_Btn_Start_x && x < ssmmBInc_Btn_Stop_x && y > ssmmBInc_Btn_Start_y && y < ssmmBInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmBInc_Btn_Start_x, ssmmBInc_Btn_Start_y, ssmmBInc_Btn_Stop_x, ssmmBInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmBInc_Btn_Start_x, ssmmBInc_Btn_Start_y, ssmmBInc_Btn_Stop_x, ssmmBInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", ssmmBInc_Btn_Start_x + 35, ssmmBInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    ssmmBIncBtnFlag = true;
    mStepsPerB += bAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(ssmmBIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmBInc_Btn_Start_x, ssmmBInc_Btn_Start_y, ssmmBInc_Btn_Stop_x, ssmmBInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmBInc_Btn_Start_x, ssmmBInc_Btn_Start_y, ssmmBInc_Btn_Stop_x, ssmmBInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", ssmmBInc_Btn_Start_x + 35, ssmmBInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    ssmmBIncBtnFlag = false;
  }

  //Motor C Increase Button
  if(x > ssmmCInc_Btn_Start_x && x < ssmmCInc_Btn_Stop_x && y > ssmmCInc_Btn_Start_y && y < ssmmCInc_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    //Change the button to a pressed appearance
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmCInc_Btn_Start_x, ssmmCInc_Btn_Start_y, ssmmCInc_Btn_Stop_x, ssmmCInc_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmCInc_Btn_Start_x, ssmmCInc_Btn_Start_y, ssmmCInc_Btn_Stop_x, ssmmCInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", ssmmCInc_Btn_Start_x + 35, ssmmCInc_Btn_Start_y + 17);
    
    //Flag the button as pressed
    ssmmCIncBtnFlag = true;
    mStepsPerC += cAmt;
    delay(50);
  } 
  //If the button is flagged as pressed but no longer being pressed
  else if(ssmmCIncBtnFlag) {
    //Change the button to an unpressed appearance
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmCInc_Btn_Start_x, ssmmCInc_Btn_Start_y, ssmmCInc_Btn_Stop_x, ssmmCInc_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmCInc_Btn_Start_x, ssmmCInc_Btn_Start_y, ssmmCInc_Btn_Stop_x,ssmmCInc_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    tft.Print_String("+", ssmmCInc_Btn_Start_x + 35, ssmmCInc_Btn_Start_y + 17);

    //Reset the button flag to unpressed
    ssmmCIncBtnFlag = false;
  }

  //Motor A Amount Toggle Button
  if(x > ssmmAamt_Btn_Start_x && x < ssmmAamt_Btn_Stop_x && y > ssmmAamt_Btn_Start_y && y < ssmmAamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmAamt_Btn_Start_x, ssmmAamt_Btn_Start_y, ssmmAamt_Btn_Stop_x, ssmmAamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmAamt_Btn_Start_x, ssmmAamt_Btn_Start_y, ssmmAamt_Btn_Stop_x, ssmmAamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(aAmt == 0.1) {
      aAmt = 1;
      tft.Print_String("1 step/mm", ssmmAamt_Btn_Start_x + 10, ssmmAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 1) {
      aAmt = 10;
      tft.Print_String("10 step/mm", ssmmAamt_Btn_Start_x + 8, ssmmAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 10) {
      aAmt = 25;
      tft.Print_String("25 step/mm", ssmmAamt_Btn_Start_x + 8, ssmmAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 25) {
      aAmt = 0.1;
      tft.Print_String("0.1 step/mm", ssmmAamt_Btn_Start_x + 7, ssmmAamt_Btn_Start_y + 16);
    }

    ssmmAamtBtnFlag = true;
    delay(50);
  }
  else if(ssmmAamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmAamt_Btn_Start_x, ssmmAamt_Btn_Start_y, ssmmAamt_Btn_Stop_x, ssmmAamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmAamt_Btn_Start_x, ssmmAamt_Btn_Start_y, ssmmAamt_Btn_Stop_x, ssmmAamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(aAmt == 0.1) {
      tft.Print_String("0.1 step/mm", ssmmAamt_Btn_Start_x + 7, ssmmAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 1) {
      tft.Print_String("1 step/mm", ssmmAamt_Btn_Start_x + 10, ssmmAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 10) {
      tft.Print_String("10 step/mm", ssmmAamt_Btn_Start_x + 8, ssmmAamt_Btn_Start_y + 16);
    }
    else if(aAmt == 25) {
      tft.Print_String("25 step/mm", ssmmAamt_Btn_Start_x + 8, ssmmAamt_Btn_Start_y + 16);
    }
    ssmmAamtBtnFlag = false;
  }
  
  //Motor B Amount Toggle Button
  if(x > ssmmBamt_Btn_Start_x && x < ssmmBamt_Btn_Stop_x && y > ssmmBamt_Btn_Start_y && y < ssmmBamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmBamt_Btn_Start_x, ssmmBamt_Btn_Start_y, ssmmBamt_Btn_Stop_x, ssmmBamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmBamt_Btn_Start_x, ssmmBamt_Btn_Start_y, ssmmBamt_Btn_Stop_x, ssmmBamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(bAmt == 0.1) {
      bAmt = 1;
      tft.Print_String("1 step/mm", ssmmBamt_Btn_Start_x + 10, ssmmBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 1) {
      bAmt = 10;
      tft.Print_String("10 step/mm", ssmmBamt_Btn_Start_x + 8, ssmmBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 10) {
      bAmt = 25;
      tft.Print_String("25 step/mm", ssmmBamt_Btn_Start_x + 8, ssmmBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 25) {
      bAmt = 0.1;
      tft.Print_String("0.1 step/mm", ssmmBamt_Btn_Start_x + 7, ssmmBamt_Btn_Start_y + 16);
    }

    ssmmBamtBtnFlag = true;
    delay(50);
  }
  else if(ssmmBamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmBamt_Btn_Start_x, ssmmBamt_Btn_Start_y, ssmmBamt_Btn_Stop_x, ssmmBamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmBamt_Btn_Start_x, ssmmBamt_Btn_Start_y, ssmmBamt_Btn_Stop_x, ssmmBamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(bAmt == 0.1) {
      tft.Print_String("0.1 step/mm", ssmmBamt_Btn_Start_x + 7, ssmmBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 1) {
      tft.Print_String("1 step/mm", ssmmBamt_Btn_Start_x + 10, ssmmBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 10) {
      tft.Print_String("10 step/mm", ssmmBamt_Btn_Start_x + 8, ssmmBamt_Btn_Start_y + 16);
    }
    else if(bAmt == 25) {
      tft.Print_String("25 step/mm", ssmmBamt_Btn_Start_x + 8, ssmmBamt_Btn_Start_y + 16);
    }
    ssmmBamtBtnFlag = false;
  }
  
  //Motor C Amount Toggle Button
  if(x > ssmmCamt_Btn_Start_x && x < ssmmCamt_Btn_Stop_x && y > ssmmCamt_Btn_Start_y && y < ssmmCamt_Btn_Stop_y && p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
    tft.Set_Draw_color(GRAY);
    tft.Fill_Rectangle(ssmmCamt_Btn_Start_x, ssmmCamt_Btn_Start_y, ssmmCamt_Btn_Stop_x, ssmmCamt_Btn_Stop_y);
    tft.Set_Draw_color(WHITE);
    tft.Draw_Rectangle(ssmmCamt_Btn_Start_x, ssmmCamt_Btn_Start_y, ssmmCamt_Btn_Stop_x, ssmmCamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(GRAY);
    tft.Set_Text_colour(BLACK);
    
    if(cAmt == 0.1) {
      cAmt = 1;
      tft.Print_String("1 step/mm", ssmmCamt_Btn_Start_x + 10, ssmmCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 1) {
      cAmt = 10;
      tft.Print_String("10 step/mm", ssmmCamt_Btn_Start_x + 8, ssmmCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 10) {
      cAmt = 25;
      tft.Print_String("50 step/mm", ssmmCamt_Btn_Start_x + 8, ssmmCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 25) {
      cAmt = 0.1;
      tft.Print_String("0.1 step/mm", ssmmCamt_Btn_Start_x + 7, ssmmCamt_Btn_Start_y + 16);
    }

    ssmmCamtBtnFlag = true;
    delay(50);
  }
  else if(ssmmCamtBtnFlag) {
    tft.Set_Draw_color(WHITE);
    tft.Fill_Rectangle(ssmmCamt_Btn_Start_x, ssmmCamt_Btn_Start_y, ssmmCamt_Btn_Stop_x, ssmmCamt_Btn_Stop_y);
    tft.Set_Draw_color(GRAY);
    tft.Draw_Rectangle(ssmmCamt_Btn_Start_x, ssmmCamt_Btn_Start_y, ssmmCamt_Btn_Stop_x, ssmmCamt_Btn_Stop_y);
    tft.Set_Text_Size(1);
    tft.Set_Text_Back_colour(WHITE);
    tft.Set_Text_colour(BLACK);
    
    if(cAmt == 0.1) {
      tft.Print_String("0.1 step/mm", ssmmCamt_Btn_Start_x + 7, ssmmCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 1) {
      tft.Print_String("1 step/mm", ssmmCamt_Btn_Start_x + 10, ssmmCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 10) {
      tft.Print_String("10 step/mm", ssmmCamt_Btn_Start_x + 8, ssmmCamt_Btn_Start_y + 16);
    }
    else if(cAmt == 25) {
      tft.Print_String("25 step/mm", ssmmCamt_Btn_Start_x + 8, ssmmCamt_Btn_Start_y + 16);
    }
    ssmmCamtBtnFlag = false;
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
      
    //Flag the button as pressed
    pnBckBtnFlag = true;
   
    //TODO:
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 0;
    inputBuilder = "";
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 4;
    drawMachineSettingsScreen();
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
      
    //Flag the button as pressed
    pnSaveBtnFlag = true;
    
    //TODO:
    //0=none, 1=profileName, 2=tClampDist, 3=bClampDist, 4=hDist1, 5=hDist2, 6=hDist3
    //7=mVelocityA, 8=mVelocityB, 9=mVelocityC, 10=mAccelA, 11=mAccelB, 12=mAccelC
    //13=mJerkA, 14=mJerkB, 15=mJerkC, 16=mUStepA, 17=mUStepB, 18=mUStepC
    //19=mStepsPerA, 20=mStepsPerB, 21=mStepsPerC, 22=homeOffsetT, 23=homeOffsetB
    //24=homeOffsetClampT, 25=homeOffsetClampB
    varInputRegister = 0;
    inputBuilder = "";
    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
    activeScreen = 4;
    drawMachineSettingsScreen();
    delay(50);
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
