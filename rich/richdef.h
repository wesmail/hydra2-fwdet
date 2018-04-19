#ifndef __RichDEF_H
#define __RichDEF_H

#define RICH_MAX_SECTORS     6
#define RICH_MAX_COLS        96
#define RICH_MAX_ROWS        96
#define RICH_MAX_PADS        4712
#define RICH_MAX_ADCS        5
#define RICH_MAX_APVS        15
#define RICH_MAX_CHANNELS    128

#define RICH700_MAX_COLS     192
#define RICH700_MAX_ROWS     192

#define RICH_SIZE_HW         16384  // 3 bits for ADCs + 4 bits for APVs + 7 bits for Channels
#define RICH_SIZE_SW         9696   // maxCols*100 + maxRows

#define RICH_SECTOR_MASK     0x7
#define RICH_ADC_MASK        0x7
#define RICH_APV_MASK        0xf
#define RICH_CHANNEL_MASK    0x7F
#define RICH_CHARGE_MASK     0x3FFF

#define RICH_SECTOR_OFFSET   28
#define RICH_ADC_OFFSET      25
#define RICH_APV_OFFSET      21
#define RICH_CHANNEL_OFFSET  14
#define RICH_CHARGE_OFFSET   0

// JAM2017: new for rich700 reconstruction
#define RICH700_MAX_TDCCHANNELS 	32
#define RICH700_MAX_PMTPIXELS		64
#define RICH700_MAX_PMT 			576
#define RICH700_MAXPIXELS (RICH700_MAX_PMT * RICH700_MAX_PMTPIXELS)


#include "haddef.h"

const Cat_t catRichRaw            = RICH_OFFSET;
const Cat_t catRichCal            = RICH_OFFSET + 1;
const Cat_t catRichHit            = RICH_OFFSET + 2;
const Cat_t catRichHitHdr         = RICH_OFFSET + 3;
const Cat_t catRichDirClus        = RICH_OFFSET + 4;
const Cat_t catRichTrack          = RICH_OFFSET + 5;
const Cat_t catRich700Raw          = RICH_OFFSET + 6;

#endif



