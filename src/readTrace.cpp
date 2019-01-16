#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include "streamFunctions.h"
#include "GRETINA.h"

#define OUTPUTFILE "tr.out"

int main(int argc, char **argv) {

  FILE *fout;
  int num, curr, i, newRead;

  int nReads = 10000;
  int overlapWidth = 2*EM + EK + 100;

  double tau = 5500; /* From Mario S. code */
  int DV = 6;
  int ledThresh = 50;
  
  unsigned long long int ledCrossings;
  int ledCrossing = 0;
  
  int pileUp = 0;
  int ledCrossed = 0;
  long long int startTS = 0;
  long long int ledTS = 0;
  long long int currTS = 0;

  int inhibitBLR = 0;
  int withBLR = 0;
  long long int BLRi = 1;

  int invert = 0;

  std::vector<double> energies;
  
  double peak = 0;
  
  if (argc < 4) {
    fprintf(stderr, "Minimum usage: readTrace <input filename> <tau> <BLR constant> <nReads> <rootOutputName> <ledThreshold> <invert? = 0>\n");
    exit(1);
  }

  TString inputName = argv[1];
  printf("Input: %s\n", argv[1]); 
  sscanf(argv[2], "%lf", &tau);
  sscanf(argv[3], "%d", &DV);  DV -= 16;
  if (argc >= 4) { sscanf(argv[4], "%d", &nReads); } else { nReads = 1000000; }
  if (nReads == -1) { nReads = 1000000; }
  TString rootoutputName;
  if (argc >= 5) {
    rootoutputName = argv[5];  
    printf("ROOT Output: %s\n", argv[5]);
  } else { 
    rootoutputName = "out.root"; 
    printf("ROOT Output: %s\n", "out.root");  
  }
  if (argc >= 6) { 
    sscanf(argv[6], "%d", &ledThresh); 
    printf("LED threshold = %d\n", ledThresh);
  }
  if (argc >= 8) {
    sscanf(argv[7], "%d", &invert);
  }
  
  streamer *data = new streamer(invert);
  curr = data->Initialize(inputName);
  data->setTau(tau);
  data->setDV(DV);
  data->setLEDThresh(ledThresh);

  g3OUT *g3 = new g3OUT();

  g3CrystalEvent g3xtal;
  g3ChannelEvent g3ch;
  
  TFile *rootOUT = new TFile(rootoutputName.Data(), "RECREATE");
  TTree *teb = new TTree("teb", "Tree - with data stuff");
  teb->Branch("g3", "g3OUT", &(g3));

  TH1F *rawEnergy = new TH1F("rawEnergy", "rawEnergy", 30000, -300, 300);
  
  fout = fopen(OUTPUTFILE, "w");
  if (fout == 0) {
    fprintf(stderr, "cannot open the output file %s\n", OUTPUTFILE);
    exit(1);
  } else {
    printf("Opened text output file %s\n", OUTPUTFILE);
  }

  startTS = 0;
  double pzSum = 0;
  int indexStart = 0;

  for (int numberOfReads = 1; numberOfReads<=nReads; numberOfReads++) {

    if (numberOfReads == 1) { indexStart = 0; }
    else { indexStart = overlapWidth; }

    /* Actual analysis is from these functions for a given chunk of waveform */
    ledCrossings = data->doLEDfilter(indexStart, curr, startTS);
    ledCrossing += ledCrossings;

    printf("numberOfReads = %d, ledCrossing = %d\r", numberOfReads, ledCrossing);
    
    data->doVHDLtrapezoid(indexStart, curr);
    
    pzSum = data->doVHDLpolezero(indexStart, curr, pzSum, tau);
    
    //data->doBaselineRestorationCC(indexStart, curr, startTS, DV);
    //data->doBaselineRestorationM2(indexStart, curr, startTS, DV);

    if(withBLR) {
      energies = data->doEnergyPeakFind(data->pzBLBuf, indexStart, curr, startTS, &pileUp);
    } else {
      energies = data->doEnergyPeakFind(data->pzBuf, indexStart, curr, startTS, &pileUp);
      //energies = data->doEnergyFixedPickOff(data->pzBuf, 470, indexStart, curr, startTS, &pileUp);
    }

    /* Write out the events in this chunk of data... */
    for (int i=0; i<data->ledOUT.size(); i++) {
      g3ch.Clear();
      g3ch.timestamp = data->ledOUT[i];
      if (i < energies.size()) {
	g3ch.eRaw = energies[i];
      } else { g3ch.eRaw = -1000; }
      
      /* Pull out the WF */
      g3ch.wf.raw.clear();
      for (int j=-200; j<300; j++) {
	g3ch.wf.raw.push_back(data->wf[data->ledOUT[i]-startTS+j]);
	if (j >= -10 && j<=-4) {
	  g3ch.baseline += (data->wf[data->ledOUT[i] - startTS+j]);
	}
      }
      g3ch.baseline /= 6.;

      g3xtal.chn.clear();
      g3xtal.chn.push_back(g3ch);
      g3xtal.crystalNum = 1;
      g3xtal.quadNum = 1;
      g3xtal.module = 1;
      g3xtal.traceLength = g3ch.wf.raw.size();

      g3->Reset();
      g3->xtals.push_back(g3xtal);
      teb->Fill();
    }

    for (int i=0; i<energies.size(); i++) { rawEnergy->Fill(energies[i]); }
    
    energies.clear();
    data->ledOUT.clear();
    
    startTS += (curr - overlapWidth);

    if (numberOfReads < nReads) {
      curr = data->Reset(overlapWidth); /* Clears and gets the next data bunch... */
      if (curr <= 0) { break; }
    }

  }

  printf("LED crossings observed = %d\n", ledCrossing);
  printf("Pileups observed = %d\n", pileUp);
  printf("MB read = %d\n", data->bytesRead/(1024*1024));

  rawEnergy->Write();
  teb->Write();
  rootOUT->Write();
  rootOUT->Close();
  
  fprintf(fout, "index/I:wf/I:led/I:trapE/I:pz/F:pzBL/F\n");      
  for (i=0; i<curr; i++) {
    fprintf(fout, "%d %d %d %d %f %f\n", i, data->wf[i], data->ledBuf[i], data->trapBuf[i], data->pzBuf[i], data->pzBLBuf[i]);
  }
}

