#ifndef __HTRBNETDEF_H__
#define __HTRBNETDEF_H__

namespace Trbnet {

    // Enumerated variables for Trbnet address ranges reserved for the
    // timing detectors

  enum eTrbnetAddressRange {
        kTrb2MinTrbnetAddress  = 0x4000,
        kTrb2MaxTrbnetAddress  = 0x4fff,
        kStartMinTrbnetAddress = 0x4000,
        kStartMaxTrbnetAddress = 0x40ff,
        kWallMinTrbnetAddress  = 0x4400,
        kWallMaxTrbnetAddress  = 0x47ff,
        kRpcMinTrbnetAddress   = 0x4800,
        kRpcMaxTrbnetAddress   = 0x4bff,
        kTofMinTrbnetAddress   = 0x4c00,
        kTofMaxTrbnetAddress   = 0x4fff,
        kPionTrackerTrb3MinTrbnetAddress = 0x3800,
        kPionTrackerTrb3MaxTrbnetAddress = 0x38ff,
        kStartTrb3MinTrbnetAddress  = 0x5000,
        kStartTrb3MaxTrbnetAddress  = 0x50ff,
        kEmcTrb3MinTrbnetAddress  = 0x6000,
        kEmcTrb3MaxTrbnetAddress  = 0x67ff
  };
}

#endif // __HTRBNETDEF_H__
