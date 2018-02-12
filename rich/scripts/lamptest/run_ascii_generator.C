#include <iostream>
#include <string>
#include <vector>

using namespace std;

const double c = 2.998E8; // speed of light
const double h = 6.626E-34; // Planck constant
const double e = 1.6022E-19; // elementary charge
const double NRefrac = 1.0003; //N2


// this is a workaround
// by some reason std::pair is not working
class MyPair
{
public:
    MyPair(double pFirst, double pSecond) {
	first = pFirst;
        second = pSecond;
    }
    double first;
    double second;
};

double getEnergy(double wl)
{
    return c/NRefrac*h/e/wl; // GeV
}

TH1D* create_photon_energy_spectra_hist()
{
    // first: wavelength in [nm]
    // second: probability [a.u.]
    vector<MyPair*> values;

    values.push_back(new MyPair(200., 10.));
    values.push_back(new MyPair(220., 15.));
    values.push_back(new MyPair(240., 60.));
    values.push_back(new MyPair(260., 50.));
    values.push_back(new MyPair(280., 115.));
    values.push_back(new MyPair(300., 90.));
    values.push_back(new MyPair(320., 50.));
    values.push_back(new MyPair(340., 30.));
    values.push_back(new MyPair(360., 18.));
    values.push_back(new MyPair(380., 18.));
    values.push_back(new MyPair(400., 15.));
    values.push_back(new MyPair(420., 12.));
    values.push_back(new MyPair(440., 10.));
    values.push_back(new MyPair(460., 7.));
    values.push_back(new MyPair(480., 4.));
    values.push_back(new MyPair(500., 2.));
    values.push_back(new MyPair(520., 2.));
    values.push_back(new MyPair(540., 4.));
    values.push_back(new MyPair(560., 7.));
    values.push_back(new MyPair(580., 27.));
    values.push_back(new MyPair(600., 39.));
    values.push_back(new MyPair(620., 43.));
    values.push_back(new MyPair(640., 40.));
    values.push_back(new MyPair(660., 30.));
    values.push_back(new MyPair(680., 20.));
    values.push_back(new MyPair(700., 14.));
    values.push_back(new MyPair(720., 9.));
    values.push_back(new MyPair(740., 6.));
    values.push_back(new MyPair(760., 4.));
    values.push_back(new MyPair(780., 3.));
    values.push_back(new MyPair(800., 2.));
    values.push_back(new MyPair(820., 1.));
    values.push_back(new MyPair(840., 0.5));
    values.push_back(new MyPair(860., 0.2));
    values.push_back(new MyPair(880., 0.1));
    values.push_back(new MyPair(900., 0.));
    values.push_back(new MyPair(920., 0.));
    values.push_back(new MyPair(940., 0.));
    values.push_back(new MyPair(960., 0.));

    double minWL = values[0]->first;
    double maxWL = values[values.size() - 1]->first;

    cout << "minWL:" << minWL << " maxWL:" << maxWL << endl;

    TH1D* hWL = new TH1D("hWL", "hWL;Wavelength [nm];Probability [a.u.]", values.size(), minWL, maxWL);

    for (int i = 0; i < values.size(); i++) {
	hWL->SetBinContent(i + 1, values[i]->second);
	cout << "wl:" << values[i]->first << " energy:" << getEnergy(values[i]->first) << endl;
    }
    //DrawH1(hWL);
    //hWL->Draw();

    return hWL;
}


void run_ascii_generator(Int_t nEvents = 10000)
{
    // output file name
    string asciiFile = "lamp_sim_photons.evt";

    // azimuthal angle [deg]
    const double minPhi = 0.;
    const double maxPhi = 360.;

    // polar angle [deg]
    const double minTheta = 0.;
    const double maxTheta = 360.;

    // Momentum [GeV/c]
    const double minMomentum = 1.e-9;
    const double maxMomentum = 10.e-9;

    // vertex coordinates [mm]
    const double minXv = -12.0;
    const double maxXv = 12.0;
    const double minYv = -12.0;
    const double maxYv = 12.0;
    const double minZv = 282.;
    const double maxZv = 322.;

    // number of photons per event
    const int nofChPhotonsPerEvent = 20;

    // const Cherenkov photon pdg
    const pdgChPhoton = 50;

    // Open output file
    ofstream file(asciiFile.c_str());

    // Initialize RN generator
    gRandom->SetSeed(10);

    TH1D* hWL = create_photon_energy_spectra_hist();

    for(Int_t iev = 0; iev < nEvents; iev++)
    {
	// Generate vertex coordinates
	double vx = gRandom->Uniform(minXv, maxXv);
	double vy = gRandom->Uniform(minYv, maxYv);
        double zv = gRandom->Uniform(minZv, maxZv);

	// Write out event header
	file << (iev+1) << " " << nofChPhotonsPerEvent << " " << 0. << " " << 0. << " " << "-44" <<  endl;

	for (int iP = 0; iP < nofChPhotonsPerEvent; iP++) {
	    // Generate momentum
            double p = getEnergy(hWL->GetRandom());
	   // double p = gRandom->Uniform(minMomentum, maxMomentum);

	    // Generate polar angle
	    double theta = TMath::Abs(gRandom->Uniform(minTheta, maxTheta));

	    // Generate azimuthal angle
	    double phi = gRandom->Uniform(minPhi, maxPhi);

	    // Calculate momentum components
	    double pt = p*TMath::Sin(theta);
	    double px = pt*TMath::Cos(phi);
	    double py = pt*TMath::Sin(phi);
	    double pz = p*TMath::Cos(theta);
	    double etot = TMath::Sqrt(px*px + py*py + pz*pz);

           // cout << etot << " " << p << endl;

	    // Vertex
	    double vx = gRandom->Uniform(minXv, maxXv);
	    double vy = gRandom->Uniform(minYv, maxYv);
            double vz = gRandom->Uniform(minZv, maxZv);

	    // Write out particles information
	    file << etot <<" " << px << " " << py << " " << pz << " " << 0. << " " << vx << " " << vy << " " << vz
		<< " " << pdgChPhoton << " " << 1 << " " << 1 << " " << 1 << " " << 1. << " " << iP << endl;

	}

	// Status output
	if(0 == (iev%1000))
	{
	    cout << "Event " << iev << endl;
	}
    }	 // events

    // Close output file
    file.close();
}


