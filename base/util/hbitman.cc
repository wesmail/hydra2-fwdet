#include "hbitman.h"

#include <stdlib.h>
#include  <iostream>
using namespace std;

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////
// HBitMan
//
// library of static functions
// look here for auxiliary functions for:
// bit manipulations.
//
// The direct bit functions can be used static.
// Bit counting starts at 0. 0 represents the lowest bit.
// The function do not check the range! Take care you
// stay inside the range.
// For the bit functions using names see the following
// example:
//
//  bits in Integer:
//  11
//  10       tofrec     (no sign)
//  9
//  8
//  7.       charge -1 0 1 (sign)
//  6.
//  5.       system 0-3 (no sign)
//  4........
//  3........sector 0-7 (no sign)
//  2........
//  1........
//
// // allocate the word map. Bit are allocated
// // in the order of declaration. word width include the
// // extra bit for sign if sign is specified!
// HBitMan bitman;
// bitman.createWord("sector",3,kFALSE);
// bitman.createWord("sys"   ,3,kFALSE);
// bitman.createWord("charge",3,kTRUE);  // 2bits+sign = 3
// bitman.createWord("tofrec",3,kFALSE);
// bitman.init();
//
//
//
// UInt_t common = 0;               // flag integer
//
// // set values
// bitman.setWord(common,"sector",3);
// bitman.setWord(common,"sys"   ,1);
// bitman.setWord(common,"charge",-1);
// bitman.setWord(common,"tofrec",3);
//
//
// bitman.printBits(common); // print binary representation
// cout<<bitman.getWord(common,"sector")<<endl;
// cout<<bitman.getWord(common,"sys")   <<endl;
// cout<<bitman.getWord(common,"charge")<<endl;
// cout<<bitman.getWord(common,"tofrec")<<endl;
// bitman.unsetWord(common,"sector");  // set bits for this word to 0
// bitman.printBits(common);
//
///////////////////////////////////////////////////////////

ClassImp(HBitMan)


    HBitMan::HBitMan()
{
}

HBitMan::~HBitMan()
{

}
//-----------------------------------------------------------
// direct bit setting
void HBitMan::printBits(UInt_t data){
    // print the binary reprentation of data.
    // Lowest bits at the right side
    for(Int_t i = 31; i >= 0; i --){ cout<< ((data >> i) & 0x1) <<flush; }
    cout<<endl;
}
void   HBitMan::setBit   (UInt_t &data,Int_t bit)  { data |=  ( 0x01 << bit ); }
void   HBitMan::unsetBit (UInt_t &data,Int_t bit)  { data &= ~( 0x01 << bit ); }
Bool_t HBitMan::getBit   (UInt_t &data,Int_t bit)  { return (data >> bit ) & 0x01 ; }

void HBitMan::setWord(UInt_t &data,Int_t num,Int_t width, Int_t val,Bool_t sign){
    // add data starting at bit num (from low) with n bits width
    // width includes the sign bit if sign is kTRUE.
    if(sign){
	data  = ( ((abs(val) & ((0x1<<(width-1))-1)) << num) | data ); //  first set word without sign
	data |= ( ( (val < 0) & 0x01) << (num+width) );                //  sign: highest bit of word=1 for negative
    } else {
	data  = ( ((val& ((0x1<<(width))-1)) << num) | data );
    }
}
void HBitMan::unsetWord(UInt_t &data,Int_t num,Int_t width){
    // unset data starting at bit num (from low) with n bits width
    data  =   ~((((0x1<<width)-1))<<num)  & data;
}
Int_t HBitMan::getWord(UInt_t &data,Int_t num,Int_t width,Bool_t sign){
    if(sign){
	if( ((data >> (num+width) ) & 0x01) == 1){
	    return  -((data >> num ) & ((0x1<<(width-1))-1));
	} else {
	    return ((data >> num ) & ((0x1<<(width-1))-1));
	}
    } else {
	return (data >> num ) & ((0x1<<width)-1);
    }
}
//-----------------------------------------------------------





//-----------------------------------------------------------
// bit setting using names
void HBitMan::createWord(const TString name,Int_t width, Bool_t sign){
    // bit setting using names
    word w;
    w.name  = name;
    w.width = width;
    w.sign  = sign;
    list.push_back(w);
}

void HBitMan::init(){
    // bit setting using names needs init after
    // defining all word to complete the catalog
    Int_t off = 0;

    for(UInt_t i = 0; i < list.size(); i ++){
	word& w  = list[i];
	w.offset = off;
	off     += w.width;
	wordmap[list[i].name] = list[i];
    }
}

void HBitMan::setWord(UInt_t &data,const TString name,Int_t val){
    // set word with lable name to value val
    word& w = wordmap[name];
    setWord(data,w.offset,w.width,val,w.sign);
}
void HBitMan::unsetWord(UInt_t &data,const TString name){
    // unset word with lable name to value 0
    word& w = wordmap[name];
    unsetWord(data,w.offset,w.width);
}
Int_t HBitMan::getWord(UInt_t &data,const TString name){
    // retrieve word with lable name
    word& w = wordmap[name];
    return getWord(data,w.offset,w.width,w.sign);
}
//-----------------------------------------------------------
