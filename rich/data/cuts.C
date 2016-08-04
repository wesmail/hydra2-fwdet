{
//select sectors and positive tof
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

TCut tofS1="HTofHit.fData.sector==0&&HTofHit.fData.tof>0";
TCut tofS2="HTofHit.fData.sector==1&&HTofHit.fData.tof>0";
TCut tofS3="HTofHit.fData.sector==2&&HTofHit.fData.tof>0";
TCut tofS4="HTofHit.fData.sector==3&&HTofHit.fData.tof>0";
TCut tofS6="HTofHit.fData.sector==5&&HTofHit.fData.tof>0";

TCut showS1="HShowerHitTof.fData.m_nSector==0&&HShowerHitTof.fData.m_fTof>0";
TCut showS2="HShowerHitTof.fData.m_nSector==1&&HShowerHitTof.fData.m_fTof>0";
TCut showS3="HShowerHitTof.fData.m_nSector==2&&HShowerHitTof.fData.m_fTof>0";
TCut showS4="HShowerHitTof.fData.m_nSector==3&&HShowerHitTof.fData.m_fTof>0";
TCut showS5="HShowerHitTof.fData.m_nSector==4&&HShowerHitTof.fData.m_fTof>0";
TCut showS6="HShowerHitTof.fData.m_nSector==5&&HShowerHitTof.fData.m_fTof>0";

// SPATIAL CORRELATIONS

//select RICH and MDC plane 2 hits correlated in polar and azimuthal angle
TCut rmth="abs(HRichHit.fData.fTheta-HMdcSeg.fData.theta*180/pi)<3";
TCut rmp6="abs(HRichHit.fData.fPhi-HMdcSeg.fData.phi*180/pi+60)<4";
TCut rmp1="abs(HRichHit.fData.fPhi-HMdcSeg.fData.phi*180/pi)<4";
TCut rmp2="abs(HRichHit.fData.fPhi-HMdcSeg.fData.phi*180/pi-60)<4";
TCut rmp3="abs(HRichHit.fData.fPhi-HMdcSeg.fData.phi*180/pi-120.0)<4";
TCut rmp4="abs(HRichHit.fData.fPhi-HMdcSeg.fData.phi*180/pi-180.0)<4";

// no spatial correlation, only sectors RICH and MDC
TCut richMdcS1=mdcS1&&rS1;
TCut richMdcS2=mdcS2&&rS2;
TCut richMdcS3=mdcS3&&rS3;
TCut richMdcS4=mdcS4&&rS4;
TCut richMdcS6=mdcS6&&rS6;

// phi correlation and sector selection  RICH and MDC
TCut richMdcS1Phi=mdcS1&&rS1&&rmp1;
TCut richMdcS2Phi=mdcS2&&rS2&&rmp2;
TCut richMdcS3Phi=mdcS3&&rS3&&rmp3;
TCut richMdcS4Phi=mdcS4&&rS4&&rmp4;
TCut richMdcS6Phi=mdcS6&&rS6&&rmp6;

//select Shower and MDC plane 2 hits corr in azimuthal and polar angle
TCut stmth="abs(HShowerHitTof.fData.m_fTheta-HMdcSeg.fData.theta*180/pi)<3";
TCut stmph1="abs(HShowerHitTof.fData.m_fPhi-HMdcSeg.fData.phi*180/pi)<4";
TCut stmph2="abs(HShowerHitTof.fData.m_fPhi-HMdcSeg.fData.phi*180/pi-60.)<4";
TCut stmph3="abs(HShowerHitTof.fData.m_fPhi-HMdcSeg.fData.phi*180/pi-120.)<4";
TCut stmph4="abs(HShowerHitTof.fData.m_fPhi-HMdcSeg.fData.phi*180/pi-180.)<4";
TCut stmph5="abs(HShowerHitTof.fData.m_fPhi-HMdcSeg.fData.phi*180/pi)<4";
TCut stmph6="abs(HShowerHitTof.fData.m_fPhi-HMdcSeg.fData.phi*180/pi+60.)<4";

//select TOF and MDC plane 2 hits corr in azimuthal and polar angle
TCut tmth ="abs(HTofHit.fData.theta-HMdcSeg.fData.theta*180/pi)<3";
TCut tmph1="abs(HTofHit.fData.phi-HMdcSeg.fData.phi*180/pi)<4";
TCut tmph2="abs(HTofHit.fData.phi-HMdcSeg.fData.phi*180/pi-60.0)<4";
TCut tmph3="abs(HTofHit.fData.phi-HMdcSeg.fData.phi*180/pi-120.0)<4";
TCut tmph4="abs(HTofHit.fData.phi-HMdcSeg.fData.phi*180/pi-180.0)<4";
TCut tmph6="abs(HTofHit.fData.phi-HMdcSeg.fData.phi*180/pi+60.0)<4";

//select RICH and TOF hits corr in azimuthal and polar angle
TCut rtth="abs(HRichHit.fData.fTheta-HTofHit.fData.theta)<3";
TCut rtph="abs(HRichHit.fData.fPhi-HTofHit.fData.phi)<4";

//sector selection RICH and TOF 
TCut richTofS1=rS1&&tofS1;
TCut richTofS2=rS2&&tofS2;
TCut richTofS3=rS3&&tofS3;
TCut richTofS4=rS4&&tofS4;
TCut richTofS6=rS6&&tofS6;

//select RICH and SHOWER hits corr in azimuthal and polar angle 
TCut rsth="abs(HRichHit.fData.fTheta-HShowerHitTof.fData.m_fTheta)<3";
TCut rsph="abs(HRichHit.fData.fPhi-HShowerHitTof.fData.m_fPhi)<4";

//sector selection RICH and SHOWER
TCut richShowS1=rS1&&showS1;
TCut richShowS2=rS2&&showS2;
TCut richShowS3=rS3&&showS3;
TCut richShowS4=rS4&&showS4;
TCut richShowS6=rS6&&showS6;

//select RICH,MDC and TOF hits corr in azimuthal and polar angle
TCut richMdcTofS1=mdcS1&&rS1&&tofS1&&rmp1&&rmth&&tmph1&&tmth;
TCut richMdcTofS2=mdcS2&&rS2&&tofS2&&rmp2&&rmth&&tmph2&&tmth;
TCut richMdcTofS3=mdcS3&&rS3&&tofS3&&rmp3&&rmth&&tmph3&&tmth;
TCut richMdcTofS4=mdcS4&&rS4&&tofS4&&rmp4&&rmth&&tmph4&&tmth;
TCut richMdcTofS6=mdcS6&&rS6&&tofS6&&rmp6&&rmth&&tmph6&&tmth;

//select RICH,MDC and SHOWER hits corr in azimuthal and polar angle
TCut richMdcShowS1=mdcS1&&rS1&&showS1&&rmp1&&rmth&&stmph1&&stmth;
TCut richMdcShowS2=mdcS2&&rS2&&showS2&&rmp2&&rmth&&stmph2&&stmth;
TCut richMdcShowS3=mdcS3&&rS3&&showS3&&rmp3&&rmth&&stmph3&&stmth;
TCut richMdcShowS4=mdcS4&&rS4&&showS4&&rmp4&&rmth&&stmph4&&stmth;
TCut richMdcShowS6=mdcS6&&rS6&&showS6&&rmp6&&rmth&&stmph6&&stmth;

//cut on time of flight from TOF wall
TCut toftof="HTofHit.fData.tof<7.5";
//cut on time of flight from TOFINO
TCut tofinotof="(HShowerHitTof.fData.m_fTof>5.)&&(HShowerHitTof.fData.m_fTof<9.)";
//cut on SHOWER condition for lepton
TCut showlep="HShowerHitTof.fData.m_nModule==0&&(HShowerHitTof.fData.m_fSum1/HShowerHitTof.fData.m_fSum0>2.||HShowerHitTof.fData.m_fSum2/HShowerHitTof.fData.m_fSum0>2.)";
//select the start detector as emission point ?
TCut start="abs(HTofHit.fData.theta-HMdcSeg.fData.theta*180/pi+19.0)<4";


}
