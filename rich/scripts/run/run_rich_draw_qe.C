#include "hrich700pmttypeenum.h"
#include "hrich700pmt.h"
#include "hrich700drawhist.h"
void run_rich_draw_qe(){
     HRich700Pmt* pmt = new HRich700Pmt();

     {
     TCanvas* c = new TCanvas("rich_qe_graph_nowls", "rich_qe_graph_nowls", 900, 900);
     TGraph* gr = pmt->getQEGraph(HRich700PmtTypeCosy17NoWls);
     HRichDrawHist::DrawGraph(gr, kLinear, kLinear, "AC*");
    }

    {
    TCanvas* c = new TCanvas("rich_qe_graph_wls", "rich_qe_graph_wls", 900, 900);
    TGraph* gr = pmt->getQEGraph(HRich700PmtTypeCosy17WithWls);
    HRichDrawHist::DrawGraph(gr, kLinear, kLinear, "AC*");
   }
}
