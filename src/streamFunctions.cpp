#include "streamFunctions.h"

int streamFunctions::Initialize(TString inputFileName) {
  wf = (short int*)calloc(READSIZE, sizeof(short int));
  ledBuf = (int*)calloc(READSIZE, sizeof(int));
  trapBuf = (int*)calloc(READSIZE, sizeof(int));
  pzBuf = (double*)calloc(READSIZE, sizeof(double));
  subBL1 = (double*)calloc(READSIZE, sizeof(double));
  subBL2 = (double*)calloc(READSIZE, sizeof(double));
  BL1 = (double*)calloc(READSIZE, sizeof(double));
  BL2 = (double*)calloc(READSIZE, sizeof(double));
  pzBLBuf = (double*)calloc(READSIZE, sizeof(double));
  
  BL1[0] = 0;  BL2[0] = 0; subBL1[0] = 0; subBL2[0] = 0;

  fin = fopen(inputFileName.Data(), "r");
  if (fin == 0) {
    fprintf(stderr, "Cannot open the input file %s\n", inputFileName.Data());
    exit(1);
  }
  
  bytesRead = 0;

  int num = fread(wf, sizeof(short int), READSIZE, fin);
  bytesRead += sizeof(short int)*num;

  return num;
}

int streamFunctions::Reset(int overlap) {
  for (int i=0; i<overlap; i++) {
    wf[i] = wf[READSIZE-overlap+i];
    ledBuf[i] = ledBuf[READSIZE-overlap+i];
    trapBuf[i] = trapBuf[READSIZE-overlap+i];
    pzBuf[i] = pzBuf[READSIZE-overlap+i];
    subBL1[i] = pzBuf[READSIZE-overlap+i];
    subBL2[i] = pzBuf[READSIZE-overlap+i];
    BL1[i] = pzBuf[READSIZE-overlap+i];
    BL2[i] = pzBuf[READSIZE-overlap+i];
    pzBLBuf[i] = pzBuf[READSIZE-overlap+i];
  }

  for (int i=overlap; i<READSIZE; i++) {
    wf[i] = 0;
    ledBuf[i] = 0;
    trapBuf[i] = 0;
    pzBuf[i] = 0.0;
    subBL1[i] = 0.0;
    subBL2[i] = 0.0;
    BL1[i] = 0.0;
    BL2[i] = 0.0;
    pzBLBuf[i] = 0.0;
  }

  ledOUT.clear();
  
  int num = fread(wf+overlap, sizeof(short int), READSIZE-overlap, fin);
  bytesRead += sizeof(short int)*num;

  return num;
  
}

/**************************************************************/

int streamFunctions::calculateLEDlevel(int index, int thresh) {
  short int d1, d2, d3, d4, d5, dd, dd1, dd2, dd3;
  index-=2;
  d1 = wf[index-2] - wf[index-LEDK-2];
  d2 = wf[index-1] - wf[index-LEDK-1];
  d3 = wf[index] - wf[index-LEDK];
  d4 = wf[index+1] - wf[index-LEDK+1];
  d5 = wf[index+2] - wf[index-LEDK+2];
  dd1 = (d1 + 2*d2 + d3)/4;
  dd2 = (d2 + 2*d3 + d4)/4;
  dd3 = (d3 + 2*d4 + d5)/4;
  dd = -(dd1 + 2*dd2 + dd3)/4;
  if (thresh > 0) { 
    if (dd > thresh) { return 100; } else { return 0; }
  } else {
    if (dd < thresh) { return 100; } else { return 0; }
  }

  return 100;
}

/**************************************************************/

int streamFunctions::doLEDfilter(int startIndex, int endIndex, int startTS) {

  for (int i=startIndex; i<endIndex; i++) {
    /* LED-like filter */
    if ((i + startTS) > 10) {
      ledBuf[i] = calculateLEDlevel(i, LEDThreshold);
      /* Look for LED crossing -- i.e. trigger */
      if (ledBuf[i] == 100 && ledBuf[i-1] == 0) {
	ledOUT.push_back(i+startTS);
      }
    }
  }
  return ledOUT.size();
  
}

/**************************************************************/

double streamFunctions::baseline(int index) {
  int sum = 0;
  for (int i=0; i<10; i++) {
    sum += wf[index-i];
  }
  return (double)(sum)/10.;
}

/**************************************************************/

