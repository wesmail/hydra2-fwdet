// -----------------------------------------------------------------------------
//
// Example macro of using the showerutli library
//
// Author: M.Jaskula Sat Nov 24 05:47:51 CET 2001
//
// -----------------------------------------------------------------------------

void dialogExample()
{
HSUDialog *pdFile = new HSUDialog("A name of dialog");
const int  ciStringLength = 1000;
Char_t     fileName[ciStringLength];
Int_t      iInt =17;
Float_t    fFloat;
Char_t     string[1000];
Int_t      bCheckButton = 1;
Int_t      bComboBox = 1;

    strcpy(string, "Some start value");

    pdFile->AddOpenFileName("Input file", fileName);
    pdFile->AddLine();

    pdFile->AddLabel("Insert some parameters");
    pdFile->AddEditInt("Some integer value", &iInt);
    pdFile->AddEditFloat("and some float", &fFloat);
    pdFile->AddEditString("and string", string, ciStringLength);
    pdFile->AddLine();

    pdFile->AddLabel("Select something");
    pdFile->AddCheckButton("Do something ?", &bCheckButton);
    pdFile->AddComboBox("What to do ?", &bComboBox);
    pdFile->AddEntry("Option 1", 1);
    pdFile->AddEntry("Second option", 2);
    pdFile->AddEntry("10th", 10);

    pdFile->AddLine();

    if( ! pdFile->Show())
    {
        printf("\nCanceled !!!\n\n");
        return;
    }

    printf("\nOutput from the dialog:\nFile      : %s\n"
            "Parameters:\n"
            "Integer   : %d\n"
            "Flaot     : %f\n"
            "String    : %s\n"
            "Selects   :\n"
            "Do        : %s\n"
            "Option    : %d\n\n", fileName, iInt,fFloat, string,
                                (bCheckButton) ? "Yes" : "No",
                                bComboBox);
}
