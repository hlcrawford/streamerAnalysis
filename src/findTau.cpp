#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#include "TFile.h"
#include "TGraph.h"
#include "TH2.h"
#include "TF1.h"

#include "streamFunctions.h"

int main(int argc, char **argv) {

  FILE *fout;
  int num, curr, i, newRead;

  int nReads = 10000;
  int overlapWidth = 2*EM + EK + 100;

  int ledThresh = 50;
  
  unsigned long long int ledCrossings;
  int ledCrossing = 0;
  
  long long int startTS = 0;
  long long int ledTS = 0;
  long long int currTS = 0;

  int invert = 0;

  Double_t xVal[1000], yVal[1000];
  for (i=0; i<1000; i++) { xVal[i] = i; }
  
  if (argc < 5) {
    fprintf(stderr, "Minimum usage: getTau <filename> <LED threshold> <nReads> <ROOT output> (<invert? == 0>) \n");
    exit(1);
  }

  TString inputName = argv[1];
  printf("Input: %s\n", argv[1]); 
  sscanf(argv[2], "%d", &ledThresh);
  sscanf(argv[3], "%d", &nReads); 
  if (nReads == -1) { nReads = 1000000; }
  TString rootoutputName;
  rootoutputName = argv[4];  
  printf("ROOT Output: %s\n", argv[4]);
  if (argc == 6) {
    sscanf(argv[5], "%d", &invert);
  }
    
  streamer *data = new streamer(invert);
  data->setLEDThresh(ledThresh);
  curr = data->Initialize(inputName);

  TFile *rootOUT = new TFile(rootoutputName.Data(), "RECREATE");

  TGraph *gr;
  TH2F *tauValues = new TH2F("tau", "tau", 3000, 0, 30000, 3000, 0, 3000);

  TF1 *expofit = new TF1("expofit", "expo(0)");
  expofit->SetParameters(100, -0.00015);
	
  startTS = 0;
  double pzSum = 0;
  int indexStart = 0;

  for (int numberOfReads = 1; numberOfReads<=nReads; numberOfReads++) {

    if (numberOfReads == 1) { indexStart = 0; }
    else { indexStart = overlapWidth; }

    /* Actual analysis is from these functions for a given chunk of waveform */
    data->doLEDfilter(indexStart, curr, startTS);
    ledCrossings = data->getLEDcrossings(indexStart, curr, startTS);
    ledCrossing += ledCrossings;

    double averageTrigger = 0;
    double stdDevTrigger = 0;
    if (numberOfReads == 1) {
      for (i=0; i<5000; i++) {
	averageTrigger += (double)data->ledBuf[i];
      } 
      averageTrigger /= 5000.;
      for (i=0; i<5000; i++) {
	stdDevTrigger += (double)(data->ledBuf[i] - averageTrigger)*(double)(data->ledBuf[i]-averageTrigger);
      }
      stdDevTrigger /= 5000.;
      stdDevTrigger = sqrt(stdDevTrigger);
      printf("Noise level = %f +/- %f\n", averageTrigger, stdDevTrigger);
    }	

    printf("numberOfReads = %d, ledCrossing = %d\r", numberOfReads, ledCrossing);

    for (i=0; i<data->ledOUT.size(); i++) {
      if ( (data->ledOUT[i+1] - data->ledOUT[i]) > 1500) {
	for (int j=0; j<800; j++) { yVal[j] = data->wf[data->ledOUT[i] - startTS + 200 + j] - data->wf[data->ledOUT[i] - startTS - 10]; }
	gr = new TGraph(800, xVal, yVal);
	gr->Fit("expofit", "Q");
	if (expofit->GetChisquare()/expofit->GetNDF() > 300) {
	  expofit->SetParameters(100, -0.00015);
	  gr->Fit("expofit", "Q");
	}
	gr->SetName(Form("gr%d", i));
	gr->Write();
	tauValues->Fill(-1./expofit->GetParameter(1), expofit->GetChisquare()/expofit->GetNDF());
	//printf("%d %f %f\n", i, 1./expofit->GetParameter(1), expofit->GetChisquare()/expofit->GetNDF());
      }
    }

    data->ledOUT.clear();
    
    startTS += (curr - overlapWidth);

    if (numberOfReads < nReads) {
      curr = data->Reset(overlapWidth); /* Clears and gets the next data bunch... */
      if (curr <= 0) { break; }
    }

  }

  printf("LED crossings observed = %d\n", ledCrossing);
  
  tauValues->Write();
  rootOUT->Write();
  rootOUT->Close();
}

