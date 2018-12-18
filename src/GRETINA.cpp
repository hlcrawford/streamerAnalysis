/*! \file GRETINA.cpp
    \brief Function code for all GRETINA and GRETINA-related classes.
*/

#include "GRETINA.h"

ClassImp(globalHeader);

/**************************************************************/
/* g3CrystalEvent Class Functions *****************************/
/**************************************************************/

/*! Puts channels from a mode3 crystal event in numerical order
    within the vector chn<g3ChannelEvent>

    \return No return value -- directly alters class structures
*/

void g3CrystalEvent::OrderChannels() {
  g3ChannelEvent temp;
  Bool_t finished = 0;
  while (!finished) {
    finished = 1;
    for (UInt_t ui=0; ui<chn.size()-1; ui++) {
      if (chn[ui].chnNum() > chn[ui].chnNum()) {
	temp = chn[ui];
	chn[ui] = chn[ui+1];
	chn[ui+1] = temp;
	finished = 0;
      }
    }
  }
}

/**************************************************************/

/*! Extracts the calibrated first central contact energy from mode3
    data (CC1 --> electronics channel 9 for a given crystal) 

    \return Returns the float value of the calibrated CC1 energy
*/

Float_t g3CrystalEvent::cc1() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 9) { return chn[ui].eCal; }
  }
  return -1.0;
}

/**************************************************************/

/*! Extracts the calibrated first central contact energy from mode3
    data (CC2 --> electronics channel 19 for a given crystal) 

    \return Returns the float value of the calibrated CC2 energy
*/

Float_t g3CrystalEvent::cc2() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 19) { return chn[ui].eCal; }
  }
  return -1.0;
}

/**************************************************************/

/*! Extracts the calibrated first central contact energy from mode3
    data (CC3 --> electronics channel 29 for a given crystal) 

    \return Returns the float value of the calibrated CC3 energy
*/

Float_t g3CrystalEvent::cc3() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 29) { return chn[ui].eCal; }
  }
  return -1.0;
}

/**************************************************************/

/*! Extracts the calibrated first central contact energy from mode3
    data (CC4 --> electronics channel 39 for a given crystal) 

    \return Returns the float value of the calibrated CC4 energy
*/

Float_t g3CrystalEvent::cc4() { 
 for (UInt_t ui=0; ui<chn.size(); ui++) {
   if (chn[ui].chnNum() == 39) { return chn[ui].eCal; }
  }
  return -1.0;
}

/**************************************************************/

/*! Extracts the uncalibrated first central contact energy from mode3
    data (CC1 --> electronics channel 9 for a given crystal) 

    \return Returns the float value of the raw (uncalibrated) CC1 energy
*/

Float_t g3CrystalEvent::cc1Raw() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 9) { return chn[ui].eRaw; }
  }
  return -1.0;
}

/**************************************************************/

/*! Extracts the uncalibrated first central contact energy from mode3
    data (CC2 --> electronics channel 19 for a given crystal) 

    \return Returns the float value of the raw (uncalibrated) CC2 energy
*/

Float_t g3CrystalEvent::cc2Raw() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 19) { return chn[ui].eRaw; }
  }
  return -1.0;
}

/**************************************************************/

/*! Extracts the uncalibrated first central contact energy from mode3
    data (CC3 --> electronics channel 29 for a given crystal) 

    \return Returns the float value of the raw (uncalibrated) CC3 energy
*/

Float_t g3CrystalEvent::cc3Raw() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 29) { return chn[ui].eRaw; }
  }
  return -1.0;
}

/**************************************************************/

/*! Extracts the uncalibrated first central contact energy from mode3
    data (CC4 --> electronics channel 39 for a given crystal) 

    \return Returns the float value of the raw (uncalibrated) CC4 energy
*/

Float_t g3CrystalEvent::cc4Raw() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 39) { return chn[ui].eRaw; }
  }
  return -1.0;
}

/**************************************************************/

/*! Extracts the first central contact energy calculated from the waveform
    in  mode3 data (CC1 --> electronics channel 9 for a given crystal) 

    \return Returns the float value of the waveform calculated CC1 energy
*/

Float_t g3CrystalEvent::cc1Calc() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 9) { return chn[ui].eCalc; }
  }
  return -1.0; 
}

/**************************************************************/

