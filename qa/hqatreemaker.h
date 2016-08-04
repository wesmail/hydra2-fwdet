#ifndef HQATREEMAKER_H
#define HQATREEMAKER_H

#include "TObject.h"

#define NSTART_STRIPS 64

class HQAFileInfoTree : public TObject{

public:
    TString fTFileName; //DST Filename
    Int_t fTNumEvents;  //Number of events in file
    Int_t fTYear;       //year when file was taken
    Int_t fTDay;        //day when file was taken
    Int_t fTHour;       //hour when file was taken
    Int_t fTMin;        //Minute when file was taken
    Int_t fTSec;        //Second when file was taken
    Int_t fTEvB;        //Eventbuider which wrote file
    TString fTType;  //Type of file (be, te,co ...)
    Int_t fTRunId;      //RunID of file
    Int_t fTRefId;      //RunID of file
    

    HQAFileInfoTree(){
	fTNumEvents=-10000;
	fTYear     =-10000;
	fTDay      =-10000;
	fTHour     =-10000;
	fTMin      =-10000;
	fTEvB      =-10000;
	fTRunId    =-10000;
	fTRefId    =-10000;
        fTFileName = "notset";
    }


    ~HQAFileInfoTree(){}

    ClassDef(HQAFileInfoTree, 1)
};


class HQAStartTree : public TObject{

public:
    Float_t fTHitTimeMean;  //Mean value of start time from StartHit
    Float_t fTHitTimeRms;   //RMS of start time from StartHit
    Float_t fTHitToNoveto;  //Ratio of Hits in start to hits in veto
    Float_t fTStCalTime[NSTART_STRIPS]; //Mean value of Start time of Start for each Strip
    Float_t fTVtCalTime[NSTART_STRIPS]; //Mean value of Start time of Veto for each Strip
    Float_t fTVertexX;      //Mean of Vertex in X
    Float_t fTVertexY;      //Mean of Vertex in Y
    Float_t fTVertexZ;      //Mean of Vertex in Z


    HQAStartTree(){
	fTHitTimeMean=-10000.0;
	fTHitTimeRms=-10000.0;
	fTHitToNoveto=-10000.0;
	fTVertexX=-10000.0;
	fTVertexY=-10000.0;
	fTVertexZ=-10000.0;
	for(Int_t i=0; i<NSTART_STRIPS;i++)
	{
	    fTStCalTime[i]=-10000.0;
	    fTVtCalTime[i]=-10000.0;
	}
    }


    ~HQAStartTree(){}

    ClassDef(HQAStartTree, 1)
};


class HQARichTree : public TObject{

public:
    Float_t fTAvChargeMax[6];  //Charge divided by number of pads for found Rings in each Sector (Most probable value)
    Float_t fTNumPadsMax[6];   //Number of pads per Ring for each Sector (Most probable value)
    Float_t fTNumRingsMax[6];  //Number of Rings for each event and each Sector (Most probable value)

    Float_t fTAvChargeMean[6];  //Charge divided by number of pads for found Rings in each Sector (Mean value)
    Float_t fTNumPadsMean[6];   //Number of pads per Ring for each Sector (Mean value)
    Float_t fTNumRingsMean[6];  //Number of Rings for each event and each Sector (Mean value)

    Float_t fTHVMean[6];        //HV mean value from Oracle for each sector(6)
    Float_t fTHVSigma[6];       //HV sigma value from Oracle for each sector(6)
    Float_t fTHVMin[6];         //HV minimum value from Oracle for each sector(6)
    Float_t fTHVMax[6];         //HV maximum value from Oracle for each sector(6)
    Float_t fTHVNData[6];       //HV NData points from Oracle for each sector(6)

    HQARichTree(){

	for(Int_t i=0; i<6;i++)
	{
	    fTAvChargeMax[i]=-10000.0;
	    fTNumPadsMax[i]=-10000.0;
	    fTNumRingsMax[i]=-10000.0;

	    fTAvChargeMean[i]=-10000.0;
	    fTNumPadsMean[i]=-10000.0;
	    fTNumRingsMean[i]=-10000.0;

            fTHVMean[i]=-10.0;
	    fTHVSigma[i]=-10.0;
	    fTHVMin[i]=-10.0;
	    fTHVMax[i]=-10.0;
	    fTHVNData[i]=-10.0;
	}
    }

    ~HQARichTree(){}

    ClassDef(HQARichTree, 1)
};



