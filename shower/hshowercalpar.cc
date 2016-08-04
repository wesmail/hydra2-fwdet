#pragma implementation
using namespace std;
#include <iostream>
#include <iomanip>
#include "hades.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hspectrometer.h"
#include "hshowerdetector.h"
#include "hshowercalpar.h"
#include "hshowercalparcell.h"

#include "TH2.h"

ClassImp(HShowerCalPar)

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
// HShowerCalPar
//
// HShowerCalPar is used to manage of all parameters for calibration
// This class contains structure HObjTable, which contains all
// HShowerCalParCell objects. One HShowerCalParCell object contains
// calibration parameters of one pad: slope and offset
//
//////////////////////////////////////////////////////////////////////

HShowerCalPar::HShowerCalPar(const Char_t* name,const Char_t* title,
                             const Char_t* context)
              : HParSet(name,title,context) {
  strcpy(detName,"Shower");
  setSetup(6, 3, 32, 32); //default setup
  setCellClassName("HShowerCalParCell");
  m_loc.setNIndex(4);
  defaultInit();
}

HShowerCalPar::~HShowerCalPar()
{
  m_ParamsTable.deleteTab();
}

void HShowerCalPar::setSetup(Int_t nSectors, Int_t nModules,
   Int_t  nRows, Int_t nColumns)
{
//set sizes of internal structure
  m_nSectors = nSectors;
  m_nModules = nModules;
  m_nRows = nRows;
  m_nColumns = nColumns;
}

void HShowerCalPar::setCellClassName(const Char_t* pszName)
//set name of class of calibration parameter container for one pad
{
  strncpy(m_szClassName, pszName, sizeof(m_szClassName));
}

Char_t* HShowerCalPar::getCellClassName()
{
//return  name of class of calibration parameter container for one pad

  return m_szClassName;
}


HShowerCalParCell*& HShowerCalPar::getSlot(HLocation &loc)
{
  //return references address to place new object at loc position
  return (HShowerCalParCell*&) m_ParamsTable.getSlot(loc);
}

HShowerCalParCell* HShowerCalPar::getObject(HLocation &loc)
{
  //return pointer to object, which contains parameter for pad
  //at loc position
  return (HShowerCalParCell*) m_ParamsTable.getObject(loc);
}

Float_t HShowerCalPar::getSlope(HLocation &loc)
{
  //return slope for the pad at postion loc
  return getObject(loc)->getSlope();
}

Float_t HShowerCalPar::getOffset(HLocation &loc)
{
  //return offset for the pad at postion loc
  return getObject(loc)->getOffset();
}

Float_t HShowerCalPar::getThreshold(HLocation &loc)
{
  //return Threshold for the pad at postion loc
  return getObject(loc)->getThreshold();
}

Float_t HShowerCalPar::getGain(HLocation &loc)
{
  //return Gain for the pad at postion loc
  return getObject(loc)->getGain();
}

void HShowerCalPar::setSlope(HLocation &loc, Float_t fSlope)
{
  //set slope for the pad at postion loc
  getObject(loc)->setSlope(fSlope);
}

void HShowerCalPar::setOffset(HLocation &loc, Float_t fOffset)
{
  //set offset for the pad at postion loc
  getObject(loc)->setOffset(fOffset);
}

void HShowerCalPar::setThreshold(HLocation &loc, Float_t fThreshold)
{
  //set Threshold for the pad at postion loc
  getObject(loc)->setThreshold(fThreshold);
}

void HShowerCalPar::setGain(HLocation &loc, Float_t fGain)
{
  //set Gain for the pad at postion loc
  getObject(loc)->setGain(fGain);
}

