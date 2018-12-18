/*! \file GRETINA.h
    \brief Class definitions for GRETINA analysis.
*/

#ifndef __GRETINA_H
#define __GRETINA_H

#include "Riostream.h"
#include "Rtypes.h"

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TMath.h"
#include "TRandom1.h"
#include "TClass.h"

#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

using std::vector;
using std::cout;  using std::endl;
using std::swap;

/******** Structures to store raw data ********/

/*! \struct globalHeader
    \brief Global event builder header

    This struct is central to the Gretina framework, providing
    a global indicator of the data payload to follow. 
*/

struct globalHeader {
  Int_t type; /*!< Integer type value, indicating type of data to follow.  
		  Type values are assigned and documented at LBNL. */
  Int_t length; /*!< Integer length, in bytes, of the data payload to follow. */
  long long timestamp; /*!< 50MHz timestamp corresponding to the data to follow. */
};

/*--------------------------------------------------------*/
/*---         GRETINA MODE 3 - DATA STRUCTURES         ---*/
/*--------------------------------------------------------*/

/*! \class g3Waveform
    \brief Mode3 Gretina waveform class; holds waveforms for output, and contains
           functions for waveform analysis.
	 
   Class for mode3 waveform analysis; contains vectors to hold the raw and pole-zero
   corrected waveform values, as well as functions for a variety of timing and 
   energy algorithms relevant to waveform analysis as we've performed to date with 
   Gretina. 
*/

class g3Waveform : public TObject {

 public:
  vector<Short_t> raw; /*!< Vector containing the ADC values without any manipulation. */ 
  vector<Float_t> pz; /*!< Vector containing the waveform following pole-zero correction; 
			populated with the g3Waveform::calcPZ function */
  
 public:
  void Clear();
  Int_t LEDLevel(Int_t index, Int_t thresh, Int_t filterK);
  Int_t LED(Int_t index, Int_t thresh, Int_t filterK);
  Float_t CFD(Int_t startSample);
  Float_t BL(Int_t start, Int_t end);
  Int_t Look4Pileup();
  void calcPZ(Float_t base, Float_t tau);
  Float_t simpleE();
  Float_t riseTime(Float_t fLow, Float_t fHigh);
  Double_t computeChiSquare(Int_t startIndex, Int_t nPoints,
			    Double_t tau, Double_t nStart, Double_t baseline);
  Double_t LSFitExpo(Int_t startIndex, Int_t nPoints,
		     Double_t nStart, Double_t tau,
		     Double_t &nStartFit, Double_t &baselineFit,
		     Double_t &nStartFitError,
		     Double_t &baselineFitError);
  Double_t LSFitLinear(Int_t startIndex, Int_t nPoints,
		       Double_t &slopeFit, Double_t &offsetFit);
  Float_t baseline2Flatten(Float_t tau, Int_t startIndexBase,
			   Int_t lengthBase, Int_t startIndexTop,
			   Int_t lengthTop, Float_t smallStep);
  
  ClassDef(g3Waveform, 1);
};

/*! \class g3ChannelEvent
    \brief Single channel event class for mode3 (raw) Gretina data.  

    Class for a single electronics channel of data from a Gretina digitizer; typically 
    we think of these in terms of Gretina as mode3 single segment event structures.
    This class contains the basic information for a single channel. 
*/

class g3ChannelEvent : public TObject {

 public:
  UShort_t hdr0; /*!< Value of original mode3DataPacket hdr[0]; kept this way because this value
		   can be decoded to multiple relevant variables. */
  UShort_t hdr1; /*!< Value of original mode3DataPacket hdr[1]; kept this way because this value
		   can be decoded to multiple relevant variables. */
  UShort_t hdr7; /*!< Value of original mode3DataPacket hdr[7]; kept this way because this value
		   can be decoded to multiple relevant variables. */
  Int_t ID; /*!< Global ID value corresponding (in principle) to the electronics channel number.  
	      Beware though, this requires correct input of quad hole numbers and electronics 
	      ordering in the settings file "gretina.set". */
  Int_t segNum; /*!< Segment number from 0-35; numbering is from front layer to back layer, 
		  with 6 segments / layer. */
  Float_t eRaw; /*!< Raw filter energy value (already divided by 32. from raw filter value). */
  Float_t eCal; /*!< Calibrated filter energy value. */
  /* These are for event history, available in firmware 2.00_006f and beyond. */
  Float_t eCalPO, prevE1, prevE2;
  UShort_t deltaT1, deltaT2;
  UShort_t PZrollover;

  Float_t eCalc; /*!< Calculated energy value from waveform; not always populated. */
  Float_t calcTime; /*!< Calculated LED or CFD crossing time from waveform; details of source
		      of value depend on analysis performed; not always populated */
  long long int timestamp; /*!< Timestamp (100MHz) of event */ 
  long long int CFDtimestamp; /*!< CFD time from digitizer; NOT IMPLEMENTED */
  Float_t baseline; /*!< Baseline value from trace (average over some number of samples); 
		      not always populated. */
  Float_t riseTime; /*!< Risetime of pulse, in waveform samples; not always populated */
  g3Waveform wf;