/*! Extracts the first central contact energy calculated from the waveform
    in  mode3 data (CC2 --> electronics channel 19 for a given crystal) 

    \return Returns the float value of the waveform calculated CC2 energy
*/

Float_t g3CrystalEvent::cc2Calc() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 19) { return chn[ui].eCalc; }
  }
  return -1.0;  
}

/**************************************************************/

/*! Extracts the first central contact energy calculated from the waveform
    in  mode3 data (CC3 --> electronics channel 29 for a given crystal) 

    \return Returns the float value of the waveform calculated CC3 energy
*/

Float_t g3CrystalEvent::cc3Calc() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 29) { return chn[ui].eCalc; }
  }
  return -1.0;  
}

/**************************************************************/

/*! Extracts the first central contact energy calculated from the waveform
    in  mode3 data (CC4 --> electronics channel 39 for a given crystal) 

    \return Returns the float value of the waveform calculated CC4 energy
*/

Float_t g3CrystalEvent::cc4Calc() { 
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].chnNum() == 39) { return chn[ui].eCalc; }
  }
  return -1.0;  
}

/**************************************************************/

/*! Calculates a raw (NOT Dino corrected) segment sum for the crystal, 
    with no threshold on the segments included

    \return Returns the float value of the raw segment sum for
            the crystal 
*/

Float_t g3CrystalEvent::segSumRaw() {
  return segSumRawThresh(0.0);
}

/**************************************************************/

/*! Calculates a raw (NOT Dino corrected) segment sum for the crystal,
    including segments that are above a threshold value

    \param thresh The float energy threshold; any segments below
           this value are not included in the sum
    \return Returns the float value of the raw segment sum for the crystal
*/

Float_t g3CrystalEvent::segSumRawThresh(Float_t thresh) {
  Float_t segSum = 0;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].eCal > thresh && chn[ui].segNum < 36) {
      segSum += chn[ui].eCal;
    }
  }
  return segSum;
}

/**************************************************************/

/*! Calculates a raw (NOT Dino corrected) segment sum for the given crystal, 
    but with a timing requirement.  Segments are only included if their 
    calcTime is within the input limits; no energy threshold is included.

    \param minT The minimum time (in trace clock ticks) with which a 
                segment will be included in the segment sum
    \param maxT The maximum time (in trace clock ticks) with which a
                segment will be included in the segment sum
    \return Returns the float values of the raw segment sum for the crystal,
            under the input timing conditions
*/

Float_t g3CrystalEvent::segSumRawTimed(Float_t minT, Float_t maxT) {
  Float_t segSum = 0;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].segNum < 36) {
      if (chn[ui].calcTime > minT &&
	  chn[ui].calcTime > maxT) {
	segSum += chn[ui].eCal;
      }
    }
  }
  return segSum;
}

/**************************************************************/

/*! Calculates a raw (NOT Dino corrected) segment sum for the given crystal, 
    with a timing requirement.  Segments are only included if their 
    calcTime is within the input limits AND above a threshold value.

    \param thresh The float energy threshold; any segments below
                  this value are not included in the sum
    \param minT The minimum time (in trace clock ticks) with which a 
                segment will be included in the segment sum
    \param maxT The maximum time (in trace clock ticks) with which a
                segment will be included in the segment sum
    \return Returns the float values of the raw segment sum for the crystal,
            under both the input timing and threshold conditions
*/

Float_t g3CrystalEvent::segSumRawThreshTimed(Float_t thresh, Float_t minT, Float_t maxT) {
  Float_t segSum = 0;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].eCal > thresh && chn[ui].segNum < 36) {
      if (chn[ui].calcTime > minT &&
	  chn[ui].calcTime > maxT) {
	segSum += chn[ui].eCal;
      }
    }
  }
  return segSum;
}

/**************************************************************/

/*! Determines the number of segments hit in the crystal above a given
    energy threshold

    \param thresh The float energy threshold -- segments with energy
           depositions above threshold are counted as hit
    \return Returns integer value of the number of segments hit above 
            the energy threshold
*/

Int_t g3CrystalEvent::segsHit(Float_t thresh) {
  Int_t hit = 0;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].eCal > thresh && chn[ui].segNum < 36) {
      hit++;
    }
  }
  return hit;
}

/**************************************************************/

