#include "honlinemonclientxml.h"

#include "honlinemonclientmain.h"
#include "honlinemonclientmainwin.h"
#include "honlinemonclientdet.h"
#include "honlinemonclienttabwin.h"
#include "honlinemonclientsimplewin.h"
#include "honlinemonclienttab.h"
#include "honlinemonclientcanvas.h"
#include "honlinemonclienthist.h"

#include <cstdlib>

ClassImp(HOnlineMonClientXML);

/* -------------------------------------------------------------------------- */
HOnlineMonClientXML::HOnlineMonClientXML() {
}
/* -------------------------------------------------------------------------- */
HOnlineMonClientXML::~HOnlineMonClientXML() {
}
/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientXML::ParseXMLFile(TString filename, HOnlineMonClientMain* pclientmain) {
    // parse the xml config file given by filename
    // get the root node 'client' and search
    // three child nodes 'config', 'MainWindow' and 'detector'
    // and call parse functions
    clientmain = pclientmain;
    TDOMParser *parser = 0;
    TXMLDocument *xmldoc = 0;
    TXMLNode *rootnode = 0;
    TXMLNode *node = 0;
    Int_t parsecode = -1;
    currentDetector = 0;

    parser = new TDOMParser();
    parsecode = parser->ParseFile(filename);

    if (parsecode != 0) { // parsing of document failed
	Printf("XML ERROR: Parsing of XML file '%s' failed. Parse code error %d .", filename.Data(), parsecode);
	exit(1);
    }

    xmldoc = parser->GetXMLDocument();

    if (xmldoc == 0) { // parsing of document failed
	Printf("XML ERROR: parsing of xml file '%s' failed.", filename.Data());
	exit(1);
    }

    rootnode = xmldoc->GetRootNode(); // root node should be 'client' node

    if ( TString(rootnode->GetNodeName()) != "client") { // root node is NOT 'client', error&exit
	Printf("XML ERROR: xml root node is not 'client'.");
	exit(1);
    }

    node = rootnode->GetChildren();

    while (node->HasNextNode()) { // get the child nodes ('config','MainWindow','detector') under root node
	node = node->GetNextNode();
	if (node->GetNodeType() == TXMLNode::kXMLElementNode) {
	    if( TString(node->GetNodeName()) == "config") {
		ParseConfigNode(node);
	    }
	    if( TString(node->GetNodeName()) == "MainWindow") {
		ParseMainWindowNode(node, clientmain->clientmainwin);
	    }
	    if( TString(node->GetNodeName()) == "detector") {
		HOnlineMonClientDet *detector = new HOnlineMonClientDet();
		ParseDetectorNode(node, detector);
		clientmain->listDetectors.Add(detector);
	    }
	}
    }

    delete parser; // deletes the xmldocument, too
    parser = 0;

    return kTRUE;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientXML::ParseHistogramNode(TXMLNode *node, HOnlineMonClientCanvas *canvas) {
    HOnlineMonClientHist *histo = new HOnlineMonClientHist();
    histo->SetGuiName(currentDetector->GetName());

    TXMLNode *child = 0;
    histo->SetActive(kFALSE);

    if (!node->HasChildren()) {
	Printf("XML ERROR: parsing histogram node of canvas '%s' failed.", canvas->GetName() );
	exit(1);
    }

    child = node->GetChildren();

    while (child->HasNextNode()) {
	child = child->GetNextNode();
	if (child->GetNodeType() == TXMLNode::kXMLElementNode) {

	    if( TString(child->GetNodeName()) == "name") {
		// name node is last layer, get text inside tag
		TString histoname = child->GetText();
		// histoname can contain underscores, then isAlnum() returns false
		// if(!histoname.IsAlnum()) {
		if(histoname.Contains(' ') || histoname.Contains('-')) { // histogram name can contain underscores
		    Printf("XML ERROR: parsing histogram node of canvas '%s' failed. Invalid histogram name '%s'.", canvas->GetName(), histoname.Data() );
		    exit(1);
		}
		histo->SetRealName(histoname);
	    }

	    if( TString(child->GetNodeName()) == "type") {
		// title node is last layer, get text inside tag
		TString value = child->GetText();
		if (value == "single") {
		    histo->SetType("single");
		} else {
		    // parse text, delimeter is a ':'
		    // example: <type>array:1:2</type>
		    // indices = -1: draw alle histograms in array
		    TObjArray* ar = value.Tokenize(":");
		    TString p1 = ((TObjString*) ar->At(0))->GetString();
		    TString p2 = ((TObjString*) ar->At(1))->GetString();
		    TString p3 = ((TObjString*) ar->At(2))->GetString();
		    if ( (p1 == "array") && (p2.IsDigit() || (p2.CompareTo("-1")==0)) && (p3.IsDigit() || (p3.CompareTo("-1")==0)) ) {
			// check ok, values valid
			histo->SetArrayIndexS(p2.Atoi());
			histo->SetArrayIndexM(p3.Atoi());
			delete ar;
		    } else {
			Printf("XML ERROR: type of histogram '%s' in canvas '%s' is not valid. Must be 'single', or 'array:s:m' with s,m integer values.", histo->GetName(), canvas->GetName() );
			Printf("found: type: '%s', index1: '%s', index2: '%s'", p1.Data(), p2.Data(), p3.Data());
			delete ar;
			exit(1);
		    }
		}

	    }

	    if( TString(child->GetNodeName()) == "subpad") {
		// subpad node is last layer, get text inside tag
		// if canvas not splitted, ignore subpadnumber given
		// by xml data and set subpadnumber to 0

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: subpad number '%s' of histogram '%s' in canvas '%s' is not a valid number", val.Data(), histo->GetName(), canvas->GetName() );
		    exit(1);
		}

		if (canvas->IsSplitted()) {
		    histo->SetSubpadnumber(val.Atoi());
		} else {
		    histo->SetSubpadnumber(0);
		}
                
		TString hname = currentDetector->GetName();
		hname += "_";
		hname += canvas->GetName();
		hname += "_";
		hname += histo->GetSubpadnumber();
		hname += "_";
		hname += histo->GetRealName();
		histo->SetName(hname);
		canvas->listHists.Add(histo);
	    }
	}
    }

}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientXML::ParseCanvasNode(TXMLNode *node, HOnlineMonClientTabWin *window, HOnlineMonClientTab *montab) {
    // parse Canvases in TabWindows

    TXMLNode *child = 0;
    HOnlineMonClientCanvas *canvas = new HOnlineMonClientCanvas();



    if (!node->HasChildren()) {
	Printf("XML ERROR: parsing canvas node of window '%s' failed.", window->GetName() );
	exit(1);
    }

    child = node->GetChildren();

    while (child->HasNextNode()) {

	child = child->GetNextNode();
	if (child->GetNodeType() == TXMLNode::kXMLElementNode) {
	    if( TString(child->GetNodeName()) == "name") {
		// name node is last layer, get text inside tag
		TString canvasname = child->GetText();
		if(!canvasname.IsAlnum()) {
		    Printf("XML ERROR: parsing canvas node of window '%s' failed. Invalid canvas name '%s'", window->GetName(), canvasname.Data() );
		    exit(1);
		}
		canvas->SetName(canvasname);
		montab->listCanvases.Add(canvas);
	    }
	    if( TString(child->GetNodeName()) == "width") {
		// width node is last layer, get text inside tag

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: width '%s' of canvas '%s' in tab '%s' in window '%s' is not a valid number.", val.Data(), canvas->GetName(), montab->GetName(), window->GetName() );
		    exit(1);
		}

		canvas->SetWidth(val.Atoi());
	    }
	    if( TString(child->GetNodeName()) == "height") {
		// height node is last layer, get text inside tag

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: width '%s' of canvas '%s' in tab '%s' in window '%s' is not a valid number.", val.Data(), canvas->GetName(), montab->GetName(), window->GetName() );
		    exit(1);
		}

		canvas->SetHeight(val.Atoi());
	    }
	    if( TString(child->GetNodeName()) == "splitted") {
		// splitted node is last layer, get text inside tag

		// type checking
		TString val = child->GetText();
		val.ToLower();

		if ( (val != "true") && (val != "false") ) {
		    Printf("XML ERROR: The value '%s' of canvas '%s' in tab '%s' in window '%s' is not valid.", val.Data(), canvas->GetName(), montab->GetName(), window->GetName() );
		    Printf("Must be 'true' or 'false'.");
		    exit(1);
		}

		canvas->SetSplitted((val == "true") ? kTRUE : kFALSE);
	    }

	    if( TString(child->GetNodeName()) == "nx") {
		// nx node is last layer, get text inside tag
		// ignore value of nx from xml if splitted=false
		// set nx=0 if splitted=false

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: The nx split value '%s' of canvas '%s' in tab '%s' in window '%s' is not a valid number.", val.Data(), canvas->GetName(), montab->GetName(), window->GetName() );
		    exit(1);
		}

		canvas->SetNx( (canvas->IsSplitted()) ? val.Atoi() : 0 );
	    }

	    if( TString(child->GetNodeName()) == "ny") {
		// ny node is last layer, get text inside tag
		// ignore value of ny from xml if splitted=false
		// set ny=0 if splitted=false

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: The ny split value '%s' of canvas '%s' in tab '%s' in window '%s' is not a valid number.", val.Data(), canvas->GetName(), montab->GetName(), window->GetName() );
		    exit(1);
		}

		canvas->SetNy( (canvas->IsSplitted()) ? val.Atoi() : 0 );
	    }

	    if( TString(child->GetNodeName()) == "histogram") {
		// if node is histogram, go one layer down
		ParseHistogramNode(child, canvas);
	    }
	}
    }
}

