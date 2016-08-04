


  TCanvas* ctime1   [4];
  TCanvas* ctime1err[4];
  TCanvas* ctime1err2d[4];
  TCanvas* ctime1err2d_modelled[4];

  TH1F* htime1_v1     [4][18];
  TH1F* htime1err_v1  [4][18];
  TH2F* htime1err2d_v1[4];
  TH2F* htime1err2d_v1_modelled[4];
  HMdcCal2ParSim*  pCalPar;


  TGraph*   gtime1err[4][18];
  TSpline3* stime1err[4][18];

  TF1* fiterr;

void calcBounderies(Int_t mdc,Int_t a,Float_t* cellEdge,Float_t* maxDist)
{
    // Calulates the bounderies of the different cell types of the MDC's underr
    // certain impact angle "a". "cellEdge" is the max distances of a track with
    // minimum distance at the cell border whereas "maxDist" is the max minimum
    // distance of a track just hitting the cell.

    Double_t cellY[4]={2.5,3.0,6.0,7.0};
    Double_t cellX[4]={2.5,2.6,4.0,5.0};
    Double_t alphaRad=(a*5)*TMath::DegToRad();
    Double_t celledgeY=cellY[mdc]/cos(alphaRad);
    Double_t celledgeX;
    (a==0)? celledgeX=cellY[mdc] : celledgeX=cellX[mdc]/sin(alphaRad);
    (celledgeY>celledgeX)? *cellEdge=(Float_t)celledgeX : *cellEdge=(Float_t)celledgeY;

    if(a!=0)
    {
	Double_t alphaRad2=((a*5-90)/180.*TMath::Pi()); // angle of track
	Double_t b= cellX[mdc]-(tan(alphaRad2)*cellY[mdc]);
	Double_t y=-b/(tan(alphaRad2)-tan(alphaRad));
	Double_t x=tan(alphaRad)*y;
	*maxDist =(Float_t)sqrt((y*y)+(x*x));
    }
    else *maxDist=(Float_t)cellY[mdc];
}

