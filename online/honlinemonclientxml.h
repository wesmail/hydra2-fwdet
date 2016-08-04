#ifndef HONLINEMONCLIENTXML_H
#define HONLINEMONCLIENTXML_H

#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TNamed.h"
#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"

class HOnlineMonClientMain;
class HOnlineMonClientMainWin;
class HOnlineMonClientDet;
class HOnlineMonClientTabWin;
class HOnlineMonClientSimpleWin;
class HOnlineMonClientTab;
class HOnlineMonClientCanvas;
class HOnlineMonClientHist;

class HOnlineMonClientXML : public TNamed {
public:
	HOnlineMonClientXML();
	~HOnlineMonClientXML();
	Bool_t ParseXMLFile(TString filename, HOnlineMonClientMain* pclientmain);

private:
	HOnlineMonClientMain* clientmain;
	HOnlineMonClientDet* currentDetector; //!
	void ParseConfigNode(TXMLNode *node);
	void ParseServerNode(TXMLNode *node);
	void ParseMainWindowNode(TXMLNode *node, HOnlineMonClientMainWin *clientmainwin);
	void ParseDetectorNode(TXMLNode *node, HOnlineMonClientDet *detector);
	void ParseWindowNode(TXMLNode *node, HOnlineMonClientDet *detector);
	void ParseTabNode(TXMLNode *node, HOnlineMonClientTabWin *window);
	void ParseCanvasNode(TXMLNode *node, HOnlineMonClientSimpleWin *window);
	void ParseCanvasNode(TXMLNode *node, HOnlineMonClientTabWin *window, HOnlineMonClientTab *montab);
	void ParseHistogramNode(TXMLNode *node, HOnlineMonClientCanvas *canvas);
	ClassDef(HOnlineMonClientXML, 1);
};

#endif

