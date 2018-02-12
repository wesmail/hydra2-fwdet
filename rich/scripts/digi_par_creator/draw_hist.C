void draw_hist()
{
    HRich700DigiParCreator* creator = new HRich700DigiParCreator();

    string fileName = "richpar_hist.root";
    string outputDir = "results/";

    creator->drawFromFile(fileName, outputDir);
}
