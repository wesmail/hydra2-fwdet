
void createTF2ParOutput(){

    TFile *fileFunction   = new TFile("../../Input/100Mioe+_Data.root");
    const Int_t nRingSeg  = 45;
    const Int_t nVertex   = 15;
    const Int_t nParMean  =  8;
    const Int_t nParSigma =  13;

    TF2* fPol2DMean[nRingSeg][nVertex];
    TF2* fPol2DSigma[nRingSeg][nVertex];


    for(Int_t bin=0; bin<nRingSeg; bin++){
	for(Int_t vertex=0; vertex<nVertex; vertex++){
	    fPol2DMean[bin][vertex]   = (TF2*) fileFunction->Get(Form("meanPol_%i_v%i",bin,vertex));
	    fPol2DSigma[bin][vertex]  = (TF2*) fileFunction->Get(Form("sigmaPol_%i_v%i",bin,vertex));
	}
    }
    TString outputName    = "TF2ParOutputMean.txt";
    ofstream output(outputName);


    for(Int_t bin=0; bin<nRingSeg; bin++){
	for(Int_t vertex=0; vertex<nVertex; vertex++){
	    for(Int_t par=0; par<nParMean; par++){
	        cout << fPol2DMean[bin][vertex]->GetParameter(par) << endl;
		output << fPol2DMean[bin][vertex]->GetParameter(par) << endl;
	    }
	}
    }

    output.close();

    outputName    = "TF2ParOutputSigma.txt";
    ofstream output(outputName);

    for(Int_t bin=0; bin<nRingSeg; bin++){
	for(Int_t vertex=0; vertex<nVertex; vertex++){
	    for(Int_t par=0; par<nParSigma; par++){
	        cout << fPol2DSigma[bin][vertex]->GetParameter(par) << endl;
		output << fPol2DSigma[bin][vertex]->GetParameter(par) << endl;
	    }
	}
    }


}