class HQAMdcTree : public TObject{

public:
    Float_t fTMboCounts[6][4][16];      //Average Number of hits for each mbo(16) in each sector(6) and each module(4)
    Float_t fTLayerCounts[6][4][6];     //Average Number of hits for each Layer(6) in each sector(6) and each module(4)
    Float_t fTCountsRaw[6][4];          //Average Number of hits in each sector(6) and each module(4) on Raw Level
    Float_t fTCountsCal[6][4];          //Average Number of hits in each sector(6) and each module(4) on Cal1 Level
    Float_t fTToTCalMean[6][4][6];      //Mean value of TimeOverThreshold for each sector, module and layer
    Float_t fTToTCalMax[6][4][6];       //TimeOverThreshold for each sector, module and layer (most probable value)
    Float_t fTTime1CalMean[6][4][6];    //Mean value of Drift time for each sector, module and layer
    Float_t fTTime1CalMax[6][4][6];     //Drift time for each sector, module and layer (most probable value)
    Float_t fTSegCounter[6][2];         //Average Number of Segments per Event in each sector(6) and Inner-Outer MDCs(2)
    Float_t fTSegCounterFit[6][2];      //Average Number of Segments per Event in each sector(6) and Inner-Outer MDCs(2) which were fitted
    Float_t fTSegChi2Mean[6][2];        //Mean Value of chi2 for all sectors(6) and Inner-Outer MDCs(2)
    Float_t fTSegChi2Max[6][2];         //Most probable value of chi2 for all sectors(6) and Inner-Outer MDCs(2)

    
    Float_t fTHVMean[6][4][12];         //HV mean value from Oracle for each plane(4) each sector(6) and each channel (12)
    Float_t fTHVSigma[6][4][12];        //HV sigma value from Oracle for each plane(5) each sector(7) and each channel (12)
    Float_t fTHVMin[6][4][12];          //HV minimum value from Oracle for each plane(5) each sector(7) and each channel (12)
    Float_t fTHVMax[6][4][12];          //HV maximum value from Oracle for each plane(5) each sector(7) and each channel (12)
    Float_t fTHVNData[6][4][12];        //HV NData points from Oracle for each plane(5) each sector(7) and each channel (12)


    HQAMdcTree(){

	for(Int_t i=0; i<6;i++)
	{
	    for(Int_t j=0; j<2; j++)
	    {
		fTSegCounter[i][j]=0.0;
		fTSegCounterFit[i][j]=0.0;
		fTSegChi2Mean[i][j]=-10.0;
                fTSegChi2Max[i][j]=-10.0;
	    }

	    for(Int_t j=0; j<4; j++)
	    {
		for(Int_t k=0; k<16; k++)
		{   
		    fTMboCounts[i][j][k]=0.0;
		}

                for(Int_t k=0; k<6; k++)
		{   
		    fTLayerCounts[i][j][k]=0.0;
		    fTToTCalMean[i][j][k]=-10.0;
		    fTToTCalMax[i][j][k]=-10.0;
		    fTTime1CalMean[i][j][k]=-10.0;
                    fTTime1CalMax[i][j][k]=-10.0;
		}

		fTCountsRaw[i][j]=0.0;
                fTCountsCal[i][j]=0.0;
	    }
	}

	for(Int_t i=0; i<6; i++)
	{
	    for(Int_t j=0; j<4; j++)
	    {
		for(Int_t k=0; k<12; k++)
		{
		    fTHVMean[i][j][k]=-10.0;
		    fTHVSigma[i][j][k]=-10.0;
		    fTHVMin[i][j][k]=-10.0;
		    fTHVMax[i][j][k]=-10.0;
		    fTHVNData[i][j][k]=-10.0;
		}
	    }
	}

    }

    ~HQAMdcTree(){}

    ClassDef(HQAMdcTree, 1)
};



class HQATofTree : public TObject{

public:
    Float_t fTHitMultiMax[6];   //Hit Multiplicity in each Sector (Most probable value)
    Float_t fTHitMultiMean[6];  //Hit Multiplicity in each Sector (Mean value)
    Float_t fTHitTotMultiMax;   //Total Hit Multiplicity  (Most probable value)
    Float_t fTHitTotMultiMax2;  //Total Hit Multiplicity for multiplicities larger than 10 (Most probable value)
    Float_t fTHitTotMultiMean;  //Total Hit Multiplicity  (Mean value)
    Float_t fTHits[6];          //Average Number of Hist for each sector
    Float_t fTHitTofMax;        //Measured time (Most probable value)
    Float_t fTHitTofMean;       //Measured time (Mean value)

    HQATofTree(){

	for(Int_t i=0; i<6;i++)
	{
	    fTHitMultiMax[i]=-10.0;
	    fTHitMultiMean[i]=-10.0;
            fTHits[i]=0.0;
	    
	}
	fTHitTotMultiMax=-10.0;
        fTHitTotMultiMax2=-10.0;
	fTHitTotMultiMean=-10.0;
	fTHitTofMax=-10.0;
        fTHitTofMean=-10.0;
    }

