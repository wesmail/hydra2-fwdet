//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : J. Markert
// Modify 05.04.2013: O.Pechenova
////////////////////////////////////////////////////////////////////////////
// HMdcWireStat
//
//  Container class for lookup parameters of the status of the
//  wire (working or dead + noise + efficiency).
//  The parameters are stored in TH2F histograms which can be plotted.
//  This container is needed by HMdcDigitizer and HMdcTrackFinder
//
//  THe Status value is coded in the following way
//
//  status of the wire: 1=working,
//                     -1=not connected,
//                     -3=no correct offset,
//                     -5=drop in efficiency,
//                     -7=dead,
//                    -10=init
//
//
////////////////////////////////////////////////////////////////////////////
#include "hmdcwirestat.h"
#include "hmessagemgr.h"
#include "hparamlist.h"
#include "TStyle.h"
#include "TObjArray.h"
#include "TH1.h"
#include "TH2F.h"
#include <stdlib.h>

ClassImp(HMdcWireStat)

HMdcWireStat::HMdcWireStat(const Char_t* name,const Char_t* title,
                           const Char_t* context)
    : HParCond(name,title,context)
{
    //
    Int_t size = 6*4*6*220;
    statusmap .Set(size);
    efficiency.Set(size);
    offset    .Set(size);
    mb        .Set(size);
    tdc       .Set(size);
    clear();
}
HMdcWireStat::~HMdcWireStat()
{
    // destructor
}
void HMdcWireStat::clear()
{
    statusmap .Reset(-10);
    efficiency.Reset(-10);
    offset    .Reset(-10);
    mb        .Reset(-10);
    tdc       .Reset(-10);

    status=kFALSE;
    resetInputVersions();
    changed=kFALSE;
}
void HMdcWireStat::printStats()
{
    Int_t working;      //  1
    Int_t notconnected; // -1
    Int_t nooffset;     // -3
    Int_t dropeffic;    // -5
    Int_t dead;         // -7

    Int_t workingsum      = 0;
    Int_t notconnectedsum = 0;
    Int_t nooffsetsum     = 0;
    Int_t dropefficsum    = 0;
    Int_t deadsum         = 0;
    Int_t stat;
    for(Int_t s = 0; s < 6; s ++){
	for(Int_t m = 0; m < 4; m ++){
	    working      = 0;
	    notconnected = 0;
	    nooffset     = 0;
	    dropeffic    = 0;
	    dead         = 0;
	    for(Int_t l = 0; l < 6; l ++){
		for(Int_t c = 0; c < 220; c ++){
	 
		    stat = statusmap[index(s,m,l,c)];
                    if(stat ==  1) working ++;
                    if(stat == -1) notconnected ++;
                    if(stat == -3) nooffset ++;
                    if(stat == -5) dropeffic ++;
                    if(stat == -7) dead ++;
		}
	    }
	    workingsum     = workingsum + working;
	    notconnectedsum= notconnectedsum + notconnected;
	    nooffsetsum    = nooffsetsum + nooffset;
	    dropefficsum   = dropefficsum + dropeffic;
            deadsum        = deadsum + dead;
	    printf("s %i m %i \n" ,s,m);
	    printf("          working: %i \n",working);
	    printf("    not connected: %i \n",notconnected);
	    printf("   no calibration: %i \n",nooffset);
	    printf("drop in efficiency: %i \n",dropeffic);
            printf("             dead: %i \n",dead);
	}
    }

    printf("SUM\n");
    printf("          working: %i \n",workingsum);
    printf("    not connected: %i \n",notconnectedsum);
    printf("   no calibration: %i \n",nooffsetsum);
    printf("drop in efficiency: %i \n",dropefficsum);
    printf("             dead: %i \n",deadsum);

}
void HMdcWireStat::printStatsSum()
{
    Int_t working;      //  1
    Int_t notconnected; // -1
    Int_t nooffset;     // -3
    Int_t dropeffic;    // -5
    Int_t dead;         // -7

    Int_t workingsum      = 0;
    Int_t notconnectedsum = 0;
    Int_t nooffsetsum     = 0;
    Int_t dropefficsum    = 0;
    Int_t deadsum         = 0;
    Int_t stat;
    for(Int_t s = 0; s < 6; s ++){
	for(Int_t m = 0; m < 4; m ++){
	    working      = 0;
	    notconnected = 0;
	    nooffset     = 0;
	    dropeffic    = 0;
	    dead         = 0;
	    for(Int_t l = 0; l < 6; l ++){
		for(Int_t c = 0; c< 220; c ++){
	 
		    stat=statusmap[index(s,m,l,c)];
                    if(stat ==  1) working ++;
                    if(stat == -1) notconnected ++;
                    if(stat == -3) nooffset ++;
                    if(stat == -5) dropeffic ++;
                    if(stat == -7) dead ++;
		}
	    }
	    workingsum     = workingsum + working;
	    notconnectedsum= notconnectedsum + notconnected;
	    nooffsetsum    = nooffsetsum + nooffset;
	    dropefficsum   = dropefficsum + dropeffic;
            deadsum        = deadsum + dead;
	}
    }

    printf("SUM\n");
    printf("          working: %i \n",workingsum);
    printf("    not connected: %i \n",notconnectedsum);
    printf("   no calibration: %i \n",nooffsetsum);
    printf("drop in efficiency: %i \n",dropefficsum);
    printf("             dead: %i \n",deadsum);

}
Int_t HMdcWireStat::getDeadWires()
{
    Int_t working;      //  1
    Int_t notconnected; // -1
    Int_t nooffset;     // -3
    Int_t dead;         // -7

    Int_t workingsum      = 0;
    Int_t notconnectedsum = 0;
    Int_t nooffsetsum     = 0;
    Int_t deadsum         = 0;
    Int_t stat;
    for(Int_t s = 0; s < 6; s ++){
	for(Int_t m = 0; m < 4; m ++){
	    working      = 0;
	    notconnected = 0;
	    nooffset     = 0;
	    dead         = 0;
	    for(Int_t l = 0; l < 6; l ++){
		for(Int_t c = 0; c < 220; c ++){
	 
		    stat = statusmap[index(s,m,l,c)];
                    if(stat ==  1) working ++;
                    if(stat == -1) notconnected ++;
                    if(stat == -3) nooffset ++;
                    if(stat == -7) dead ++;
		}
	    }
	    workingsum      = workingsum + working;
	    notconnectedsum = notconnectedsum + notconnected;
	    nooffsetsum     = nooffsetsum + nooffset;
            deadsum         = deadsum + dead;
	}
    }
    return nooffsetsum;
}
Int_t HMdcWireStat::getDeadWires(Int_t sec,Int_t mod)
{
    Int_t working      = 0;
    Int_t notconnected = 0;
    Int_t nooffset     = 0;
    Int_t dead         = 0;
    Int_t s = sec;
    Int_t m = mod;
    Int_t stat;
    for(Int_t l = 0; l < 6; l ++){
	for(Int_t c = 0; c <220; c ++){

	    stat = statusmap[index(s,m,l,c)];
	    if(stat ==  1) working ++;
	    if(stat == -1) notconnected ++;
	    if(stat == -3) nooffset ++;
	    if(stat == -7) dead ++;
	}
    }
    return nooffset;
}
void HMdcWireStat::printParam(void)
{
    // prints the parameters of HMdcWireStat to the screen.
    SEPERATOR_msg("#",60);
    INFO_msg(10,HMessageMgr::DET_MDC,"HMdcWireStat:");
    INFO_msg(10,HMessageMgr::DET_MDC,"Status:");
    for(Int_t s = 0; s < 6; s ++){
	for(Int_t m = 0; m < 4; m ++){
	    for(Int_t l = 0; l < 6; l ++){
		for(Int_t c = 0; c < 220; c ++){
                    Int_t ind = index(s,m,l,c);
		    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
					   "s %i m %i l %i c %3i mb %2i tdc %2i status %i efficiency %5.4f offset %5.4f",
					   s,m,l,c, mb[ind], tdc[ind], statusmap[ind], efficiency[ind], offset[ind]);
		}
	    }
	}
    }
    SEPERATOR_msg("#",60);
}
void HMdcWireStat::putParams(HParamList* l)
{
    // Puts all params of HMdcWireStat to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;

    l->add("mb"        ,mb);
    l->add("tdc"       ,tdc);
    l->add("status"    ,statusmap);
    l->add("efficiency",efficiency);
    l->add("offset"    ,offset);
}
Bool_t HMdcWireStat::getParams(HParamList* l)
{
    if (!l) return kFALSE;

    if(!( l->fill("mb"        ,&mb )))       return kFALSE;
    if(!( l->fill("tdc"       ,&tdc)))       return kFALSE;
    if(!( l->fill("status"    ,&statusmap))) return kFALSE;
    if(!( l->fill("efficiency",&efficiency)))return kFALSE;
    if(!( l->fill("offset"    ,&offset    )))return kFALSE;


    return kTRUE;
}
TCanvas* HMdcWireStat::plotStatusMbVsTdc()
{
    gStyle->SetOptStat(0);
    Char_t name[400];
    sprintf(name,"%s","Mb_vs_Tdc");
    TCanvas* result = new TCanvas(name,name,1000,800);
    result->Divide(6,4);

    TH2F* hmb_vs_tdc[6][4];
    Int_t mbo,t,st;
    for(Int_t sec = 0; sec < 6; sec ++){
        for(Int_t mod = 0; mod < 4; mod ++){
            sprintf(name,"%s%i%s%i","hStatusMb_vs_Tdc_sec",sec,"_mod",mod);
            hmb_vs_tdc[sec][mod] = new TH2F(name,name,96,0,96,16,0,16);
            hmb_vs_tdc[sec][mod]->SetXTitle("tdc channel");
            hmb_vs_tdc[sec][mod]->SetYTitle("Mbo");
	    for(Int_t bx = 0; bx < 96; bx ++){
		for(Int_t by = 0; by < 16; by ++){
		    hmb_vs_tdc[sec][mod]->SetBinContent(bx+1,by+1,-10);
		}
	    }
            for(Int_t l = 0; l < 6; l ++){
                for(Int_t c = 0; c < 220; c ++){
                    Int_t ind = index(sec,mod,l,c);
		    mbo = mb       [ind];
                    t   = tdc      [ind];
                    st  = statusmap[ind];
                    hmb_vs_tdc[sec][mod]->SetBinContent(t+1,mbo+1,st);
                }
            }
            hmb_vs_tdc[sec][mod]->SetMinimum(-4);
            result->cd(mod*6+sec+1);
            hmb_vs_tdc[sec][mod]->DrawCopy("Colz");
        }
    }
    return result;
}
TCanvas* HMdcWireStat::plotEfficiencyMbVsTdc()
{
    gStyle->SetOptStat(0);
    Char_t name[400];
    sprintf(name,"%s","Mb_vs_Tdc");
    TCanvas* result = new TCanvas(name,name,1000,800);
    result->Divide(6,4);

    TH2F* hmb_vs_tdc[6][4];
    Int_t mbo,t;
    Float_t eff;
    for(Int_t sec = 0; sec < 6; sec ++){
        for(Int_t mod = 0; mod < 4; mod ++){
            sprintf(name,"%s%i%s%i","hEfficiencyMb_vs_Tdc_sec",sec,"_mod",mod);
            hmb_vs_tdc[sec][mod] = new TH2F(name,name,96,0,96,16,0,16);
            hmb_vs_tdc[sec][mod]->SetXTitle("tdc channel");
            hmb_vs_tdc[sec][mod]->SetYTitle("Mbo");

	    for(Int_t bx = 0; bx < 96; bx ++){
		for(Int_t by = 0; by < 16; by ++){
		    hmb_vs_tdc[sec][mod]->SetBinContent(bx+1,by+1,-10);
		}
	    }

	    for(Int_t l = 0; l < 6; l ++){
                for(Int_t c = 0; c < 220; c ++){
		    Int_t ind = index(sec,mod,l,c);
		    mbo = mb        [ind];
		    t   = tdc       [ind];
		    eff = efficiency[ind];
		    hmb_vs_tdc[sec][mod]->SetBinContent(t+1,mbo+1,eff);
                }
            }
            result->cd(mod*6+sec+1);
            hmb_vs_tdc[sec][mod]->DrawCopy("Colz");
        }
    }
    return result;
}

