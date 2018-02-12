#ifndef RICHUTILS_H_
#define RICHUTILS_H_

class TCanvas;
class TH1;
class TH1D;
class TH2;
class TH2D;

#include "Rtypes.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using std::string;
using std::vector;

namespace RichUtils
{

template <class T>
string ToString(
   const T& value)
{
   std::stringstream ss;
   ss << (T)value;
   return ss.str();
}

template <class T>
string NumberToString(
   const T& value, Int_t precision = 1)
{
   // First determine number of digits in float
   string digis = ToString<Int_t>(value);
   Int_t ndigis = digis.size();

   std::stringstream ss;
   ss.precision(ndigis + precision);
   ss << value;
   return ss.str();
}

// Returns -1 if x<0, +1 if x>0, 0 if x==0
template <class T>
Int_t Sign(
   const T& x)
{
   static const T ZERO = 0;
   return (x > ZERO) ? 1 : ((x < ZERO) ? -1 : 0);
}

template< typename T, size_t N >
std::vector<T> MakeVector( const T (&data)[N] )
{
    return std::vector<T>(data, data+N);
}

void SaveCanvasAsImage(
   TCanvas* c,
   const std::string& dir,
   const std::string& option = "eps;png;gif");

string FindAndReplace(
      const string& name,
      const string& oldSubstr,
      const string& newSubstr);

vector<string> Split(
      const string& name,
      Char_t delimiter);

TH1D* DivideH1(
      TH1* h1,
      TH1* h2,
      const string& histName = "",
      Double_t scale = 100.,
      const string& titleYaxis = "Efficiency [%]");


TH2D* DivideH2(
      TH2* h1,
      TH2* h2,
     const string& histName = "",
     Double_t scale = 100.,
     const string& titleZaxis = "Efficiency [%]");

}

#endif /* RICHUTILS_H_ */
