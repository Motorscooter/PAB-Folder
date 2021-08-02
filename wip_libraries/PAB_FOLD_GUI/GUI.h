#ifndef GUI_H
#define GUI_H

#include "Arduino.h"
#include <LCDWIKI_KBV.h>
#include <TouchScreen.h>

class GUI {
	public:
		GUI(int _mStepsA, int _mStepsB, int _mStepsC, float _mStepsPerA, float _mStepsPerB, float _mStepsPerC);
		
		void drawMainScreen();
		void updateMainScreen();
		
		void drawInputScreen();
		void updateInputScreen();
		
		void drawProfileLoadScreen();
		void updateProfileLoadScreen();
		
		void drawNewProfileScreen();
		void updateNewProfileScreen();
		
		void drawEditProfileScreen();
		void updateEditProfileScreen();
		
		void drawMachineSettingsScreen();
		void updateMachineSettingsScreen();
		
		void drawVelScreen();
		void updateVelScreen();
		
		void drawAccelAcreen();
		void updateAccelScreen();
		
		void drawJerkScreen();
		void updateJerkScreen();
		
		void drawMicrostepScreen();
		void updateMicrostepScreen();
		
		void drawStepsMMScreen();
		void updateStepsMMScreen();
		
		//Touch Screen Connections
		int YP;  // must be an analog pin, use "An" notation!
		int XM;  // must be an analog pin, use "An" notation!
		int YM;   // can be a digital pin
		int XP;   // can be a digital pin
			
		//Touch parameter calibration
		int TS_LEFT;
		int TS_RIGHT;
		int TS_TOP;
		int TS_BOT;
		
		int activeScreen; //0 = home screen
		float inputVar;
		
		int mStepsA = _mStepsA;
		int mStepsB = _mStepsB;
		int mStepsC = _mStepsC;
		int mStepsPerA = _mStepsPerA;
		int mStepsPerB = _mStepsPerB;
		int mStepsPerC = _mStepsPerC;

	private:	
		int prevActiveScreen;
		
	//#region Main Screen
		//Button States
		boolean loadBtnFlag;
		boolean newBtnFlag;
		boolean editBtnFlag;
		boolean setBtnFlag;
		boolean tClampBtnFlag;
		boolean bClampBtnFlag;
		boolean homeBtnFlag;
		boolean prevBtnFlag;
		boolean nextBtnFlag;
		boolean togMotBtnFlag;
		boolean upTBtnFlag;
		boolean downTBtnFlag;
		boolean upBBtnFlag;
		boolean downBBtnFlag;
		boolean inBtnFlag;
		boolean outBtnFlag;
		boolean amountBtnFlag;
		
		//Button Dimensions
		int xPadding;
		int yPadding;

		int configBtnX;
		int configBtnY;

		int settingBtnX;
		int settingBtnY;

		int clampBtnX;
		int clampBtnY;

		int homeBtnX;
		int homeBtnY;

		int mStepBtnX;
		int mStepBtnY;

		int mEnBtnX;
		int mEnBtnY;

		int manualBtnX;
		int manualBtnY;
		
		//Button Variable
		float moveAmount;
		
		//Button Positioning
		int load_Btn_Start_x;
		int load_Btn_Start_y;
		int load_Btn_Stop_x;
		int load_Btn_Stop_y;

		int new_Btn_Start_x;
		int new_Btn_Start_y;
		int new_Btn_Stop_x;
		int new_Btn_Stop_y;

		int edit_Btn_Start_x;
		int edit_Btn_Start_y;
		int edit_Btn_Stop_x;
		int edit_Btn_Stop_y;

		int set_Btn_Start_x;
		int set_Btn_Start_y;
		int set_Btn_Stop_x;
		int set_Btn_Stop_y;

		int top_clamp_Btn_Start_x;
		int top_clamp_Btn_Start_y;
		int top_clamp_Btn_Stop_x;
		int top_clamp_Btn_Stop_y;

		int bot_clamp_Btn_Start_x;
		int bot_clamp_Btn_Start_y;
		int bot_clamp_Btn_Stop_x;
		int bot_clamp_Btn_Stop_y;