TCanvas* HMdcWireStat::plotOffsetMbVsTdc()
{
    gStyle->SetOptStat(0);
    Char_t name[400];
    sprintf(name,"%s","Mb_vs_Tdc");
    TCanvas* result = new TCanvas(name,name,1000,800);
    result->Divide(6,4);

    TH2F* hmb_vs_tdc[6][4];
    Int_t mbo,t;
    Float_t off;
    for(Int_t sec = 0; sec < 6; sec ++){
        for(Int_t mod = 0; mod < 4; mod ++){
            sprintf(name,"%s%i%s%i","hOffsetMb_vs_Tdc_sec",sec,"_mod",mod);
            hmb_vs_tdc[sec][mod] = new TH2F(name,name,96,0,96,16,0,16);
            hmb_vs_tdc[sec][mod]->SetXTitle("tdc channel");
            hmb_vs_tdc[sec][mod]->SetYTitle("Mbo");

	    for(Int_t bx = 0; bx < 96; bx ++){
		for(Int_t by = 0; by < 16; by ++){
		    hmb_vs_tdc[sec][mod]->SetBinContent(bx+1,by+1,-10);
		}
	    }

	    for(Int_t l = 0; l < 6; l ++){
                for(Int_t c = 0; c < 220; c ++){
                    Int_t ind = index(sec,mod,l,c);
		    mbo = mb    [ind];
                    t   = tdc   [ind];
                    off = offset[ind];
                    hmb_vs_tdc[sec][mod]->SetBinContent(t+1,mbo+1,off);
                }
            }
            result->cd(mod*6+sec+1);
            hmb_vs_tdc[sec][mod]->DrawCopy("Colz");
        }
    }
    return result;
}
TCanvas* HMdcWireStat::plotStatus(Int_t sec,Int_t mod)
{
    gStyle->SetOptStat(0);
    Char_t name[400];
    sprintf(name,"status_s%i_m%i",sec,mod);
    TCanvas* result = new TCanvas(name,name,1000,800);
    result->Divide(1,6);

    TH1F* hmdcstatus[6];
    Int_t st;
    for(Int_t l = 0; l < 6; l ++){
        sprintf(name,"%s%i%s%i%s%i","hmdcstatus_sec",sec,"_mod",mod,"_layer",l);
        hmdcstatus[l] = new TH1F(name,name,220,0,220);
        hmdcstatus[l]->SetXTitle("wire number");
        hmdcstatus[l]->SetYTitle("status");
        hmdcstatus[l]->SetFillColor(8);
        for(Int_t  c = 0; c < 220; c ++){
            Int_t ind = index(sec,mod,l,c);
            st  = statusmap[ind];
	    hmdcstatus[l]->SetBinContent(c+1,st);
        }
        result->cd(l+1);
        hmdcstatus[l]->DrawCopy();
    }
    return result;
}
TCanvas* HMdcWireStat::plotEfficiency(Int_t sec,Int_t mod)
{
    gStyle->SetOptStat(0);
    Char_t name[400];
    sprintf(name,"efficiency_s%i_m%i",sec,mod);
    TCanvas* result = new TCanvas(name,name,1000,800);
    result->Divide(1,6);

    TH1F* hmdcefficiency[6];
    Float_t eff;
    for(Int_t l = 0; l < 6; l ++){
        sprintf(name,"%s%i%s%i%s%i","hmdcefficiency_sec",sec,"_mod",mod,"_layer",l);
        hmdcefficiency[l] = new TH1F(name,name,220,0,220);
        hmdcefficiency[l]->SetXTitle("wire number");
        hmdcefficiency[l]->SetYTitle("efficiency");
        for(Int_t c = 0; c < 220; c ++){
            Int_t ind = index(sec,mod,l,c);
            eff = efficiency[ind];
            hmdcefficiency[l]->SetBinContent(c+1,eff);
        }
        result->cd(l+1);
        hmdcefficiency[l]->DrawCopy();
    }
    return result;
}
TCanvas* HMdcWireStat::plotOffset(Int_t sec,Int_t mod)
{
    gStyle->SetOptStat(0);
    Char_t name[400];
    sprintf(name,"offset_s%i_m%i",sec,mod);
    TCanvas* result = new TCanvas(name,name,1000,800);
    result->Divide(1,6);

    TH1F* hmdcoffset[6];
    Float_t off;
    for(Int_t l = 0; l < 6; l ++){
        sprintf(name,"%s%i%s%i%s%i","hmdcoffset_sec",sec,"_mod",mod,"_layer",l);
        hmdcoffset[l] = new TH1F(name,name,220,0,220);
        hmdcoffset[l]->SetXTitle("wire number");
        hmdcoffset[l]->SetYTitle("offset [ns]");
        for(Int_t c = 0; c < 220; c ++){
            Int_t ind = index(sec,mod,l,c);
            off = offset[ind];
            hmdcoffset[l]->SetBinContent(c+1,off);
        }
        result->cd(l+1);
        hmdcoffset[l]->DrawCopy();
    }
    return result;
}