Double_t func(Double_t* x, Double_t* par){

    // |  \                  /
    // |   \ par1      par3 /
    // |    \|            |/
    // |     -------------/ - - - par0
    // |
    // |---------------------------------> x

    if(x[0] <= par[1]){                   // first part
	return par[2] * pow(x[0]-par[1],2.) + par[0];
    }
    if(x[0] > par[1] && x[0] <= par[3]){  // middle part
        return par[0];
    }
    if(x[0] > par[3]){                    // last part
	return pow(x[0]-par[3],par[4]*4.) + par[0];
    }
}


  void save(Int_t useData = 1,TString filename = "HMdcCa2ParSim_edit.txt")
  {
      // useData == 1 --> Garfield errors will be taken from edited Histograms (red)
      // useData == 0 --> Modelled errors will be taken from Graphs (blue)
    cout << "saving changed hist to HMdcCal2ParSim:"<<endl;

    for(Int_t m = 0; m < 4; m ++){
	for(Int_t a = 0; a < 18; a ++){
	    for(Int_t b = 0; b < 100; b ++){
		if(useData == 1){
		    ((*pCalPar)[0][m][a]).setDriftTime1     (b,htime1_v1   [m][a]->GetBinContent(b+1));
		    ((*pCalPar)[0][m][a]).setDriftTime1Error(b,htime1err_v1[m][a]->GetBinContent(b+1));
		}
		else {
		    Double_t* val = gtime1err[m][a]->GetY();
		    Int_t n = gtime1err[m][a]->GetN();
		    Double_t value = 100;
		    if(b >= n) value = 30.;
		    else {
			if(val[b] > 30.) value = 30.;
			else             value = val[b];
		    }
		    ((*pCalPar)[0][m][a]).setDriftTime1     (b,htime1_v1   [m][a]->GetBinContent(b+1));
		    ((*pCalPar)[0][m][a]).setDriftTime1Error(b,value);
		}

	    }
	}
    }

    Float_t mtime1_corr    [4][18][100];
    Float_t mtime1_err_corr[4][18][100];
    Float_t mtime2_corr    [4][18][100];
    Float_t mtime2_err_corr[4][18][100];

    for(Int_t m = 0; m < 4; m ++){
	for(Int_t a = 0; a < 18; a ++){
	    for(Int_t b = 0; b < 100; b ++){
		mtime1_corr    [m][a][b] = ((*pCalPar)[0][m][a]).getDriftTime1(b);
                mtime1_err_corr[m][a][b] = ((*pCalPar)[0][m][a]).getDriftTime1Error(b);
                mtime2_corr    [m][a][b] = ((*pCalPar)[0][m][a]).getDriftTime2(b);
                mtime2_err_corr[m][a][b] = ((*pCalPar)[0][m][a]).getDriftTime2Error(b);
	    }
	}
    }

    cout << "writing HMdcCal2ParSim to file :"<<filename.Data()<<endl;

    FILE* output=fopen(filename.Data(),"w");

    Char_t tag[60];
    sprintf(tag,"%s","[MdcCal2ParSim]\n");
    fprintf(output,"############################################################################\n");
    fprintf(output,"# HMdcCal2ParSim\n");
    fprintf(output,"# format type:0=time1, 1=variation time1, 2=time2, 3=variation time2\n");
    fprintf(output,"# sector module angle type par0 par1 par2 par3 par4 par5 par6 par7 par8 par9 \n");
    fprintf(output,"############################################################################\n");
    fprintf(output,tag);
    for(Int_t sector=0;sector<6;sector++){
	for(Int_t mdctype=0;mdctype<4;mdctype++){
	    for(Int_t angle=0;angle<18;angle++){  // 0-17 from 0 to 85 impactc angle
		for(Int_t type=0;type<4;type++){  // time1,variation time1,time2, varitation time2
		    for(Int_t dist=0;dist<10;dist++){  // 10*10 steps=100
			fprintf(output,"%i %i %2i %2i ",
				sector,mdctype,angle,type);
			if(type==0)
			{
			    fprintf(output,"%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f \n",
				    mtime1_corr[mdctype][angle][dist*10 + 0],mtime1_corr[mdctype][angle][dist*10 + 1],
				    mtime1_corr[mdctype][angle][dist*10 + 2],mtime1_corr[mdctype][angle][dist*10 + 3],
				    mtime1_corr[mdctype][angle][dist*10 + 4],mtime1_corr[mdctype][angle][dist*10 + 5],
				    mtime1_corr[mdctype][angle][dist*10 + 6],mtime1_corr[mdctype][angle][dist*10 + 7],
				    mtime1_corr[mdctype][angle][dist*10 + 8],mtime1_corr[mdctype][angle][dist*10 + 9]);
			}
			if(type==1)
			{
			    fprintf(output,"%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f \n",
				    mtime1_err_corr[mdctype][angle][dist*10 + 0],mtime1_err_corr[mdctype][angle][dist*10 + 1],
				    mtime1_err_corr[mdctype][angle][dist*10 + 2],mtime1_err_corr[mdctype][angle][dist*10 + 3],
				    mtime1_err_corr[mdctype][angle][dist*10 + 4],mtime1_err_corr[mdctype][angle][dist*10 + 5],
				    mtime1_err_corr[mdctype][angle][dist*10 + 6],mtime1_err_corr[mdctype][angle][dist*10 + 7],
				    mtime1_err_corr[mdctype][angle][dist*10 + 8],mtime1_err_corr[mdctype][angle][dist*10 + 9]);
			}
			if(type==2)
			{
				fprintf(output,"%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f \n",
					mtime2_corr[mdctype][angle][dist*10 + 0],mtime2_corr[mdctype][angle][dist*10 + 1],
					mtime2_corr[mdctype][angle][dist*10 + 2],mtime2_corr[mdctype][angle][dist*10 + 3],
					mtime2_corr[mdctype][angle][dist*10 + 4],mtime2_corr[mdctype][angle][dist*10 + 5],
					mtime2_corr[mdctype][angle][dist*10 + 6],mtime2_corr[mdctype][angle][dist*10 + 7],
					mtime2_corr[mdctype][angle][dist*10 + 8],mtime2_corr[mdctype][angle][dist*10 + 9]);
			}
			if(type==3)
			{
			    fprintf(output,"%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f \n",
				    mtime2_err_corr[mdctype][angle][dist*10 + 0],mtime2_err_corr[mdctype][angle][dist*10 + 1],
				    mtime2_err_corr[mdctype][angle][dist*10 + 2],mtime2_err_corr[mdctype][angle][dist*10 + 3],
				    mtime2_err_corr[mdctype][angle][dist*10 + 4],mtime2_err_corr[mdctype][angle][dist*10 + 5],
				    mtime2_err_corr[mdctype][angle][dist*10 + 6],mtime2_err_corr[mdctype][angle][dist*10 + 7],
				    mtime2_err_corr[mdctype][angle][dist*10 + 8],mtime2_err_corr[mdctype][angle][dist*10 + 9]);
			}
		    }
		}
	    }
	}
    }
    fprintf(output,"############################################################################\n");
    fclose(output);

    cout << "finished writing HMdcCal2ParSim"<<endl;

    //delete gHades;
}