		int home_Btn_Start_x;
		int home_Btn_Start_y;
		int home_Btn_Stop_x;
		int home_Btn_Stop_y;

		int prev_Btn_Start_x;
		int prev_Btn_Start_y;
		int prev_Btn_Stop_x;
		int prev_Btn_Stop_y;

		int next_Btn_Start_x;
		int next_Btn_Start_y;
		int next_Btn_Stop_x;
		int next_Btn_Stop_y;

		int mEN_Btn_Start_x;
		int mEN_Btn_Start_y;
		int mEN_Btn_Stop_x;
		int mEN_Btn_Stop_y;

		int upT_Btn_Start_x;
		int upT_Btn_Start_y;
		int upT_Btn_Stop_x;
		int upT_Btn_Stop_y;

		int downT_Btn_Start_x;
		int downT_Btn_Start_y;
		int downT_Btn_Stop_x;
		int downT_Btn_Stop_y;

		int upB_Btn_Start_x;
		int upB_Btn_Start_y;
		int upB_Btn_Stop_x;
		int upB_Btn_Stop_y;

		int downB_Btn_Start_x;
		int downB_Btn_Start_y;
		int downB_Btn_Stop_x;
		int downB_Btn_Stop_y;

		int in_Btn_Start_x;
		int in_Btn_Start_y;
		int in_Btn_Stop_x;
		int in_Btn_Stop_y;

		int out_Btn_Start_x;
		int out_Btn_Start_y;
		int out_Btn_Stop_x;
		int out_Btn_Stop_y;

		int amount_Btn_Start_x;
		int amount_Btn_Start_y;
		int amount_Btn_Stop_x;
		int amount_Btn_Stop_y;
	//#endregion

	//#region InputScreen
		//Button States
		boolean delBtnFlag;
		boolean lettBtnFlag;
		boolean n1BtnFlag;
		boolean n2BtnFlag;
		boolean n3BtnFlag;
		boolean n4BtnFlag;
		boolean n5BtnFlag;
		boolean n6BtnFlag;
		boolean n7BtnFlag;
		boolean n8BtnFlag;
		boolean n9BtnFlag;
		boolean backBtnFlag;
		boolean decimalBtnFlag;
		boolean okBtnFlag;
		
		//Button Dimensions
		int numBtnX;
		int numBtnY;

		//Button Positioning
		int numDel_Btn_Start_x;
		int numDel_Btn_Start_y;
		int numDel_Btn_Stop_x;
		int numDel_Btn_Stop_y;

		int numLett_Btn_Start_x;
		int numLett_Btn_Start_y;
		int numLett_Btn_Stop_x;
		int numLett_Btn_Stop_y;

		int num1_Btn_Start_x;
		int num1_Btn_Start_y;
		int num1_Btn_Stop_x;
		int num1_Btn_Stop_y;

		int num2_Btn_Start_x;
		int num2_Btn_Start_y;
		int num2_Btn_Stop_x;
		int num2_Btn_Stop_y;

		int num3_Btn_Start_x;
		int num3_Btn_Start_y;
		int num3_Btn_Stop_x;
		int num3_Btn_Stop_y;

		int num4_Btn_Start_x;
		int num4_Btn_Start_y;
		int num4_Btn_Stop_x;
		int num4_Btn_Stop_y;

		int num5_Btn_Start_x;
		int num5_Btn_Start_y;
		int num5_Btn_Stop_x;
		int num5_Btn_Stop_y;

		int num6_Btn_Start_x;
		int num6_Btn_Start_y;
		int num6_Btn_Stop_x;
		int num6_Btn_Stop_y;

		int num7_Btn_Start_x;
		int num7_Btn_Start_y;
		int num7_Btn_Stop_x;
		int num7_Btn_Stop_y;

		int num8_Btn_Start_x;
		int num8_Btn_Start_y;
		int num8_Btn_Stop_x;
		int num8_Btn_Stop_y;

		int num9_Btn_Start_x;
		int num9_Btn_Start_y;
		int num9_Btn_Stop_x;
		int num9_Btn_Stop_y;
		
