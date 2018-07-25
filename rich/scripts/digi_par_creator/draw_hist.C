void draw_hist()
{
    HRich700DigiParCreator* creator = new HRich700DigiParCreator();

    string fileName = "richpar_hist.root";
    string outputDir = "results_june2018/";

    creator->drawFromFile(fileName, outputDir);
}
