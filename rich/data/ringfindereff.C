{
TChain *T = new TChain("T");
T->Add("xx01112020509_DST.root");
T->Add("xx01112021206_DST.root");
T->Add("xx01112025319_DST.root");
T->Add("xx01112040044_DST.root");


TFile ff("evtsfromtargetbetterlist.root","READ");
TEventList *list = (TEventList*)ff->Get("evtsfromTarget");
T.SetEventList(list); 
TCut mdcS1="(HMdcSeg.fData.ind /(2**29) & 0x7) == 0";
TCut mdcS2="(HMdcSeg.fData.ind /(2**29) & 0x7) == 1";
TCut mdcS3="(HMdcSeg.fData.ind /(2**29) & 0x7) == 2";
TCut mdcS4="(HMdcSeg.fData.ind /(2**29) & 0x7) == 3";
TCut mdcS6="(HMdcSeg.fData.ind /(2**29) & 0x7) == 5";
TCut rS1="HRichHit.fData.nSector==0";
TCut rS2="HRichHit.fData.nSector==1";
TCut rS3="HRichHit.fData.nSector==2";
TCut rS4="HRichHit.fData.nSector==3";
TCut rS6="HRichHit.fData.nSector==5";
TCut rmth="abs(HRichHit.fData.fTheta-HMdcSeg.fData.theta*180/pi+1.5)<3";
TCut rmp6="abs(HRichHit.fData.fPhi-HMdcSeg.fData.phi*180/pi+59.5)<4";
TCut rmp1="abs(HRichHit.fData.fPhi-HMdcSeg.fData.phi*180/pi-0.5)<4";
TCut rmp2="abs(HRichHit.fData.fPhi-HMdcSeg.fData.phi*180/pi-59.5)<4";
TCut rmp3="abs(HRichHit.fData.fPhi-HMdcSeg.fData.phi*180/pi-120.0)<4";
TCut rmp4="abs(HRichHit.fData.fPhi-HMdcSeg.fData.phi*180/pi-179.0)<4";
TCut richMdcS1=mdcS1&&rS1&&rmp1;
TCut richMdcS2=mdcS2&&rS2&&rmp2;
TCut richMdcS3=mdcS3&&rS3&&rmp3;
TCut richMdcS4=mdcS4&&rS4&&rmp4;
TCut richMdcS6=mdcS6&&rS6&&rmp6;
TCut tofS1="HTofHit.fData.sector==0&&HTofHit.fData.tof>0";
TCut tofS2="HTofHit.fData.sector==1&&HTofHit.fData.tof>0";
TCut tofS3="HTofHit.fData.sector==2&&HTofHit.fData.tof>0";
TCut tofS4="HTofHit.fData.sector==3&&HTofHit.fData.tof>0";
TCut tofS6="HTofHit.fData.sector==5&&HTofHit.fData.tof>0";
TCut tmph1="abs(HTofHit.fData.phi-HMdcSeg.fData.phi*180/pi)<4";
TCut tmph2="abs(HTofHit.fData.phi-HMdcSeg.fData.phi*180/pi-60.0)<4";
TCut tmph3="abs(HTofHit.fData.phi-HMdcSeg.fData.phi*180/pi-120.0)<4";
TCut tmph4="abs(HTofHit.fData.phi-HMdcSeg.fData.phi*180/pi-180.0)<4";
TCut tmph6="abs(HTofHit.fData.phi-HMdcSeg.fData.phi*180/pi+60.0)<4";
TCut richMdcTofS1=mdcS1&&rS1&&rmp1&&rmth&&tofS1&&tmph1;
TCut richMdcTofS2=mdcS2&&rS2&&rmp2&&rmth&&tofS2&&tmph2;
TCut richMdcTofS3=mdcS3&&rS3&&rmp3&&rmth&&tofS3&&tmph3;
TCut richMdcTofS4=mdcS4&&rS4&&rmp4&&rmth&&tofS4&&tmph4;
TCut richMdcTofS6=mdcS6&&rS6&&rmp6&&rmth&&tofS6&&tmph6;
TCut tmth="abs(HTofHit.fData.theta-HMdcSeg.fData.theta*180/pi-2.)<3";
TCut mdcTofS1=mdcS1&&tofS1&&tmph1;
TCut mdcTofS2=mdcS2&&tofS2&&tmph2;
TCut mdcTofS3=mdcS3&&tofS3&&tmph3;
TCut mdcTofS4=mdcS4&&tofS4&&tmph4;
TCut mdcTofS6=mdcS6&&tofS6&&tmph6;
TCut start="abs(HTofHit.fData.theta-HMdcSeg.fData.theta*180/pi+19.0)<4";
TCut tofundso ="HTofHit.fData.tof<7.5&&abs(HTofHit.fData.theta-HMdcSeg.fData.theta*180/pi)<3";

TH1F *h1 = new TH1F("t","t",100,0,1300);
const Int_t punti = 100;
Double_t *entries = new Double_t[punti];
char *cond1 = new char[40];
for (Int_t helper=0;helper<punti;helper++) entries[helper]=0;

for (Int_t i=0;i<punti;i++){
    int j = i*TMath::Nint(1200./punti);
    sprintf(cond1,"HRichHit.fData.iRingHouTra>%d",j);
    TCut qualcut=cond1;
    T.Draw("HRichHit.fData.iRingAmplitude>>t",qualcut);
    //T.Draw("HRichHit.fData.iRingAmplitude>>t",!(!richMdcTofS2&&!richMdcTofS3)&&tofundso&&qualcut);
    //T.Draw("HRichHit.fData.iRingAmplitude>>t",qualcut);
    cout<<"min HT qual: "<<j<<endl;
    entries[i] = t->GetEntries();
    cout<<"ring yield: "<<entries[i]<<endl;
    
}

Double_t x_axis[punti];
for (int m=0;m<punti;m++) x_axis[m]=m*TMath::Nint(1200./punti);
TGraph *graph = new TGraph(punti,x_axis,entries);
graph->Draw("AC*");
}