		int numBck_Btn_Start_x;
		int numBck_Btn_Start_y;
		int numBck_Btn_Stop_x;
		int numBck_Btn_Stop_y;

		int numPd_Btn_Start_x;
		int numPd_Btn_Start_y;
		int numPd_Btn_Stop_x;
		int numPd_Btn_Stop_y;

		int numOk_Btn_Start_x;
		int numOk_Btn_Start_y;
		int numOk_Btn_Stop_x;
		int numOk_Btn_Stop_y;
	//#endregion

	//#region Load Profile Screen
		//Button States	
		boolean pBackBtnFlag;
		boolean pCfmBtnFlag;
		boolean p1BtnFlag;
		boolean p2BtnFlag;
		boolean p3BtnFlag;
		boolean p4BtnFlag;
		boolean p5BtnFlag;
		boolean p6BtnFlag;
		boolean p7BtnFlag;
		boolean p8BtnFlag;
		boolean p9BtnFlag;
		boolean p10BtnFlag;
		
		//Button Dimensions
		int backBtnX;
		int backBtnY;

		int confirmBtnX;
		int confirmBtnY;

		int profileBtnX;
		int profileBtnY;
		
		//Button Positioning
		int pBack_Btn_Start_x;
		int pBack_Btn_Start_y;
		int pBack_Btn_Stop_x;
		int pBack_Btn_Stop_y;

		int pCfm_Btn_Start_x;
		int pCfm_Btn_Start_y;
		int pCfm_Btn_Stop_x;
		int pCfm_Btn_Stop_y;

		int p1_Btn_Start_x;
		int p1_Btn_Start_y;
		int p1_Btn_Stop_x;
		int p1_Btn_Stop_y;

		int p2_Btn_Start_x;
		int p2_Btn_Start_y;
		int p2_Btn_Stop_x;
		int p2_Btn_Stop_y;

		int p3_Btn_Start_x;
		int p3_Btn_Start_y;
		int p3_Btn_Stop_x;
		int p3_Btn_Stop_y;

		int p4_Btn_Start_x;
		int p4_Btn_Start_y;
		int p4_Btn_Stop_x;
		int p4_Btn_Stop_y;

		int p5_Btn_Start_x;
		int p5_Btn_Start_y;
		int p5_Btn_Stop_x;
		int p5_Btn_Stop_y;

		int p6_Btn_Start_x;
		int p6_Btn_Start_y;
		int p6_Btn_Stop_x;
		int p6_Btn_Stop_y;

		int p7_Btn_Start_x;
		int p7_Btn_Start_y;
		int p7_Btn_Stop_x;
		int p7_Btn_Stop_y;

		int p8_Btn_Start_x;
		int p8_Btn_Start_y;
		int p8_Btn_Stop_x;
		int p8_Btn_Stop_y;

		int p9_Btn_Start_x;
		int p9_Btn_Start_y;
		int p9_Btn_Stop_x;
		int p9_Btn_Stop_y;
		
		int p10_Btn_Start_x;
		int p10_Btn_Start_y;
		int p10_Btn_Stop_x;
		int p10_Btn_Stop_y;
	//#endregion

	//#region New Profile Screen
		//Button States
		boolean pnNameBtnFlag;
		boolean pnTClampBtnFlag;
		boolean pnBClampBtnFlag;
		boolean pnFChBtnFlag;
		boolean pnStp1BtnFlag;
		boolean pnStp2BtnFlag;
		boolean pnStp3BtnFlag;
		boolean pnStp4BtnFlag;
		boolean pnStp5BtnFlag;
		boolean pnBckBtnFlag;
		boolean pnSaveBtnFlag;
		
		//Button Dimensions
		int fieldBtnX;
		int fieldBtnY;

		int stepBtnX;
		int stepBtnY;

		int saveBtnX;
		int saveBtnY;
		
		//Button Positioning
		int pnName_Btn_Start_x;
		int pnName_Btn_Start_y;
		int pnName_Btn_Stop_x;
		int pnName_Btn_Stop_y;

		int pnTClamp_Btn_Start_x;
		int pnTClamp_Btn_Start_y;
		int pnTClamp_Btn_Stop_x;
		int pnTClamp_Btn_Stop_y;

