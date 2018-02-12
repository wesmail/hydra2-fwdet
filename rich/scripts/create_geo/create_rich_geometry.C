#include "hrich700pmttypeenum.h"

  string richGeomDataFile = "rich_geom_data.dat";
  string outputGeoFile = "rich700n_feb2018.geo";
  string outputParFile = "rich700n_param_feb2018.dat";

 // if pmtType > 0 then all PMTs have the same QE
 // if pmtType < 0 then the mixedMode is used see getPmtType()
  Int_t pmtType = -1; //HRich700PmtTypeCosy17NoWls, HRich700PmtTypeCosy17WithWls;

  const Double_t pmtSize = 52.; // in mm
  const Double_t pmtSensThickness = 2.; // in mm
  const Double_t pmtGap = 1.; // in mm
  const Double_t pmtElectronicsThickness = 10.; // mm
  // the cherenkov photons are registered at the EXIT of the volume
  // thats why we need to shift Z position by +pmtSensThickness
  // we have to add pmtSensThickness to compensate the shift
  const Double_t pmtThicknessGap = 41.7;// + pmtSensThickness;
  const Double_t fullPmtThickness = pmtSensThickness  + pmtThicknessGap +  pmtElectronicsThickness;
  const Double_t halfFullPmtThickness = fullPmtThickness / 2.;
  const Double_t halfPmtElectronicsThickness = pmtElectronicsThickness / 2.;
  ifstream inDataFile;
  ofstream outFile;
  ofstream outParFile;

  // upper eft pmt (front view)
  const Double_t xRef = -609.5;
  const Double_t yRef = -609.5;
  const Double_t zFrontPmtI = -113.6; // global position of the back PMT (photon is detected on exit), z coordinate of the hits
  const Double_t zFrontPmtO = -241.6;  //
  const Double_t zRich = -440.1;
  // the cherenkov photons are registered at the EXIT of the volume
  // thats why we need to shift Z position by +pmtSensThickness
  const Double_t zI = zFrontPmtI - zRich -  halfFullPmtThickness + pmtSensThickness;
  const Double_t zO = zFrontPmtO - zRich -  halfFullPmtThickness + pmtSensThickness;
  const Int_t maxNumberOfPmtsInRow = 24;

  Int_t pmtCopyIndex = 1;

void create_rich_geometry(){

  inDataFile.open(richGeomDataFile.c_str());
  outFile.open(outputGeoFile.c_str());
  outParFile.open(outputParFile.c_str());

  string line;
  while ( getline (inDataFile, line) )
  {
    outFile << line << '\n';
  }

  // front view
  // [y][x] - first index is row, second is column
  // [0][0] is the lower right pmt due to the coordinate system of HADES
  Double_t zVec[maxNumberOfPmtsInRow][maxNumberOfPmtsInRow] = {
      {zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO,     zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO,     zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO,     zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO,     zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO,     zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},


      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zI, zI, zI, zI, zI, zI, zI,     zI, zI, zI, zI, zI, zI, zI, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO,     zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO,     zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO,     zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO,     zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO},
      {zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO,     zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO, zO}
    };

  // 0 - not active, 1 - active
  Int_t pmts[maxNumberOfPmtsInRow][maxNumberOfPmtsInRow] = {
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,     1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,     1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},


      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,     1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,     1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}
    };

  createGeom(zVec, pmts);

  outFile.close();
  inDataFile.close();
  outParFile.close();
  cout << "Geometry is written to the file " << outputGeoFile << endl;
  cout << "Number of PMTs:" << pmtCopyIndex - 1 << endl;
  cout << "Parameters are written to:" << outputParFile << endl;
}

