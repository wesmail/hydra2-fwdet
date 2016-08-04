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
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHPADGRAPH_H
#define HRICHPADGRAPH_H

#include "TObject.h"


// ****************************************************************************

class HRichPadGraph: public TObject  {

// ****************************************************************************

private:

   Int_t iNodeListLen;
   Int_t iNodeNr;
   Int_t iLinkNodeNr;

public:

   Int_t iLabel;

   HRichPadGraph **pNodeList; //!

   HRichPadGraph();
   virtual ~HRichPadGraph();

   HRichPadGraph(const HRichPadGraph& source);
   HRichPadGraph& operator=(const HRichPadGraph& source);

   void InitGraph(Int_t nodenr, Int_t listlen);
   void AddNode(HRichPadGraph* nodeptr);
   Int_t MarkNodes(Int_t labelnr);

   ClassDef(HRichPadGraph, 1)
};

//============================================================================


#endif // HRICHPADGRAPH_H