void editCal2ParSim()
{
    //--------------------------------------------------
    // HOWTO:
    //
    // 1. '.L editCal2ParSim.C'
    // 2. 'editCal2ParSim()'
    // 3. switch 'Can Edit Histograms' from the Canvas menue 'Options'
    // 4. edit the parameters as you like
    // 5. 'save(Int_t errors,TString outputfile)' (select modelled errors (1) or garfield errrors (0)!
    //              if no filename is specified the default output
    //              will be "HMdcCa2ParSim_edit.txt")
    //--------------------------------------------------



  Int_t runID_v1 = 1672125425 ; // sep05 (needed to init the 4 MODULES of SEC 0, no need to change this)
                                // parameters of HMdcCal2ParSim will be read from ascii file


  //----------------------------------------------------
  TString input_v1 = "/misc/kempter/projects/cal2/mdc_cal-2/create_own/sep05_signals_1370-1750-2000-2200_reducedouter70percent_edit.txt";
  //----------------------------------------------------

  Hades* myHades=new Hades;
  HSpectrometer* spec=gHades->getSetup();
  HRuntimeDb* rtdb=gHades->getRuntimeDb();

  HMdcDetector* mdc=new HMdcDetector;
  //---------------------------------
  // just first sector all modules
  Int_t mdcMods[6][4]=
      { {1,1,1,1},
        {0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0}
      };
  //---------------------------------

  for(Int_t i=0;i<6;i++) mdc->setModules(i,mdcMods[i]);
  spec->addDetector(mdc);

  HParAsciiFileIo* input=new HParAsciiFileIo;
  input->open(input_v1.Data(),"in");
  rtdb->setFirstInput(input);

  HParOraIo* input2=new HParOraIo;
  input2->open();
  rtdb->setSecondInput(input2);

  HParAsciiFileIo* output=new HParAsciiFileIo;
  output->open("testora.txt","out");
  rtdb->setOutput(output);

  rtdb->getContainer("MdcRawStruct");
  rtdb->getContainer("MdcGeomStruct");

  rtdb->initContainers(runID_v1);

  rtdb->setContainersStatic();


  pCalPar=(HMdcCal2ParSim*)rtdb->getContainer("MdcCal2ParSim");




  //------------------------------------------------------------------------
  // create all canvas and hists

  Float_t range   [4]={1.0,1.0,1.0,1.0}; // t1 display range for hists per MDC type
  Float_t rangeerr[4]={4.0,4.5,6.5,7.5}; // t1err display range for hists per MDC type
  Float_t max     [4]={20.,20.,20.,20.}; // max y display range for hists per MDC type (-1 to switch off)


  TF1* ferr = new TF1("ferr",func,0,10,5);
  ferr->SetParameter(0,2.0); // err level mid cell
  ferr->SetParameter(1,2.0); // position 1
  ferr->SetParameter(2,1.0); // scale parabel 1
  ferr->SetParameter(3,5.0); // position 2  --> vary with cellEdge
  ferr->SetParameter(4,1.0); // scale parabel 2


  Double_t par0[]={ 1.40, 1.30,1.70,1.75}; // err level mid cell
  Double_t par1[]={ 1.50, 1.50,1.80,2.20}; // position 1
  Double_t par2[]={ 1.60, 1.60,1.40,1.00}; // scale parabel 1
  Double_t par4[]={ 1.50, 1.50,1.00,1.00}; // scale parabel 2

  Double_t par3[]={0.65,0.65,0.65,0.68};
  Float_t cellEdge;
  Float_t maxDist;

  for(Int_t m = 0; m < 4; m ++){
      htime1err2d_v1         [m] = new TH2F(Form("htime1err2d_v1[%i]"         ,m),Form("htime1err2d_v1[%i]"         ,m),18,0,18,100,0,10);
      htime1err2d_v1_modelled[m] = new TH2F(Form("htime1err2d_v1_modelled[%i]",m),Form("htime1err2d_v1_modelled[%i]",m),18,0,18,100,0,10);
      for(Int_t a = 0; a < 18; a ++){
	  gtime1err[m][a]=new TGraph();
	  gtime1err[m][a]->SetMarkerStyle(20);
          gtime1err[m][a]->SetMarkerColor(4);
          gtime1err[m][a]->SetLineColor(4);

	  calcBounderies(m,a,&cellEdge,&maxDist);
          ferr->SetParameter(0,par0[m]);
	  ferr->SetParameter(1,par1[m]);
	  ferr->SetParameter(2,par2[m]);
	  ferr->SetParameter(3,par3[m]* cellEdge);
	  ferr->SetParameter(4,par4[m]);
	  for(Int_t i = 0; i < 100; i ++){
	      if(ferr->Eval(i*0.1) < 30) {
		  gtime1err[m][a]->SetPoint(i,i*0.1 + 0.05,ferr->Eval(i*0.1));
		  htime1err2d_v1_modelled[m]->SetBinContent(a+1,i+1,ferr->Eval(i*0.1));
	      }
	      else {
		  htime1err2d_v1_modelled[m]->SetBinContent(a+1,i+1,30.);
	      }
	     
	  }
      }
  }
  for(Int_t m = 0; m < 4; m ++){

      ctime1               [m] = new TCanvas(Form("MDC%i_t1"     ,m) ,Form("MDC%i_t1"     ,m));
      ctime1err            [m] = new TCanvas(Form("MDC%i_t1err"  ,m) ,Form("MDC%i_t1err"  ,m));
      ctime1err2d          [m] = new TCanvas(Form("MDC%i_t1err2d",m) ,Form("MDC%i_t1err2d",m));
      ctime1err2d_modelled [m] = new TCanvas(Form("MDC%i_t1err2d_modelled",m) ,Form("MDC%i_t1err2d_modelled",m));
      ctime1      [m]->Divide(6,3);
      ctime1err   [m]->Divide(6,3);


      for(Int_t a = 0; a < 18; a ++){
	  htime1_v1   [m][a] = new TH1F(Form("htime1_v1[%i][%02i]"   ,m,a),Form("htime1_v1[%i][%02i]"   ,m,a),100,0,10);
	  htime1err_v1[m][a] = new TH1F(Form("htime1err_v1[%i][%02i]",m,a),Form("htime1err_v1[%i][%02i]",m,a),100,0,10);
          htime1_v1   [m][a]->SetLineColor(2);
          htime1err_v1[m][a]->SetLineColor(2);

	  htime1_v1   [m][a]->SetAxisRange(0. , range[m]);
          htime1err_v1[m][a]->SetAxisRange(0. , rangeerr[m]);
      }

  }
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  // get parameter versions
  // 1. version
  rtdb->initContainers(runID_v1);

  for(Int_t m = 0; m < 4; m ++){
      for(Int_t a = 0; a < 18; a ++){
	  for(Int_t b = 0; b < 100; b ++){
	      htime1_v1   [m][a]->SetBinContent(b+1,((*pCalPar)[0][m][a]).getDriftTime1(b));
	      htime1err_v1[m][a]->SetBinContent(b+1,((*pCalPar)[0][m][a]).getDriftTime1Error(b));
	      htime1err2d_v1[m]->SetBinContent(a+1,b+1,((*pCalPar)[0][m][a]).getDriftTime1Error(b));
	  }
      }
  }
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  // plot control histograms
  // 1. comparison t1 vs distance for all angular bins
  for(Int_t m = 0; m < 4; m ++){
      for(Int_t a = 0; a < 18; a ++){
	  ctime1[m]->cd(a+1);
	  if(max[m] > 0) {
	      htime1_v1[m][a]->SetMaximum(max[m]);
	  }
	  htime1_v1[m][a]->SetMinimum(0.);
	  htime1_v1[m][a]->Draw();
      }
  }

  // 1. comparison t1err vs distance for all angular bins
  TLine* boundary;

  for(Int_t m = 0; m < 4; m ++){
      for(Int_t a = 0; a < 18; a ++){
	  ctime1err[m]->cd(a+1);
	  htime1err_v1[m][a]->SetMaximum(30);
	  htime1err_v1[m][a]->SetMinimum(0);
	  htime1err_v1[m][a]->Draw();
	  gtime1err   [m][a]->Draw("Lsame");
	  calcBounderies(m,a,&cellEdge,&maxDist);
	  boundary = new TLine(cellEdge, 0, cellEdge, 30);
	  boundary->SetLineColor(1);
          boundary->SetLineStyle(2);
          boundary->Draw();

	  boundary = new TLine(par1[m], 0, par1[m], 30);
	  boundary->SetLineColor(4);
          boundary->SetLineStyle(2);
          boundary->Draw();

	  boundary = new TLine(par3[m]*cellEdge, 0, par3[m]*cellEdge, 30);
	  boundary->SetLineColor(8);
          boundary->SetLineStyle(2);
          boundary->Draw();


      }
  }
  //------------------------------------------------------------------------

    for(Int_t m = 0; m < 4; m ++){
	ctime1err2d[m]->cd();
	htime1err2d_v1[m]->SetMinimum(0);
        htime1err2d_v1[m]->Draw("lego2");
    }

  for(Int_t m = 0; m < 4; m ++){
	ctime1err2d_modelled[m]->cd();
	htime1err2d_v1_modelled[m]->SetMinimum(0);
        htime1err2d_v1_modelled[m]->Draw("lego2");
    }
  
}