void writePmtBox(Double_t x, Double_t y, Double_t z, Int_t copyIndex, Int_t indX, Int_t indY)
{
  Double_t halfPmtSize = pmtSize / 2.;
  Double_t halfPmtSensThickness = pmtSensThickness / 2.;

  outFile << "RPMT" << copyIndex << "\n";
  outFile << "RMET" << "\n";
  outFile << "BOX" << "\n";
  outFile << "RICH_GAS_N2_DIS$" << "\n";
  outFile << halfPmtSize << " " << -halfPmtSize << " "<< -halfFullPmtThickness << "\n";
  outFile << halfPmtSize << " " << halfPmtSize << " "<< -halfFullPmtThickness << "\n";
  outFile << -halfPmtSize << " " << halfPmtSize << " "<< -halfFullPmtThickness << "\n";
  outFile << -halfPmtSize << " " << -halfPmtSize << " "<< -halfFullPmtThickness << "\n";
  outFile << halfPmtSize << " " << -halfPmtSize << " "<< halfFullPmtThickness << "\n";
  outFile << halfPmtSize << " " << halfPmtSize << " "<< halfFullPmtThickness << "\n";
  outFile << -halfPmtSize << " " << halfPmtSize << " "<< halfFullPmtThickness << "\n";
  outFile << -halfPmtSize << " " << -halfPmtSize << " "<< halfFullPmtThickness << "\n";
  outFile << x << " " << y << " " << z  << "\n";
  outFile << "1.0000000  0.0000000  0.0000000  0.0000000  1.0000000  0.0000000  0.0000000  0.0000000  1.0000000" << "\n";
  outFile << "//----------------------------------------------------------\n";

  outFile << "RDET" <<  "\n";
  outFile << "RPMT" << copyIndex <<  "\n";
  outFile << "BOX" << "\n";
  outFile << "RICHPHOTDET_PMT$" << "\n";
  outFile << halfPmtSize << " " << -halfPmtSize << " "<< -halfPmtSensThickness << "\n";
  outFile << halfPmtSize << " " << halfPmtSize << " "<< -halfPmtSensThickness << "\n";
  outFile << -halfPmtSize << " " << halfPmtSize << " "<< -halfPmtSensThickness << "\n";
  outFile << -halfPmtSize << " " << -halfPmtSize << " "<< -halfPmtSensThickness << "\n";
  outFile << halfPmtSize << " " << -halfPmtSize << " "<< halfPmtSensThickness << "\n";
  outFile << halfPmtSize << " " << halfPmtSize << " "<< halfPmtSensThickness << "\n";
  outFile << -halfPmtSize << " " << halfPmtSize << " "<< halfPmtSensThickness << "\n";
  outFile << -halfPmtSize << " " << -halfPmtSize << " "<< halfPmtSensThickness << "\n";
  outFile << 0. << " " << 0. << " " << halfFullPmtThickness - halfPmtSensThickness << "\n";  ;
  outFile << "1.0000000  0.0000000  0.0000000  0.0000000  1.0000000  0.0000000  0.0000000  0.0000000  1.0000000" << "\n";
  outFile << "//----------------------------------------------------------\n";

  outFile << "RELE" <<  "\n";
  outFile << "RPMT" << copyIndex << "\n";
  outFile << "BOX" << "\n";
  outFile << "ALUMINIUM$" << "\n";
  outFile << halfPmtSize << " " << -halfPmtSize << " "<< -halfPmtElectronicsThickness << "\n";
  outFile << halfPmtSize << " " << halfPmtSize << " "<< -halfPmtElectronicsThickness << "\n";
  outFile << -halfPmtSize << " " << halfPmtSize << " "<< -halfPmtElectronicsThickness << "\n";
  outFile << -halfPmtSize << " " << -halfPmtSize << " "<< -halfPmtElectronicsThickness << "\n";
  outFile << halfPmtSize << " " << -halfPmtSize << " "<< halfPmtElectronicsThickness << "\n";
  outFile << halfPmtSize << " " << halfPmtSize << " "<< halfPmtElectronicsThickness << "\n";
  outFile << -halfPmtSize << " " << halfPmtSize << " "<< halfPmtElectronicsThickness << "\n";
  outFile << -halfPmtSize << " " << -halfPmtSize << " "<< halfPmtElectronicsThickness << "\n";
  outFile << 0. << " " << 0. << " " <<  -halfFullPmtThickness + halfPmtElectronicsThickness  << "\n";
  outFile << "1.0000000  0.0000000  0.0000000  0.0000000  1.0000000  0.0000000  0.0000000  0.0000000  1.0000000" << "\n";
  outFile << "//----------------------------------------------------------\n";

  outParFile << copyIndex << " " << indX << " " << indY << " " << x << " " << y << " " << z << " " << getPmtType(z) << " 0.0 0.0 \\\n";
}

void writePmtBoxCopy(Double_t x, Double_t y, Double_t z, Int_t copyIndex, Int_t indX, Int_t indY)
{
  outFile << "RPMT" << copyIndex << "\n";
  outFile << "RMET" << "\n";
  outFile << x << " " << y << " " << z  << "\n";
  outFile << "1.0000000  0.0000000  0.0000000  0.0000000  1.0000000  0.0000000  0.0000000  0.0000000  1.0000000" << "\n";
  outFile << "//----------------------------------------------------------\n";

  outParFile << copyIndex << " " << indX << " " << indY << " " << x << " " << y << " " << z << " " << getPmtType(z) << " 0.0 0.0 \\\n";
}

void createGeom(
  Double_t zVec[maxNumberOfPmtsInRow][maxNumberOfPmtsInRow],
  Int_t pmts[maxNumberOfPmtsInRow][maxNumberOfPmtsInRow])
{
  bool firstPmtCreated = false;
  for (Int_t iy = 0; iy < maxNumberOfPmtsInRow; iy++) { // loop row
    for (Int_t ix = 0; ix < maxNumberOfPmtsInRow; ix++) { // loop column
      if (pmts[iy][ix] > 0) {
        Double_t x = xRef + ix*(pmtSize + pmtGap);
        Double_t y = yRef + iy*(pmtSize + pmtGap);
        Double_t z = zVec[iy][ix];
        if (!firstPmtCreated) {
	        writePmtBox(x, y, z, pmtCopyIndex, ix, iy);
            firstPmtCreated = true;
        } else {
            writePmtBoxCopy(x, y, z, pmtCopyIndex, ix, iy);
        }
      }
      // always increase copyIndex
      pmtCopyIndex++;
    } //ix
  } //iy
}

Int_t getPmtType(Double_t z)
{
    if (pmtType < 0) {
        if (z == zI) {
            return HRich700PmtTypeCosy17WithWls;
        } else {
            return HRich700PmtTypeCosy17NoWls;
        }

    } else {
        return pmtType;
    }
}
