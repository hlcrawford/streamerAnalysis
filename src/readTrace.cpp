#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <signal.h>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include "streamFunctions.h"
#include "GRETINA.h"

#define OUTPUTFILE "tr.out"

int gotsignal;
void breakhandler(int dummy) {
  printf("Got break signal.  Aborting cleanly...\n");
  gotsignal = 1;
}

int main(int argc, char **argv) {

  gotsignal = 0;
  signal(SIGINT, breakhandler);

  FILE *fout;
  int num, curr, newRead;

  int nReads = 10000;
  
  unsigned long long int ledCrossings;
  int ledCrossing = 0;
  
  int pileUp = 0;
  int ledCrossed = 0;
  long long int startTS = 0;
  long long int ledTS = 0;
  long long int currTS = 0;

  int withBLR = 1;
  int BLRcountdown = 0;

  std::vector<double> energies;
  std::vector<double> energiesPO;
    
  if (argc < 9) {
    fprintf(stderr, "Minimum usage: Analyze -fIn <input filename> -fOut <rootOutputName> -fSet <settingsName> -n <nReads> \n");
    exit(1);
  }
  
  TString rootoutputName, inputName, fileNameSet;
  int inputFile = 0, outputFile = 0, setFile = 0;

  int i = 1;
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
      if (nReads == -1) { nReads = 1000000; }
      i++; i++;
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
  
  streamer *data = new streamer();
  curr = data->Initialize(inputName, fileNameSet);
  data->reportSettings();

  int overlapWidth = 2*(2*data->EM + data->EK);
 
  /* Initialize ROOT output stuff */
  g3OUT *g3 = new g3OUT();
  g3CrystalEvent g3xtal;
  g3ChannelEvent g3ch;
  
  TFile *rootOUT = new TFile(rootoutputName.Data(), "RECREATE");
  TTree *teb = new TTree("teb", "Tree - with data stuff");
  teb->Branch("g3", "g3OUT", &(g3));
  
  TH1F *rawEnergy = new TH1F("rawEnergy", "rawEnergy", 300000, 0, 300000);
  
  /* Initialize file for chunk of waveform output */
  fout = fopen(OUTPUTFILE, "w");
  if (fout == 0) {
    fprintf(stderr, "Cannot open the output file %s\n", OUTPUTFILE);
    exit(1);
  } else {
    printf("Opened text output file %s\n", OUTPUTFILE);
  }

  startTS = 0;
  double pzSum = 0;
  int indexStart = 0;

  for (int numberOfReads = 1; numberOfReads<=nReads; numberOfReads++) {
    if (numberOfReads == 1) { indexStart = 0; curr -= overlapWidth/2; }
    else { indexStart = overlapWidth/2; curr += overlapWidth/2; }

    /* First basic filtering ... 
          for first read, we go from [0] to [END - 1/2 of overlap]
	  for subsequent reads, we go from [1/2 of overlap] to [END - 1/2 of overlap] */
    data->doLEDfilter(indexStart, curr, startTS);
    ledCrossings = data->getLEDcrossings(indexStart, curr, startTS);
    ledCrossing += ledCrossings;
    printf("numberOfReads = %d, ledCrossing = %d\n", numberOfReads, ledCrossing);

    if (0) {
      //energies = data->doPeakSensing(indexStart, curr, startTS, &pileUp);
      energies = data->doPeakIntegrate(indexStart, curr, startTS, &pileUp);
    } else {
      
      /* Filling histogram with locally optimized energy value... */
      for (i=0; i<data->ledOUT.size(); i++) {
	if (data->ledOUT[i]-startTS-3*(2*data->EM + data->EK) > 0 && data->ledOUT[i]-startTS+6*(2*data->EM + data->EK) < curr) {
	  rawEnergy->Fill(data->doLocalPZandEnergy(data->ledOUT[i]-startTS-3*(2*data->EM + data->EK), 
						   data->ledOUT[i]-startTS+6*(2*data->EM + data->EK), 
						   data->ledOUT[i]-startTS, data->tau));
	}
      }
      
      data->doTrapezoid(indexStart, curr, startTS, numberOfReads);    
      pzSum = data->doPolezeroBasic(indexStart, curr, pzSum, numberOfReads);
      //pzSum = data->twoPolePolezero(indexStart, curr, pzSum, numberOfReads);
      
      /* Making this work over boundaries is going to take some thinking... */
      if (data->useBLR) {
	BLRcountdown = data->doBaselineRestorationCC(indexStart, curr, startTS, numberOfReads, BLRcountdown);
	//data->doBaselineRestorationCC(indexStart, curr, startTS, numberOfReads);
	//data->doBaselineRestorationM2(indexStart, curr, startTS, numberOfReads);
      }
      
      if(data->useBLR) {
	energies = data->doEnergyPeakFind(data->pzBLBuf, indexStart, curr, startTS, &pileUp); 
	energiesPO = data->doEnergyFixedPickOff(data->pzBLBuf, indexStart, curr, startTS, &pileUp); 
      } else {
	energies = data->doEnergyPeakFind(data->pzBuf, indexStart, curr, startTS, &pileUp); 
	energiesPO = data->doEnergyFixedPickOff(data->pzBuf, indexStart, curr, startTS, &pileUp); 
      }
    }    

    /* Write out the events in this chunk of data... */
    for (i=0; i<data->ledOUT.size(); i++) {
      g3ch.Clear();
      g3ch.timestamp = data->ledOUT[i];
      if (i*2 < energies.size()) {
	g3ch.eRaw = energies[i*2];
	g3ch.eCalPO = energiesPO[i*2];
	
	if (energies[i*2+1] == 1) { g3ch.hdr7 = 0x8000; } else { g3ch.hdr7 = 0; } /* Pile up flag... */
	if (i >= 2) {	  
	  g3ch.prevE1 = energies[i*2-2];
	  g3ch.prevE2 = energies[i*2-4];
	} else if (i == 1) {	  
	  g3ch.prevE1 = energies[i*2-2];
	  g3ch.prevE2 = -1;
	}
      } else {
 	g3ch.eRaw = -1;
	g3ch.prevE1 = -1;
	g3ch.prevE2 = -1;
      }
      if (i >= 2) {
	if (data->ledOUT[i]-data->ledOUT[i-1] >= 65536) { 
	  g3ch.deltaT1 = 0;
	} else {
	  g3ch.deltaT1 = data->ledOUT[i]-data->ledOUT[i-1];
	}
	if (data->ledOUT[i-1]-data->ledOUT[i-2] >= 65536) { 
	  g3ch.deltaT2 = 0;
	} else {
	  g3ch.deltaT2 = data->ledOUT[i-1]-data->ledOUT[i-2];
	}
      } else if (i == 1) {
	if (data->ledOUT[i]-data->ledOUT[i-1] >= 65536) { 
	  g3ch.deltaT1 = 0;
	} else {
	  g3ch.deltaT1 = data->ledOUT[i]-data->ledOUT[i-1];
	}
      }
      
      g3ch.hdr0 = numberOfReads;
      g3ch.CFDtimestamp = data->ledOUT[i] - startTS;
      
      /* Pull out the WF */
      g3ch.wf.raw.clear();
      for (int j=-200; j<300; j++) {
	g3ch.wf.raw.push_back(data->wf[data->ledOUT[i]-startTS+j]);
      }
      g3ch.baseline = data->baseline(data->ledOUT[i]-startTS-25);
      
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

    energies.clear();
    energiesPO.clear();
    data->ledOUT.clear();
    
    startTS += (curr - overlapWidth);

    if (numberOfReads < nReads) {
      curr = data->Reset(overlapWidth); /* Clears and gets the next data bunch... */
      if (curr <= 0) { break; }
    }

    if (gotsignal == 1) { numberOfReads = nReads+1; printf("Exiting gracefully.\n");}
  }

  printf("LED crossings observed = %d\n", ledCrossing);
  printf("Pileups observed = %d\n", pileUp);
  printf("MB read = %d\n", data->bytesRead/(1024*1024));

  rawEnergy->Write();
  teb->Write();
  rootOUT->Write();
  rootOUT->Close();
  
  fprintf(fout, "index/I:wf/I:led/I:trapE/I:pz/F:pzBL/F\n");      
  for (i=0; i<curr/10; i++) {
    fprintf(fout, "%d %d %d %d %f %f\n", i, data->wf[i], data->ledBuf[i], data->trapBuf[i], data->pzBuf[i], data->pzBLBuf[i]);
  }
}

