#ifndef __HSTLTOOL_H__
#define __HSTLTOOL_H__

#include "TObject.h"

#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <iostream>

using namespace std;

class HSTLTool : public TObject {

public:

    HSTLTool();
    ~HSTLTool();

    //-------------------------------------------------------------
    // min / max element index
    template<typename T> static Int_t maxIndex(vector<T>& v){
	return v.empty() ? -1 : max_element(v.begin(), v.end()) - v.begin();
    }
    template<typename T> static Int_t minIndex(vector<T>& v){
	return v.empty() ? -1 : min_element(v.begin(), v.end()) - v.begin();
    }
    //-------------------------------------------------------------
    // min max element
    template<typename T> static T maxVal(vector<T>& v){
	return v.empty() ? -1 : *max_element(v.begin(), v.end());
    }
    template<class T> static T minVal(vector<T>& v){
	return v.empty() ? -1 : *min_element(v.begin(), v.end());
    }
    //-------------------------------------------------------------
    // sum up range
    template<typename T> static T sum(vector<T>& v,Int_t ind1 = -1,Int_t ind2 =-1){
	Int_t size = v.size();
	T sum = 0;
	Int_t start = ind1 >=0 && ind1 < size ? ind1 : 0;
	Int_t end   = ind2 >=0 && ind2 < size ? ind2 : size;
	for(Int_t i = start; i < end; i ++){
	    sum += v[i];
	}
	return sum;
    }

    //-------------------------------------------------------------
    // find in range
    template<typename T> static Int_t find(vector<T>& v,T val){
	typename vector<T>::iterator result = std::find(v.begin(), v.end(), val);
	if(result == v.end()) return -1;
	else {
	    return result - v.begin();
	}
    }

    /*
     //-------------------------------------------------------------
     // find in range
     template<typename T> static Int_t find(vector<T>& v,T val, Int_t ind1=-1, Int_t ind2=-1){
     Int_t size = v.size();
     Int_t start = ind1 >=0 && ind1 < size-1 ? ind1 : 0;
     Int_t end   = ind2 >=0 && ind2 < size-1 ? ind2 : size;
     if(size == 0 || ind2 < ind1) return -1;

     typename vector<T>::iterator i1 = (v.begin() + start);
     typename vector<T>::iterator i2 = (v.begin() + end);

     cout<<v.begin()<<" "<<v.end()<<" "<<i1<<" "<<i2<<endl;

     typename vector<T>::iterator result = std::find( (v.begin() + start), (v.begin() + end), val);
     cout<<"test "<<*result<<endl;
     if(result == v.end()) return -1;
     else {
     return result - v.begin();
     }
     }
     */
    //-------------------------------------------------------------
    // print range
    template<typename T> static T print(vector<T>& v,Int_t ind1 = -1,Int_t ind2 =-1){
	Int_t size = v.size();
	T sum = 0;
	Int_t start = ind1 >=0 && ind1 < size ? ind1 : 0;
	Int_t end   = ind2 >=0 && ind2 < size ? ind2 : size;
	for(Int_t i = start; i < end; i ++){
	    cout<<i<<" \t: "<<v[i]<<endl;
	}
	return sum;
    }

    ClassDef(HSTLTool,0)
};

#endif //__HSTLTOOL_H__