		int pnBClamp_Btn_Start_x;
		int pnBClamp_Btn_Start_y;
		int pnBClamp_Btn_Stop_x;
		int pnBClamp_Btn_Stop_y;

		int pnFCh_Btn_Start_x;
		int pnFCh_Btn_Start_y;
		int pnFCh_Btn_Stop_x;
		int pnFCh_Btn_Stop_y;

		int pnStp1_Btn_Start_x;
		int pnStp1_Btn_Start_y;
		int pnStp1_Btn_Stop_x;
		int pnStp1_Btn_Stop_y;
		
		int pnStp2_Btn_Start_x;
		int pnStp2_Btn_Start_y;
		int pnStp2_Btn_Stop_x;
		int pnStp2_Btn_Stop_y;

		int pnStp3_Btn_Start_x;
		int pnStp3_Btn_Start_y;
		int pnStp3_Btn_Stop_x;
		int pnStp3_Btn_Stop_y;

		int pnStp4_Btn_Start_x;
		int pnStp4_Btn_Start_y;
		int pnStp4_Btn_Stop_x;
		int pnStp4_Btn_Stop_y;

		int pnStp5_Btn_Start_x;
		int pnStp5_Btn_Start_y;
		int pnStp5_Btn_Stop_x;
		int pnStp5_Btn_Stop_y;

		int pnBck_Btn_Start_x;
		int pnBck_Btn_Start_y;
		int pnBck_Btn_Stop_x;
		int pnBck_Btn_Stop_y;

		int pnSave_Btn_Start_x;
		int pnSave_Btn_Start_y;
		int pnSave_Btn_Stop_x;
		int pnSave_Btn_Stop_y;
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
		boolean sVelBtnFlag;
		boolean sAccelBtnFlag;
		boolean sJerkBtnFlag;
		boolean sUSBtnFlag;
		boolean sSmmBtnFlag;
		boolean sRstBtnFlag;
		
		//Button Dimensions
		int setgBtnX;
		int setgBtnY;

		//Button Positioning
		int sVel_Btn_Start_x;
		int sVel_Btn_Start_y;
		int sVel_Btn_Stop_x;
		int sVel_Btn_Stop_y;

		int sAccel_Btn_Start_x;
		int sAccel_Btn_Start_y;
		int sAccel_Btn_Stop_x;
		int sAccel_Btn_Stop_y;

		int sJerk_Btn_Start_x;
		int sJerk_Btn_Start_y;
		int sJerk_Btn_Stop_x;
		int sJerk_Btn_Stop_y;

		int sUS_Btn_Start_x;
		int sUS_Btn_Start_y;
		int sUS_Btn_Stop_x;
		int sUS_Btn_Stop_y;

		int sSmm_Btn_Start_x;
		int sSmm_Btn_Start_y;
		int sSmm_Btn_Stop_x;
		int sSmm_Btn_Stop_y;

		int sRst_Btn_Start_x;
		int sRst_Btn_Start_y;
		int sRst_Btn_Stop_x;
		int sRst_Btn_Stop_y;
	//#endregion

	//#region Velocity Settings Screen
		//Button States
		boolean svAValBtnFlag;
		boolean svBValBtnFlag;
		boolean svCValBtnFlag;
		boolean svADecBtnFlag;
		boolean svBDecBtnFlag;
		boolean svCDecBtnFlag;
		boolean svAIncBtnFlag;
		boolean svBIncBtnFlag;
		boolean svCIncBtnFlag;
		boolean svAamtBtnFlag;
		boolean svBamtBtnFlag;
		boolean svCamtBtnFlag;
		
		//Button Dimensions
		int field2BtnX;
		int field2BtnY;

		int chgValBtnX;
		int chgValBtnY;

		//Button Values
		float aAmt;
		float bAmt;
		float cAmt;

		//Button Positioning
		int svAVal_Btn_Start_x;
		int svAVal_Btn_Start_y;
		int svAVal_Btn_Stop_x;
		int svAVal_Btn_Stop_y;

