#include "Arduino.h"
#include "GUI.h"
#include <LCDWIKI_GUI.h>
#include <LCDWIKI_KBV.h>
#include <TouchScreen.h>
   
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

//LCD Pin Connections
#define LCD_RESET A4
#define LCD_CS    A3 
#define LCD_CD    A2
#define LCD_WR    A1  
#define LCD_RD    A0

const int YP = A2;  // must be an analog pin, use "An" notation!
const int XM = A3;  // must be an analog pin, use "An" notation!
const int YM = 8;   // can be a digital pin
const int XP = 9;   // can be a digital pin

LCDWIKI_KBV tft(ILI9486,LCD_CD,LCD_CD,LCD_WR,LCD_RD,LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

GUI::GUI(int _mStepsA, int _mStepsB, int _mStepsC, float _mStepsPerA, float _mStepsPerB, float _mStepsPerC) {		//Touch Screen Connections
int mStepsA = _mStepsA;
int mStepsB = _mStepsB;
int mStepsC = _mStepsC;
int mStepsPerA = _mStepsPerA;
int mStepsPerB = _mStepsPerB;
int mStepsPerC = _mStepsPerC;

int prevActiveScreen;
		
//#region Main Screen
	//Button States
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
	
	//Button Dimensions
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
	
	//Button Variables
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
//#endregion

//#region InputScreen
	//Button States
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
	
	//Button Dimensions
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
//#endregion

//#region	Load Profile Screen
	//Button States	
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
	
	//Button Dimensions
	int backBtnX = 80;
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
//#endregion

//#region New Profile Screen
	//Button States
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
	
	//Button Dimensions
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
//#endregion

//#region Edit Profile Screen
	//Button States
	//Same as 'New Profile Screen'
	
	//Button Dimensions
	//Same as 'New Profile Screen'
	
	//Button Positioning
	//Same as 'New Profile Screen'
//#endregion

//#region Machine Settings Screen
	//Button States
	boolean sVelBtnFlag = false;
	boolean sAccelBtnFlag = false;
	boolean sJerkBtnFlag = false;
	boolean sUSBtnFlag = false;
	boolean sSmmBtnFlag = false;
	boolean sRstBtnFlag = false;
	
	//Button Dimensions
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
//#endregion

//#region Velocity Settings Screen
	//Button States
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
	
	//Button Dimensions
	int field2BtnX = 100;
	int field2BtnY = 40;

	int chgValBtnX = 40;
	int chgValBtnY = 40;

	//Button Values
	float aAmt = 1;
	float bAmt = 1;
	float cAmt = 1;

	//Button Positioning
	int svAVal_Btn_Start_x = 30;
	int svAVal_Btn_Start_y = 40;
	int svAVal_Btn_Stop_x = svAVal_Btn_Start_x + fieldBtnX;
	int svAVal_Btn_Stop_y = svAVal_Btn_Start_y + fieldBtnY;

	int svBVal_Btn_Start_x = svAVal_Btn_Stop_x + 10;
	int svBVal_Btn_Start_y = svAVal_Btn_Stop_y + 40;
	int svBVal_Btn_Stop_x = svBVal_Btn_Start_x + fieldBtnX;
	int svBVal_Btn_Stop_y = svBVal_Btn_Start_y + fieldBtnY;

	int svCVal_Btn_Start_x = svBVal_Btn_Stop_x + 10;
	int svCVal_Btn_Start_y = svBVal_Btn_Stop_y + 40;
	int svCVal_Btn_Stop_x = svCVal_Btn_Start_x + fieldBtnX;
	int svCVal_Btn_Stop_y = svCVal_Btn_Start_y + fieldBtnY;

	int svADec_Btn_Start_x = svAVal_Btn_Stop_x + 10;
	int svADec_Btn_Start_y = svAVal_Btn_Start_y;
	int svADec_Btn_Stop_x = svADec_Btn_Start_x + chgValBtnX;
	int svADec_Btn_Stop_y = svADec_Btn_Start_y + chgValBtnY;

	int svBDec_Btn_Start_x = svBVal_Btn_Stop_x + 10;
	int svBDec_Btn_Start_y = svBVal_Btn_Start_y;
	int svBDec_Btn_Stop_x = svBDec_Btn_Start_x + chgValBtnX;
	int svBDec_Btn_Stop_y = svBDec_Btn_Start_y + chgValBtnY;

	int svCDec_Btn_Start_x = svCVal_Btn_Stop_x + 10;
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
//#endregion

//#region Acceleration Settings Screen
	//Button States
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
	
	//Button Dimensions
	//Same as 'Velocity Settings Screen'

	//Button Positioning
	int saAVal_Btn_Start_x = 30;
	int saAVal_Btn_Start_y = 40;
	int saAVal_Btn_Stop_x = saAVal_Btn_Start_x + fieldBtnX;
	int saAVal_Btn_Stop_y = saAVal_Btn_Start_y + fieldBtnY;

	int saBVal_Btn_Start_x = saAVal_Btn_Stop_x + 10;
	int saBVal_Btn_Start_y = saAVal_Btn_Stop_y + 40;
	int saBVal_Btn_Stop_x = saBVal_Btn_Start_x + fieldBtnX;
	int saBVal_Btn_Stop_y = saBVal_Btn_Start_y + fieldBtnY;

	int saCVal_Btn_Start_x = saBVal_Btn_Stop_x + 10;
	int saCVal_Btn_Start_y = saBVal_Btn_Stop_y + 40;
	int saCVal_Btn_Stop_x = saCVal_Btn_Start_x + fieldBtnX;
	int saCVal_Btn_Stop_y = saCVal_Btn_Start_y + fieldBtnY;

	int saADec_Btn_Start_x = saAVal_Btn_Stop_x + 10;
	int saADec_Btn_Start_y = saAVal_Btn_Start_y;
	int saADec_Btn_Stop_x = saADec_Btn_Start_x + chgValBtnX;
	int saADec_Btn_Stop_y = saADec_Btn_Start_y + chgValBtnY;

	int saBDec_Btn_Start_x = saBVal_Btn_Stop_x + 10;
	int saBDec_Btn_Start_y = saBVal_Btn_Start_y;
	int saBDec_Btn_Stop_x = saBDec_Btn_Start_x + chgValBtnX;
	int saBDec_Btn_Stop_y = saBDec_Btn_Start_y + chgValBtnY;

	int saCDec_Btn_Start_x = saCVal_Btn_Stop_x + 10;
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
//#endregion

//#region Jerk Settings Screen
	//Button States
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
	
	//Button Dimensions
	//Same as 'Velocity Settings Screen'

	//Button Positioning
	int sjAVal_Btn_Start_x = 30;
	int sjAVal_Btn_Start_y = 40;
	int sjAVal_Btn_Stop_x = sjAVal_Btn_Start_x + fieldBtnX;
	int sjAVal_Btn_Stop_y = sjAVal_Btn_Start_y + fieldBtnY;

	int sjBVal_Btn_Start_x = sjAVal_Btn_Stop_x + 10;
	int sjBVal_Btn_Start_y = sjAVal_Btn_Stop_y + 40;
	int sjBVal_Btn_Stop_x = sjBVal_Btn_Start_x + fieldBtnX;
	int sjBVal_Btn_Stop_y = sjBVal_Btn_Start_y + fieldBtnY;

	int sjCVal_Btn_Start_x = sjBVal_Btn_Stop_x + 10;
	int sjCVal_Btn_Start_y = sjBVal_Btn_Stop_y + 40;
	int sjCVal_Btn_Stop_x = sjCVal_Btn_Start_x + fieldBtnX;
	int sjCVal_Btn_Stop_y = sjCVal_Btn_Start_y + fieldBtnY;

	int sjADec_Btn_Start_x = sjAVal_Btn_Stop_x + 10;
	int sjADec_Btn_Start_y = sjAVal_Btn_Start_y;
	int sjADec_Btn_Stop_x = sjADec_Btn_Start_x + chgValBtnX;
	int sjADec_Btn_Stop_y = sjADec_Btn_Start_y + chgValBtnY;

	int sjBDec_Btn_Start_x = sjBVal_Btn_Stop_x + 10;
	int sjBDec_Btn_Start_y = sjBVal_Btn_Start_y;
	int sjBDec_Btn_Stop_x = sjBDec_Btn_Start_x + chgValBtnX;
	int sjBDec_Btn_Stop_y = sjBDec_Btn_Start_y + chgValBtnY;

	int sjCDec_Btn_Start_x = sjCVal_Btn_Stop_x + 10;
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
//#endregion

//#region Microstep Settings Screen
	//Button States
	boolean mAuSBtnFlag = false;
	boolean mBuSBtnFlag = false;
	boolean mCuSBtnFlag = false;
	boolean mAinvBtnFlag = false;
	boolean mBinvBtnFlag = false;
	boolean mCinvBtnFlag = false;
	
	//Button Dimensions
	int uSBtnX = 100;
	int uSBtnY = 100;

	int invBtnX = 25;
	int invBtnY = 25;

	//Button Positioning
	int mAuS_Btn_Start_x = 50;
	int mAuS_Btn_Start_y = 100;
	int mAuS_Btn_Stop_x = mAuS_Btn_Start_x + uSBtnX;
	int mAuS_Btn_Stop_y = mAuS_Btn_Stop_y + uSBtnY;

	int mBuS_Btn_Start_x = mAuS_Btn_Start_x + uSBtnX + 10;
	int mBuS_Btn_Start_y = 100;
	int mBuS_Btn_Stop_x = mBuS_Btn_Start_x + uSBtnX;
	int mBuS_Btn_Stop_y = mBuS_Btn_Stop_y + uSBtnY;

	int mCuS_Btn_Start_x = mBuS_Btn_Start_x + uSBtnX + 15;
	int mCuS_Btn_Start_y = 100;
	int mCuS_Btn_Stop_x = mCuS_Btn_Start_x + uSBtnX;
	int mCuS_Btn_Stop_y = mCuS_Btn_Start_y + uSBtnY;

	int mAinv_Btn_Start_x = mAuS_Btn_Start_x;
	int mAinv_Btn_Start_y = 200;
	int mAinv_Btn_Stop_x = mAinv_Btn_Start_x + invBtnX;
	int mAinv_Btn_Stop_y = mAinv_Btn_Start_y + invBtnY;

	int mBinv_Btn_Start_x = mBuS_Btn_Start_x;
	int mBinv_Btn_Start_y = 200;
	int mBinv_Btn_Stop_x = mBinv_Btn_Start_x + invBtnX;
	int mBinv_Btn_Stop_y = mBinv_Btn_Start_y + invBtnY;

	int mCinv_Btn_Start_x = mCuS_Btn_Start_x;
	int mCinv_Btn_Start_y = 200;
	int mCinv_Btn_Stop_x = mCinv_Btn_Start_x + invBtnX;
	int mCinv_Btn_Stop_y = mCinv_Btn_Start_y + invBtnY;
//#endregion

//#region Steps/mm Settings Screen
	//Button States
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

	
	//Button Dimensions
	//Same as 'Velocity Settings Screen'

	//Button Positioning
	int ssmmAVal_Btn_Start_x = 30;
	int ssmmAVal_Btn_Start_y = 40;
	int ssmmAVal_Btn_Stop_x = ssmmAVal_Btn_Start_x + fieldBtnX;
	int ssmmAVal_Btn_Stop_y = ssmmAVal_Btn_Start_y + fieldBtnY;

	int ssmmBVal_Btn_Start_x = ssmmAVal_Btn_Stop_x + 10;
	int ssmmBVal_Btn_Start_y = ssmmAVal_Btn_Stop_y + 40;
	int ssmmBVal_Btn_Stop_x = ssmmBVal_Btn_Start_x + fieldBtnX;
	int ssmmBVal_Btn_Stop_y = ssmmBVal_Btn_Start_y + fieldBtnY;

	int ssmmCVal_Btn_Start_x = ssmmBVal_Btn_Stop_x + 10;
	int ssmmCVal_Btn_Start_y = ssmmBVal_Btn_Stop_y + 40;
	int ssmmCVal_Btn_Stop_x = ssmmCVal_Btn_Start_x + fieldBtnX;
	int ssmmCVal_Btn_Stop_y = ssmmCVal_Btn_Start_y + fieldBtnY;

	int ssmmADec_Btn_Start_x = ssmmAVal_Btn_Stop_x + 10;
	int ssmmADec_Btn_Start_y = ssmmAVal_Btn_Start_y;
	int ssmmADec_Btn_Stop_x = ssmmADec_Btn_Start_x + chgValBtnX;
	int ssmmADec_Btn_Stop_y = ssmmADec_Btn_Start_y + chgValBtnY;

	int ssmmBDec_Btn_Start_x = ssmmBVal_Btn_Stop_x + 10;
	int ssmmBDec_Btn_Start_y = ssmmBVal_Btn_Start_y;
	int ssmmBDec_Btn_Stop_x = ssmmBDec_Btn_Start_x + chgValBtnX;
	int ssmmBDec_Btn_Stop_y = ssmmBDec_Btn_Start_y + chgValBtnY;

	int ssmmCDec_Btn_Start_x = ssmmCVal_Btn_Stop_x + 10;
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

	//Touch parameter calibration
	int TS_LEFT = 70;
	int TS_RIGHT = 940;
	int TS_TOP = 130;
	int TS_BOT = 900;

	int activeScreen = 0; //0 = home screen
		
	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);
  
	/***Screen Setup***/
	tft.Init_LCD();
	tft.Set_Rotation(1);
	tft.Fill_Screen(BLACK);
	tft.Set_Text_Mode(0);
  
	//0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=microstep, 10=step/mm
	activeScreen = 0;
	drawMainScreen();
}