void streamFunctions::doVHDLtrapezoid(int startIndex, int endIndex) {

  for (int i=startIndex; i<endIndex; i++) {
    if (i == 0) { trapBuf[i] = 0; }
    else if (i <= (2*EM+EK)) { trapBuf[i] = wf[i] - trapBuf[i-1]; }
    else if (i > (2*EM+EK)) {
      int scratch =  ( (wf[i]) - (wf[i-EM]) - (wf[i-EM-EK]) + (wf[i-2*EM-EK]) );
      trapBuf[i] = trapBuf[i-1] + scratch;
    }


  }
  
}

/**************************************************************/

double streamFunctions::doVHDLpolezero(int startIndex, int endIndex, double sum, double tau) {

  for (int i=startIndex; i<endIndex; i++) {
    if (i < (2*EM+EK)) { pzBuf[i] = 0.; }
    else if (i == (2*EM+EK)) { trapBuf[i] = (tau+1)*trapBuf[i-1]; }
    else if (i > (2*EM+EK)) {
      if (sum>0) {
	sum += (double)trapBuf[i] - (0.00015);
      } else {
	sum += (double)trapBuf[i] + (0.00015);
	pzBuf[i] = trapBuf[i-1] + sum*(1/tau) + 7.e-6;
      }
    }
  }

  return sum;
  
}

/**************************************************************/

void streamFunctions::doBaselineRestorationCC(int startIndex, int endIndex, int startTS, int DV) {

  int j=0;
  long long int ledTS = 0;
  int BLRinhibit = 0;
    
  for (int i=startIndex; i<endIndex; i++) {

    if (i+startTS > 2*EM+EK) {
      if (ledOUT.size() > 0) {
	if (i == (ledOUT[j]-startTS-10)) {
	  ledTS = (ledOUT[j]-startTS);  BLRinhibit = 1;  j++;
	}
      }
      if (i > (ledTS + 2*EM + EK)) {
	BLRinhibit = 0;
      }
      
      subBL1[i] = (pzBuf[i-1]-BL1[i-1])*(pow(2., DV));
      subBL2[i] = (BL1[i-1]-BL2[i-1])*(pow(2., DV));
      
      if (!BLRinhibit) {
	BL1[i] = BL1[i-1] + subBL1[i-1];
	BL2[i] = BL2[i-1] + subBL2[i-1];
      } else {
	BL1[i] = BL1[i-1];
	BL2[i] = BL2[i-1];
      }
    }

    pzBLBuf[i] = pzBuf[i] - BL2[i];
    
  }

}

/**************************************************************/

void streamFunctions::doBaselineRestorationM2(int startIndex, int endIndex, int startTS, int DV) {

  int j=0;
  long long int ledTS = 0;
  int BLRinhibit = 0;

  double div = pow(2., DV);
  
  for (int i=startIndex; i<endIndex; i++) {

    if (ledOUT.size() > 0) {
      if (i == (ledOUT[j]-startTS-10)) {
	ledTS = (ledOUT[j]-startTS);  BLRinhibit = 1;  j++;
      }
    }
    if (i > (ledTS + 2*EM + EK)) {
      BLRinhibit = 0;
    }
    if (!BLRinhibit) {
      BL1[i] = BL1[i-1]*(1-div) + pzBuf[i];
      subBL1[i] = BL1[i]*(div);
      BL2[i] = BL2[i-1]*(1-div) + subBL1[i];
      subBL2[i] = BL2[i]*(div);
    } else {
      BL1[i] = BL1[i-1];
      subBL1[i] = subBL1[i-1];
      BL2[i] = BL2[i-1];
      subBL2[i] = subBL2[i-1];
    }
    
    pzBLBuf[i] = pzBuf[i] - subBL2[i];
    
  }

}

/**************************************************************/

std::vector<double> streamFunctions::doEnergyPeakFind(double *in, int startIndex, int endIndex, int startTS, int *pileUp) {

  std::vector<double> energies;

  if (ledOUT.size() > 0) {
    int j=0, ledCrossed = 0, piledUp = 0;
    double peak = 0;
    long long int ledTS = 0;
    
    for (int i=startIndex; i<endIndex; i++) {
      if (i == (ledOUT[j]-startTS)) {
	if (ledCrossed) { (*pileUp)++; piledUp = 1; }
	ledTS = (ledOUT[j]-startTS);  ledCrossed = 1;  j++;
      }
      if (ledCrossed && in[i] < peak && (i-ledTS) < (EM+EK)) { peak = in[i]; }
      if (ledCrossed && (i-ledTS) >= EM+EK) {
	if (!piledUp) {
	  energies.push_back(-(peak - in[ledTS-10])/32.);
	}
	ledCrossed = 0;  piledUp = 0;
	if (in[i] < 0) { peak = 0; }
	else if (in[i] > 0) { peak = std::numeric_limits<double>::max(); }
      }
    }
  }
  
  return energies;
}
