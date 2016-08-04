//*-- AUTHOR : Olga Pechenova
//*-- Modified : 2006 by Olga Pechenova
//*-- Modified : 2013 by Olga Pechenova
//*-- Modified : 2015 by Olga Pechenova
//
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////
//
//  HEnergyLossCorrPar
//
//  Container class keep parameters for energy loss correction
//
//
//  Double_t getDeltaMom(Int_t pId, Double_t mom, Double_t theta);
//     Function getDeltaMom returns the correction value (delP) for momentum.
//     Parameters: - pId is particle ID. If parameter pId is not correct
//  function getDeltaMom returns 0.
//  - mom is Runge-Kutta reconstructed momentum (MeV).
//  - theta is theta angle in the interval [15;87] (degrees).
//  If parameter theta is out of this interval
//  function getDeltaMom returns 0.
//
//  Double_t getCorrMom(Int_t pId, Double_t mom, Double_t theta) {
//          return mom+getDeltaMom(pId,mom,theta);}
//     Function getCorrMom returns the corrected momentum.
//     Parameters have the same meanings as for function getDeltaMom.
//
//
//  Alternative method of getting parameters (without usage of runTimeDb):
//
//  Bool_t  setDefaultPar(const Char_t *rn);
//     Function setDefaultPar sets default values to parameters
//     for corresponding beam time.
//
//  Example:
//        HEnergyLossCorrPar enLossCorr;
//        enLossCorr.setDefaultPar("jan04"); // or "nov02", or "aug04", or "apr12", or 
//        "jul14_W" - Wolfram target, "jul14_C3" - Carbon 3 segments, "jul14_PE" & "aug14_PE" - PE target, "aug14_C7" - Carbon 7 segments
//        Double_t delP;
//        ..............................
//        beginning of your loop;
//          ..............................................
//          delP = enLossCorr.getDeltaMom(14, mom, theta);
//          ..............................................
//        end of your loop;
/////////////////////////////////////////////////////////////////////////

#include "TMath.h"
#include "TString.h"
#include "henergylosscorrpar.h"
#include "hphysicsconstants.h"
#include "hparamlist.h"

#include <stdlib.h>
#include <cmath>

using namespace TMath;

HEnergyLossCorrPar* HEnergyLossCorrPar::gEnergyLossCorrPar = NULL;

ClassImp(HEnergyLossCorrPar)

HEnergyLossCorrPar::HEnergyLossCorrPar(const Char_t* name,const Char_t* title, const Char_t* context) {
  // constructor
  momInt1=720.;
  momInt2=2000.;
  nThetaReg=7;
  nParams=10;
  thetaStep=10.;
  thetaMiddle0=23.;
  for(Int_t i=0;i<nThetaReg*nParams;i++) parMomCorrH[i]=0.;
// for electrons
  nParamsElect=5;
  for(Int_t i=0;i<nParamsElect;i++) parMomCorrElect[i]=0.;
  typePar = 0;
  gEnergyLossCorrPar =  this;
}

void HEnergyLossCorrPar::clear(void) {
   for(Int_t i=0;i<nThetaReg*nParams;i++) parMomCorrH[i]=0.;
  for(Int_t i=0;i<nParamsElect;i++) parMomCorrElect[i]=0.;
  status=kFALSE;
  resetInputVersions();
  changed=kFALSE;
  typePar = 0;
}

void HEnergyLossCorrPar::putParams(HParamList* l) {
  // puts all parameters to the parameter list, which is used by the io
  if (!l) return;
  l->add("momInt1",        momInt1);
  l->add("momInt2",        momInt2);
  l->add("nThetaReg",      nThetaReg);
  l->add("nParams",    nParams);
  l->add("parMomCorrH", parMomCorrH,nThetaReg*nParams);
  l->add("thetaMiddle0",   thetaMiddle0);
  l->add("thetaStep",      thetaStep);

  l->add("nParamsElect",    nParamsElect);
  l->add("parMomCorrElect", parMomCorrElect,nParamsElect);
}


Bool_t HEnergyLossCorrPar::getParams(HParamList* l) {
  // gets all parameters from the parameter list, which is used by the io
  if (!l) return kFALSE;
  if (!(l->fill("momInt1",       &momInt1)))          return kFALSE;
  if (!(l->fill("momInt2",       &momInt2)))          return kFALSE;
  if (!(l->fill("nThetaReg",     &nThetaReg)))        return kFALSE;
  if (!(l->fill("nParams",   &nParams)))      return kFALSE;
  if (!(l->fill("parMomCorrH",parMomCorrH,nThetaReg*nParams)))   return kFALSE;
  if (!(l->fill("thetaMiddle0",  &thetaMiddle0)))     return kFALSE;
  if (!(l->fill("thetaStep",     &thetaStep)))        return kFALSE;

  if (!(l->fill("nParamsElect",       &nParamsElect)))     return kFALSE;
  if (!(l->fill("parMomCorrElect",parMomCorrElect,nParamsElect)))   return kFALSE;
  return kTRUE;
}