    ~HQATofTree(){}

    ClassDef(HQATofTree, 1)
};



class HQARpcTree : public TObject{

public:
    Float_t fTHitMultiMax[6];   //Hit Multiplicity in each Sector (Most probable value)
    Float_t fTHitMultiMean[6];  //Hit Multiplicity in each Sector (Mean value)
    Float_t fTHitTotMultiMax;   //Total Hit Multiplicity  (Most probable value)
    Float_t fTHitTotMultiMax2;  //Total Hit Multiplicity for multiplicities larger than 10 (Most probable value)
    Float_t fTHitTotMultiMean;  //Total Hit Multiplicity  (Mean value)
    Float_t fTHits[6];          //Average Number of Hist for each sector
    Float_t fTHitTofMax;        //Measured time (Most probable value)
    Float_t fTHitTofMean;       //Measured time (Mean value)

    HQARpcTree(){

	for(Int_t i=0; i<6;i++)
	{
	    fTHitMultiMax[i]=-10.0;
	    fTHitMultiMean[i]=-10.0;
            fTHits[i]=0.0;
	    
	}
	fTHitTotMultiMax=-10.0;
        fTHitTotMultiMax2=-10.0;
	fTHitTotMultiMean=-10.0;
	fTHitTofMax=-10.0;
        fTHitTofMean=-10.0;
    }

    ~HQARpcTree(){}

    ClassDef(HQARpcTree, 1)
};


class HQAShowerTree : public TObject{

public:
    Float_t fTHitMultiMax[6][3];   //Hit Multiplicity in each Sector(6) and Module(3) (Most probable value)
    Float_t fTHitMultiMean[6][3];  //Hit Multiplicity in each Sector(6) and Module(3) (Mean value)
    Float_t fTHitChargeMax[6][3];   //Charge in each Sector(6) and Module(3) (Most probable value)
    Float_t fTHitChargeMean[6][3];  //Charge in each Sector(6) and Module(3) (Mean value)

    HQAShowerTree(){

	for(Int_t i=0; i<6;i++)
	{
	    for(Int_t j=0; j<3; j++)
	    {
		fTHitMultiMax[i][j]=-10.0;
		fTHitMultiMean[i][j]=-10.0;
		fTHitChargeMax[i][j]=-10.0;
                fTHitChargeMean[i][j]=-10.0;
	    }
	}
    }

    ~HQAShowerTree(){}

    ClassDef(HQAShowerTree, 1)
};



class HQAPhysicsTree : public TObject{

public:

   //From ParticleCand:
   Float_t fTMass_proton;              //Fitted mass of Proton Canddidate from both detectors
   Float_t fTMass_pip;                 //Fitted mass of Pip Canddidate from both detectors
   Float_t fTMass_pim;                 //Fitted mass of Pim Canddidate from both detectors
   Float_t fTMass_proton_RPC;          //Fitted mass of Proton Canddidate from RPC only
   Float_t fTMass_pip_RPC;             //Fitted mass of Pip Canddidate from RPC only
   Float_t fTMass_pim_RPC;             //Fitted mass of Pim Canddidate from RPC only
   Float_t fTMass_proton_TOF;          //Fitted mass of Proton Canddidate from TOF only
   Float_t fTMass_pip_TOF;             //Fitted mass of Pip Canddidate from TOF only
   Float_t fTMass_pim_TOF;             //Fitted mass of Pim Canddidate from TOF only

   Float_t fTChi2_proton;              //Fitted chi2 of Proton Canddidate from both detectors
   Float_t fTChi2_pip;                 //Fitted chi2 of Pip Canddidate from both detectors
   Float_t fTChi2_pim;                 //Fitted chi2 of Pim Canddidate from both detectors
   Float_t fTChi2_proton_RPC;          //Fitted chi2 of Proton Canddidate from RPC only
   Float_t fTChi2_pip_RPC;             //Fitted chi2 of Pip Canddidate from RPC only
   Float_t fTChi2_pim_RPC;             //Fitted chi2 of Pim Canddidate from RPC only
   Float_t fTChi2_proton_TOF;          //Fitted chi2 of Proton Canddidate from TOF only
   Float_t fTChi2_pip_TOF;             //Fitted chi2 of Pip Canddidate from TOF only
   Float_t fTChi2_pim_TOF;             //Fitted chi2 of Pim Canddidate from TOF only