		int svBVal_Btn_Start_x;
		int svBVal_Btn_Start_y;
		int svBVal_Btn_Stop_x;
		int svBVal_Btn_Stop_y;

		int svCVal_Btn_Start_x;
		int svCVal_Btn_Start_y;
		int svCVal_Btn_Stop_x;
		int svCVal_Btn_Stop_y;

		int svADec_Btn_Start_x;
		int svADec_Btn_Start_y;
		int svADec_Btn_Stop_x;
		int svADec_Btn_Stop_y;

		int svBDec_Btn_Start_x;
		int svBDec_Btn_Start_y;
		int svBDec_Btn_Stop_x;
		int svBDec_Btn_Stop_y;

		int svCDec_Btn_Start_x;
		int svCDec_Btn_Start_y;
		int svCDec_Btn_Stop_x;
		int svCDec_Btn_Stop_y;

		int svAInc_Btn_Start_x;
		int svAInc_Btn_Start_y;
		int svAInc_Btn_Stop_x;
		int svAInc_Btn_Stop_y;

		int svBInc_Btn_Start_x;
		int svBInc_Btn_Start_y;
		int svBInc_Btn_Stop_x;
		int svBInc_Btn_Stop_y;

		int svCInc_Btn_Start_x;
		int svCInc_Btn_Start_y;
		int svCInc_Btn_Stop_x;
		int svCInc_Btn_Stop_y;

		int svAamt_Btn_Start_x;
		int svAamt_Btn_Start_y;
		int svAamt_Btn_Stop_x;
		int svAamt_Btn_Stop_y;

		int svBamt_Btn_Start_x;
		int svBamt_Btn_Start_y;
		int svBamt_Btn_Stop_x;
		int svBamt_Btn_Stop_y;

		int svCamt_Btn_Start_x;
		int svCamt_Btn_Start_y;
		int svCamt_Btn_Stop_x;
		int svCamt_Btn_Stop_y;
	//#endregion
	
	//#region Acceleration Settings Screen
		//Button States
		boolean saAValBtnFlag;
		boolean saBValBtnFlag;
		boolean saCValBtnFlag;
		boolean saADecBtnFlag;
		boolean saBDecBtnFlag;
		boolean saCDecBtnFlag;
		boolean saAIncBtnFlag;
		boolean saBIncBtnFlag;
		boolean saCIncBtnFlag;
		boolean saAamtBtnFlag;
		boolean saBamtBtnFlag;
		boolean saCamtBtnFlag;
		
		//Button Dimensions
		//Same as 'Velocity Settings Screen'

		//Button Positioning
		int saAVal_Btn_Start_x;
		int saAVal_Btn_Start_y;
		int saAVal_Btn_Stop_x;
		int saAVal_Btn_Stop_y;

		int saBVal_Btn_Start_x;
		int saBVal_Btn_Start_y;
		int saBVal_Btn_Stop_x;
		int saBVal_Btn_Stop_y;

		int saCVal_Btn_Start_x;
		int saCVal_Btn_Start_y;
		int saCVal_Btn_Stop_x;
		int saCVal_Btn_Stop_y;

		int saADec_Btn_Start_x;
		int saADec_Btn_Start_y;
		int saADec_Btn_Stop_x;
		int saADec_Btn_Stop_y;

		int saBDec_Btn_Start_x;
		int saBDec_Btn_Start_y;
		int saBDec_Btn_Stop_x;
		int saBDec_Btn_Stop_y;

		int saCDec_Btn_Start_x;
		int saCDec_Btn_Start_y;
		int saCDec_Btn_Stop_x;
		int saCDec_Btn_Stop_y;

		int saAInc_Btn_Start_x;
		int saAInc_Btn_Start_y;
		int saAInc_Btn_Stop_x;
		int saAInc_Btn_Stop_y;

		int saBInc_Btn_Start_x;
		int saBInc_Btn_Start_y;
		int saBInc_Btn_Stop_x;
		int saBInc_Btn_Stop_y;

		int saCInc_Btn_Start_x;
		int saCInc_Btn_Start_y;
		int saCInc_Btn_Stop_x;
		int saCInc_Btn_Stop_y;
		
