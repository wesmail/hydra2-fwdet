
    HOWTO SETUP AND RUN SERVER/CLIENT:

	1.  	. /misc/kempter/svn/hydraTrans/defalls.sh
        2. 		build sever/client
    		cd /misc/kempter/svn/hydraTrans/online/server/
    		make clean build install
    		cd /misc/kempter/svn/hydraTrans/online/client/
    		make clean build install
	3.     	cp config files:
		cp 	/misc/kempter/svn/hydraTrans/online/client/ClientConfig.xml .
	4. 	setup  ClientConfig.xml for needed hists (if needed)
		setup  analysisParams.txt to define parameter input , file input etc

	5.	start server
		hadesonlineserver.exe name hostname port ""
		hadesonlineserver.exe OnlineMon lxg0453.gsi.de 9876 ""

	6. 	start client

		./hadesonlineclient.exe  ClientConfig.xml
                ./hadesonlineclient.exe  hostname port stop   ==> stop server
                ./hadesonlineclient.exe  hostname port list   ==> print available histograms on the server


    HOWTO ADD/CHANGE HISTOGRAMS TO MONITORING:

	1.	each detector has its own monitor. 
	    	in online/server you find different macros:
            	a. hadesonlineserver.cc ==> main program
	    	b. createHades.C ==> setup your server program
	    	b. Detectorname.C (like Mdc.C) ==> monitoring the detector
	    	This file contains
	    	createHistsDetectorname() (like createHistsMdc())
	    	add hists to the histpool and a local map which is
            	used to access the histograms by name
	    	fillHistsDetectorname() (like fillHistsMdc())
	    	access the histograms by name to fill them (the function
	    	will be called once per event)
	    	both functions are called from hadesonlineserver.cc
	    	hadesonlineserver.cc has to be recompiled and restarted
	    	after changes.

	2.	After the histograms are added and the fill routine is
	    	defined the histogram has to be added to the Monitor GUI.
	    	Simply add the histogram online/client/ClientConfig.xml
	    	New detectors can be added on the fly. The monitor client
	    	does not need to be recompiled, it creates the GUI dynamically
	    	by parsing the xml file. Cahnges on the client side needs
	    	no restart of the server as long as no new histograms have
            	been defined.

    AVAILABLE MONITORING HISTOGRAMS AND HOW TO USE THEM

    	1.	All histogram types are derived from HOnlineMonHistAddon.
                
                HOnlineMonHist		:	1-Dim Histogram
                HOnlineMonHist2		:	2-Dim Histogram
                HOnlineTrendHist	:       1-Dim Trend Histgram
						(new values added on
						the left side of the histogram, 
                                                old values moved to the right)
                
                HOnlineHistArray	:	1/2-Dim Array of 1-Dim Histograms
                HOnlineHistArray2	:	1/2-Dim Array of 2-Dim Histograms
                HOnlineTrendArray	:	1/2-Dim Array of Trend Histograms
                
                
                HOnlineMonStack         :       A composite object,
                                                allowing to plot
                                                multiple TH1,TH2, HistArrays,TGraph(only for graphics),
                                                into one canvas. Special: it does not create hists etc, 
                                                just packs them. This makes it quite different from the 
                                                other objects
                                                
                                                REMARK: a. TGraphs cannot be plotted on empty canvas, just overlayed
                                                        b. Legends should be added last
                                                        c. HOnlineHistArray ignores graphic options
                                                        d. TGraph,TH1 uses MARKER,LINE,FILL,OPT
                                                        d. TH2 uses MARKER,OPT
                                                        e. values =-1 are ignored  
                
                ----------------------------------------------------------------------------
                expample for HOnlineMonStack: 
                      in Detector.C  createHistDetector.C after  
                      mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);
                      
                      TLegend*   leg = new TLegend(0.82,0.98,0.98,0.5,"","brNDC");
                      HOnlineMonStack* stack = new HOnlineMonStack("hMdcTestStack");
                      stack->addToStack(get("hMdctime1Mod0")); // add 6       1-Dim hists from HistArray
                      stack->addToStack(get("hMdctime1Mod2")); // add 6 other 1-Dim hists from HistArray
    
                      for(Int_t s=0;s<6;s++){
	                 leg->AddEntry(get("hMdctime1Mod0")->getP(0,s),Form("S%i",s),"lpf");
	                 // add 6 single TH1 hists with individual 
                         // setup of line col,style,marker etc. Graphic 
                         // options will be used on the client side, allowing
                         // different representation of the same hist
                         // addToStack() supports LINE,FILL,MARKER,OPT (explanation see below)
                         // REMARK:
                         // TGraphs
                         stack->addToStack(get("hMdctime1Mod3")->getP(0,s), Form("LINE#%i:2 MARKER#%i:20:0.7 OPT#E0",colorsSector[s],colorsSector[s]));
                      }
                      stack->addToStack(leg);

                      stack->setGridX(kTRUE);
                      stack->setGridY(kTRUE);

                      mapHolder::addStack(stack,histpool);
                ----------------------------------------------------------------------------

                      
                
        2.      All histograms are created by a definition String:
        	
                In Detector.C (like Mdc.C) in createHistsDetectorname.C
		(like createHistsMdc()) :
                
                Text_t* hists[] = {
			 "FORMAT#array TYPE#1F NAME#hMdctime1Cal1 TITLE#Mdc_timeCal1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#800:-100:700:0:0:0 SIZE#1:2 AXIS#time_[channel]:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
			,"FORMAT#mon TYPE#1F NAME#hMdctime1Cal1MeanTrendtemp TITLE#time1Cal1MeanTrendtemp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1200:0:1200:0:0:0 SIZE#0:0 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
			,"FORMAT#mon TYPE#2F NAME#hMdccal1hits TITLE#Mdc_hcal1hits ACTIVE#1 RESET#1 REFRESH#5000 BIN#8:0:4:12:0:6 SIZE#0:0 AXIS#module:sector:no DIR#no OPT#lego2 STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
			,"FORMAT#trendarray TYPE#1F NAME#hMdccal1hitstrend TITLE#Mdc_hcal1hits_trend ACTIVE#1 RESET#0 REFRESH#500 BIN#50:0:50:0:0:0 SIZE#6:4 AXIS#trend:multiplicity:no DIR#no OPT#p STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:S%iM%i"
			,"FORMAT#mon TYPE#2F NAME#hMdcrawRoc_Subev TITLE#Mdc_Raw_Roc_SubEvent_Size ACTIVE#1 RESET#0 REFRESH#5000 BIN#120:0:24:40:0:160 SIZE#0:0 AXIS#no:sub_evt_size:counts DIR#no OPT#COLZ STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
			,"FORMAT#array TYPE#2F NAME#hMdcmbotdcCalib TITLE#Mdc_mbo_tdc_calib ACTIVE#1 RESET#1 REFRESH#5000 BIN#16:0:16:12:0:12 SIZE#6:4 AXIS#mbo:tdc:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
    		};

                a.	The names of the histograms must be unique.
			To avoid overlap with other detectors use
                        hDetectorname..... (like hMdc....)
                        They are used to retrieve the histograms by the
			clients and inside the macros.
                b.	FLAGS inside the definition:
                	FORMAT:		mon    		==> HOnlineMonHist
                       			array   	==> HOnlineHistArray / OnlineHistArray2
                                        trendarry       ==> HOnlineTrendArray
                        TYPE:		1F / 2F		==> 1/2 Dim Histograms
                        NAME:				==> name of Histogram
                        ACTIVE:		0/1		==> Create Histogram
                        RESET:		0/1		==> Should the Histogram be refreshed if the Refresh count is reached ?
                        REFRESH:                        ==> N events before reset 
                        BIN:   nBinsX:xMin:xMax:nBinsY:yMin:yMax
                                			==> definition for Histogram (leave y values 0 for 1 Dim)
                        SIZE:      nx:ny		==> 2-Dim array definition for FOAMT array/trendarray
                                   1:n                      1-Dim
                                   0:0			    no array
                        
                        
                        LEGEND: xmin:ymin:xmax:ymax:format 
                                                        ==> For histogram arrays an atomatic 
                                                            legend can be created. The legend will
                                                            be drawn only if the full array is drawn.
                                                            Legends stored with Histarrays are suppressed
                                                            when the histarray is used in a HMonOnlineStack.
                                xmin ... ymax  Legend coordinates in NDC
                                values =-1 disable Legend 
                                format  : no          use the name of histogram
                                          Sec%i       (example)format string for 1-Dim array
                                          Sec%iMod%i  (exmaple)format string for 2-Dim array
                               -------------------------------------------------------------------------------------
		               EXAMPLES FOR LEGEND : 
                        
                                  automatic legend for histarray (taking histnames)             : LEGEND#0.9:0.98:0.98:0.5:no
                                  automatic legend for histarray (custom entries,1-Dim array)   : LEGEND#0.9:0.98:0.98:0.5:SEC%i
                                  automatic legend for histarray (custom entries,2-Dim array)   : LEGEND#0.9:0.98:0.98:0.5:SEC%i%MOD%i
                                  no legend                                                     : LEGEND#-1:-1:-1:-1:no
                
                                  Legends can be added also by hand (if dirrent text is needed for each entry):
                        
                                  // in Detector.C : createHistsDetector() after  mapHolder::createHists(sizeof(hists)/sizeof(Text_t*),hists,histpool);
                                  if(get("myHist")){
                                    TLegend* leg = new TLegend(0.9:0.98:0.98:0.5,"","brNDC");
                                    for(Int_t i=0;i<3;i++) leg->AddEntry(get("myHist")->getP(0,i),Form("myLable %i",i),"lpf"); // add single hists of 1-Dim array size = 3
                                    get("myHist")->setLegend(leg);
                                  }
                                -------------------------------------------------------------------------------------
		  
                
                c.	mapHolder::createHists(sizeof(hists)/sizeof(Text_t*),hists,histpool);
	                will create the Histrograms accoring to the
			definition and add them to the pool of Histograms on the server
                        and the std::map <TString, HOnlineMonHistAddon*> detnameMap
                        defined inside the macro.


		d.	The histograms can be accessed with get("histogramname")
                	inside the macro. In case the name is not
			contained in 
                
                e.      Fill histograms :
                	From any histogram type you can retrieve the
                        pointer to the standard ROOT histograms by
                        
                        get("histogramname") -> getP()     // 1-Dim
                        get("histogramname") -> getP(0,i)  // 1-Dim array
                        get("histogramname") -> getP(i,j)  // 2-Dim array
                        
                        For trend histograms / array trend histograms
                        use special fill() function.
 
 			get("histogramname") -> fill(val)     // 1-Dim
                        get("histogramname") -> fill(0,i,val) // 1-Dim array
                        get("histogramname") -> fill(i,j,val) // 2-Dim array
                       
		f.	HOWTO USE trend histograms:
                
                        trend histgrams are usually filled with some
                        variables which are obtained as avarage over
                        several events (like avarage count rate, mean
                        values, rms etc). The strategy is to fill a
                        temporary histogram to collect the values over
                        the events and if the Refreshrate of the trend
                        histogram is reached get the needed values from
                        the temp histograms and fill them to the trend.
                        It could look like:
                        
                        // loop over data for each event
                        // inside fillHistsDetectorname(evtCt)
                           
                           HCategory* mdcRawCat  = gHades->getCurrentEvent()->getCategory(catMdcRaw);
                           HMdcRaw* raw;
                           for(Int_t i = 0; i < mdcRawCat->getEntries(); i ++) {                  	
	    			raw = (HMdcRaw*)mdcRawCat->getObject(i);
	    			if(raw)
	   			{
                                  get("histtrendtemp")->getP()->Fill(raw->getTime(1));
                                }
                           }
                        // end loop
                        
                        //---------------------------------------
                        // now fill trend hist
                        
                        if(get("histtrend") && get("histtrendtemp") && // both hists exists
	   		   evtCt%get("histtrend")->getRefreshRate() ==0 && evtCt > 0){ // reached refresh
	    			get("histtrend")->fill(get("histtrendtemp")->getP()->GetMean());
	    			get("histtrendtemp")->getP()->Reset(); // now rest the tem hist
			}
                        //
                        //---------------------------------------
                        
                        
                        
	HOWTO ADD HISTS TO THE CLIENT GUI: 
        
        
        1.	The configuration of the Client is done via online/clien/ClientConfig.xml
        2.	The Client does not need to be recompiled
        3.	Each detector GUI is created on the fly by parsing
        	the xml file.It might look like:
          	
            // in xml file
            //---------------------------------------
            <client> 
             
	    <!-- program configuration -->
    	    <config>
            <server>
                <host>hostname</host>     // host where the server is running
                <port>portnumber</port>   // potnumber for connection to server
            </server>
            </config>
    
    
             <!-- configuration for main window -->
             <MainWindow>
                <name>HADESMonitoring</name>
                <title>HADES Monitoring</title>
                <width>200</width>
                <height>400</height>
             </MainWindow>
             
             
             <!-- detector configuration -->
    		<detector>
        	    <name>TOF</name>     // this name will be use inthe main panel
        	    <title>TOF</title>
        		<window>
            		<name>TOFMon</name>
            		<title>TOFMon</title>
            		<tabbed>true</tabbed>  // the window will contain tabs
            		<tab>
                	   <name>Main</name>   // first tab name
                	   <title>Main</title>
                	   <canvas>            // canvas inside tab
                    		<name>Main</name>
                    		<width>1000</width>  // size of canvas
                    		<height>800</height>
                    		<splitted>true</splitted> // create 3x2pads
                    		<nx>3</nx>
                    		<ny>2</ny>
                    		<histogram>               
                        	    <name>hdetectorname</name> //name of histogram
                        	    <type>single</type>        // plot hist or allhists of array
                        	    <subpad>1</subpad>         // pad number starts from 1 (0 if not splitted)
                    		</histogram>
                                <histogram>               
                        	    <name>hdetectorname2array</name> //name of histogram
                        	    <type>array:0:0</type>           //plot hist with index of array
                        	    <subpad>2</subpad>               //pad number starts from 1 (0 if not splitted)
                    		</histogram>
                    		<histogram>               
                        	    <name>hdetectorname3Stack</name> //name of histogram stack
                        	    <type>single</type>             // plot hists,Legend etc or allhists of stack
                        	    <subpad>3</subpad>              // pad number starts from 1 (0 if not splitted)
                    		</histogram>
                                
                           </canvas>
                        </tab>
                        </window>
    		</detector>
             </client>
            //---------------------------------------
            
            DO NOT PUT THE // comments INTO YOUR REAL CONFIG!!
            FOR ALL POSSIBLE TAGS LOOK DOCU INSIDE XML FILE.
        
        