   Float_t fTMetaMatchTofMean[6];         //MetaMatch Quality for Tof (Mean Value)
   Float_t fTMetaMatchTofMax[6];          //MetaMatch Quality for Tof (Most Probable Value)
   Float_t fTMetaMatchTofMax2[6];         //MetaMatch Quality for Tof with Quality >0 (Most Probable Value)
   Float_t fTMetaMatchRpcMean[6];         //MetaMatch Quality for Rpc (Mean Value)
   Float_t fTMetaMatchRpcMax[6];          //MetaMatch Quality for Rpc (Most Probable Value)
   Float_t fTMetaMatchRpcMax2[6];         //MetaMatch Quality for Rpc with Quality >0 (Most Probable Value)
   Float_t fTMetaMatchShwMean[6];         //MetaMatch Quality for Rpc (Mean Value)
   Float_t fTMetaMatchShwMax[6];          //MetaMatch Quality for Rpc (Most Probable Value)
   Float_t fTMetaMatchShwMax2[6];         //MetaMatch Quality for Rpc with Quality >0 (Most Probable Value)

   Float_t fTMultiMax[6][2];              //Multiplicity for each sector (6) and each system (2) (Most Probable Value)
   Float_t fTMultiMean[6][2];             //Multiplicity for each sector (6) and each system (2) (Mean Value)
   Float_t fTMultiMaxSelect[6][2];        //Multiplicity for each sector (6) and each system (2) for accepted tracks (Most Probable Value)
   Float_t fTMultiMeanSelect[6][2];       //Multiplicity for each sector (6) and each system (2) for accepted tracks (Mean Value)

   Float_t fTTimeLepSumSys0Mean;          //Normalized Time of leptons in System 0 (Mean value)
   Float_t fTTimeLepSumSys0Max;           //Normalized Time of leptons in System 0 (Most Probable Value)
   Float_t fTTimeLepSumSys0Sig;           //Normalized Time of leptons in System 0 (Sigma)
   Float_t fTTimeLepStaSys0Mean[NSTART_STRIPS];       //Normalized Time of leptons in System 0 and each start strip (Mean value)
   Float_t fTTimeLepStaSys0Max[NSTART_STRIPS];        //Normalized Time of leptons in System 0 and each start strip (Most Probable Value)
   Float_t fTTimeLepStaSys0Sig[NSTART_STRIPS];        //Normalized Time of leptons in System 0 and each start strip (Sigma)
   Float_t fTTimeLepSumSys1Mean;          //Normalized Time of leptons in System 1 (Mean value)
   Float_t fTTimeLepSumSys1Max;           //Normalized Time of leptons in System 1 (Most Probable Value)
   Float_t fTTimeLepSumSys1Sig;           //Normalized Time of leptons in System 1 (Sigma)
   Float_t fTTimeLepStaSys1Mean[NSTART_STRIPS];       //Normalized Time of leptons in System 1 and each start strip (Mean value)
   Float_t fTTimeLepStaSys1Max[NSTART_STRIPS];        //Normalized Time of leptons in System 1 and each start strip (Most Probable Value)
   Float_t fTTimeLepStaSys1Sig[NSTART_STRIPS];        //Normalized Time of leptons in System 1 and each start strip (Sigma)

   Float_t fTTimePiSumSys0Mean;           //Time diff of pions in System 0 (Mean value)
   Float_t fTTimePiSumSys0Max;            //Time diff of pions in System 0 (Most Probable Value)
   Float_t fTTimePiSumSys0Sig;            //Time diff of pions in System 0 (Sigma)
   Float_t fTTimePiStaSys0Mean[NSTART_STRIPS];        //Time diff of pions in System 0 and each start strip (Mean value)
   Float_t fTTimePiStaSys0Max[NSTART_STRIPS];         //Time diff of pions in System 0 and each start strip (Most Probable Value)
   Float_t fTTimePiStaSys0Sig[NSTART_STRIPS];         //Time diff of pions in System 0 and each start strip (Sigma)
   Float_t fTTimePiSumSys1Mean;           //Time diff of pions in System 1 (Mean value)
   Float_t fTTimePiSumSys1Max;            //Time diff of pions in System 1 (Most Probable Value)
   Float_t fTTimePiSumSys1Sig;            //Time diff of pions in System 1 (Sigma)
   Float_t fTTimePiStaSys1Mean[NSTART_STRIPS];        //Time diff of pions in System 1 and each start strip (Mean value)
   Float_t fTTimePiStaSys1Max[NSTART_STRIPS];         //Time diff of pions in System 1 and each start strip (Most Probable Value)
   Float_t fTTimePiStaSys1Sig[NSTART_STRIPS];         //Time diff of pions in System 1 and each start strip (Sigma)
   Float_t fTTimePiSumSys0FitMean;        //Time diff of pions in System 0 (Fit Mean value)
   Float_t fTTimePiSumSys1FitMean;        //Time diff of pions in System 1 (Fit Mean value)
   Float_t fTTimePiStaSys0FitMean[NSTART_STRIPS];     //Time diff of pions in System 0 and each start strip (Fit Mean value)
   Float_t fTTimePiStaSys1FitMean[NSTART_STRIPS];     //Time diff of pions in System 1 and each start strip (Fit Mean value)