/*! Calculates a segment sum value based on the waveform-derived segment
  energy values.

  \return Returns the float value of the segment sum energy for the crystal,
  which in this case is the sum of segments with valid waveform calculated energies.
*/

Float_t g3CrystalEvent::segSumCalc() {
  Float_t segSum = segSumCalcThresh(0.0);
  return segSum;
}

/**************************************************************/

/*! Calculates a segment sum value based on the waveform-derived segment
  energy values, only including segments with calculated energies above
  the input threshold value.

  \param thresh The float energy threshold value -- segments are only 
  included in the sum if the waveform calculated energy is above this threshold.
  \return Returns the float value of the segment sum energy for the crystal,
  which in this case is the sum of segments with valid waveform calculated energies,
  which were above the input threshold.
*/

Float_t g3CrystalEvent::segSumCalcThresh(Float_t thresh) {
  Float_t segSum = 0.;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].segNum < 36 && chn[ui].eCalc < thresh) {
      segSum += chn[ui].eCalc;
    }
  }
  return segSum;
}

/**************************************************************/

Int_t g3CrystalEvent::segsHitCalc(Float_t thresh) {
  Int_t hit = 0;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].eCalc > thresh && chn[ui].segNum < 36) {
      hit++;
    }
  }
  return hit;
}

/**************************************************************/

/*! Determines the maximum segment energy in the event, returning that energy value.

  \return Returns the energy (from the trapezoidal filter output) of
  the segment with the maximum energy in the crystal.
*/

Float_t g3CrystalEvent::maxSegE() {
  Float_t maxE = 0.;  Int_t max = -1;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].eCal > maxE && chn[ui].segNum < 36) {
      max = ui;
      maxE = chn[ui].eCal;
    }
  }
  return maxE;
}

/**************************************************************/

/*! Determines the maximum segment energy in the event, based
  on the energies calculated from the waveform analysis, returning that energy value.

  \return Returns the energy (from the waveform energy filter analysis) of
  the segment with the maximum energy in the crystal.
*/

Float_t g3CrystalEvent::maxSegECalc() {
  Float_t maxE = 0.;  Int_t max = -1;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].eCalc > maxE && chn[ui].segNum < 36) {
      max = ui;
      maxE = chn[ui].eCalc;
    }
  }
  return maxE;
}

/**************************************************************/

/*! Determines the segment number of the segment with the maximum energy 
  deposition, returning the segment number (from 0-35).  Based on FPGA calculated energies.

  \return Returns the segment number (0-35) of the segment with the largest 
  energy deposition in the crystal.  
*/

Int_t g3CrystalEvent::maxSegNum() {
  Float_t maxE = 0.;  Int_t max = -1;
 
  for (Int_t u=0; u<chn.size(); u++) {
    if (chn[u].eCal > maxE && chn[u].segNum < 36) {
      max = u;
      maxE = chn[u].eCal;
    }
  }

  return chn[max].segNum;
}

/**************************************************************/

/*! Determines the segment number of the segment with the second highest
  energy deposition, returning the segment number (0-35).  Based on FPGA calculated
  energies.

  \return Returns the segment number (0-35) of the segment with the second largest
  energy deposition in the crystal.
*/

Int_t g3CrystalEvent::secondSegNum() {
  Float_t maxE = 0.;  Int_t max = -1;
  
  for (Int_t u=0; u<chn.size(); u++) {
    if (chn[u].eCal > maxE && chn[u].segNum < 36 && chn[u].eCal != maxSegE()) {
      max = u;
      maxE = chn[u].eCal;
    }
  }

  return chn[max].segNum;
}

/**************************************************************/

/*! Determines the segment number of the segment with the maximum energy 
  deposition, returning the segment number (from 0-35).  Based on waveform calculated energies.

  \return Returns the segment number (0-35) of the segment with the largest 
  energy deposition in the crystal (based on waveform energy filter analysis).  
*/

Int_t g3CrystalEvent::maxSegNumCalc() {
  Float_t maxE = 0.;  UInt_t max = -1;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].eCalc > maxE && chn[ui].segNum < 36) {
      max = ui;
      maxE = chn[ui].eCalc;
    }
  }
  return chn[max].segNum;
}

/**************************************************************/