Bool_t HShowerCalPar::defaultInit()
{
  //initialization of calibration parameters for full setup
  //of pre-SHOWER detector. Default calibration using
  //values of slope==1 and offset==0
  Int_t sect, mod, row, col;
  HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                                  ->getDetector("Shower");
  if (!pShowerDet) return kFALSE;

  m_nSectors = pShowerDet->getShowerSectors();
  m_nModules = pShowerDet->getShowerModules();
  m_nRows = pShowerDet->getRows();
  m_nColumns = pShowerDet->getColumns();

  //  printf("%d - %d - %d - %d\n", m_nSectors, m_nModules, m_nRows, m_nColumns);
  m_ParamsTable.set(4, m_nSectors, m_nModules, m_nRows, m_nColumns);

  m_ParamsTable.setCellClassName(getCellClassName());
  m_ParamsTable.makeObjTable();

  HShowerCalParCell *pCell;

  for(sect = 0; sect < m_nSectors; sect++)
    for(mod = 0; mod < m_nModules; mod++)
      for(row = 0; row < m_nRows; row++)
         for(col = 0; col < m_nColumns; col++) {
           m_loc[0] = sect;
           m_loc[1] = mod;
           m_loc[2] = row;
           m_loc[3] = col;

           pCell = (HShowerCalParCell*) getSlot(m_loc);
           if (pCell) {
                 pCell = new(pCell) HShowerCalParCell;
                 pCell->setSlope(1.0);
                 pCell->setOffset(0.0);
           }

            //printf("XX : ");
           //pCell->print();
         }

  //  printf("Shower calpar default initialization\n");
  return kTRUE;
}

Bool_t HShowerCalPar::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HShowerParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HShowerCalPar::write(HParIo* output) {
  // writes the shower container to the output
  HDetParIo* out=output->getDetParIo("HShowerParIo");
  if (out) return out->write(this);
  return kFALSE;
}

Bool_t HShowerCalPar::readline(const Char_t* buf) {
// Decodes one line read from ASCII file I/O and fills the cells
  Int_t sec=0, mod=0, row=0, col=0;
  Float_t offset=0., slope=0.,threshold=0.,gain=0.;
  sscanf(buf,"%i%i%i%i%f%f%f%f",&sec,&mod,&row,&col,&slope,&offset,&threshold,&gain);
  m_loc[0] = sec;
  m_loc[1] = mod;
  m_loc[2] = row;
  m_loc[3] = col;
  HShowerCalParCell* pCell = (HShowerCalParCell*) getSlot(m_loc);
  if (pCell) {
    pCell = new(pCell) HShowerCalParCell;
    pCell->setSlope(slope);
    pCell->setThreshold(threshold);
    pCell->setOffset(offset);
    pCell->setGain(gain);
    return kTRUE;
  } else {
    Error("readline(const Char_t*)","No slot found: %i %i %i %i",sec,mod,row,col);
    return kFALSE;
  }
}

void HShowerCalPar::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HShowerParAsciiFileIo
  header=
    "# Calibration parameters of the Shower\n"
    "# Format:\n"
    "# sector  module  row  column  slope  offset  threshold  gain\n";
}

void HShowerCalPar::write(fstream& fout) {
  // writes the calibration parameters to an ASCII file
  for(Int_t sec = 0; sec < m_nSectors; sec++) {
    m_loc[0] = sec;
    for(Int_t mod = 0; mod < m_nModules; mod++) {
      m_loc[1] = mod;
      for(Int_t row = 0; row < m_nRows; row++) {
        m_loc[2] = row;
        for(Int_t col = 0; col < m_nColumns; col++) {
          m_loc[3] = col;
          fout<<setw(3)<<sec<<setw(3)<<mod
              <<setw(4)<<right<<row<<setw(4)<<right<<col
              <<" "<<setw(10)<<setprecision(6)<<getSlope(m_loc)
              <<" "<<setw(10)<<setprecision(5)<<getOffset(m_loc)
              <<" "<<setw(10)<<setprecision(5)<<getThreshold(m_loc)
              <<" "<<setw(10)<<setprecision(6)<<getGain(m_loc)<<endl;
        }
      }
    }
  }
}