Bool_t HEnergyLossCorrPar::setDefaultPar(TString run) {
  run.ToLower();
    if(run == "jul14_w") {
	Info("setDefaultPar()","Setting dedx correction table for %s",run.Data());
    nParams      = 12;
    nThetaReg    = 22; //from 20 grad till 86 grad
    thetaStep    = 3.;
    thetaMiddle0 = 21.5;
    momInt1      = 700;
    momInt2      = 2500;
    Double_t par[12*22] = {
      340.901,-1.50393,0.00278068,-0.00000238587,0.0000778523/100000.,119.233,-0.00334163,-0.00093208,8.25497,16.7969,-0.00141246,5.51394,
      359.447,-1.76346,0.00366044,-0.00000356899,0.000134229/100000.,104.042,-0.0033692,-0.000844377,7.23206,19.212,-0.00150658,4.77071,
      359.447,-1.76346,0.00366044,-0.00000356899,0.000134229/100000.,104.042,-0.0033692,-0.000844377,7.23206,19.212,-0.00150658,4.77071,
      359.447,-1.76346,0.00366044,-0.00000356899,0.000134229/100000.,104.042,-0.0033692,-0.000844377,7.23206,19.212,-0.00150658,4.77071,
      365.447,-1.79692,0.00377476,-0.00000375582,0.000145219/100000.,106.245,-0.00332119,-0.000823538,7.47061,22.6797,-0.00154827,5.03414,
      365.447,-1.79692,0.00377476,-0.00000375582,0.000145219/100000.,106.245,-0.00332119,-0.000823538,7.47061,22.6797,-0.00154827,5.03414,
      365.447,-1.79692,0.00377476,-0.00000375582,0.000145219/100000.,106.245,-0.00332119,-0.000823538,7.47061,22.6797,-0.00154827,5.03414,
      365.447,-1.79692,0.00377476,-0.00000375582,0.000145219/100000.,106.245,-0.00332119,-0.000823538,7.47061,22.6797,-0.00154827,5.03414,
      363.522,-1.69303,0.00336942,-0.00000317385,0.000116155/100000.,122.517,-0.00337433,-0.000959434,8.43587,29.2724,-0.00161355,5.63564,
      363.522,-1.69303,0.00336942,-0.00000317385,0.000116155/100000.,122.517,-0.00337433,-0.000959434,8.43587,29.2724,-0.00161355,5.63564,
      363.522,-1.69303,0.00336942,-0.00000317385,0.000116155/100000.,122.517,-0.00337433,-0.000959434,8.43587,29.2724,-0.00161355,5.63564,
      379.355,-1.74201,0.00348639,-0.00000335548,0.000127136/100000.,141.13,-0.00337822,-0.00111639,9.76923,25.7175,-0.00151109,6.5268,
      379.355,-1.74201,0.00348639,-0.00000335548,0.000127136/100000.,141.13,-0.00337822,-0.00111639,9.76923,25.7175,-0.00151109,6.5268,
      379.355,-1.74201,0.00348639,-0.00000335548,0.000127136/100000.,141.13,-0.00337822,-0.00111639,9.76923,25.7175,-0.00151109,6.5268,
      377.386,-1.61336,0.00297582,-0.00000259877,0.0000877779/100000.,147.441,-0.00325714,-0.00111654,10.7727,36.4515,-0.00158623,7.42574,
      368.669,-1.47622,0.00249971,-0.00000193881,0.0000549969/100000.,167.991,-0.0033598,-0.00137807,12.0193,28.8549,-0.00149969,8.04572,
      349.535,-1.24392,0.00174129,-0.00000092267,0.00000571075/100000.,166.974,-0.00325946,-0.00146554,12.9929,43.945,-0.00161646,8.77291,
      375.433,-1.41188,0.00225412,-0.00000162605,0.0000408913/100000.,185.174,-0.00326443,-0.00147595,13.9235,35.1007,-0.00147076,9.54521,
      367.516,-1.24152,0.00163618,-0.000000760779,-0.00000162891/100000.,202.942,-0.00326404,-0.00166685,15.5439,53.5905,-0.00159346,10.6089,
      367.516,-1.24152,0.00163618,-0.000000760779,-0.00000162891/100000.,202.942,-0.00326404,-0.00166685,15.5439,53.5905,-0.00159346,10.6089,
      410.091,-1.48901,0.00236275,-0.00000179646,0.0000538164/100000,211.017,-0.00321069,-0.00173945,16.8942,34.2646,-0.00141895,11.7299,
      338.12,-0.750049,0.0000830358,0.00000112099,-0.0000814397/100000.,249.746,-0.00326935,-0.00196529,18.2082,36.0361,-0.0014173,12.3444
      };
    fillParMomCorrH(nThetaReg*nParams,par);

    //for electrons pol6
    nParamsElect=7;
    parMomCorrElect[0] = 36.8903;
    parMomCorrElect[1] = -3.45134;
    parMomCorrElect[2] = 0.140583;
    parMomCorrElect[3] = -0.00259951;
    parMomCorrElect[4] = 0.0000198486;
    parMomCorrElect[5] = -0.000662151/100000.;
    parMomCorrElect[6] = -0.0000403811/100000.;

    typePar = 3;
  } else  if(run == "jul14_c3") {
	Info("setDefaultPar()","Setting dedx correction table for %s",run.Data());
    nParams      = 12;
    nThetaReg    = 22; //from 20 grad till 86 grad
    thetaStep    = 3.;
    thetaMiddle0 = 21.5;
    momInt1      = 700;
    momInt2      = 2500;
    Double_t par[12*22] = {
      363.813,-1.87234,0.00404871,-0.0000041178,0.00016226/100000.,90.8896,-0.00338883,-0.000731713,5.95128,5.35734,-0.000983874,3.73794,
      340.39,-1.78571,0.00390809,-0.00000400218,0.000158149/100000.,82.3474,-0.00344345,-0.000638642,5.1968,5.85834,-0.00108304,3.2622, 
      340.39,-1.78571,0.00390809,-0.00000400218,0.000158149/100000.,82.3474,-0.00344345,-0.000638642,5.1968,5.85834,-0.00108304,3.2622, 
      340.39,-1.78571,0.00390809,-0.00000400218,0.000158149/100000.,82.3474,-0.00344345,-0.000638642,5.1968,5.85834,-0.00108304,3.2622, 
      339.486,-1.78318,0.00390811,-0.00000400864,0.000158707/100000.,81.6281,-0.00343837,-0.000641385,5.19119,5.62965,-0.00106968,3.25296, 
      339.486,-1.78318,0.00390811,-0.00000400864,0.000158707/100000.,81.6281,-0.00343837,-0.000641385,5.19119,5.62965,-0.00106968,3.25296, 
      339.486,-1.78318,0.00390811,-0.00000400864,0.000158707/100000.,81.6281,-0.00343837,-0.000641385,5.19119,5.62965,-0.00106968,3.25296, 
      339.486,-1.78318,0.00390811,-0.00000400864,0.000158707/100000.,81.6281,-0.00343837,-0.000641385,5.19119,5.62965,-0.00106968,3.25296, 
      345.581,-1.79395,0.00389273,-0.00000395957,0.000155694/100000.,84.0997,-0.00341154,-0.000663507,5.42551,5.65421,-0.00105822,3.41813,
      345.581,-1.79395,0.00389273,-0.00000395957,0.000155694/100000.,84.0997,-0.00341154,-0.000663507,5.42551,5.65421,-0.00105822,3.41813,
      345.581,-1.79395,0.00389273,-0.00000395957,0.000155694/100000.,84.0997,-0.00341154,-0.000663507,5.42551,5.65421,-0.00105822,3.41813,
      351.345,-1.81068,0.00390588,-0.00000395676,0.000155283/100000.,88.5021,-0.00344,-0.000700565,5.64477,5.92098,-0.00106608,3.53526, 
      351.345,-1.81068,0.00390588,-0.00000395676,0.000155283/100000.,88.5021,-0.00344,-0.000700565,5.64477,5.92098,-0.00106608,3.53526, 
      351.345,-1.81068,0.00390588,-0.00000395676,0.000155283/100000.,88.5021,-0.00344,-0.000700565,5.64477,5.92098,-0.00106608,3.53526, 
      355.35,-1.82723,0.00394577,-0.00000401542,0.000158839/100000.,92.513,-0.00347466,-0.00074445,5.83361,6.01486,-0.00106656,3.61997, 
      358.09,-1.82336,0.00386925,-0.00000383628,0.000146582/100000.,89.2857,-0.00340005,-0.000704394,5.80144,6.47893,-0.00108469,3.66993, 
      363.225,-1.86139,0.00399638,-0.00000402954,0.00015732/100000.,94.2199,-0.00345562,-0.000745102,5.96009,6.5169,-0.00108865,3.7278,
      367.319,-1.87481,0.00399567,-0.00000398468,0.000153338/100000.,92.8587,-0.00340985,-0.000728312,5.97965,6.31287,-0.00106943,3.78329, 
      365.256,-1.83977,0.00388251,-0.00000384856,0.000147824/100000.,96.1542,-0.00342691,-0.000763212,6.18846,6.87292,-0.00108557,3.89155, 
      365.256,-1.83977,0.00388251,-0.00000384856,0.000147824/100000.,96.1542,-0.00342691,-0.000763212,6.18846,6.87292,-0.00108557,3.89155, 
      377.967,-1.89942,0.0039926,-0.00000393773,0.000150343/100000.,99.3301,-0.00345412,-0.000839982,6.4699,6.84137,-0.00107652,3.98033,
      377.967,-1.89942,0.0039926,-0.00000393773,0.000150343/100000.,99.3301,-0.00345412,-0.000839982,6.4699,6.84137,-0.00107652,3.98033
      };
    fillParMomCorrH(nThetaReg*nParams,par);

    //for electrons pol6
    nParamsElect=7;
    parMomCorrElect[0] = 19.0501;
    parMomCorrElect[1] = -1.78541;
    parMomCorrElect[2] = 0.0799932;
    parMomCorrElect[3] = -0.00181062;
    parMomCorrElect[4] = 0.0000220232;
    parMomCorrElect[5] = -0.000000136325;
    parMomCorrElect[6] = 0.0000334713/100000.;

    typePar = 3;
  } else if(run == "jul14_pe" || run == "aug14_pe") {
	Info("setDefaultPar()","Setting dedx correction table for %s",run.Data());
    nParams      = 12;
    nThetaReg    = 22; //from 20 grad till 86 grad
    thetaStep    = 3.;
    thetaMiddle0 = 21.5;
    momInt1      = 700;
    momInt2      = 2500;
    Double_t par[12*22] = {
      395.219,-1.94427,0.00406475,-0.00000403356,0.000156339/100000.,113.782,-0.00340005,-0.000931158,7.49854,6.7844,-0.000981211,4.68952, 
      366.057,-1.81668,0.00377552,-0.00000368103,0.000138881/100000.,101.211,-0.00345251,-0.000818066,6.47664,6.09025,-0.00101831,4.02327, 
      366.057,-1.81668,0.00377552,-0.00000368103,0.000138881/100000.,101.211,-0.00345251,-0.000818066,6.47664,6.09025,-0.00101831,4.02327, 
      366.057,-1.81668,0.00377552,-0.00000368103,0.000138881/100000.,101.211,-0.00345251,-0.000818066,6.47664,6.09025,-0.00101831,4.02327, 
      352.218,-1.79696,0.00382033,-0.00000380159,0.000146201/100000.,90.0212,-0.00345928,-0.00069451,5.62582,6.19342,-0.00108187,3.53111, 
      352.218,-1.79696,0.00382033,-0.00000380159,0.000146201/100000.,90.0212,-0.00345928,-0.00069451,5.62582,6.19342,-0.00108187,3.53111, 
      352.218,-1.79696,0.00382033,-0.00000380159,0.000146201/100000.,90.0212,-0.00345928,-0.00069451,5.62582,6.19342,-0.00108187,3.53111, 
      352.218,-1.79696,0.00382033,-0.00000380159,0.000146201/100000.,90.0212,-0.00345928,-0.00069451,5.62582,6.19342,-0.00108187,3.53111, 
      354.009,-1.86253,0.00408366,-0.00000419045,0.000166054/100000.,83.2562,-0.00343848,-0.000651417,5.28283,6.08154,-0.00110533,3.32809,
      354.009,-1.86253,0.00408366,-0.00000419045,0.000166054/100000.,83.2562,-0.00343848,-0.000651417,5.28283,6.08154,-0.00110533,3.32809,
      354.009,-1.86253,0.00408366,-0.00000419045,0.000166054/100000.,83.2562,-0.00343848,-0.000651417,5.28283,6.08154,-0.00110533,3.32809,
      348.382,-1.8238,0.00396499,-0.0000040237,0.000157397/100000.,82.5202,-0.00345177,-0.000645806,5.19877,6.15786,-0.00112109,3.26955, 
      348.382,-1.8238,0.00396499,-0.0000040237,0.000157397/100000.,82.5202,-0.00345177,-0.000645806,5.19877,6.15786,-0.00112109,3.26955, 
      348.382,-1.8238,0.00396499,-0.0000040237,0.000157397/100000.,82.5202,-0.00345177,-0.000645806,5.19877,6.15786,-0.00112109,3.26955, 
      353.894,-1.87342,0.00412526,-0.00000424506,0.000168473/100000.,83.1282,-0.00346394,-0.000648254,5.20019,6.28797,-0.00113295,3.27027, 
      353.85,-1.86659,0.00409331,-0.00000419413,0.00016577/100000.,83.926,-0.00347166,-0.000660473,5.2443,6.19997,-0.00112843,3.28492, 
      351.858,-1.84545,0.00402381,-0.00000409948,0.000161127/100000.,84.8005,-0.00347937,-0.000657066,5.26311,6.31226,-0.00113253,3.30581,
      356.568,-1.87939,0.00411911,-0.00000421744,0.000166505/100000.,86.025,-0.00349049,-0.00067037,5.32277,5.9717,-0.00111004,3.33111, 
      357.728,-1.87848,0.00410439,-0.00000419094,0.000165047/100000.,85.0026,-0.00344817,-0.000657249,5.3514,6.1102,-0.00111809,3.3878, 
      357.728,-1.87848,0.00410439,-0.00000419094,0.000165047/100000.,85.0026,-0.00344817,-0.000657249,5.3514,6.1102,-0.00111809,3.3878, 
      358.954,-1.877,0.00408277,-0.00000415016,0.000162742/100000.,78.3719,-0.0033126,-0.000622673,5.28907,7.45359,-0.00119627,3.41713,
      358.954,-1.877,0.00408277,-0.00000415016,0.000162742/100000.,78.3719,-0.0033126,-0.000622673,5.28907,7.45359,-0.00119627,3.41713
      };
    fillParMomCorrH(nThetaReg*nParams,par);

    //for electrons pol6
    nParamsElect=7;
    parMomCorrElect[0] = 15.1994;
    parMomCorrElect[1] = -1.15608;
    parMomCorrElect[2] = 0.0509546;
    parMomCorrElect[3] = -0.00122827;
    parMomCorrElect[4] = 0.0000165499;
    parMomCorrElect[5] = -0.000000116424;
    parMomCorrElect[6] = 0.0000333018/100000.;

    typePar = 3;
  } else if(run == "aug14_c7") {
	Info("setDefaultPar()","Setting dedx correction table for %s",run.Data());
    nParams      = 12;
    nThetaReg    = 22; //from 20 grad till 86 grad
    thetaStep    = 3.;
    thetaMiddle0 = 21.5;
    momInt1      = 700;
    momInt2      = 2500;
    Double_t par[12*22] = {
      377.29,-1.86591,0.00389833,-0.00000384956,0.000147982/100000.,108.716,-0.00346533,-0.000906983,7.00846,6.17732,-0.000989527,4.2911, 
      362.489,-1.8547,0.00397136,-0.00000398657,0.000154622/100000.,91.5262,-0.00341486,-0.000743405,5.95775,6.29919,-0.00105339,3.71595, 
      362.489,-1.8547,0.00397136,-0.00000398657,0.000154622/100000.,91.5262,-0.00341486,-0.000743405,5.95775,6.29919,-0.00105339,3.71595, 
      362.489,-1.8547,0.00397136,-0.00000398657,0.000154622/100000.,91.5262,-0.00341486,-0.000743405,5.95775,6.29919,-0.00105339,3.71595, 
      349.768,-1.84078,0.00403105,-0.00000412447,0.000162726/100000.,81.3723,-0.00342136,-0.000642395,5.22003,5.82418,-0.00108067,3.27976, 
      349.768,-1.84078,0.00403105,-0.00000412447,0.000162726/100000.,81.3723,-0.00342136,-0.000642395,5.22003,5.82418,-0.00108067,3.27976, 
      349.768,-1.84078,0.00403105,-0.00000412447,0.000162726/100000.,81.3723,-0.00342136,-0.000642395,5.22003,5.82418,-0.00108067,3.27976, 
      349.768,-1.84078,0.00403105,-0.00000412447,0.000162726/100000.,81.3723,-0.00342136,-0.000642395,5.22003,5.82418,-0.00108067,3.27976, 
      338.529,-1.81398,0.00403322,-0.00000418336,0.000167129/100000.,76.5941,-0.00346187,-0.000600158,4.79564,5.78833,-0.00112573,3.00538,
      338.529,-1.81398,0.00403322,-0.00000418336,0.000167129/100000.,76.5941,-0.00346187,-0.000600158,4.79564,5.78833,-0.00112573,3.00538,
      338.529,-1.81398,0.00403322,-0.00000418336,0.000167129/100000.,76.5941,-0.00346187,-0.000600158,4.79564,5.78833,-0.00112573,3.00538,
      337.236,-1.79613,0.00397232,-0.00000409852,0.000162844/100000.,77.4287,-0.00345466,-0.000610697,4.88556,5.68106,-0.00111172,3.06302, 
      337.236,-1.79613,0.00397232,-0.00000409852,0.000162844/100000.,77.4287,-0.00345466,-0.000610697,4.88556,5.68106,-0.00111172,3.06302, 
      337.236,-1.79613,0.00397232,-0.00000409852,0.000162844/100000.,77.4287,-0.00345466,-0.000610697,4.88556,5.68106,-0.00111172,3.06302, 
      344.483,-1.82317,0.00401882,-0.00000414132,0.000164604/100000.,79.2906,-0.00340516,-0.000610728,5.07564,6.62067,-0.00114708,3.23332, 
      349.516,-1.83814,0.00402836,-0.00000412621,0.000162918/100000.,82.5493,-0.00342159,-0.00064238,5.28058,6.18647,-0.00111463,3.34096, 
      349.407,-1.80421,0.00387578,-0.00000388649,0.000150131/100000.,87.0329,-0.00345109,-0.000670834,5.47806,6.88032,-0.00113039,3.45412,
      356.488,-1.84309,0.00397902,-0.00000402071,0.000156785/100000.,88.8678,-0.00343584,-0.000695147,5.65209,6.5324,-0.00110471,3.56262, 
      362.666,-1.85719,0.00397536,-0.00000398646,0.000154446/100000.,93.5664,-0.0034514,-0.000736251,5.93534,6.36717,-0.00107681,3.72505, 
      362.666,-1.85719,0.00397536,-0.00000398646,0.000154446/100000.,93.5664,-0.0034514,-0.000736251,5.93534,6.36717,-0.00107681,3.72505, 
      377.254,-1.89525,0.00397359,-0.00000390074,0.00014792/100000.,95.1167,-0.00338462,-0.000754639,6.17869,7.24617,-0.00110272,3.90527,
      377.254,-1.89525,0.00397359,-0.00000390074,0.00014792/100000.,95.1167,-0.00338462,-0.000754639,6.17869,7.24617,-0.00110272,3.90527
      };
    fillParMomCorrH(nThetaReg*nParams,par);

    //for electrons pol6
    nParamsElect=7;
    parMomCorrElect[0] = 22.9808;
    parMomCorrElect[1] = -2.53549;
    parMomCorrElect[2] = 0.139744;
    parMomCorrElect[3] = -0.00403039;
    parMomCorrElect[4] = 0.0000628111;
    parMomCorrElect[5] = -0.000000498781;
    parMomCorrElect[6] = 0.000158241/100000.;

    typePar = 3;
  } else if(run == "apr12") {
	Info("setDefaultPar()","Setting dedx correction table for %s",run.Data());
    nParams      = 7;
    nThetaReg    = 22;
    thetaStep    = 3.;
    thetaMiddle0 = 21.5;
    Double_t par[7*22] = {
      -91.8651,0.0434063,678.858,-1120.65,992.81,-617.704,161.391,
      -98.9038,0.032164,717.974,-1187.02,1071.36,-686.091,185.454,
      -98.6181,0.0339081,716.166,-1183.66,1066.6,-681.832,184.06,
      -94.5398,0.0440444,696.528,-1155.27,1041.17,-663.52,178.263,
      -97.3125,0.0402414,713.659,-1185.34,1075.45,-691.278,187.469,
      -96.829,0.0388894,711.733,-1182.84,1073.93,-691.003,187.68,
      -99.0192,0.0378554,724.509,-1204.06,1095.97,-707.519,192.79,
      -98.4861,0.0402044,722.362,-1200.01,1089.7,-701.493,190.621,
      -100.162,0.0366257,731.766,-1215.2,1105.28,-713.395,194.44,
      -95.8465,0.0382859,708.559,-1184.38,1087.36,-706.755,193.663,
      -69.4872,0.0325539,550.01,-938.753,876.194,-573.516,157.748,
      -63.2317,0.0276368,511.495,-889.159,856.492,-575.452,161.753,
      -64.3656,0.0312026,520.12,-904.467,872.46,-587.034,165.186,
      -68.398,0.0260268,542.253,-938.799,904.784,-610.257,172.378,
      -69.6065,0.0253658,549.615,-949.945,913.837,-615.737,173.836,
      -71.149,0.0273896,558.977,-963.62,923.416,-620.118,174.521,
      -70.434,0.0328006,556.942,-959.752,915.907,-611.974,171.355,
      -73.0875,0.0344005,573.283,-985.193,938.032,-626.329,175.396,
      -73.3104,0.0371747,575.988,-988.623,937.965,-624.185,174.31,
      -74.2248,0.0409931,582.787,-998.393,943.378,-625.427,174.068,
      -78.2616,0.0396539,605.01,-1030.55,967.985,-639.483,177.565,
      -83.4854,0.0227933,629.292,-1062.32,989.642,-651.417,180.689 };
    fillParMomCorrH(nThetaReg*nParams,par);
    typePar = 2;
  } else if(run == "nov02") {
	Info("setDefaultPar()","Setting dedx correction table for %s",run.Data());
    momInt1=600.;
    momInt2=2000.;
    nThetaReg=7;
    thetaStep=10.;
    thetaMiddle0=23.;
    nParams=10;
    Double_t par[70] = {
      0.0506414,-68.3320,30017.5,87.1936,-0.00405782,-0.000841998,4.52039,5.37248,-0.00121608,2.40080,   // 1st theta interval
      0.056843, -73.7507,30890.6,85.2889,-0.00406022,-0.000823641,4.41504,3.97163,-0.00105256,2.31750,   // 2nd theta interval
      0.0692018,-85.0326,33230.6,86.0512,-0.00408880,-0.000838878,4.39438,4.30892,-0.00110695,2.29046,   // 3rd theta interval
      0.05393,  -72.6496,31345.2,92.3181,-0.00414646,-0.000836083,4.41321,4.73116,-0.00115162,2.30843,   // 4th theta interval
      0.052317, -70.9782,31023.3,94.4471,-0.00416224,-0.000882047,4.56496,5.04969,-0.00117343,2.36712,   // 5th theta interval
      0.0524236,-71.4746,31505.2,97.8558,-0.00416946,-0.000919853,4.75295,5.53359,-0.00120439,2.46908,   // 6th theta interval
      0.0532758,-72.6315,32132.7,100.503,-0.00416881,-0.000962655,4.95385,5.78055,-0.00120932,2.57004 }; // 7th theta interval
    fillParMomCorrH(nThetaReg*nParams,par);
		
    //for electrons
    nParamsElect=5;
    parMomCorrElect[0]=2.29512;
    parMomCorrElect[1]=0.042546;
    parMomCorrElect[2]=-0.00175886;
    parMomCorrElect[3]=2.47346/100000;
    parMomCorrElect[4]=-1.0764/10000000;

    typePar = 1;
  } else if(run=="jan04") {
	Info("setDefaultPar()","Setting dedx correction table for %s",run.Data());
    momInt1=720.;
    momInt2=2000.;
    nThetaReg=7;
    nParams=10;
    thetaStep=10.;
    thetaMiddle0=23.;
     
    Double_t par[70] = {
      0.0404892,-63.2212,31512.3,88.7387,-0.00383467,-0.000831376,4.83807,8.32566,-0.00134054,2.69398,  // 1st theta interval
      0.0392893,-61.7120,31187.0,67.0670,-0.00344743,-0.000770298,4.73321,4.08464,-0.00100119,2.70498,  // 2nd theta interval
      0.0402474,-63.1836,31819.4,95.3604,-0.00391875,-0.000930825,5.09812,4.57863,-0.00103624,2.68722,  // 3rd theta interval
      0.0421779,-64.9599,31849.2,85.6957,-0.00379927,-0.000812833,4.78868,9.17729,-0.00138972,2.64987,  // 4th theta interval
      0.0417347,-64.2374,31548.9,97.1928,-0.00399476,-0.000827159,4.80468,6.59782,-0.00125117,2.63429,  // 4th theta interval
      0.0404892,-63.2212,31512.3,88.7387,-0.00383467,-0.000831377,4.83807,8.32566,-0.00134054,2.69398,  // 6th theta interval
      0.0396831,-62.4906,31610.8,86.3941,-0.00374661,-0.000816029,4.88749,4.51922,-0.00104096,2.73135}; // 7th theta interval
    fillParMomCorrH(nThetaReg*nParams,par);

    //for electrons
    nParamsElect=5;
    parMomCorrElect[0]=2.21734;
    parMomCorrElect[1]=0.0794622;
    parMomCorrElect[2]=-0.00258538;
    parMomCorrElect[3]=3.03896/100000;
    parMomCorrElect[4]=-1.11898/10000000;

    typePar = 1;
  } else if(run=="aug04") {
	Info("setDefaultPar()","Setting dedx correction table for %s",run.Data());
    momInt1=800.;
    momInt2=2000.;
    nThetaReg=7;
    thetaStep=10.;
    thetaMiddle0=23.;
    nParams=10;
    Double_t par[70] = {
      0.0319386,-52.3932,28573.9,65.0824,-0.00340829,-0.000751249,4.91880,7.01293,-0.001202890,2.87892,   // 1st theta interval
      0.0354272,-57.1894,30552.1,73.0873,-0.00349743,-0.000814299,5.21396,5.66944,-0.001081390,2.99888,   // 2nd theta interval
      0.0369929,-59.2908,31560.5,74.1808,-0.00343821,-0.000746043,5.24870,5.80041,-0.001114980,3.16789,   // 3rd theta interval
      0.0381148,-62.3502,34058.2,74.2051,-0.00327892,-0.000714812,5.57025,7.58451,-0.001158990,3.49298,   // 4th theta interval
      0.0405264,-66.2530,36385.7,80.9644,-0.00328172,-0.000733450,6.02206,5.72358,-0.000985652,3.77823,   // 5th theta interval
      0.0442906,-72.6287,39923.8,112.382,-0.00364194,-0.001174410,7.26952,10.0128,-0.001196320,4.12544,   // 6th theta interval
      0.0419634,-70.9195,40627.6,122.213,-0.00366415,-0.001261510,7.75827,9.35643,-0.001134410,4.36657 }; // 7th theta interval
    fillParMomCorrH(nThetaReg*nParams,par);
		
    //for electrons
    nParamsElect=5;
    parMomCorrElect[0]=3.21963;
    parMomCorrElect[1]=-0.0257486;
    parMomCorrElect[2]=0.000407469;
    parMomCorrElect[3]=9.67088e-06;
    parMomCorrElect[4]=-9.93232e-08;
    typePar = 1;
  } else{
	Warning("setDefaultPar()","unknown run %s, no correction table loaded!",run.Data());
  }
  return typePar > 0;
}
void HEnergyLossCorrPar::fillParMomCorrH(Int_t size,Double_t *par) {
  for(Int_t i=0;i<size;i++) parMomCorrH[i] = par[i];
}