 public:
  Int_t module() { return (hdr1 >> 4); } /*!< Extracts module number from header data.
					   \return Returns integer module number; value is 
					   defined based on settings in Gretina DAQ when 
					   data was taken. */
  Int_t boardID() { return (hdr0 >> 11); } /*!< Extracts boardID (3-6) from header data.
					     \return Returns integer boardID; value is 
					     3, 4, 5 or 6; offset from 1 is due to physical 
					     presence of IOC and a gap in electronics crates. */
  Int_t chanID() { return (hdr1 & 0xf); } /*!< Extracts channelID (0-9) from header data.
					    \return Returns integer channelID; value is 
					    from 0-9, corresponding to 10 channels / digitizer. */
  Int_t tracelength() { return (((hdr0 & 0x7ff)*2) - 14); } /*!< Extracts number of samples in trace
							      from header data.
							      \return Returns integer number of 
							      trace (waveform) samples recorded; 
							      minimum should be 6. */
  Int_t chnNum() { return (ID%40); } /*!< Determines crystal channel number which ranges from 
				       0 - 39; this is just the modulus of the global ID number 
				       with the number of channels per crystal (40).
				       \return Returns the integer channelID number (0-39). */
  Bool_t sign() { return (hdr7 & 0x0100); } /*!< Determines if there is a sign change needed in
					      the waveform from the header data. 
					      \return Returns 0 if no sign flip; 1 if there is */
  Int_t pileUp() { return ((hdr7 & 0x8000) >> 15); }    /*!< Determines the value of the digitizer 
							  pileup flag from the header data.
							  \return Returns 0 if no pileup flag; 
							  1 if there is a pileup flag. */

 public:

  ClassDef(g3ChannelEvent, 3);
};

/*! \class g3CrystalEvent
    \brief Single crystal event class for mode3 (raw) Gretina data; includes crystal-level
    information and a vector of g3ChannelEvent objects, corresponding to the individual segments.  

    Class for a full crystal event in mode3 data; includes a vector of individual channel events 
    (g3ChannelEvent objects), and basic information which is valid for the total crystal.
*/

class g3CrystalEvent : public TObject {
 
 public:
  Int_t crystalNum; /*!< Crystal number, in the 'global' numbering scheme; nominally based on the electronics ordering. */
  Int_t quadNum; /*!< Quad number; this is only a good value if the mapping input in "gretina.set" is good 
		   (for quad <--> hole and quad <--> electronics order). */
  Int_t module; /*!< Module number encodes both the geometric hole number (module / 16), and the crystal number (0-3)
		  within the quad module = (module/4)%4. */
  Int_t runNumber; /*!< The runNumber is filled based on the file being analyzed, if relevant. */
  Float_t dopplerSeg; /*!< This is the Doppler correction multiplier, based on the input beta, and the angle
		       from the target to the center of the segment with the highest energy deposition. */
  Float_t dopplerCrystal; /*!< This is the Doppler correction multiplier, based on the input beta, and the angle
			    from the target to the center of the front face of the crystal --> (0, 0, 0) in the crystal
			    coordinate frame */
  Int_t traceLength; /*!< The number of trace samples available in the waveforms; 
		       minimum value should be 6 (essentially no trace). */
  Float_t segSum; /*!< A cross-talk corrected segment sum value. */
  Float_t segSumTimed; /*!< A cross-talk corrected segment sum value, including only segments that satisfy
			 a waveform-based timing cut. */
  vector<g3ChannelEvent> chn; /*!< Vector of g3ChannelEvents containing the details for all channels with data in the crystal. */

 public:
  void OrderChannels();
  Float_t cc1();
  Float_t cc2();
  Float_t cc3();
  Float_t cc4();
  Float_t cc1Raw();
  Float_t cc2Raw();
  Float_t cc3Raw();
  Float_t cc4Raw();
  Float_t cc1Calc();
  Float_t cc2Calc();
  Float_t cc3Calc();
  Float_t cc4Calc();
  Float_t segSumRaw();
  Float_t segSumRawThresh(Float_t thresh);
  Float_t segSumRawTimed(Float_t minT, Float_t maxT);
  Float_t segSumRawThreshTimed(Float_t thresh, Float_t minT, Float_t maxT);
  Int_t segsHit(Float_t thresh);
  Float_t segSumCalc();
  Float_t segSumCalcThresh(Float_t thresh);
  Int_t segsHitCalc(Float_t thresh);
  Float_t maxSegE();
  Float_t maxSegECalc();
  Int_t maxSegNum();
  Int_t secondSegNum();
  Int_t maxSegNumCalc();
  Int_t deepRingHit(Float_t thresh);
  long long int LEDLow();
  long long int LEDHigh();
  UInt_t crystalBuild();
  long long int LEDRange();
  void mode3ToFile(FILE* outFile);

 private:
  ClassDef(g3CrystalEvent, 1);
};

/*! \class g3OUT
   \brief Output class for mode3 (raw) data.

   This class is the output class for mode3 Gretina data (raw data).  It holds a vector 
   of g3CrystalEvent structures, up to the observed crystal multiplicity of a physics
   event.  It also contains limited "array-wide" information.
*/

class g3OUT : public TObject {

 public:
  vector <g3CrystalEvent> xtals;

 public:
  g3OUT() { ; }
  ~g3OUT() { ; }
  void Reset();
  UInt_t crystalMult();
  Float_t calorimeterE();

 private:
  ClassDef(g3OUT, 1);
};

#endif
