//*-- Author : J.Wuestenfeld
//*-- Modified : 11/19/2003 by J.Wuestenfeld

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////
//HldRfioEvent
//
//  Class used for reading HLD events from file on taperobot
//
/////////////////////////////////////////////////////
using namespace std;
#include "hldrfiofilevt.h"
#include "hldsubevt.h"
#include "hldunpack.h"

ClassImp(HldRFIOFilEvt)

Bool_t HldRFIOFilEvt::setFile(const Text_t *name)
{
	// sets the name of a new file
	if(file)
		delete file;
  file=new HRFIOFile(name,ios::in);
	if(file->good())
		return kTRUE;
	else
		{
			delete file;
			file=NULL;
			return kFALSE;
		}
}

Bool_t HldRFIOFilEvt::read(void)
{
	// reads a event from the file
  if(file)
		{
			if (file->eof())
				{
					return kFALSE;
				}
			if (pData)
				{
					delete[] pData;
					pData = 0;
				}
			if (file->read((Char_t *)(pHdr), getHdrSize()))
				{
					if (isSwapped()) swapHdr();
					if (getSize() > getHdrSize())
						{
#warning "Hardcoded maximum event size is 1000000"
							if ((getDataLen() > 1000000) && (getDataLen() == 0)) return kFALSE;
							pData = new UInt4[getDataLen()];
							if (file->read((Char_t*)(pData), getDataSize()))
								{
									if(getPaddedSize() - getSize() > 0)
										file->ignore(getPaddedSize() - getSize());
								}
							else
								return kFALSE;
						}
				}
			else
				return kFALSE;
			return kTRUE;
		} 
  return kFALSE;
}


Bool_t HldRFIOFilEvt::readSubEvt(size_t i)
{
	// splits the event into subevents
  UInt4* p;
  if (i)
    p = subEvt[i-1].getPaddedEnd();
	else
    p = pData;
  if (p < getPaddedEnd())
    subEvt[i] = HldSubEvt(p);
  else
    return kFALSE;
  return kTRUE;
}
  


Bool_t HldRFIOFilEvt::execute()
{
  if (read())
		{
			for (size_t idx = 0; idx < lastSubEvtIdx; idx++) *subEvtTable[idx].p = 0;
			for (size_t i = 0; i < maxSubEvts /*&& readSubEvt(i)*/; i++) { // loop subevts
				if(!readSubEvt(i)) break;
				Bool_t unpacked=kFALSE;
				for (size_t idx = 0; idx < lastSubEvtIdx; idx++) { // loop unpackers
					if (subEvt[i].getId() == subEvtTable[idx].id) {
						subEvt[i].swapData();
						*subEvtTable[idx].p = subEvt + i;
						//        cout << "pass a pointer to subevt to its unpacker" << endl;
						unpacked=kTRUE;
					}
				}
				if (isWritable && !unpacked) subEvt[i].swapData();
			}
			return kTRUE;
		}
	else
		{
			return kFALSE;
		}
} 

Bool_t HldRFIOFilEvt::swap()
{
	//only swapping correctly the header  
  if (read())
		{
			return kTRUE;
		}
	else
		{
			return kFALSE;
		}
}