		int saAamt_Btn_Start_x;
		int saAamt_Btn_Start_y;
		int saAamt_Btn_Stop_x;
		int saAamt_Btn_Stop_y;

		int saBamt_Btn_Start_x;
		int saBamt_Btn_Start_y;
		int saBamt_Btn_Stop_x;
		int saBamt_Btn_Stop_y;

		int saCamt_Btn_Start_x;
		int saCamt_Btn_Start_y;
		int saCamt_Btn_Stop_x;
		int saCamt_Btn_Stop_y;
	//#endregion
	
	//#region Jerk Settings Screen
		//Button States
		boolean sjAValBtnFlag;
		boolean sjBValBtnFlag;
		boolean sjCValBtnFlag;
		boolean sjADecBtnFlag;
		boolean sjBDecBtnFlag;
		boolean sjCDecBtnFlag;
		boolean sjAIncBtnFlag;
		boolean sjBIncBtnFlag;
		boolean sjCIncBtnFlag;
		boolean sjAamtBtnFlag;
		boolean sjBamtBtnFlag;
		boolean sjCamtBtnFlag;
		
		//Button Dimensions
		//Same as 'Velocity Settings Screen'

		//Button Positioning
		int sjAVal_Btn_Start_x;
		int sjAVal_Btn_Start_y;
		int sjAVal_Btn_Stop_x;
		int sjAVal_Btn_Stop_y;

		int sjBVal_Btn_Start_x;
		int sjBVal_Btn_Start_y;
		int sjBVal_Btn_Stop_x;
		int sjBVal_Btn_Stop_y;

		int sjCVal_Btn_Start_x;
		int sjCVal_Btn_Start_y;
		int sjCVal_Btn_Stop_x;
		int sjCVal_Btn_Stop_y;

		int sjADec_Btn_Start_x;
		int sjADec_Btn_Start_y;
		int sjADec_Btn_Stop_x;
		int sjADec_Btn_Stop_y;

		int sjBDec_Btn_Start_x;
		int sjBDec_Btn_Start_y;
		int sjBDec_Btn_Stop_x;
		int sjBDec_Btn_Stop_y;

		int sjCDec_Btn_Start_x;
		int sjCDec_Btn_Start_y;
		int sjCDec_Btn_Stop_x;
		int sjCDec_Btn_Stop_y;

		int sjAInc_Btn_Start_x;
		int sjAInc_Btn_Start_y;
		int sjAInc_Btn_Stop_x;
		int sjAInc_Btn_Stop_y;

		int sjBInc_Btn_Start_x;
		int sjBInc_Btn_Start_y;
		int sjBInc_Btn_Stop_x;
		int sjBInc_Btn_Stop_y;

		int sjCInc_Btn_Start_x;
		int sjCInc_Btn_Start_y;
		int sjCInc_Btn_Stop_x;
		int sjCInc_Btn_Stop_y;

		int sjAamt_Btn_Start_x;
		int sjAamt_Btn_Start_y;
		int sjAamt_Btn_Stop_x;
		int sjAamt_Btn_Stop_y;

		int sjBamt_Btn_Start_x;
		int sjBamt_Btn_Start_y;
		int sjBamt_Btn_Stop_x;
		int sjBamt_Btn_Stop_y;

		int sjCamt_Btn_Start_x;
		int sjCamt_Btn_Start_y;
		int sjCamt_Btn_Stop_x;
		int sjCamt_Btn_Stop_y;
	//#endregion
	
	//#region Microstep Settings Screen
		//Button States
		boolean mAuSBtnFlag;
		boolean mBuSBtnFlag;
		boolean mCuSBtnFlag;
		boolean mAinvBtnFlag;
		boolean mBinvBtnFlag;
		boolean mCinvBtnFlag;
		
		//Button Dimensions
		int uSBtnX;
		int uSBtnY;

		int invBtnX;
		int invBtnY;

		//Button Positioning
		int mAuS_Btn_Start_x;
		int mAuS_Btn_Start_y;
		int mAuS_Btn_Stop_x;
		int mAuS_Btn_Stop_y;