void HShowerCalPar::printParams() {
  Int_t i, j, k, l;
  Float_t slope, offset,threshold,gain;
  for (i = 0; i < m_nSectors; i++) {
    for (j = 0; j < m_nModules; j++) {
      for (k = 0; k < m_nRows; k++) {
        for (l = 0; l < m_nColumns; l++) {
          m_loc[0] = i;
          m_loc[1] = j;
          m_loc[2] = k;
          m_loc[3] = l;
          slope = getSlope(m_loc);
          offset = getOffset(m_loc);
          threshold = getThreshold(m_loc);
          gain = getGain(m_loc);
          if (offset < 0)
          printf("%d:%d:%d:%d %f %f %f %f\n", i, j, k, l, slope, offset,threshold,gain);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------

void HShowerCalPar::changeValues(Int_t iVar, Int_t iOpr, Float_t f)
{
Int_t                iSec, iMod, iRow, iCol;
Float_t            fVals[4];
HShowerCalParCell *pCell;

    for(iSec = 0; iSec < m_nSectors; iSec++)
    {
        m_loc[0] = iSec;

        for(iMod = 0; iMod < m_nModules; iMod++)
        {
            m_loc[1] = iMod;

            for(iRow = 0; iRow < m_nRows; iRow++)
            {
                m_loc[2] = iRow;

                for(iCol = 0; iCol < m_nColumns; iCol++)
                {
                    m_loc[3] = iCol;

                    if((pCell = (HShowerCalParCell *) getSlot(m_loc)) == NULL)
                        continue;

                    fVals[HSCPV_SLOPE]     = pCell->getSlope();
                    fVals[HSCPV_OFFSET]    = pCell->getOffset();
                    fVals[HSCPV_THRESHOLD] = pCell->getThreshold();
                    fVals[HSCPV_GAIN]      = pCell->getGain();

                    switch(iOpr)
                    {
                        case HSCPM_SET:
                            fVals[iVar] = f;
                            break;

                        case HSCPM_ADD:
                            fVals[iVar] += f;
                            break;

                        default:
                            fVals[iVar] *= f;
                            break;
                    }

                    pCell->setParams(fVals[HSCPV_SLOPE], fVals[HSCPV_OFFSET],
                                fVals[HSCPV_THRESHOLD], fVals[HSCPV_GAIN]);
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

TH2F* HShowerCalPar::getHist(Int_t iSector, Int_t iModule,
                enum_HSCPM_Var enType)
{
// Create a new histogram with a calibartion values for the selected chamber.
// User must delete the histogram himself

    // check parameters
    if((iSector < 0) || (iSector >= m_nSectors))
    {
        Error("getHist", "Sector %d out of bounds [0, %d]",
                    iSector, m_nSectors - 1);

        return NULL;
    }

    if((iModule < 0) || (iModule >= m_nModules))
    {
        Error("getHist", "Module %d out of bounds [0, %d]",
                    iModule, m_nModules - 1);

        return NULL;
    }

    if(m_nRows <= 0)
    {
        Error("getHist", "m_nRows = %d", m_nRows);
        return NULL;
    }

    if(m_nColumns <= 0)
    {
        Error("getHist", "m_nColumns = %d", m_nColumns);
        return NULL;
    }

TH2F *h;
Char_t  sT[100];
Char_t  sN[100];

    // prepare the names
    strcpy(sT, "Shower calibration params: ");
    strcpy(sN, "hShwCal_");
    switch(enType)
    {
        case HSCPV_SLOPE:
            strcat(sT, "slopes");
            strcat(sN, "slope");
            break;

        case HSCPV_OFFSET:
            strcat(sT, "offsets");
            strcat(sN, "off");
            break;

        case HSCPV_THRESHOLD:
            strcat(sT, "thresholds");
            strcat(sN, "thr");
            break;

        case HSCPV_GAIN:
            strcat(sT, "gains");
            strcat(sN, "gain");
            break;

        default:
            Error("getHist", "Wrong option enType=%d", enType);
            return NULL;
    }

    sprintf(sT + strlen(sT), " sec: %d mod: %d", iSector, iModule);
    sprintf(sN + strlen(sN), "_%d_%d", iSector, iModule);

    // create histogram
    if((h = new TH2F(sN, sT, m_nColumns, 0, m_nColumns, m_nRows, 0, m_nRows))
            == NULL)
    {
        Error("getHist", "Cannot create histogram");
        return NULL;
    }

    // fill histogram
HLocation hLoc;
Float_t   f;

    hLoc.setNIndex(4);
    hLoc[0] = iSector;
    hLoc[1] = iModule;

    for(hLoc[2] = 0; hLoc[2] < m_nRows; hLoc[2]++)
    {
        for(hLoc[3] = 0; hLoc[3] < m_nColumns; hLoc[3]++)
        {
            switch(enType)
            {
                case HSCPV_SLOPE:
                    f = getSlope(hLoc);
                    break;

                case HSCPV_OFFSET:
                    f = getOffset(hLoc);
                    break;

                case HSCPV_THRESHOLD:
                    f = getThreshold(hLoc);
                    break;

                default:
                    f = getGain(hLoc);
                    break;
            }

            h->Fill(hLoc[3], hLoc[2], f);
        }
    }

    return h;
}
