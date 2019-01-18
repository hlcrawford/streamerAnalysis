#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#include "TFile.h"
#include "TGraph.h"
#include "TH2.h"
#include "TF1.h"

#include "streamFunctions.h"

int gotsignal;
void breakhandler(int dummy) {
  printf("Got break signal.  Aborting cleanly...\n");
  gotsignal = 1;
}

int main(int argc, char **argv) {

  FILE *fout;
  int num, curr, i, newRead;

  int nReads = 10000;
  
  unsigned long long int ledCrossings;
  int ledCrossing = 0;
  
  long long int startTS = 0;
  long long int ledTS = 0;
  long long int currTS = 0;

  int twoPoles = 0;
  int invert = 0;

  Double_t xVal[18000], yVal[18000];
  for (i=0; i<18000; i++) { xVal[i] = i; }

  if (argc < 9) {
    fprintf(stderr, "Minimum usage: getTau -fIn <input filename> -fOut <rootOutputName> -fSet <settingsName> -n <nReads> \n");
    exit(1);
  }
  
  TString rootoutputName, inputName, fileNameSet;
  int inputFile = 0, outputFile = 0, setFile = 0;

  i = 1;
  while (i < argc) {
    if (strcmp(argv[i], "-fIn") == 0) {
      inputName = argv[i+1]; i++; i++;
      inputFile = 1;
    } else if (strcmp(argv[i], "-fOut") == 0) {
      rootoutputName = argv[i+1]; i++; i++;
      outputFile = 1;
    } else if (strcmp(argv[i], "-fSet") == 0) {
      fileNameSet = argv[i+1]; i++; i++;
      setFile = 1;
    } else if (strcmp(argv[i], "-n") == 0) {
      sscanf(argv[i+1], "%d", &nReads);
    } else {
      cout << ALERTTEXT;
      printf("Error -- unrecognized input flag: <%s>\n", argv[i]);
      cout << RESET_COLOR; fflush(stdout);
      exit(-1);
    }
  }

  if (!inputFile || !outputFile) {
    cout << ALERTTEXT;
    printf("Error -- missing arguments!  Try again!\n");
    cout << RESET_COLOR; fflush(stdout);
    exit(-1);
  }
  if (!setFile) {
    fileNameSet = "settings.set";
  }
  
  streamer *data = new streamer(invert);
  curr = data->Initialize(inputName, fileNameSet);
  
  int overlapWidth = 2*(2*data->EM + data->EK);

  TFile *rootOUT = new TFile(rootoutputName.Data(), "RECREATE");

  TGraph *gr;
  TH2F *tauValues;
  if (!twoPoles) {
    tauValues = new TH2F("tau", "tau", 3000, 0, 30000, 3000, 0, 3000);
  } else {
    tauValues = new TH2F("tau", "tau", 3000, 0, 30000, 3000, 0, 30000);
  }
  
  TF1 *expofit;
  if (!twoPoles) {
    expofit = new TF1("expofit", "expo(0)");
    expofit->SetParameters(100, -0.00015);
  } else {
    expofit = new TF1("expofit", "expo(0) + expo(2)");
    expofit->SetParameters(7, -0.0003, 2, -0.00005);
  }
	
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
      if ( (data->ledOUT[i+1] - data->ledOUT[i]) > 11000) {
	for (int j=0; j<10000; j++) { yVal[j] = data->wf[data->ledOUT[i] - startTS + 200 + j] - data->wf[data->ledOUT[i] - startTS - 10]; }
	gr = new TGraph(10000, xVal, yVal);
	gr->Fit("expofit", "Q");
	if (!twoPoles && expofit->GetChisquare()/expofit->GetNDF() > 300) {
	  expofit->SetParameters(100, -0.00015);
	  gr->Fit("expofit", "Q");
	}
	if (twoPoles && expofit->GetChisquare()/expofit->GetNDF() > 300) {
	  expofit->SetParameters(7, -0.0003, 2, -0.00005);
	  gr->Fit("expofit", "Q");
	}
	gr->SetName(Form("gr%d", i));
	gr->Write();
	if (!twoPoles) {
	  tauValues->Fill(-1./expofit->GetParameter(1), expofit->GetChisquare()/expofit->GetNDF());
	  printf("%d %f %f\n", i, 1./expofit->GetParameter(1), expofit->GetChisquare()/expofit->GetNDF());
	} else {
	  if (expofit->GetParameter(1) > expofit->GetParameter(3)) {
	    tauValues->Fill(-1./expofit->GetParameter(1), -1./expofit->GetParameter(3));
	  } else {
	    tauValues->Fill(-1./expofit->GetParameter(3), -1./expofit->GetParameter(1));
	  }
	  printf("%d %f %f %f \n", i, -1./expofit->GetParameter(1), -1./expofit->GetParameter(3), expofit->GetChisquare()/expofit->GetNDF());
	}
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

