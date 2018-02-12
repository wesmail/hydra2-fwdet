//
// start up a button bar with standart HGeant commands for Pion Beam
//
void menu(void) {
   TControlBar *menu = new TControlBar("vertical","HGeant Menu",20,650);
   menu->AddButton("KUIP","goKuip()","Go to KUIP command interpreter");
   menu->AddButton("Draw Event","DrawEvent()","Draw cave and next event into HIGZ window");
   menu->AddButton("Draw Cave","DrawCave()","Draw cave into HIGZ window");
   menu->AddButton("Do Event","TriggerEv(1)","Simulate one event");
   menu->AddButton("Clear Screen","ClearHIGZ()","Clear HIGZ window");
   menu->AddButton("EXIT",".q","Exit HGeant");

   gROOT->SaveContext();
   menu->Show();
}

void TriggerEv(Int_t nEv) {
   char command[20];
   sprintf(command,"trigger %d \0",nEv);
   doGeant(command);
}

void DrawEvent() {
   doGeant("next");
   doGeant("dcut cave 1 1 10 10 .11 .11");
   //   doGeant("dcut cave 1 0 65 10 1.2 1.2");
   doGeant("swit 2 3");  // default colors
   //   doGeant("swit 2 6");  // custom colors defined in hgeant2/geant/gdcxyz.F
   doGeant("trigger 1");
}

void DrawCave() {
  //   doGeant("draw cave 90 0 0 10 12 0.03 0.03");
   doGeant("dcut cave 1 0 16 10 .12 .12");
   doGeant("swit 2 3");
}

void ClearHIGZ() {
   doGeant("next");
}
