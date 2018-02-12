
#ifndef HRICH700_PMTTYPEENUM_H_
#define HRICH700_PMTTYPEENUM_H_

enum HRich700PmtTypeEnum {
    // COSY 2017 measurements, NO WLS
    HRich700PmtTypeCosy17NoWls = 20,

    // COSY 2017 measurements, WITH WLS
    HRich700PmtTypeCosy17WithWls = 21,

    // PMT efficiencies for Protvino-type PMT
    HRich700PmtTypeProtvino = 1,

    // quantum efficiency for CsI photocathode
    // approximately read off from fig.3 in NIM A 433 (1999) 201 (HADES)
    HRich700PmtTypeCsi = 3,

    // PMT efficiencies for Hamamatsu H8500
    // (Flat type Multianode Photomultiplier)
    HRich700PmtTypeH8500 = 2,

    // PMT efficiencies for Hamamatsu H8500-03
    // (Flat type Multianode Photomultiplier with UV window)
    HRich700PmtTypeH8500_03 = 4,

    // PMT efficiencies for Hamamatsu H8500 + WLS film
    // (Flat type Multianode Photomultiplier with UV window)
    HRich700PmtTypeH8500WithWls = 5,


    // -------------------------------------------------------------------------------------------
    //   QE measured at Wuppertal University (BUW), spring 2011
    // -------------------------------------------------------------------------------------------

    // Measured PMT efficiencies for MAPMTs (H8500D-03) at posF (BUW measurement) ##### CernOct2011 #####
    // (Flat type Multianode Photomultiplier with BA cathode + UV window)
    HRich700PmtTypeCern11H8500_6 = 6,

    // Measured PMT efficiencies for MAPMTs (H8500D-03) at posH (BUW measurement) --dipcoated WLS film -- ##### CernOct2011 #####
    // (Flat type Multianode Photomultiplier with UV window)
    HRich700PmtTypeCern11H8500_10 = 10,

    // Measured PMT efficiencies for MAPMTs (H8500D-03) at posH (BUW measurement) --dipcoated WLS film -- ##### CernOct2011 #####
    // (Flat type Multianode Photomultiplier with UV window)         ##### -3.8 % due to inhomogeneity #####
    HRich700PmtTypeCern11H8500_11 = 11,

    // Measured PMT efficiencies for MAPMTs (H8500D-03) at posD (BUW measurement) ##### CernOct2011 #####
    // (Flat type Multianode Photomultiplier with UV window)
    HRich700PmtTypeCern11H8500_12 = 12,

    // Measured PMT efficiencies for R11265 (BUW measurement)
    // (Flat type Multianode Photomultiplier with UV window, SBA, 1 square inch)
    HRich700PmtTypeR11265_13 = 13,

    // Measured PMT efficiencies for R11265 -- dipcoated WLS film -- (BUW measurement)
    // (Flat type Multianode Photomultiplier with UV window, SBA, 1 square inch)
    HRich700PmtTypeR11265_14 = 14,

    // Measured PMT efficiencies for MAPMTs (H8500D-03) at posC (BUW measurement) ##### CernOct2012 #####
    // (Flat type Multianode Photomultiplier with UV window)
    HRich700PmtTypeCern12H8500_15 = 15,

    // Measured PMT efficiencies for MAPMTs (H8500D-03) at posC -- optimized dipcoated WLS film -- (BUW measurement) ##### CernOct2012 #####
    // (Flat type Multianode Photomultiplier with UV window)
    HRich700PmtTypeCern12H8500_16 = 16,

    // Measured PMT efficiencies for one MAPMT (H10966A-103) at posE (BUW measurement) ##### CernOct2012 #####
    // (Flat type Multianode Photomultiplier with UV window)
    HRich700PmtTypeCern12H10966A_17 = 17,

    // Measured PMT efficiencies for one MAPMT (H10966A-103) at posE -- optimized dipcoated WLS film -- (BUW measurement) ##### CernOct2012 #####
    // (Flat type Multianode Photomultiplier with UV window)
    HRich700PmtTypeCern12H10966A_18 = 18,

    //ideal detector
    HRich700PmtTypeIdeal = 0

};

#endif /* HRICH700_PMTTYPEENUM_H_ */