		int mBuS_Btn_Start_x;
		int mBuS_Btn_Start_y;
		int mBuS_Btn_Stop_x;
		int mBuS_Btn_Stop_y;

		int mCuS_Btn_Start_x;
		int mCuS_Btn_Start_y;
		int mCuS_Btn_Stop_x;
		int mCuS_Btn_Stop_y;

		int mAinv_Btn_Start_x;
		int mAinv_Btn_Start_y;
		int mAinv_Btn_Stop_x;
		int mAinv_Btn_Stop_y;

		int mBinv_Btn_Start_x;
		int mBinv_Btn_Start_y;
		int mBinv_Btn_Stop_x;
		int mBinv_Btn_Stop_y;

		int mCinv_Btn_Start_x;
		int mCinv_Btn_Start_y;
		int mCinv_Btn_Stop_x;
		int mCinv_Btn_Stop_y;
	//#endregion
	
	//#region Steps/mm Settings Screen
		//Button States
		boolean ssmmAValBtnFlag;
		boolean ssmmBValBtnFlag;
		boolean ssmmCValBtnFlag;
		boolean ssmmADecBtnFlag;
		boolean ssmmBDecBtnFlag;
		boolean ssmmCDecBtnFlag;
		boolean ssmmAIncBtnFlag;
		boolean ssmmBIncBtnFlag;
		boolean ssmmCIncBtnFlag;
		boolean ssmmAamtBtnFlag;
		boolean ssmmBamtBtnFlag;
		boolean ssmmCamtBtnFlag;

		
		//Button Dimensions
		//Same as 'Velocity Settings Screen'

		//Button Positioning
		int ssmmAVal_Btn_Start_x;
		int ssmmAVal_Btn_Start_y;
		int ssmmAVal_Btn_Stop_x;
		int ssmmAVal_Btn_Stop_y;
		
		int ssmmBVal_Btn_Start_x;
		int ssmmBVal_Btn_Start_y;
		int ssmmBVal_Btn_Stop_x;
		int ssmmBVal_Btn_Stop_y;

		int ssmmCVal_Btn_Start_x;
		int ssmmCVal_Btn_Start_y;
		int ssmmCVal_Btn_Stop_x;
		int ssmmCVal_Btn_Stop_y;
		
		int ssmmADec_Btn_Start_x;
		int ssmmADec_Btn_Start_y;
		int ssmmADec_Btn_Stop_x;
		int ssmmADec_Btn_Stop_y;

		int ssmmBDec_Btn_Start_x;
		int ssmmBDec_Btn_Start_y;
		int ssmmBDec_Btn_Stop_x;
		int ssmmBDec_Btn_Stop_y;

		int ssmmCDec_Btn_Start_x;
		int ssmmCDec_Btn_Start_y;
		int ssmmCDec_Btn_Stop_x;
		int ssmmCDec_Btn_Stop_y;

		int ssmmAInc_Btn_Start_x;
		int ssmmAInc_Btn_Start_y;
		int ssmmAInc_Btn_Stop_x;
		int ssmmAInc_Btn_Stop_y;

		int ssmmBInc_Btn_Start_x;
		int ssmmBInc_Btn_Start_y;
		int ssmmBInc_Btn_Stop_x;
		int ssmmBInc_Btn_Stop_y;
		
		int ssmmCInc_Btn_Start_x;
		int ssmmCInc_Btn_Start_y;
		int ssmmCInc_Btn_Stop_x;
		int ssmmCInc_Btn_Stop_y;
		
		int ssmmAamt_Btn_Start_x;
		int ssmmAamt_Btn_Start_y;
		int ssmmAamt_Btn_Stop_x;
		int ssmmAamt_Btn_Stop_y;

		int ssmmBamt_Btn_Start_x;
		int ssmmBamt_Btn_Start_y;
		int ssmmBamt_Btn_Stop_x;
		int ssmmBamt_Btn_Stop_y;

		int ssmmCamt_Btn_Start_x;
		int ssmmCamt_Btn_Start_y;
		int ssmmCamt_Btn_Stop_x;
		int ssmmCamt_Btn_Stop_y;
	//#endregion

};

#endif