Double_t HEnergyLossCorrPar::getDeltaMomT1(Int_t pId, Double_t mom, Double_t theta) const {
  
//  if(theta<15.||theta>87.) return 0.;
  if(theta<15.) theta = 15.;
  if(theta>87.) theta = 87.;

  if(pId==2||pId==3) {
//    if(mom<50.) return 0.;
    if(mom<40.) mom = 40.;
    // 4th order polynomial
    return parMomCorrElect[0]+theta*(parMomCorrElect[1]+theta*(parMomCorrElect[2]+theta*(parMomCorrElect[3]+parMomCorrElect[4]*theta)));
  }
  else {

    Double_t pMom, K[nParams];
    Double_t mPr   = HPhysicsConstants::mass(14);
    Double_t mPart = HPhysicsConstants::mass(pId);
    Int_t   chg   = HPhysicsConstants::charge(pId);
    if(mPart<=0.||(chg!=1&&chg!=-1)) return 0.;

    if(pId==14) pMom=mom;
    else pMom = Sqrt(Power(mPr/mPart*(Sqrt(mom*mom+mPart*mPart)-mPart)+mPr,2)-Power(mPr,2));

//    if(pMom<170.) return 0.;
    if(pMom<170.) pMom = 170.;

    Int_t i=Int_t((theta-thetaMiddle0)/thetaStep+1);
    if(i==nThetaReg) i=nThetaReg-1;

    Int_t j;

    if(pMom<momInt1) j=0; else if(pMom<momInt2) j=3; else j=7;
    if(i==0) {
    // first theta interval
      Double_t thVal = (thetaMiddle0-theta)/thetaStep;
      K[j]   = parMomCorrH[j]   - (parMomCorrH[nParams+j]-parMomCorrH[j])*thVal;
      K[j+1] = parMomCorrH[j+1] - (parMomCorrH[nParams+j+1]-parMomCorrH[j+1])*thVal;
      K[j+2] = parMomCorrH[j+2] - (parMomCorrH[nParams+j+2]-parMomCorrH[j+2])*thVal;
      if(pMom>momInt1 && pMom<momInt2)
        K[j+3] = parMomCorrH[j+3] - (parMomCorrH[nParams+j+3]-parMomCorrH[j+3])*thVal;

//      K[j]=parMomCorrH[j]; K[j+1]=parMomCorrH[j+1]; K[j+2]=parMomCorrH[j+2];
//      if(pMom>momInt1&&pMom<momInt2) K[j+3]=parMomCorrH[j+3];
    }//i=0
    else {
      Int_t bound1 = (i-1)*nParams;
      Int_t bound2 = i*nParams;
      Double_t thInsideInt = (theta-(thetaMiddle0+thetaStep*(i-1)))/thetaStep;
      if(i==nThetaReg-1) {
      // last theta interval
        K[j]=parMomCorrH[bound2+j] + (parMomCorrH[bound2+j]-parMomCorrH[bound1+j])*thInsideInt;
        K[j+1]=parMomCorrH[bound2+j+1] + (parMomCorrH[bound2+j+1]-parMomCorrH[bound1+j+1])*thInsideInt;
        K[j+2]=parMomCorrH[bound2+j+2] + (parMomCorrH[bound2+j+2]-parMomCorrH[bound1+j+2])*thInsideInt;
        if(pMom>momInt1&&pMom<momInt2)
          K[j+3]=parMomCorrH[bound2+j+3] + (parMomCorrH[bound2+j+3]-parMomCorrH[bound1+j+3])*thInsideInt;
      }
      else {
      // theta intervals excluding first and last
        K[j]=parMomCorrH[bound1+j] + (parMomCorrH[bound2+j]-parMomCorrH[bound1+j])*thInsideInt;
        K[j+1]=parMomCorrH[bound1+j+1] + (parMomCorrH[bound2+j+1]-parMomCorrH[bound1+j+1])*thInsideInt;
        K[j+2]=parMomCorrH[bound1+j+2] + (parMomCorrH[bound2+j+2]-parMomCorrH[bound1+j+2])*thInsideInt;
        if(pMom>momInt1&&pMom<momInt2)
          K[j+3]=parMomCorrH[bound1+j+3] + (parMomCorrH[bound2+j+3]-parMomCorrH[bound1+j+3])*thInsideInt;
      } //else
    } //else

    if(pMom<momInt1) return K[j]*pMom+K[j+1]+K[j+2]/pMom;
    if(pMom<momInt2) return K[j]*exp(K[j+1]*pMom)+K[j+2]*pMom+K[j+3];
    return K[j]*exp(K[j+1]*pMom)+K[j+2];
  } // else not electrons

  return 0.;
}

