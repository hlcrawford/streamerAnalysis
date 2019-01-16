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

#define READSIZE 1024*1024

#define LEDK 5
#define EK 100 
#define EM 400

class streamer : public TObject {

 public:
  FILE *fin;
  
  short int *wf; // raw trace buffer
  int *ledBuf, *trapBuf;
  double *pzBuf, *subBL1, *subBL2, *BL1, *BL2, *pzBLBuf;
  std::vector<long int> ledOUT;
  
  int bytesRead;
  double tau;
  int DV;
  int LEDThreshold;

  int invertWF;
  
 public:
  streamer(int invert) { invertWF = invert; }
  ~streamer() {;}
  int Initialize(TString inputFileName);
  int Reset(int overlap);
  int calculateLEDlevel(int index, int thresh);
  int doLEDfilter(int startIndex, int endIndex, int startTS);
  double baseline(int index);
  void doVHDLtrapezoid(int startIndex, int endIndex);
  double doVHDLpolezero(int startIndex, int endIndex, double sum, double tau);
  void doBaselineRestorationCC(int startIndex, int endIndex, int startTS, int DV);
  void doBaselineRestorationM2(int startIndex, int endIndex, int startTS, int DV);
  std::vector<double> doEnergyPeakFind(double *in, int startIndex, int endIndex, int startTS, int *pileUp);
  std::vector<double> doEnergyFixedPickOff(double *in, int pickOff, int startIndex, int endIndex, int startTS, int *pileUp);

  void setTau(double itau) { tau = itau; }
  void setDV(double iDV) { DV = iDV; }
  void setLEDThresh(double iLEDThreshold) { LEDThreshold = iLEDThreshold; }
  
  ClassDef(streamer, 1);
};



#endif
