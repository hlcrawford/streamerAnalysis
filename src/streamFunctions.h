#ifndef __STREAMFUNCTIONS_H
#define __STREAMFUNCTIONS_H

#include "Riostream.h"
#include "Rtypes.h"

#include "TObject.h"
#include "TString.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <limits>
#include "math.h"

#include "colors.h"

#define READSIZE 1000*1000*10 //0.1 seconds at a time...

class streamer : public TObject {

 public:
  FILE *fin;
  
  short int *wf; // raw trace buffer
  int *ledBuf, *trapBuf;
  double *pzBuf, *subBL1, *subBL2, *BL1, *BL2, *pzBLBuf;
  double *tempBuf;
  std::vector<long int> ledOUT;
  
  int bytesRead;

  double tau;
  int DV;
  int LEDThreshold;
  int BLRinhibitLength, BLRretrigger;

  int invertWF;

  int EK, EM, LEDK;
  int useBLR; int usePO;
  int POtime;
  
 public:
  streamer() { ; }
  ~streamer() {;}
  int Initialize(TString inputFileName, TString setFileName);
  int Reset(int overlap);
  int getSettings(TString setFile);
  void reportSettings();
  int calculateLEDlevel(int index, int thresh);
  void doLEDfilter(int startIndex, int endIndex, int first);
  int getLEDcrossings(int startIndex, int endIndex, int first);
  double baseline(int index);
  void doTrapezoid(int startIndex, int endIndex, int startTS, int first);
  double doPolezeroBasic(int startIndex, int endIndex, double sum, int first);
  double twoPolePolezero(int startIndex, int endIndex, double sum, int first);
  double doLocalPZandEnergy(int startIndex, int endIndex, int LEDIndex, double tau);
  int doBaselineRestorationCC(int startIndex, int endIndex, int startTS, int first, int countdown);
  void doBaselineRestorationM2(int startIndex, int endIndex, int startTS, int first);
  std::vector<double> doEnergyPeakFind(double *in, int startIndex, int endIndex, int startTS, int *pileUp);
  std::vector<double> doPeakSensing(int startIndex, int endIndex, int startTS, int *pileUp);
  std::vector<double> doPeakIntegrate(int startIndex, int endIndex, int startTS, int *pileUp);
  std::vector<double> doEnergyFixedPickOff(double *in, int startIndex, int endIndex, int startTS, int *pileUp);

  void setTau(double itau) { tau = itau; }
  void setLEDThresh(double iLEDThreshold) { LEDThreshold = iLEDThreshold; }
  void setIntTime(int iEM) { EM = iEM; }
  void setGapTime(int iEK) { EK = iEK; }
  void setLEDGapTime(int iEK) { LEDK = iEK; }
  void setBLRValue(int iDV) { DV = iDV; }
  void setBLRInhibit(int iBLRi) { BLRinhibitLength = iBLRi; }
  void setBLRTrigger(int iBLRT) { BLRretrigger = iBLRT; }
  void setBLR(int iBLR) { useBLR = iBLR; }
  void setEnergyPO(int iPO) { usePO = iPO; }
  void setPOTime(int iPOT) { POtime = iPOT; }

  double getTau() { return tau; }
  int getDV() { return DV; }
  int getLEDThresh() { return LEDThreshold; }
  int getIntTime() { return EM; }
  int getGapTime() { return EK; }
  int getLEDGapTime() { return LEDK; }
  int getBLRValue() { return DV; }
  int getBLRInhibit() { return BLRinhibitLength; }
  int getBLRTrigger() { return BLRretrigger; }
  int getBLR() { return useBLR; }
  int getEnergyPO() { return usePO; }
  int getPOTime() { return POtime; }

  
  ClassDef(streamer, 1);
};



#endif