Double_t HEnergyLossCorrPar::getDeltaMom(Int_t pId, Double_t mom, Double_t theta) const {
  if(typePar == 1) return getDeltaMomT1(pId,mom,theta);
  if(typePar == 3) return getDeltaMomT3(pId,mom,theta);
  if(typePar != 2) return 0.;

//  if(theta<15.||theta>87.) return 0.;
  if(theta<15.) theta = 15.;
  if(theta>87.) theta = 87.;

  if(pId==2||pId==3) {
//    if(mom<10.) return 0.;
    if(mom<40.) mom = 40.;

    if(theta<28.)   return 2.96406766784;
    if(theta<50.88) return -7.83473+1.31585*theta-0.0591126*Power(theta,2)+0.00115799*Power(theta,3)-0.00000833136*Power(theta,4);
    if(theta<51.81) return 27.1723-0.479349*theta;
    if(theta<80.28) return 2916.99-261.345*theta+9.7235*Power(theta,2)-0.192166*Power(theta,3)+0.00212771*Power(theta,4)
                               -0.0000125143*Power(theta,5)+0.0000000305468*Power(theta,6);
    return                 0.505533+0.0260891*theta;

  }  // leptons
  else {
  // beamtime = apr12
    Double_t mPart = HPhysicsConstants::mass(pId);
    Int_t    chg2  = HPhysicsConstants::charge(pId);
    if(mPart<=0.|| chg2==0) return 0.;
    chg2 = chg2*chg2;
    Double_t b = mom/Sqrt(mom*mom + mPart*mPart); // Beta
    if(b < 0.11) return 0; // b=0.11 => Pproton = 100 MeV/c
    Int_t i = Int_t((theta-thetaMiddle0)/thetaStep);
    if(i < 0) i = 0;
    if(i >= nThetaReg-1) i = nThetaReg-2;
    Double_t mixtCoef = (theta-(thetaMiddle0+thetaStep*i))/thetaStep;
    return chg2*deltaMomT2(b,mixtCoef, parMomCorrH+i*nParams);

  } // else not electrons

  return 0.;
}

