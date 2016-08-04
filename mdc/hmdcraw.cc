//*-- Author : Ilse Koenig
//*-- Modified: May 05, 2004 by Peter Zumbruch
//*-- Modified: March 11, 2005 by Y.C.Pachmayer

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HMdcRaw
//
// Unpacked raw data of the MDC
//
// In measurement mode the TDC accepts two hits per channel. The TDC can hereby
// trigger either on two leading edges (hit multiplicity nHits: -1 or -2) or
// on a leading and the trailing edge of a single pulse (nHits: +1 or +2).
// The TDC is able to generate internal calibration events. In this case a
// single channel sends 6 time informations (nHits: +6).
//
// The addresses of the cell can be accessed via the inline functions
//     void setSector(const Int_t n)
//     void setModule(const Int_t n)
//     void setMbo(const Int_t n)
//     void setTdc(const Int_t n)
//     void setAddress(const Int_t sector,const Int_t module,const Int_t mbo,
//                     const Int_t tdc)
//     Int_t getSector(void) const
//     Int_t getModule(void) const
//     Int_t getMbo(void) const
//     Int_t getTdc(void) const
//     void getAddress(Int_t& sector,Int_t& module,Int_t& mbo,Int_t& tdc)
// and the number of hits with the inline function
//     Int_t getNHits(void) const
//
// The inline function clear() sets the data data members to the following
// values:
//          nHits=0;
//          sector=module=mbo=tdc=-1;
//          time1=time2=time3=time4=time5=time6=-999;
//
///////////////////////////////////////////////////////////////////////////////

#include "hmdcraw.h"
#include "hades.h"

ClassImp(HMdcRaw)

  const Int_t HMdcRaw::kDefaultValueTime   = -999;
  const Int_t HMdcRaw::kDefaultValueSector = -1;
  const Int_t HMdcRaw::kDefaultValueModule = -1;
  const Int_t HMdcRaw::kDefaultValueMbo    = -1;
  const Int_t HMdcRaw::kDefaultValueTdc    = -1;

Bool_t HMdcRaw::setTime(const Int_t time,const Int_t mode, const Bool_t noComment)
{
  // Stores the given time in the next data element time* and sets the
  // multiplicity.
  // The TDC can be operated in 2 different modes:
  // mode 0 (default) : trigger on leading and trailing edge
  //                    The multiplicity nHits is incremented by 1.
  // mode 1           : trigger on 2 leading edges:
  //                    The multiplicity nHits is decremented by 1.
  //
  // at maximum one element can only hold 6 times
  // If you try to fill a 7th time kFALSE is returned
  // If gHades exists,
  //    then in addition it is checked in the case of REAL events
  //    whether the number of hits exceeds 2. Also in this case kFALSE is returned
  //    and the time information is not filled,
  //    but the number of hits are increased, so that nHits represents the trials to fill in a time

  if (gHades)
    {
      if (gHades->isReal())
	{
	  if (abs(nHits) >= 2)
	    {
	      if(nHits>0)
	      {
		  nTrialsToFillHits++;
	      }
	      else
	      {
		  nTrialsToFillHits--;
	      }
	      if (!noComment)
		{
		  Warning("setTime()","number of hits for REAL events: %i > 2, datum not filled",abs(nTrialsToFillHits));
		}
	      return kFALSE;
	    }
	}
    }

  switch (nHits)
    {
    case 0:
      time1 = time;
      if (mode)
        { nHits--; nTrialsToFillHits--;}
      else
        { nHits++; nTrialsToFillHits++;}
      break;
    case -1:
      time2 = time;
      nHits--;
      nTrialsToFillHits--;
      break;
      //     case -2:
      //       time3 = time;
      //       nHits--;
      //       break;
    case 1:
      time2 = time;
      nHits++;
      nTrialsToFillHits++;
      break;
    case 2:
      time3 = time;
      nHits++;
      nTrialsToFillHits++;
      break;
    case 3:
      time4 = time;
      nHits++;
      nTrialsToFillHits++;
      break;
    case 4:
      time5 = time;
      nHits++;
      nTrialsToFillHits++;
      break;
    case 5:
      time6 = time;
      nHits++;
      nTrialsToFillHits++;
      break;
    default:
      if (nHits < 0 && !noComment)
      {
          nTrialsToFillHits--;
          Warning("setTime()",
		  "number of hits for REAL events: %i > 2, datum not filled",
		  abs(nTrialsToFillHits));
      }
      if (nHits > 0 && !noComment)
      {
          nTrialsToFillHits++;
	  Warning("setTime()",
		  "number of hits for CALIBRATION events: %i > 6, datum not filled",
		  nTrialsToFillHits);
      }
      return kFALSE;
    }
  return kTRUE;
}

Int_t HMdcRaw::getTime(const Int_t n) const {
  // Returns the time of nth hit (1<=n<=6)
  switch (n) {
    case 1: return time1;
    case 2: return time2;
    case 3: return time3;
    case 4: return time4;
    case 5: return time5;
    case 6: return time6;
    default:
      Error("getTime()","time number: %i out of range [1,6]",n);
      return -1;
  }
}


Bool_t HMdcRaw::setTimeNew(const Int_t time, const Int_t nrtime)
{
  // sets new time value: setTimeNew(timevalue, timeindex)
  // only possible if abs(nHits)>=nrtime
  // return kTRUE if everything is fine, else kFALSE if the number of times is not ok
  if(abs(nHits)>=nrtime)
    {
      switch (nrtime)
	{
	case 1:
	  time1=time;
	  break;
	case 2:
	  time2=time;
	  break;
	case 3:
	  time3=time;
	  break;
	case 4:
	  time4=time;
	  break;
	case 5:
	  time5=time;
	  break;
	case 6:
	  time6=time;
	  break;
	default:
	  {
	    Error("setTimeNew()","number of times not ok, because: %i !=casevalue", nrtime);
	    return kFALSE;
	  }
	}
    }
  else
    {
      Error("setTimeNew()","number of times %i exceeds allowed nHits %i", nrtime,nHits);
      return kFALSE;
    }
  return kTRUE;
}
