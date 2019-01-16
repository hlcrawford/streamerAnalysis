#include "streamFunctions.h"

int streamer::Initialize(TString inputFileName) {
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

  //inputFileName = "zcat " + inputFileName;
  //fin = popen(inputFileName.Data(), "r");
  fin = fopen(inputFileName.Data(), "r");
  if (fin == 0) {
    fprintf(stderr, "Cannot open the input file %s\n", inputFileName.Data());
    exit(1);
  }
  
  bytesRead = 0;

  int num = fread(wf, sizeof(short int), READSIZE, fin);
  if (invertWF) { for (int j=0; j<READSIZE; j++) { wf[j] = -wf[j]; } }
  bytesRead += sizeof(short int)*num;
 
  return num;
}

int streamer::Reset(int overlap) {
  for (int i=0; i<overlap; i++) {
    wf[i] = wf[READSIZE-overlap+i];
    ledBuf[i] = ledBuf[READSIZE-overlap+i];
    trapBuf[i] = trapBuf[READSIZE-overlap+i];
    pzBuf[i] = pzBuf[READSIZE-overlap+i];
    subBL1[i] = subBL1[READSIZE-overlap+i];
    subBL2[i] = subBL2[READSIZE-overlap+i];
    BL1[i] = BL1[READSIZE-overlap+i];
    BL2[i] = BL2[READSIZE-overlap+i];
    pzBLBuf[i] = pzBLBuf[READSIZE-overlap+i];
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
  if (invertWF) { for (int j=overlap; j<READSIZE; j++) { wf[j] = -wf[j]; } }
  bytesRead += sizeof(short int)*num;

  return num;
  
}

/**************************************************************/

int streamer::calculateLEDlevel(int index, int thresh) {
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
  dd = (dd1 + 2*dd2 + dd3)/4;
  return dd;
}

/**************************************************************/

int streamer::doLEDfilter(int startIndex, int endIndex, int startTS) {

  int now = 0; int previous = 0;

  for (int i=startIndex; i<endIndex; i++) {
    /* LED-like filter */
    if ((i + startTS) > 10) {
      ledBuf[i] = calculateLEDlevel(i, LEDThreshold);
      /* Look for LED crossing -- i.e. trigger */
      if ((LEDThreshold > 0 && (ledBuf[i] >= LEDThreshold) && (ledBuf[i-1] < LEDThreshold)) ||
	  (LEDThreshold < 0 && (ledBuf[i] <= LEDThreshold) && (ledBuf[i-1] > LEDThreshold))) {
     	ledOUT.push_back(i+startTS);
      }
    }
  }
  return ledOUT.size();
  
}

/**************************************************************/

double streamer::baseline(int index) {
  int sum = 0;
  for (int i=0; i<10; i++) {
    sum += wf[index-i];
  }
  return (double)(sum)/10.;
}

/**************************************************************/

void streamer::doTrapezoid(int startIndex, int endIndex) {

  /* Trapezoid filter straight out of Jordanov and Knoll, NIM A 345, 337 (1994). */

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

double streamer::doPolezeroBasic(int startIndex, int endIndex, double sum, double tau) {

  for (int i=startIndex; i<endIndex; i++) {
    if (i < (2*EM+EK)) { pzBuf[i] = 0.; }
    else if (i == (2*EM+EK)) { pzBuf[i] = (tau+1)*trapBuf[i-1]; }
    else if (i > (2*EM+EK)) {
      if (sum>0) {
	sum += (double)trapBuf[i];
	pzBuf[i] = trapBuf[i-1] + sum*(1/tau);
      } else {
	sum += (double)trapBuf[i];
	pzBuf[i] = trapBuf[i-1] + sum*(1/tau);
      }
    }
    //printf("%d %d %f %f\n", i, trapBuf[i], sum, pzBuf[i]);
  }

  return sum;
  
}

/**************************************************************/

void streamer::doBaselineRestorationCC(int startIndex, int endIndex, int startTS, int DV) {

  int j=0;
  long int ledTS = 0;
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

void streamer::doBaselineRestorationM2(int startIndex, int endIndex, int startTS, int DV) {

  int j=0;
  long int ledTS = 0;
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

void streamer::doBaselineRestorationSZ() {
  


}

/**************************************************************/

std::vector<double> streamer::doEnergyPeakFind(double *in, int startIndex, int endIndex, int startTS, int *pileUp) {

  std::vector<double> energies;

  if (ledOUT.size() > 0) {
    int j=0, ledCrossed = 0, piledUp = 0;
    double peak = 0;
    long int ledTS = 0;
    
    for (int i=startIndex; i<endIndex; i++) {
      if (i == (ledOUT[j]-startTS)) {
	if (ledCrossed) { (*pileUp)++; piledUp = 1; }
	ledTS = (ledOUT[j]-startTS);  ledCrossed = 1;  j++;
      }
      if (ledCrossed && in[i] > peak && (i-ledTS) < (EM+EK)) { peak = in[i]; }
      if (ledCrossed && (i-ledTS) >= EM+EK) {
	if (!piledUp) {
	  energies.push_back((peak - in[ledTS-10])/32.);
	  energies.push_back(0);
	} else {
	  energies.push_back((peak - in[ledTS-10])/32.);
	  energies.push_back(1);
	}
	ledCrossed = 0;  piledUp = 0;
	if (in[i] > 0) { peak = 0; }
	else if (in[i] < 0) { peak = std::numeric_limits<double>::max(); }
      }
    }
  }
  
  return energies;
}

std::vector<double> streamer::doEnergyFixedPickOff(double *in, int pickOff, int startIndex, int endIndex, int startTS, int *pileUp) {

  std::vector<double> energies;
  if (ledOUT.size() > 0) {
    int j=0;
    int piledUp = 0;

    for (int i=startIndex; i<endIndex; i++) {
      if (i == (ledOUT[j]-startTS) && j < ledOUT.size()-1) {
	/* Check for pileup condition... */	
	if ( ((ledOUT[j]-ledOUT[j-1])< EM+EK) ||
	     ((ledOUT[j+1]-ledOUT[j])< EM+EK) ) {
	  (*pileUp)++; piledUp = 1;
	  energies.push_back((in[ledOUT[j]-startTS + pickOff] - in[ledOUT[j]-startTS-10])/32.);
	  energies.push_back(1);
	} else { piledUp = 0; }
	if (!piledUp) {
	  // energies.push_back(in[ledOUT[j]-startTS + pickOff])
	  energies.push_back((in[ledOUT[j]-startTS + pickOff] - in[ledOUT[j]-startTS-10])/32.);
	  energies.push_back(0);
	}
	j++;
      }
    }
  }

  return energies;

}