Double_t HEnergyLossCorrPar::deltaMomT2(Double_t b,Double_t rs, const Double_t *par) const {
  if(b>=1. || b<=0.) return 0.;

  Double_t b2    = b*b;
  Double_t bg2   = b/Sqrt(1.-b2);
  Double_t lnbg2 = Log(bg2)/b2;
  Double_t odb   = 1./b;
  Double_t osb   = Sqrt(odb);
  Double_t dp1 = par[0]*odb+par[1]*lnbg2+par[2]*osb + par[3]+par[4]*b+par[5]*b2+par[6]*b2*b;
  par += nParams;  // next theta region
  Double_t dp2 = par[0]*odb+par[1]*lnbg2+par[2]*osb + par[3]+par[4]*b+par[5]*b2+par[6]*b2*b;
  return  dp1 + (dp2-dp1)*rs;
}

Double_t HEnergyLossCorrPar::getDeltaMomT3(Int_t pId, Double_t mom, Double_t theta) const {

// beamtime = jul14 and aug14
//  if(theta<15.||theta>86.) return 0.;
  if(theta<15.) theta = 15.;
  if(theta>87.) theta = 87.;

  if(pId==2||pId==3) {
    if(mom<40.) mom = 40.;  //return 0.;
    // pol6
    return parMomCorrElect[0]+theta*(parMomCorrElect[1]+theta*(parMomCorrElect[2]+theta*(parMomCorrElect[3]+theta*(parMomCorrElect[4]+theta*(parMomCorrElect[5]+parMomCorrElect[6]*theta)))));
  }
  else {

    Double_t pMom, K[nParams];
    Double_t mPr   = HPhysicsConstants::mass(14);
    Double_t mPart = HPhysicsConstants::mass(pId);
    Int_t   chg   = HPhysicsConstants::charge(pId);
    if(mPart<=0.||(chg!=1&&chg!=-1)) return 0.;

    if(pId==14) pMom=mom;
    else pMom = Sqrt(Power(mPr/mPart*(Sqrt(mom*mom+mPart*mPart)-mPart)+mPr,2)-Power(mPr,2));

    if(pMom<170.) pMom = 170.;  //return 0.;

    Int_t i=Int_t((theta-thetaMiddle0)/thetaStep+1);
    if(i==nThetaReg) i=nThetaReg-1;

    Int_t j;

    if(pMom<momInt1) j=0; else if(pMom<momInt2) j=5; else j=9;
    if(i==0) {
    // first theta interval
      Double_t thVal = (thetaMiddle0-theta)/thetaStep;
      K[j]   = parMomCorrH[j]   - (parMomCorrH[nParams+j]-parMomCorrH[j])*thVal;
      K[j+1] = parMomCorrH[j+1] - (parMomCorrH[nParams+j+1]-parMomCorrH[j+1])*thVal;
      K[j+2] = parMomCorrH[j+2] - (parMomCorrH[nParams+j+2]-parMomCorrH[j+2])*thVal;
      if(pMom<momInt2)
        K[j+3] = parMomCorrH[j+3] - (parMomCorrH[nParams+j+3]-parMomCorrH[j+3])*thVal;
      if(pMom<momInt1)
        K[j+4] = parMomCorrH[j+4] - (parMomCorrH[nParams+j+4]-parMomCorrH[j+4])*thVal;

    }//i=0
    else {
      Int_t bound1 = (i-1)*nParams;
      Int_t bound2 = i*nParams;
      Double_t thInsideInt = (theta-(thetaMiddle0+thetaStep*(i-1)))/thetaStep;
      if(i==nThetaReg-1) {
      // last theta interval
        K[j]=parMomCorrH[bound2+j] + (parMomCorrH[bound2+j]-parMomCorrH[bound1+j])*thInsideInt;
        K[j+1]=parMomCorrH[bound2+j+1] + (parMomCorrH[bound2+j+1]-parMomCorrH[bound1+j+1])*thInsideInt;
        K[j+2]=parMomCorrH[bound2+j+2] + (parMomCorrH[bound2+j+2]-parMomCorrH[bound1+j+2])*thInsideInt;
        if(pMom<momInt2)
          K[j+3]=parMomCorrH[bound2+j+3] + (parMomCorrH[bound2+j+3]-parMomCorrH[bound1+j+3])*thInsideInt;
        if(pMom<momInt1)
          K[j+4]=parMomCorrH[bound2+j+4] + (parMomCorrH[bound2+j+4]-parMomCorrH[bound1+j+4])*thInsideInt;
      }
      else {
      // theta intervals excluding first and last
        K[j]=parMomCorrH[bound1+j] + (parMomCorrH[bound2+j]-parMomCorrH[bound1+j])*thInsideInt;
        K[j+1]=parMomCorrH[bound1+j+1] + (parMomCorrH[bound2+j+1]-parMomCorrH[bound1+j+1])*thInsideInt;
        K[j+2]=parMomCorrH[bound1+j+2] + (parMomCorrH[bound2+j+2]-parMomCorrH[bound1+j+2])*thInsideInt;
        if(pMom<momInt2)
          K[j+3]=parMomCorrH[bound1+j+3] + (parMomCorrH[bound2+j+3]-parMomCorrH[bound1+j+3])*thInsideInt;
        if(pMom<momInt1)
          K[j+4]=parMomCorrH[bound1+j+4] + (parMomCorrH[bound2+j+4]-parMomCorrH[bound1+j+4])*thInsideInt;
      } //else
    } //else

    if(pMom<momInt1) return K[j]+pMom*(K[j+1]+pMom*(K[j+2]+pMom*(K[j+3]+K[j+4]*pMom)));
    if(pMom<momInt2) return K[j]*exp(K[j+1]*pMom)+K[j+2]*pMom+K[j+3];
    return K[j]*exp(K[j+1]*pMom)+K[j+2];
  } // else not electrons

  return 0.;
}