void GUI::drawMainScreen() {
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

void GUI::updateMainScreen() {
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

void GUI::drawInputScreen() {
	tft.Fill_Screen(BLACK);   //Change the screen to solid black
  
  //Value Field Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);

  //Value Text
  tft.Set_Text_Size(4);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
  
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

void GUI::updateInputScreen() {
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
    
    int integerPart = (int)inputVal;
    float decimalPart = (inputVal - integerPart);        
    
    if(decimalBtnFlag) {
      inputVal = integerPart + ((inputVal - integerPart)/10);
    }
    else {   
      inputVal = (integerPart / 10) + decimalPart;  
      
      if(inputVal < 0.01) {    //If the new value is less than the display precision (0.00)
        inputVal = 0;           //Set the input value to 0
      }
    }
    tft.Set_Draw_color(YELLOW);
    tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
    tft.Set_Text_Size(4);
    tft.Set_Text_Back_colour(BLACK);
    tft.Set_Text_colour(YELLOW);
    tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
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

      //TODO: Add '1' to the input
      if(decimalBtnFlag) {
        int integerPart = (int)inputVal;
        int decimalPart = (inputVal - integerPart);        
        inputVal = integerPart + ((inputVal - integerPart)/10) + (1/10);
        
        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + numBtnX + xPadding, yPadding, 480 - numBtnX - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
      else {    
        inputVal = (inputVal * 10) + 1;           //Move decimal one point left

        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
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
    tft.Print_String("2", num1_Btn_Start_x + 70, num1_Btn_Start_y + 18);

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
      
      //TODO: Add '2' to the input
      if(decimalBtnFlag) {
        int integerPart = (int)inputVal;
        int decimalPart = (inputVal - integerPart);        
        inputVal = integerPart + ((inputVal - integerPart)/10) + (2/10);
        
        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
      else {    
        inputVal = (inputVal * 10) + 2;           //Move decimal one point left

        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
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

      //TODO: Add '3' to the input
      if(decimalBtnFlag) {
        int integerPart = (int)inputVal;
        int decimalPart = (inputVal - integerPart);        
        inputVal = integerPart + ((inputVal - integerPart)/10) + (3/10);
        
        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
      else {    
        inputVal = (inputVal * 10) + 3;           //Move decimal one point left

        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
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

      //TODO: Add '4' to the input
      if(decimalBtnFlag) {
        int integerPart = (int)inputVal;
        int decimalPart = (inputVal - integerPart);        
        inputVal = integerPart + ((inputVal - integerPart)/10) + (4/10);
        
        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
      else {    
        inputVal = (inputVal * 10) + 4;           //Move decimal one point left

        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
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

      //TODO: Add '5' to the input
      if(decimalBtnFlag) {
        int integerPart = (int)inputVal;
        int decimalPart = (inputVal - integerPart);        
        inputVal = integerPart + ((inputVal - integerPart)/10) + (5/10);
        
        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
      else {    
        inputVal = (inputVal * 10) + 5;           //Move decimal one point left

        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
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

      //TODO: Add '6' to the input
      if(decimalBtnFlag) {
        int integerPart = (int)inputVal;
        int decimalPart = (inputVal - integerPart);        
        inputVal = integerPart + (inputVal - integerPart)/10 + 6/10;
        
        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
      else {    
        inputVal = (inputVal * 10) + 6;           //Move decimal one point left

        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
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

      //TODO: Add '7' to the input
      if(decimalBtnFlag) {
        int integerPart = (int)inputVal;
        int decimalPart = (inputVal - integerPart);        
        inputVal = integerPart + ((inputVal - integerPart)/10) + (7/10);
        
        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
      else {    
        inputVal = (inputVal * 10) + 7;           //Move decimal one point left

        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
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

      //TODO: Add '8' to the input
      if(decimalBtnFlag) {
        int integerPart = (int)inputVal;
        int decimalPart = (inputVal - integerPart);        
        inputVal = integerPart + ((inputVal - integerPart)/10) + (8/10);
        
        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
      else {    
        inputVal = (inputVal * 10) + 8;           //Move decimal one point left

        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
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

      //TODO: Add '9' to the input
      if(decimalBtnFlag) {
        int integerPart = (int)inputVal;
        int decimalPart = (inputVal - integerPart);        
        inputVal = integerPart + ((inputVal - integerPart)/10) + (9/10);
        
        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
      else {    
        inputVal = (inputVal * 10) + 9;           //Move decimal one point left

        tft.Set_Draw_color(YELLOW);
        tft.Draw_Rectangle(xPadding + (numBtnX / 2) + xPadding, yPadding, 480 - (numBtnX / 2) - (xPadding * 2), numBtnY + yPadding);
        tft.Set_Text_Size(4);
        tft.Set_Text_Back_colour(BLACK);
        tft.Set_Text_colour(YELLOW);
        tft.Print_Number_Float(inputVal, 2, numBtnX + 45, yPadding + 18, '.', 0, ' ');   //(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
      }
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
      
      //Flag the decimal button as pressed
      decimalBtnFlag = true;  
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
    //TODO: 
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

void GUI::drawProfileLoadScreen() {
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
  tft.Print_String("Back", pBack_Btn_Start_x + 25, pBack_Btn_Start_y + 17);

  //Button 'Confirm'
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pCfm_Btn_Start_x, pCfm_Btn_Start_y, pCfm_Btn_Stop_x, pCfm_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pCfm_Btn_Start_x, pCfm_Btn_Start_y, pCfm_Btn_Stop_x, pCfm_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("Confirm", pCfm_Btn_Start_x + 25, pCfm_Btn_Start_y + 17);

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

void GUI::updateProfileLoadScreen() {
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
      //TODO: 
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
      //TODO:
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
      //TODO:
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
      //TODO:
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
      //TODO:
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
      //TODO:
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
      //TODO:
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
      //TODO:
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
      //TODO:
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
      //TODO:
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

void GUI::drawNewProfileScreen() {
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
  tft.Print_String("Num of Steps", 30, pnStp1_Btn_Start_y);

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

void GUI::updateNewProfileScreen() {
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
    
    //TODO: Save profile and move to position
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

void GUI::drawEditProfileScreen() {
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
  tft.Print_String((String)profile, pnName_Btn_Start_x + 17, pnName_Btn_Start_y + 17);

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
  tft.Print_String((String)tClampDist, pnTClamp_Btn_Start_x + 17, pnTClamp_Btn_Start_y + 17);

  //Bottom Clamp Position Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Bottom Clamp Position: ", 30, pnBClamp_Btn_Start_y);

  //Bottom Clamp Position Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(pnBClamp_Btn_Start_x, pnBClamp_Btn_Start_y, pnBClamp_Btn_Stop_x, pnBClamp_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)bClampDist, pnBClamp_Btn_Start_x + 17, pnBClamp_Btn_Start_y + 17);

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
  tft.Print_String((String)hDist1, pnFCh_Btn_Start_x + 17, pnFCh_Btn_Start_y + 17);

  //Number of Steps Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(WHITE);
  tft.Print_String("Num of Steps", 30, pnStp1_Btn_Start_y);

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

void GUI::updateEditProfileScreen() {
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
      tft.Print_String((String)profile, pnName_Btn_Start_x + 17, pnName_Btn_Start_y + 17);
        
      //Flag the delete button as pressed
      pnNameBtnFlag = true;
      
      //TODO: Load input screen and display data
      //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
      prevActiveScreen = 3;
      activeScreen = 5;
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

    //0=main, 1=load, 2=new, 3=edit, 4=settings, 5=input, 6=vel, 7=accel, 8=jerk, 9=step/mm
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
    
    //TODO: Save profile and move to position
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

void GUI::drawMachineSettingsScreen() {
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

void GUI::updateMachineSettingsScreen() {
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

void GUI::drawVelScreen() {
	 tft.Fill_Screen(BLACK);    //Change the screen to solid black  

  //Machine Settings Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 40);

  //Machine Settings Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Velocity Settings", 240, yPadding);

  //Motor A Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Top Clamp Motor (A): ", 240, yPadding);

  //Motor B Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Bottom Clamp Motor (B): ", 240, yPadding);
  
  //Motor C Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Frame Carriage Motors (C): ", 240, yPadding);

  //Top Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svAVal_Btn_Start_x, svAVal_Btn_Start_y, svAVal_Btn_Stop_x, svAVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svAVal_Btn_Start_x, svAVal_Btn_Start_y, svAVal_Btn_Stop_x, svAVal_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mVelocityA, svAVal_Btn_Start_x + 17, svAVal_Btn_Start_y + 17);

  //Bottom Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svBVal_Btn_Start_x, svBVal_Btn_Start_y, svBVal_Btn_Stop_x, svBVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svBVal_Btn_Start_x, svBVal_Btn_Start_y, svBVal_Btn_Stop_x, svBVal_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mVelocityB, svBVal_Btn_Start_x + 17, svBVal_Btn_Start_y + 17);
  
  //Frame Carriage Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svCVal_Btn_Start_x, svCVal_Btn_Start_y, svCVal_Btn_Stop_x, svCVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svCVal_Btn_Start_x, svCVal_Btn_Start_y, svCVal_Btn_Stop_x, svCVal_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mVelocityC, svCVal_Btn_Start_x + 17, svCVal_Btn_Start_y + 17);

  //Motor A Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s", svAVal_Btn_Stop_x + 5, svAVal_Btn_Start_y + 5);

  //Motor B Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s", svBVal_Btn_Stop_x + 5, svBVal_Btn_Start_y + 5);
  
  //Motor C Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s", svCVal_Btn_Stop_x + 5, svCVal_Btn_Start_y + 5);

  //Decrease A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svADec_Btn_Start_x, svADec_Btn_Start_y, svADec_Btn_Stop_x, svADec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svADec_Btn_Start_x, svADec_Btn_Start_y, svADec_Btn_Stop_x, svADec_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", svADec_Btn_Start_x + 7, svADec_Btn_Start_y + 15);

  //Increase A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svAInc_Btn_Start_x, svAInc_Btn_Start_y, svAInc_Btn_Stop_x, svAInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svAInc_Btn_Start_x, svAInc_Btn_Start_y, svAInc_Btn_Stop_x, svAInc_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", svAInc_Btn_Start_x + 7, svAInc_Btn_Start_y + 15);

  //Amount Button A
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svAamt_Btn_Start_x, svAamt_Btn_Start_y, svAamt_Btn_Stop_x, svAamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svAamt_Btn_Start_x, svAamt_Btn_Start_y, svAamt_Btn_Stop_x, svAamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.01", svAamt_Btn_Start_x + 7, svAamt_Btn_Start_y + 15);

  //Decrease B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svBDec_Btn_Start_x, svBDec_Btn_Start_y, svBDec_Btn_Stop_x, svBDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svBDec_Btn_Start_x, svBDec_Btn_Start_y, svBDec_Btn_Stop_x, svBDec_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", svBDec_Btn_Start_x + 7, svBDec_Btn_Start_y + 15);

  //Increase B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svBInc_Btn_Start_x, svBInc_Btn_Start_y, svBInc_Btn_Stop_x, svBInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svBInc_Btn_Start_x, svBInc_Btn_Start_y, svBInc_Btn_Stop_x, svBInc_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", svBInc_Btn_Start_x + 7, svBInc_Btn_Start_y + 15);

  //Amount Button B
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svBamt_Btn_Start_x, svBamt_Btn_Start_y, svBamt_Btn_Stop_x, svBamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svBamt_Btn_Start_x, svBamt_Btn_Start_y, svBamt_Btn_Stop_x, svBamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.01", svBamt_Btn_Start_x + 7, svBamt_Btn_Start_y + 15);

  //Decrease C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svCDec_Btn_Start_x, svCDec_Btn_Start_y, svCDec_Btn_Stop_x, svCDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svCDec_Btn_Start_x, svCDec_Btn_Start_y, svCDec_Btn_Stop_x, svCDec_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", svCDec_Btn_Start_x + 7, svCDec_Btn_Start_y + 15);

  //Increase C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svCInc_Btn_Start_x, svCInc_Btn_Start_y, svCInc_Btn_Stop_x, svCInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svCInc_Btn_Start_x, svCInc_Btn_Start_y, svCInc_Btn_Stop_x, svCInc_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", svCInc_Btn_Start_x + 7, svCInc_Btn_Start_y + 15);

  //Amount Button C
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(svCamt_Btn_Start_x, svCamt_Btn_Start_y, svCamt_Btn_Stop_x, svCamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(svCamt_Btn_Start_x, svCamt_Btn_Start_y, svCamt_Btn_Stop_x, svCamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.01", svCamt_Btn_Start_x + 7, svCamt_Btn_Start_y + 15);

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

void GUI::updateVelScreen() {
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    
    //TODO:
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

void GUI::drawAccelScreen() {
	tft.Fill_Screen(BLACK);    //Change the screen to solid black  

  //Machine Settings Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 40);

  //Machine Settings Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Acceleration Settings", 240, yPadding);

  //Motor A Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Top Clamp Motor (A): ", 240, yPadding);

  //Motor B Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Bottom Clamp Motor (B): ", 240, yPadding);
  
  //Motor C Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Frame Carriage Motors (C): ", 240, yPadding);

  //Top Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saAVal_Btn_Start_x, saAVal_Btn_Start_y, saAVal_Btn_Stop_x, saAVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saAVal_Btn_Start_x, saAVal_Btn_Start_y, saAVal_Btn_Stop_x, saAVal_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mAccelA, saAVal_Btn_Start_x + 17, saAVal_Btn_Start_y + 17);

  //Bottom Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saBVal_Btn_Start_x, saBVal_Btn_Start_y, saBVal_Btn_Stop_x, saBVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saBVal_Btn_Start_x, saBVal_Btn_Start_y, saBVal_Btn_Stop_x, saBVal_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mAccelB, saBVal_Btn_Start_x + 17, saBVal_Btn_Start_y + 17);
  
  //Frame Carriage Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saCVal_Btn_Start_x, saCVal_Btn_Start_y, saCVal_Btn_Stop_x, saCVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saCVal_Btn_Start_x, saCVal_Btn_Start_y, saCVal_Btn_Stop_x, saCVal_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mAccelC, saCVal_Btn_Start_x + 17, saCVal_Btn_Start_y + 17);

  //Motor A Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^2", saAVal_Btn_Stop_x + 5, saAVal_Btn_Start_y + 5);

  //Motor B Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^2", saBVal_Btn_Stop_x + 5, saBVal_Btn_Start_y + 5);
  
  //Motor C Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^2", saCVal_Btn_Stop_x + 5, saCVal_Btn_Start_y + 5);

  //Decrease A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saADec_Btn_Start_x, saADec_Btn_Start_y, saADec_Btn_Stop_x, saADec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saADec_Btn_Start_x, saADec_Btn_Start_y, saADec_Btn_Stop_x, saADec_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", saADec_Btn_Start_x + 7, saADec_Btn_Start_y + 15);

  //Increase A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saAInc_Btn_Start_x, saAInc_Btn_Start_y, saAInc_Btn_Stop_x, saAInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saAInc_Btn_Start_x, saAInc_Btn_Start_y, saAInc_Btn_Stop_x, saAInc_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", saAInc_Btn_Start_x + 7, saAInc_Btn_Start_y + 15);

  //Amount Button A
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saAamt_Btn_Start_x, saAamt_Btn_Start_y, saAamt_Btn_Stop_x, saAamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saAamt_Btn_Start_x, saAamt_Btn_Start_y, saAamt_Btn_Stop_x, saAamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.01", saAamt_Btn_Start_x + 7, saAamt_Btn_Start_y + 15);

  //Decrease B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saBDec_Btn_Start_x, saBDec_Btn_Start_y, saBDec_Btn_Stop_x, saBDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saBDec_Btn_Start_x, saBDec_Btn_Start_y, saBDec_Btn_Stop_x, saBDec_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", saBDec_Btn_Start_x + 7, saBDec_Btn_Start_y + 15);

  //Increase B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saBInc_Btn_Start_x, saBInc_Btn_Start_y, saBInc_Btn_Stop_x, saBInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saBInc_Btn_Start_x, saBInc_Btn_Start_y, saBInc_Btn_Stop_x, saBInc_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", saBInc_Btn_Start_x + 7, saBInc_Btn_Start_y + 15);

  //Amount Button B
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saBamt_Btn_Start_x, saBamt_Btn_Start_y, saBamt_Btn_Stop_x, saBamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saBamt_Btn_Start_x, saBamt_Btn_Start_y, saBamt_Btn_Stop_x, saBamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.01", saBamt_Btn_Start_x + 7, saBamt_Btn_Start_y + 15);

  //Decrease C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saCDec_Btn_Start_x, saCDec_Btn_Start_y, saCDec_Btn_Stop_x, saCDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saCDec_Btn_Start_x, saCDec_Btn_Start_y, saCDec_Btn_Stop_x, saCDec_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", saCDec_Btn_Start_x + 7, saCDec_Btn_Start_y + 15);

  //Increase C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saCInc_Btn_Start_x, saCInc_Btn_Start_y, saCInc_Btn_Stop_x, saCInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saCInc_Btn_Start_x, saCInc_Btn_Start_y, saCInc_Btn_Stop_x, saCInc_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", saCInc_Btn_Start_x + 7, saCInc_Btn_Start_y + 15);

  //Amount Button C
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(saCamt_Btn_Start_x, saCamt_Btn_Start_y, saCamt_Btn_Stop_x, saCamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(saCamt_Btn_Start_x, saCamt_Btn_Start_y, saCamt_Btn_Stop_x, saCamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.01", saCamt_Btn_Start_x + 7, saCamt_Btn_Start_y + 15);

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

void GUI::updateAccelScreen() {
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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

void GUI::drawJerkScreen() {
	tft.Fill_Screen(BLACK);    //Change the screen to solid black  

  //Machine Settings Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 40);

  //Machine Settings Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Jerk Settings", 240, yPadding);

  //Motor A Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Top Clamp Motor (A): ", 240, yPadding);

  //Motor B Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Bottom Clamp Motor (B): ", 240, yPadding);
  
  //Motor C Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Frame Carriage Motors (C): ", 240, yPadding);

  //Top Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjAVal_Btn_Start_x, sjAVal_Btn_Start_y, sjAVal_Btn_Stop_x, sjAVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjAVal_Btn_Start_x, sjAVal_Btn_Start_y, sjAVal_Btn_Stop_x, sjAVal_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mJerkA, sjAVal_Btn_Start_x + 17, sjAVal_Btn_Start_y + 17);

  //Bottom Clamp Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjBVal_Btn_Start_x, sjBVal_Btn_Start_y, sjBVal_Btn_Stop_x, sjBVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjBVal_Btn_Start_x, sjBVal_Btn_Start_y, sjBVal_Btn_Stop_x, sjBVal_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mJerkB, sjBVal_Btn_Start_x + 17, sjBVal_Btn_Start_y + 17);
  
  //Frame Carriage Value Field
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjCVal_Btn_Start_x, sjCVal_Btn_Start_y, sjCVal_Btn_Stop_x, sjCVal_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjCVal_Btn_Start_x, sjCVal_Btn_Start_y, sjCVal_Btn_Stop_x, sjCVal_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String((String)mJerkC, sjCVal_Btn_Start_x + 17, sjCVal_Btn_Start_y + 17);

  //Motor A Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^3", sjAVal_Btn_Stop_x + 5, sjAVal_Btn_Start_y + 5);

  //Motor B Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^3", sjBVal_Btn_Stop_x + 5, sjBVal_Btn_Start_y + 5);
  
  //Motor C Units Text
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("mm/s^3", sjCVal_Btn_Stop_x + 5, sjCVal_Btn_Start_y + 5);

  //Decrease A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjADec_Btn_Start_x, sjADec_Btn_Start_y, sjADec_Btn_Stop_x, sjADec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjADec_Btn_Start_x, sjADec_Btn_Start_y, sjADec_Btn_Stop_x, sjADec_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", sjADec_Btn_Start_x + 7, sjADec_Btn_Start_y + 15);

  //Increase A Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjAInc_Btn_Start_x, sjAInc_Btn_Start_y, sjAInc_Btn_Stop_x, sjAInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjAInc_Btn_Start_x, sjAInc_Btn_Start_y, sjAInc_Btn_Stop_x, sjAInc_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", sjAInc_Btn_Start_x + 7, sjAInc_Btn_Start_y + 15);

  //Amount Button A
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjAamt_Btn_Start_x, sjAamt_Btn_Start_y, sjAamt_Btn_Stop_x, sjAamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjAamt_Btn_Start_x, sjAamt_Btn_Start_y, sjAamt_Btn_Stop_x, sjAamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.01", sjAamt_Btn_Start_x + 7, sjAamt_Btn_Start_y + 15);

  //Decrease B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjBDec_Btn_Start_x, sjBDec_Btn_Start_y, sjBDec_Btn_Stop_x, sjBDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjBDec_Btn_Start_x, sjBDec_Btn_Start_y, sjBDec_Btn_Stop_x, sjBDec_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", sjBDec_Btn_Start_x + 7, sjBDec_Btn_Start_y + 15);

  //Increase B Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjBInc_Btn_Start_x, sjBInc_Btn_Start_y, sjBInc_Btn_Stop_x, sjBInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjBInc_Btn_Start_x, sjBInc_Btn_Start_y, sjBInc_Btn_Stop_x, sjBInc_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", sjBInc_Btn_Start_x + 7, sjBInc_Btn_Start_y + 15);

  //Amount Button B
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjBamt_Btn_Start_x, sjBamt_Btn_Start_y, sjBamt_Btn_Stop_x, sjBamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjBamt_Btn_Start_x, sjBamt_Btn_Start_y, sjBamt_Btn_Stop_x, sjBamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.01", sjBamt_Btn_Start_x + 7, sjBamt_Btn_Start_y + 15);

  //Decrease C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjCDec_Btn_Start_x, sjCDec_Btn_Start_y, sjCDec_Btn_Stop_x, sjCDec_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjCDec_Btn_Start_x, sjCDec_Btn_Start_y, sjCDec_Btn_Stop_x, sjCDec_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("-", sjCDec_Btn_Start_x + 7, sjCDec_Btn_Start_y + 15);

  //Increase C Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjCInc_Btn_Start_x, sjCInc_Btn_Start_y, sjCInc_Btn_Stop_x, sjCInc_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjCInc_Btn_Start_x, sjCInc_Btn_Start_y, sjCInc_Btn_Stop_x, sjCInc_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("+", sjCInc_Btn_Start_x + 7, sjCInc_Btn_Start_y + 15);

  //Amount Button C
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(sjCamt_Btn_Start_x, sjCamt_Btn_Start_y, sjCamt_Btn_Stop_x, sjCamt_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(sjCamt_Btn_Start_x, sjCamt_Btn_Start_y, sjCamt_Btn_Stop_x, sjCamt_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String("0.01", sjCamt_Btn_Start_x + 7, sjCamt_Btn_Start_y + 15);

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

void GUI::updateJerkScreen() {
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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

void GUI::drawMicrostepScreen() {
	tft.Fill_Screen(BLACK);    //Change the screen to solid black

  //Microstepping Title Border
  tft.Set_Draw_color(YELLOW);
  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 40);

  //Microstepping Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Microstepping", 220, yPadding);

  //Motor A uStep Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Top Clamp", mAuS_Btn_Start_x + 5, mAuS_Btn_Start_y - 15);

  //Motor B uStep Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Bottom Clamp", mBuS_Btn_Start_x + 5, mBuS_Btn_Start_y - 15);

  //Motor C uStep Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Frame Carriage", mCuS_Btn_Start_x + 5, mCuS_Btn_Start_y - 15);

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
    mBuStep = "1/6";
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
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String(mAuStep, mAuS_Btn_Start_x + 17, mAuS_Btn_Start_y + 17);
  
  //Motor B uStep Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(mBuS_Btn_Start_x, mBuS_Btn_Start_y, mBuS_Btn_Stop_x, mBuS_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mBuS_Btn_Start_x, mBuS_Btn_Start_y, mBuS_Btn_Stop_x, mBuS_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String(mBuStep, mBuS_Btn_Start_x + 17, mBuS_Btn_Start_y + 17);

  //Motor C uStep Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(mCuS_Btn_Start_x, mCuS_Btn_Start_y, mCuS_Btn_Stop_x, mCuS_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mCuS_Btn_Start_x, mCuS_Btn_Start_y, mCuS_Btn_Stop_x, mCuS_Btn_Stop_y);
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(WHITE);
  tft.Set_Text_colour(BLACK);
  tft.Print_String(mCuStep, mCuS_Btn_Start_x + 17, mCuS_Btn_Start_y + 17);

  //Motor A Invert Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(mAinv_Btn_Start_x, mAinv_Btn_Start_y, mAinv_Btn_Stop_x, mAinv_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mAinv_Btn_Start_x, mAinv_Btn_Start_y, mAinv_Btn_Stop_x, mAinv_Btn_Stop_y);

  //Motor B Invert Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(mBinv_Btn_Start_x, mBinv_Btn_Start_y, mBinv_Btn_Stop_x, mBinv_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mBinv_Btn_Start_x, mBinv_Btn_Start_y, mBinv_Btn_Stop_x, mBinv_Btn_Stop_y);

  //Motor C Invert Button
  tft.Set_Draw_color(WHITE);
  tft.Fill_Rectangle(mCinv_Btn_Start_x, mCinv_Btn_Start_y, mCinv_Btn_Stop_x, mCinv_Btn_Stop_y);
  tft.Set_Draw_color(GRAY);
  tft.Draw_Rectangle(mCinv_Btn_Start_x, mCinv_Btn_Start_y, mCinv_Btn_Stop_x, mCinv_Btn_Stop_y);

  //Motor A Invert Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Invert", mAinv_Btn_Stop_x + 5, mAinv_Btn_Start_y  + 25);

  //Motor B Invert Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Invert", mBinv_Btn_Stop_x + 5, mAinv_Btn_Start_y + 25);

  //Motor C Invert Title
  tft.Set_Text_Size(1);
  tft.Set_Text_Back_colour(BLACK);
  tft.Set_Text_colour(YELLOW);
  tft.Print_String("Invert", mCinv_Btn_Start_x + 5, mCinv_Btn_Start_y + 25);

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

void GUI::updateMicrostepScreen() {
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

void GUI::drawStepsMMScreen() {
	tft.Fill_Screen(BLACK);    //Change the screen to solid black  

	//Machine Settings Title Border
	  tft.Set_Draw_color(YELLOW);
	  tft.Draw_Rectangle(xPadding, yPadding, 480-xPadding, yPadding + 40);

	  //Machine Settings Title
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(BLACK);
	  tft.Set_Text_colour(YELLOW);
	  tft.Print_String("Steps/mm Settings", 240, yPadding);

	  //Motor A Title
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(BLACK);
	  tft.Set_Text_colour(YELLOW);
	  tft.Print_String("Top Clamp Motor (A): ", 240, yPadding);

	  //Motor B Title
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(BLACK);
	  tft.Set_Text_colour(YELLOW);
	  tft.Print_String("Bottom Clamp Motor (B): ", 240, yPadding);
	  
	  //Motor C Title
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(BLACK);
	  tft.Set_Text_colour(YELLOW);
	  tft.Print_String("Frame Carriage Motors (C): ", 240, yPadding);

	  //Top Clamp Value Field
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmAVal_Btn_Start_x, ssmmAVal_Btn_Start_y, ssmmAVal_Btn_Stop_x, ssmmAVal_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmAVal_Btn_Start_x, ssmmAVal_Btn_Start_y, ssmmAVal_Btn_Stop_x, ssmmAVal_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String((String)mStepsPerA, ssmmAVal_Btn_Start_x + 17, ssmmAVal_Btn_Start_y + 17);

	  //Bottom Clamp Value Field
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmBVal_Btn_Start_x, ssmmBVal_Btn_Start_y, ssmmBVal_Btn_Stop_x, ssmmBVal_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmBVal_Btn_Start_x, ssmmBVal_Btn_Start_y, ssmmBVal_Btn_Stop_x, ssmmBVal_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String((String)mStepsPerB, ssmmBVal_Btn_Start_x + 17, ssmmBVal_Btn_Start_y + 17);
	  
	  //Frame Carriage Value Field
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmCVal_Btn_Start_x, ssmmCVal_Btn_Start_y, ssmmCVal_Btn_Stop_x, ssmmCVal_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmCVal_Btn_Start_x, ssmmCVal_Btn_Start_y, ssmmCVal_Btn_Stop_x, ssmmCVal_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String((String)mStepsPerC, ssmmCVal_Btn_Start_x + 17, ssmmCVal_Btn_Start_y + 17);

	  //Motor A Units Text
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(BLACK);
	  tft.Set_Text_colour(YELLOW);
	  tft.Print_String("Steps/mm", ssmmCVal_Btn_Stop_x + 5, ssmmCVal_Btn_Start_y + 5);

	  //Motor B Units Text
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(BLACK);
	  tft.Set_Text_colour(YELLOW);
	  tft.Print_String("Steps/mm", ssmmCVal_Btn_Stop_x + 5, ssmmCVal_Btn_Start_y + 5);
	  
	  //Motor C Units Text
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(BLACK);
	  tft.Set_Text_colour(YELLOW);
	  tft.Print_String("Steps/mm", ssmmCVal_Btn_Stop_x + 5, ssmmCVal_Btn_Start_y + 5);

	  //Decrease A Button
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmADec_Btn_Start_x, ssmmADec_Btn_Start_y, ssmmADec_Btn_Stop_x, ssmmADec_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmADec_Btn_Start_x, ssmmADec_Btn_Start_y, ssmmADec_Btn_Stop_x, ssmmADec_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String("-", ssmmADec_Btn_Start_x + 7, ssmmADec_Btn_Start_y + 15);

	  //Increase A Button
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmAInc_Btn_Start_x, ssmmAInc_Btn_Start_y, ssmmAInc_Btn_Stop_x, ssmmAInc_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmAInc_Btn_Start_x, ssmmAInc_Btn_Start_y, ssmmAInc_Btn_Stop_x, ssmmAInc_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String("+", ssmmAInc_Btn_Start_x + 7, ssmmAInc_Btn_Start_y + 15);

	  //Amount Button A
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmAamt_Btn_Start_x, ssmmAamt_Btn_Start_y, ssmmAamt_Btn_Stop_x, ssmmAamt_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmAamt_Btn_Start_x, ssmmAamt_Btn_Start_y, ssmmAamt_Btn_Stop_x, ssmmAamt_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String("0.01", ssmmAamt_Btn_Start_x + 7, ssmmAamt_Btn_Start_y + 15);

	  //Decrease B Button
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmBDec_Btn_Start_x, ssmmBDec_Btn_Start_y, ssmmBDec_Btn_Stop_x, ssmmBDec_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmBDec_Btn_Start_x, ssmmBDec_Btn_Start_y, ssmmBDec_Btn_Stop_x, ssmmBDec_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String("-", ssmmBDec_Btn_Start_x + 7, ssmmBDec_Btn_Start_y + 15);

	  //Increase B Button
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmBInc_Btn_Start_x, ssmmBInc_Btn_Start_y, ssmmBInc_Btn_Stop_x, ssmmBInc_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmBInc_Btn_Start_x, ssmmBInc_Btn_Start_y, ssmmBInc_Btn_Stop_x, ssmmBInc_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String("+", ssmmBInc_Btn_Start_x + 7, ssmmBInc_Btn_Start_y + 15);

	  //Amount Button B
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmBamt_Btn_Start_x, ssmmBamt_Btn_Start_y, ssmmBamt_Btn_Stop_x, ssmmBamt_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmBamt_Btn_Start_x, ssmmBamt_Btn_Start_y, ssmmBamt_Btn_Stop_x, ssmmBamt_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String("0.01", ssmmBamt_Btn_Start_x + 7, ssmmBamt_Btn_Start_y + 15);

	  //Decrease C Button
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmCDec_Btn_Start_x, ssmmCDec_Btn_Start_y, ssmmCDec_Btn_Stop_x, ssmmCDec_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmCDec_Btn_Start_x, ssmmCDec_Btn_Start_y, ssmmCDec_Btn_Stop_x, ssmmCDec_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String("-", ssmmCDec_Btn_Start_x + 7, ssmmCDec_Btn_Start_y + 15);

	  //Increase C Button
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmCInc_Btn_Start_x, ssmmCInc_Btn_Start_y, ssmmCInc_Btn_Stop_x, ssmmCInc_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmCInc_Btn_Start_x, ssmmCInc_Btn_Start_y, ssmmCInc_Btn_Stop_x, ssmmCInc_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String("+", ssmmCInc_Btn_Start_x + 7, ssmmCInc_Btn_Start_y + 15);

	  //Amount Button C
	  tft.Set_Draw_color(WHITE);
	  tft.Fill_Rectangle(ssmmCamt_Btn_Start_x, ssmmCamt_Btn_Start_y, ssmmCamt_Btn_Stop_x, ssmmCamt_Btn_Stop_y);
	  tft.Set_Draw_color(GRAY);
	  tft.Draw_Rectangle(ssmmCamt_Btn_Start_x, ssmmCamt_Btn_Start_y, ssmmCamt_Btn_Stop_x, ssmmCamt_Btn_Stop_y);
	  tft.Set_Text_Size(1);
	  tft.Set_Text_Back_colour(WHITE);
	  tft.Set_Text_colour(BLACK);
	  tft.Print_String("0.01", ssmmCamt_Btn_Start_x + 7, ssmmCamt_Btn_Start_y + 15);

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

void GUI::updateStepsMMScreen() {
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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
    delay(50);
    //TODO:
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