/*! Determines the deepest layer in the crystal that saw a net energy
  deposition in the event.  Loops over all segments, and keeps track of
  the layers hit; returns the deepest layer in which a segment was hit.

  \param thresh The float value for the energy threshold above which segments
  are considered to have been hit.
  \return Returns the integer value from 1-6 of the deepest layer with a segment
  fired above threshold.
*/

Int_t g3CrystalEvent::deepRingHit(Float_t thresh) {
  Int_t deep = -1;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (chn[ui].segNum < 6 && chn[ui].eCal > thresh && deep < 1) {
      deep = 1; 
    } else if (chn[ui].segNum < 12 && chn[ui].segNum > 5 && 
	       chn[ui].eCal > thresh && deep < 2) {
      deep = 2;
    } else if (chn[ui].segNum < 18 && chn[ui].segNum > 11 && 
	       chn[ui].eCal > thresh && deep < 3) {
      deep = 3;
    } else if (chn[ui].segNum < 24 && chn[ui].segNum > 17 && 
	       chn[ui].eCal > thresh && deep < 4) {
      deep = 4;
    } else if (chn[ui].segNum < 30 && chn[ui].segNum > 23 && 
	       chn[ui].eCal > thresh && deep < 5) {
      deep = 5;
    } else if (chn[ui].segNum < 36 && chn[ui].segNum > 29 && 
	       chn[ui].eCal > thresh && deep < 1) {
      deep = 6;
    }
  }
  return deep;
}

/**************************************************************/

/*! Searches for the smallest LED timestamp value of the channels in 
  the crystal event.

  \return Returns the long long int timestamp value corresponding to
  the earliest TS recorded for a channel event in the crystal. 
*/

long long int g3CrystalEvent::LEDLow() {
  long long int ledLow = -1;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (ledLow == -1) { ledLow = chn[ui].timestamp; }
    if (ledLow > chn[ui].timestamp) {
      ledLow = chn[ui].timestamp;
    }
  }
  return ledLow;
}

/**************************************************************/

/*! Searches for the largest LED timestamp value of the channels in 
  the crystal event.

  \return Returns the long long int timestamp value corresponding to
  the latest TS recorded for a channel event in the crystal. 
*/

long long int g3CrystalEvent::LEDHigh() {
  long long int ledHigh = 0;
  for (UInt_t ui=0; ui<chn.size(); ui++) {
    if (ledHigh < chn[ui].timestamp) {
      ledHigh = chn[ui].timestamp;
    }
  }
  return ledHigh;
}

/**************************************************************/

/*! Function returns the number of channel events for the crystal in the 
  current physics event.  'crystalBuild' refers to the fact that we expect
  40 channel events for a properly 'built' crystal event.

  \return Returns the unsigned integer value of the number of individual channel
  events included in the crystal event.
*/

UInt_t g3CrystalEvent::crystalBuild() { return chn.size(); }

/**************************************************************/

/*! Function calculates the difference between the high and low 
    LED timestamps for channel events within the mode3 crystal event,
    essentially the 'width' of the event for the crystal

    \return Returns the difference between the first and last LED
            timestamps as a long long int
*/

long long int g3CrystalEvent::LEDRange() {
  return (LEDHigh() - LEDLow());
}

/**************************************************************/
/* g3OUT Class Functions **************************************/
/**************************************************************/

/*! Resets the mode3 output class -- clears channel vector

    \return No return value -- clears data structures in class
            directly
*/

void g3OUT::Reset() {
  UInt_t crystals = crystalMult();
  for (UInt_t ui=0; ui<crystals; ui++) {
    xtals[ui].chn.clear();
  }
  xtals.clear();
}

/**************************************************************/

/*! Extracts the number of crystals hit within a mode3 event, namely
    the size of the xtals vector

    \return Returns the unsigned integer value of the size of the
            xtals array
*/

UInt_t g3OUT::crystalMult() { return xtals.size(); }

/**************************************************************/

/*! Calculates a sum energy for the array (calorimetric) energy, based
    on the central contact (CC1) energies reported by each crystal.

    \return Returns the sum of CC1 energies as the calorimeterE.
*/

Float_t g3OUT::calorimeterE() {
  Float_t sum = 0.;
  for (UInt_t ui=0; ui<crystalMult(); ui++) {
    sum += xtals[ui].cc1();
  }
  return sum;
}