/* -------------------------------------------------------------------------- */
void HOnlineMonClientXML::ParseCanvasNode(TXMLNode *node, HOnlineMonClientSimpleWin *window) {
    // parse Canvases in SimpleWindows

    TXMLNode *child = 0;
    HOnlineMonClientCanvas *canvas = new HOnlineMonClientCanvas();


    if (!node->HasChildren()) {
	Printf("XML ERROR: parsing canvas node of window '%s' failed.", window->GetName() );
	exit(1);
    }

    child = node->GetChildren();

    while (child->HasNextNode()) {

	child = child->GetNextNode();
	if (child->GetNodeType() == TXMLNode::kXMLElementNode) {

	    if( TString(child->GetNodeName()) == "name") {
		// name node is last layer, get text inside tag
		TString canvasname = child->GetText();
		if(!canvasname.IsAlnum()) {
		    Printf("XML ERROR: parsing canvas node of window '%s' failed. Invalid canvas name '%s'", window->GetName(), canvasname.Data() );
		    exit(1);
		}
		canvas->SetName(canvasname);
		window->listCanvases.Add(canvas);
	    }

	    if( TString(child->GetNodeName()) == "title") {
		// title node is last layer, get text inside tag
		canvas->SetTitle(child->GetText());
	    }

	    if( TString(child->GetNodeName()) == "width") {
		// width node is last layer, get text inside tag

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: width of canvas '%s' in window '%s' is not a valid number.", canvas->GetName(), window->GetName() );
		    exit(1);
		}

		canvas->SetWidth(val.Atoi());
	    }

	    if( TString(child->GetNodeName()) == "height") {
		// height node is last layer, get text inside tag

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: height of canvas '%s' in window '%s' is not a valid number.", canvas->GetName(), window->GetName() );
		    exit(1);
		}

		canvas->SetHeight(val.Atoi());
	    }

	    if( TString(child->GetNodeName()) == "splitted") {
		// splitted node is last layer, get text inside tag

		// type checking
		TString val = child->GetText();
		val.ToLower();

		if ( (val != "true") && (val != "false") ) {
		    Printf("XML ERROR: The value '%s' of 'splitted' of canvas '%s' in window '%s' is not valid.", val.Data(), canvas->GetName(), window->GetName() );
		    Printf("Must be 'true' or 'false'.");
		    exit(1);
		}

		canvas->SetSplitted( (val == "true") ? kTRUE : kFALSE );
	    }

	    if( TString(child->GetNodeName()) == "nx") {
		// nx node is last layer, get text inside tag
		// ignore value of nx from xml if splitted=false
		// set nx=0 if splitted=false

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: The nx split value '%s' of canvas '%s' in window '%s' is not a valid number.", val.Data(), canvas->GetName(), window->GetName() );
		    exit(1);
		}

		canvas->SetNx( (canvas->IsSplitted()) ? val.Atoi() : 0 );
	    }

	    if( TString(child->GetNodeName()) == "ny") {
		// ny node is last layer, get text inside tag
		// ignore value of ny from xml if splitted=false
		// set ny=0 if splitted=false

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: The ny split value '%s' of canvas '%s' in window '%s' is not a valid number.", val.Data(), canvas->GetName(), window->GetName() );
		    exit(1);
		}

		canvas->SetNy( (canvas->IsSplitted()) ? val.Atoi() : 0 );
	    }

	    if( TString(child->GetNodeName()) == "histogram") {
		// if node is window, go one layer down
		ParseHistogramNode(child, canvas);
	    }
	}
    }

}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientXML::ParseTabNode(TXMLNode *node, HOnlineMonClientTabWin *window) {

    HOnlineMonClientTab *montab = new HOnlineMonClientTab();
    TXMLNode *child = 0;



    if (!node->HasChildren()) {
	Printf("XML ERROR: parsing tab node of window '%s' failed.", window->GetName() );
	exit(1);
    }

    child = node->GetChildren();

    while (child->HasNextNode()) {
	child = child->GetNextNode();
	if (child->GetNodeType() == TXMLNode::kXMLElementNode) {
	    if( TString(child->GetNodeName()) == "name") {
		// name node is last layer, get text inside tag
		TString tabname = child->GetText();
		if(!tabname.IsAlnum()) {
		    Printf("XML ERROR: parsing tab node of window '%s' failed. Invalid tab name '%s'", window->GetName(), tabname.Data() );
		    exit(1);
		}
		montab->SetName(tabname);
		window->listTabs.Add(montab);
	    }
	    if( TString(child->GetNodeName()) == "title") {
		// title node is last layer, get text inside tag
		montab->SetTitle(child->GetText());
	    }
	    if( TString(child->GetNodeName()) == "canvas") {
		// if node is window, go one layer down
		ParseCanvasNode(child, window, montab);
	    }

	}
    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientXML::ParseWindowNode(TXMLNode *node, HOnlineMonClientDet *detector) {
    Bool_t tabbed = kFALSE;
    TString name = "";
    TString title = "";
    TXMLNode *child = 0;

    if (!node->HasChildren()) {
	Printf("XML ERROR: parsing window node of detector '%s' failed.", detector->GetName() );
	exit(1);
    }

    child = node->GetChildren();

    while (child->HasNextNode()) {
	child = child->GetNextNode();

	if (child->GetNodeType() == TXMLNode::kXMLElementNode) {
	    if( TString(child->GetNodeName()) == "name") {
		// name node is last layer, get text inside tag
		TString windowname = child->GetText();
		if (!windowname.IsAlnum()) {
		    Printf("XML ERROR: parsing window node of detector '%s' failed. Invalid window name '%s'", detector->GetName(), windowname.Data() );
		    exit(1);
		}
		name = windowname;
	    }
	    if( TString(child->GetNodeName()) == "title") {
		// title node is last layer, get text inside tag
		title = child->GetText();
	    }
	    if( TString(child->GetNodeName()) == "tabbed") {
		// tabbed node is last layer, get text inside tag
		if (TString(child->GetText()) == "true") {
		    // tabbed window
		    tabbed = kTRUE;
		    HOnlineMonClientTabWin *window = new HOnlineMonClientTabWin();
		    window->SetName(name);
		    window->SetTitle(title);
		    detector->listWindows.Add(window);
		} else {
		    // simple window
		    tabbed = kFALSE;
		    HOnlineMonClientSimpleWin *window = new HOnlineMonClientSimpleWin();
		    window->SetName(name);
		    window->SetTitle(title);
		    detector->listWindows.Add(window);
		}
	    }
	    if( TString(child->GetNodeName()) == "canvas") {
		// if node is canvas, go one layer down
		// is possible in nontabbed window ONLY
		// so tabbed must be false, otherwise the xml configuration file is incorrect
		if (tabbed) {
		    Printf("XML ERROR: xml file wrong, found tabbed=true and canvas directly under window.");
		    Printf("Add a tab tag or set tabbed to false.");
		    Printf("Window name: %s", name.Data() );
		    Printf("Detector name: %s", detector->GetName() );
		    exit(1);
		}

		ParseCanvasNode(child, (HOnlineMonClientSimpleWin*)detector->listWindows.FindObject(name)); // window is always HOnlineMonClientSimpleWin
	    }
	    if( TString(child->GetNodeName()) == "tab") {
		// if node is tab, go one layer down
		ParseTabNode(child,  (HOnlineMonClientTabWin*)detector->listWindows.FindObject(name)); // window is always HOnlineMonClientTabWin
	    }
	}
    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientXML::ParseDetectorNode(TXMLNode *node, HOnlineMonClientDet *detector) {
    TXMLNode *child = 0;

    currentDetector = detector;

    if (!node->HasChildren()) {
	Printf("XML ERROR: parsing detector node failed.");
	exit(1);
    }

    child = node->GetChildren();

    while (child->HasNextNode()) {
	child = child->GetNextNode();

	if (child->GetNodeType() == TXMLNode::kXMLElementNode) {
	    if( TString(child->GetNodeName()) == "name") {
		// name node is last layer, get text inside tag
		TString detname = child->GetText();
		if (!detname.IsAlnum()) {
		    Printf("XML ERROR: parsing detector node failed. Invalid detector name '%s'", detname.Data());
		    exit(1);
		}

		detector->SetName(detname);
	    }
	    if( TString(child->GetNodeName()) == "title") {
		// title node is last layer, get text inside tag
		detector->SetTitle(child->GetText());
	    }
	    if( TString(child->GetNodeName()) == "window") {
		// if node is window, go one layer down
		ParseWindowNode(child, detector);
	    }

	}
    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientXML::ParseServerNode(TXMLNode *node) {
    TXMLNode *child = 0;

    if (!node->HasChildren()) {
	Printf("XML ERROR: parsing server node failed.");
	exit(1);
    }

    child = node->GetChildren();

    while (child->HasNextNode()) {
	child = child->GetNextNode();

	if (child->GetNodeType() == TXMLNode::kXMLElementNode) {
	    if( TString(child->GetNodeName()) == "host") {
		// host node is last layer, get text inside tag
		clientmain->SetServerhost(child->GetText());
	    }
	    if( TString(child->GetNodeName()) == "port") {
		// port node is last layer, get text inside tag

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: The server port is not a valid number.");
		    exit(1);
		}

		clientmain->SetServerport(val.Atoi());
	    }

	}

    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientXML::ParseConfigNode(TXMLNode *node) {
    TXMLNode *child = 0;

    if (!node->HasChildren()) {
	Printf("XML ERROR: parsing config node failed.");
	exit(1);
    }

    child = node->GetChildren();

    while (child->HasNextNode()) {
	child = child->GetNextNode();

	if (child->GetNodeType() == TXMLNode::kXMLElementNode) {
	    if( TString(child->GetNodeName()) == "server") {
		// if node is server, go one layer down
		ParseServerNode(child);
	    }
	}

    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientXML::ParseMainWindowNode(TXMLNode *node, HOnlineMonClientMainWin *clientmainwin) {
    TXMLNode *child = 0;

    if (!node->HasChildren()) {
	Printf("XML ERROR: parsing MainWindow node failed.");
	exit(1);
    }

    child = node->GetChildren();

    while (child->HasNextNode()) {
	child = child->GetNextNode();

	if (child->GetNodeType() == TXMLNode::kXMLElementNode) {
	    if( TString(child->GetNodeName()) == "name") {
		// name node is last layer, get text inside tag
		TString windowname = child->GetText();
		if (!windowname.IsAlnum()) {
		    Printf("XML ERROR: parsing MainWindow node failed. Invalid MainWindow name '%s'", windowname.Data());
		    exit(1);
		}
		clientmainwin->SetName(windowname);
	    }

	    if( TString(child->GetNodeName()) == "title") {
		// title node is last layer, get text inside tag
		clientmainwin->SetTitle(child->GetText());
	    }

	    if( TString(child->GetNodeName()) == "width") {
		// width node is last layer, get text inside tag

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: width of MainWindow is not a valid number.");
		    exit(1);
		}

		clientmainwin->SetWidth(val.Atoi());
	    }

	    if( TString(child->GetNodeName()) == "height") {
		// height node is last layer, get text inside tag

		// type checking
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: height of MainWindow is not a valid number.");
		    exit(1);
		}

		clientmainwin->SetHeight(val.Atoi());
	    }

            if( TString(child->GetNodeName()) == "snapshotpath") {
		clientmainwin->SetSnapShotPath(child->GetText());
	    }
            if( TString(child->GetNodeName()) == "snapshotpathonline") {
		clientmainwin->fTextSnapshotPathOnline=child->GetText();
	    }

	    if( TString(child->GetNodeName()) == "autosaverate") {
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: autosaverate of MainWindow is not a valid number.");
		    exit(1);
		}
		clientmainwin->SetAutoSaveRate(val.Atoi());
		HOnlineMonClientMain::SetAutoSaveInterval(val.Atoi());
	    }
	    if( TString(child->GetNodeName()) == "autosaverateonline") {
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: autosaverateonline of MainWindow is not a valid number.");
		    exit(1);
		}
		HOnlineMonClientMain::SetAutoSaveIntervalOnline(val.Atoi());
	    }

	    if( TString(child->GetNodeName()) == "openlist") {
		TString val = child->GetText();

		TObjArray* myarguments = val.Tokenize(",");
		TIterator* myiter = myarguments->MakeIterator();
                TObjString* stemp = 0;
		// iterate over the list of arguments
		while ((stemp = (TObjString*)myiter->Next()) != 0)
		{
		    TString argument = stemp->GetString();
		    clientmainwin->listOpenDetectors.Add(new TObjString(argument.Data()));
		}
		myarguments->Delete();
		delete myarguments;
                delete myiter;

	    }

	    if( TString(child->GetNodeName()) == "reconnect") {
		TString val = child->GetText();
		if (!val.IsDigit()) {
		    Printf("XML ERROR: reconnect of MainWindow is not a valid number.");
		    exit(1);
		}

                clientmainwin->GetMainClient()->SetReconnect(val.Atoi());
	    }

	}

    }
}