   Float_t fTTofdEdxMean;                 //dEdx from Tof detector (Mean value)
   Float_t fTTofdEdxMax;                  //dEdx from Tof detector (Most Probable value)


    HQAPhysicsTree(){

	fTMass_proton=-10000.0;
	fTMass_pip=-10000.0;
	fTMass_pim=-10000.0;
	fTMass_proton_RPC=-10000.0;
	fTMass_pip_RPC=-10000.0;
	fTMass_pim_RPC=-10000.0;
	fTMass_proton_TOF=-10000.0;
	fTMass_pip_TOF=-10000.0;
	fTMass_pim_TOF=-10000.0;

	fTChi2_proton=-10000.0;
	fTChi2_pip=-10000.0;
	fTChi2_pim=-10000.0;
	fTChi2_proton_RPC=-10000.0;
	fTChi2_pip_RPC=-10000.0;
	fTChi2_pim_RPC=-10000.0;
	fTChi2_proton_TOF=-10000.0;
	fTChi2_pip_TOF=-10000.0;
	fTChi2_pim_TOF=-10000.0;

        fTTimeLepSumSys0Mean=-1000.0;
	fTTimeLepSumSys0Max=-1000.0;
	fTTimeLepSumSys0Sig=-1000.0;
        fTTimeLepSumSys1Mean=-1000.0;
	fTTimeLepSumSys1Max=-1000.0;
	fTTimeLepSumSys1Sig=-1000.0;
        fTTimePiSumSys0Mean=-1000.0;
	fTTimePiSumSys0Max=-1000.0;
	fTTimePiSumSys0Sig=-1000.0;
        fTTimePiSumSys1Mean=-1000.0;
	fTTimePiSumSys1Max=-1000.0;
	fTTimePiSumSys1Sig=-1000.0;
	fTTimePiSumSys0FitMean=-1000.0;
	fTTimePiSumSys1FitMean=-1000.0;

	fTTofdEdxMean=-1000.0;
        fTTofdEdxMax=-1000.0;

	for(Int_t i=0; i<6; i++)
	{
	    fTMetaMatchTofMean[i]=-10.0;
	    fTMetaMatchTofMax[i]=-10.0;
	    fTMetaMatchTofMax2[i]=-10.0;
	    fTMetaMatchRpcMean[i]=-10.0;
	    fTMetaMatchRpcMax[i]=-10.0;
	    fTMetaMatchRpcMax2[i]=-10.0;
	    fTMetaMatchShwMean[i]=-10.0;
	    fTMetaMatchShwMax[i]=-10.0;
	    fTMetaMatchShwMax2[i]=-10.0;

	    for(Int_t j=0; j<2; j++)
	    {
		fTMultiMax[i][j]=-10.0;
		fTMultiMean[i][j]=-10.0;
		fTMultiMaxSelect[i][j]=-10.0;
                fTMultiMeanSelect[i][j]=-10.0;
	    }
	}

	for(Int_t i=0; i<NSTART_STRIPS; i++)
	{
            fTTimeLepStaSys0Mean[i]=-1000.0;
	    fTTimeLepStaSys0Max[i]=-1000.0;
	    fTTimeLepStaSys0Sig[i]=-1000.0;
            fTTimeLepStaSys1Mean[i]=-1000.0;
	    fTTimeLepStaSys1Max[i]=-1000.0;
	    fTTimeLepStaSys1Sig[i]=-1000.0;
            fTTimePiStaSys0Mean[i]=-1000.0;
	    fTTimePiStaSys0Max[i]=-1000.0;
	    fTTimePiStaSys0Sig[i]=-1000.0;
            fTTimePiStaSys1Mean[i]=-1000.0;
	    fTTimePiStaSys1Max[i]=-1000.0;
	    fTTimePiStaSys1Sig[i]=-1000.0;
	    fTTimePiStaSys0FitMean[i]=-1000.0;
            fTTimePiStaSys1FitMean[i]=-1000.0;
	}
    }

    ~HQAPhysicsTree(){}

    ClassDef(HQAPhysicsTree, 1)
};









#endif
