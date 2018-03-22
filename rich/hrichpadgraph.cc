//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichPadGraph
//
//  This class contains graph definition used in pad labeling.
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichpadgraph.h"

#include "TBuffer.h"

#include <iomanip>
#include <iostream>

using namespace std;

ClassImp(HRichPadGraph)

//----------------------------------------------------------------------------
HRichPadGraph::HRichPadGraph()
{
   iNodeListLen = 0;
   iNodeNr = 0;
   iLinkNodeNr = 0;
   iLabel = 0;
   pNodeList = NULL;
}
//============================================================================

//----------------------------------------------------------------------------
HRichPadGraph::~HRichPadGraph()
{
   if (pNodeList) {
      delete [] pNodeList;
      pNodeList = NULL;
   }
}
//============================================================================

//----------------------------------------------------------------------------
HRichPadGraph::HRichPadGraph(const HRichPadGraph& source):TObject(source)
{
   iNodeListLen = source.iNodeListLen;
   iNodeNr = source.iNodeNr;
   iLinkNodeNr = source.iLinkNodeNr;
   iLabel = source.iLabel;
   if (pNodeList) delete [] pNodeList;
   if (source.pNodeList) {
      pNodeList = new HRichPadGraph*[iNodeListLen];
      for (Int_t i = 0; i < iNodeListLen; i++) pNodeList[i] = source.pNodeList[i];
   } else pNodeList = NULL;
}
//============================================================================

//----------------------------------------------------------------------------
HRichPadGraph& HRichPadGraph::operator=(const HRichPadGraph& source)
{
   if (this != &source) {
      iNodeNr = source.iNodeNr;
      iLinkNodeNr = source.iLinkNodeNr;
      iLabel = source.iLabel;
      if (source.pNodeList == NULL) {
         if (pNodeList) delete [] pNodeList;
         pNodeList = NULL;
      } else if (iNodeListLen == source.iNodeListLen) {
         for (Int_t i = 0; i < iNodeListLen; i++) pNodeList[i] = source.pNodeList[i];
      } else if (iNodeListLen == 0) {
         pNodeList = new HRichPadGraph*[source.iNodeListLen];
         for (Int_t i = 0; i < source.iNodeListLen; i++) pNodeList[i] = source.pNodeList[i];
      } else {
         delete [] pNodeList;
         pNodeList = new HRichPadGraph*[source.iNodeListLen];
         for (Int_t i = 0; i < source.iNodeListLen; i++) pNodeList[i] = source.pNodeList[i];
      }
      iNodeListLen = source.iNodeListLen;
   }

   return *this;
}
//============================================================================

//----------------------------------------------------------------------------
void HRichPadGraph::Streamer(TBuffer &R__b)
{
// Stream an object of class HRichPadGraph.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion();
      if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> iNodeListLen;
      R__b >> iNodeNr;
      R__b >> iLinkNodeNr;
      R__b >> iLabel;
      pNodeList = new HRichPadGraph*[iNodeListLen];
      R__b.ReadFastArray((Long_t *)pNodeList, iNodeListLen);
      //R__b.ReadArray(pNodeList);
   } else {
      R__b.WriteVersion(HRichPadGraph::IsA());
      TObject::Streamer(R__b);
      R__b << iNodeListLen;
      R__b << iNodeNr;
      R__b << iLinkNodeNr;
      R__b << iLabel;
      R__b.WriteFastArray((const Long_t *)pNodeList, iNodeListLen);
      //R__b.WriteArray(pNodeList, __COUNTER__);
   }
}
//============================================================================

//----------------------------------------------------------------------------
void HRichPadGraph::InitGraph(Int_t nodenr, Int_t listlen)
{
   iNodeNr = nodenr;
   iNodeListLen = listlen;
   iLinkNodeNr = 0;
   iLabel = 0;
   if (iNodeListLen > 0) pNodeList = new HRichPadGraph*[iNodeListLen];
}
//============================================================================

//----------------------------------------------------------------------------
void HRichPadGraph::AddNode(HRichPadGraph* nodeptr)
{
// warning: there is no control of array range
   pNodeList[iLinkNodeNr++] = nodeptr;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t HRichPadGraph::MarkNodes(Int_t labelnr)
{
   if (iLabel) return 0;
   iLabel = labelnr + 1;
   for (Int_t i = 0; i < iLinkNodeNr; i++) pNodeList[i]->MarkNodes(labelnr);

   return 1;
}
//============================================================================


