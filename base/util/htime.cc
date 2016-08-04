#include "htime.h"
#include "TRegexp.h"
#include "TSystem.h"
#include <cstdio>
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////
// HTime
//
// Helper class to extract the time and date from
// hld file names and runids.
///////////////////////////////////////////////////////////

ClassImp(HTime)

HTime::HTime(){;}
HTime::~HTime(){;}


Bool_t HTime::isHldName(TString name,Bool_t EvtBuilder,Bool_t silent)
{
    TString file = name;
    Int_t minLength = 15;
    if(!EvtBuilder) minLength = 13;

    if(file.Length() < minLength) {
	if(!silent)printf("IsHldName() : Filename = %s is shorter than %i Characters, cannot be a correct hld name!\n",file.Data(),minLength);
        return kFALSE;
    }

    TRegexp reg ("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]"); // match 13 didgits
    TRegexp reg1("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]"); // match 11 didgits

    Ssiz_t ext;
    Ssiz_t i = 0;
    Ssiz_t out;


    if( EvtBuilder)out = file.Index(reg ,&ext,i);
    else           out = file.Index(reg1,&ext,i);


    if(out > 1 && ext == minLength-2){ // 13/11 digits + 2 char in front matched
	return kTRUE;
    } else {
	if(!silent) printf("isHldName() : Filename = %s does not match 2Char+%i digits, cannot be a correct hld name!\n",file.Data(),minLength-2);
	return kFALSE;
    }
}


TString HTime::stripFileName(TString name,Bool_t removeEvtBuilder, Bool_t silent)
{
    // removes path and extension (.root,.hld,.evt)
    // and returns the new name. Assumes that the
    // filename is composed like 'something+[2char+13digit}+something.root'
    // if removeEvtBuilder=kTRUE the last 2 digits for the evt builder
    // will be removed
    TString file = gSystem->BaseName(name);
    TRegexp reg("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]"); // match 13 didgits

    Ssiz_t ext;
    Ssiz_t i = 0;

    Ssiz_t out = file.Index(reg,&ext,i);


    if(out > 1 && ext == 13){ // 13 digits + 2 char in front matched

       file.Replace(0 ,out - 2,"");        // cut leading stuff

       if(file.Length() < 15) {
	   if(!silent)printf("stripFileName() : Filename = %s is shorter than 15 Characters, cannot be a correct hld name!\n",file.Data());
	   file = "";

       } else { //
	   file.Replace(15,file.Length() - 15,"");
           if(removeEvtBuilder)  file.Remove(file.Length()-2,2);

       }
    } else {
	if(!silent)printf("stripFileName() : Filename = %s does not match 2Char+13 digits, cannot be a correct hld name!\n",file.Data());
        file = "";
    }

    return file;
}


void HTime::dayOfYearToDate(Int_t year,Int_t dayOfYear,Int_t& month,Int_t& day,Bool_t print)
{
    // calulates from year (like 2011) and day of year (like 227)
    // the corresounding month (1-12) and day (1-31) of month.

    if(year < 2000) year+=2000;

    month    =0;
    day      =0;

    struct tm begin;
    time_t time_of_day;

    begin.tm_year  = year-1900;   // only years since 1900
    begin.tm_mon   = 0;           // counting from 0
    begin.tm_mday  = 1;           // counting from 1
    begin.tm_hour  = 0;
    begin.tm_min   = 0;
    begin.tm_sec   = 0;
    begin.tm_isdst = -1;          // 1= active, 0 =not active, -1 not available

    time_of_day = mktime(&begin);

    time_of_day += (dayOfYear-1)*24*60*60;

    tm* filetime = gmtime(&time_of_day);

    month=filetime->tm_mon+1;
    day  =filetime->tm_mday+1;

    if(print){ printf("dayOfYearToDate : y = %04i, day of y = %3i ===> month = %02i, day = %02i \n",year,dayOfYear,month,day); }
}

void HTime::splitFileName(TString name,TString& type,Int_t& year,Int_t& dayOfYear,Int_t& hour,Int_t& minute,Int_t& second,Int_t& evtbuild,Bool_t print)
{
    // splits the file name like 'be1122719211201'
    // into components like
    // type=be
    // year (since 2000), dayOfYear (1-366), hour (0-23),minutes (0-59),second (0-59)
    // evtbuilder number
    // The time stamp is given in local time of the event builder (time zone = +1)
    Char_t t[5];
    sscanf(name.Data(),"%2s%2d%3d%2d%2d%2d%2d",t,&year,&dayOfYear,&hour,&minute,&second,&evtbuild);
    type = t;
    if(print) {
	printf("splitFileName : file = %s , type %s, y = %02i, day of y = %03i, time %02i:%02i:%02i, evtbuild %2i \n"
	       ,name.Data(),type.Data(),year,dayOfYear,hour,minute,second,evtbuild );
    }
}

TString HTime::getTypeFileName(TString name,Bool_t print)
{
    // splits the file name like 'be1122719211201'
    Int_t year,dayOfYear,hour,minute,second,evtbuild;
    TString type;
    Char_t t[5];
    sscanf(name.Data(),"%2s%2d%3d%2d%2d%2d%2d",t,&year,&dayOfYear,&hour,&minute,&second,&evtbuild);
    type = t;
    if(print) {
	printf("splitFileName : file = %s , type = %s \n",name.Data(),type.Data());
    }
    return type;
}

Int_t HTime::getYearFileName(TString name,Bool_t print)
{
    // splits the file name like 'be1122719211201'
    TString type;
    Int_t year,dayOfYear,hour,minute,second,evtbuild;
    Char_t t[5];
    sscanf(name.Data(),"%2s%2d%3d%2d%2d%2d%2d",t,&year,&dayOfYear,&hour,&minute,&second,&evtbuild);
    type = t;
    if(print) {
	printf("splitFileName : file = %s , y = %04i \n",name.Data(),year);
    }
    return year;
}

Int_t HTime::getDayFileName(TString name,Bool_t print)
{
    // splits the file name like 'be1122719211201'
    TString type;
    Int_t year,dayOfYear,hour,minute,second,evtbuild;
    Char_t t[5];
    sscanf(name.Data(),"%2s%2d%3d%2d%2d%2d%2d",t,&year,&dayOfYear,&hour,&minute,&second,&evtbuild);
    type = t;
    if(print) {
	printf("splitFileName : file = %s , day of y = %03i \n",name.Data(),dayOfYear);
    }
    return dayOfYear;
}

void HTime::getTimeFileName(TString name,Int_t& hour,Int_t& minute,Int_t& second,Bool_t print)
{
    // splits the file name like 'be1122719211201'
    TString type;
    Int_t year,dayOfYear,evtbuild;
    Char_t t[5];
    sscanf(name.Data(),"%2s%2d%3d%2d%2d%2d%2d",t,&year,&dayOfYear,&hour,&minute,&second,&evtbuild);
    type = t;
    if(print) {
	printf("splitFileName : file = %s , time %02i:%02i:%02i \n",name.Data(),hour,minute,second);
    }
}

Int_t HTime::getEvtBuilderFileName(TString name,Bool_t print)
{
    // splits the file name like 'be1122719211201'
    TString type;
    Int_t year,dayOfYear,hour,minute,second,evtbuild;
    Char_t t[5];
    sscanf(name.Data(),"%2s%2d%3d%2d%2d%2d%2d",t,&year,&dayOfYear,&hour,&minute,&second,&evtbuild);
    type = t;
    if(print) {
	printf("splitFileName : file = %s , evtbuild %02i\n",name.Data(),evtbuild);
    }
    return evtbuild;
}

time_t HTime::fileNameToTime(TString name, Bool_t print)
{
    // splits the file name like 'be1122719211201'
    // into components and returns the corresponding
    // time_t (seconds from 1.1.1970).
    // The time stamp is given in local time of
    // the event builder (time zone = +1)
    // For help with the time functions (time_t ...)
    // lookup man pages ctime,localtime,gmtime etc.
    // use gmtime()/localtime() to convert to broken down time struct.

    TString type;
    Int_t y,doy,h,min,sec,evbuild;

    splitFileName(name,type,y,doy,h,min,sec,evbuild);

    Int_t month,day;
    dayOfYearToDate(y+2000,doy,month,day);

    struct tm t;
    time_t time_of_day;
    time_t gtime_of_day;

    t.tm_year  = y+100;   // only years since  1900
    t.tm_mon   = month-1; // counting from 0   0-11
    t.tm_mday  = day;     // counting from 1   1-31
    t.tm_hour  = h;       //                   0-23
    t.tm_min   = min;     //                   0-59
    t.tm_sec   = sec;     //                   0-59 (leap 60)
    t.tm_isdst = 0;       //                  1=daylight saving time active  , 0 =not active, -1 not available find out from local timezone
                          //                  ~ endmarch - end october
    time_of_day = mktime(&t);           // local time
    gtime_of_day = time_of_day - 3600;  // gmtime :  +1 timezone substracted


    struct tm* t2 = gmtime(&gtime_of_day);  // we nee to compare to findout if DST was used

    if(t2->tm_hour+2 == h) { // we have to use summer time (+1 time zone + summer shift)

	t.tm_year  = y+100;   // only years since  1900
	t.tm_mon   = month-1; // counting from 0   0-11
	t.tm_mday  = day;     // counting from 1   1-31
	t.tm_hour  = h;       //                   0-23
	t.tm_min   = min;     //                   0-59
	t.tm_sec   = sec;     //                   0-59 (leap 60)
	t.tm_isdst = 1;
	time_of_day = mktime(&t);
    }

    if(print) { cout<<"fileNameToTime : time = "<<time_of_day<<" , "<<flush; printf("%s\n",ctime(&time_of_day)); }
    return time_of_day;
}

Int_t HTime::isDST(TString name)
{
    // expects the file name like 'be1122719211201'
    // returns 1 if DST (summer time) was active
    // or 0 if incative while the file was recorded

    TString type;
    Int_t y,doy,h,min,sec,evbuild;

    splitFileName(name,type,y,doy,h,min,sec,evbuild);

    Int_t month,day;
    dayOfYearToDate(y+2000,doy,month,day);

    struct tm t;
    time_t time_of_day;
    time_t gtime_of_day;

    Int_t isDST = 0;

    t.tm_year  = y+100;   // only years since  1900
    t.tm_mon   = month-1; // counting from 0   0-11
    t.tm_mday  = day;     // counting from 1   1-31
    t.tm_hour  = h;       //                   0-23
    t.tm_min   = min;     //                   0-59
    t.tm_sec   = sec;     //                   0-59 (leap 60)
    t.tm_isdst = 0;       //                  1=daylight saving time active  , 0 =not active, -1 not available find out from local timezone
                          //                  ~ endmarch - end october
    time_of_day = mktime(&t);           // local time
    gtime_of_day = time_of_day - 3600;  // gmtime :  +1 timezone substracted


    struct tm* t2 = gmtime(&gtime_of_day);  // we nee to compare to findout if DST was used

    if(t2->tm_hour+2 == h) { // we have to use summer time (+1 time zone + summer shift)

	t.tm_year  = y+100;   // only years since  1900
	t.tm_mon   = month-1; // counting from 0   0-11
	t.tm_mday  = day;     // counting from 1   1-31
	t.tm_hour  = h;       //                   0-23
	t.tm_min   = min;     //                   0-59
	t.tm_sec   = sec;     //                   0-59 (leap 60)
	t.tm_isdst = 1;
	time_of_day = mktime(&t);
        isDST = 1;
    }

    return isDST;
}

time_t HTime::runIdToTime(time_t runid, Int_t timezone, Bool_t print)
{
    // recalulates the runID to time (add offset 1200000000)
    // The time stamp is given in local time of
    // the event builder (time zone = +1). Since there
    // is no way to determine from the runID allone
    // if summer/winter time (DST) has been used while
    // recording your have to provide your
    // local time zone.
    // For help with the time functions (time_t ...)
    // lookup man pages ctime,localtime,gmtime etc.
    // use gmtime()/localtime() to convert to broken down
    // time struct.

    time_t t = runid;

    t += 1200000000;

    Int_t diff = 1 - timezone;

    t -= 3600 * diff;

    if(print) { cout<<"runIdToTime : runID "<<runid<<" , timezone ="<<timezone<<" , time = "<<t<<" , "<<flush; printf("%s\n",ctime(&t)); }

    return t;
}


void HTime::runIdToBrokenTime(Int_t runid,
			      Int_t& year,Int_t& month,Int_t& day,
			      Int_t& hour,Int_t& min  ,Int_t& sec,
			      Int_t timezone,Bool_t print)
{
    // recalulates the runID to broken time (year since 0, month 1-12,
    // day 1-31, min 0-59, sec 0-59)
    // The time stamp is given in local time of
    // the event builder (time zone = +1). Since there
    // is no way to determine from the runID allone
    // if summer/winter time (DST) has been used while
    // recording your have to provide your
    // local time zone.
    // For help with the time functions (time_t ...)
    // lookup man pages ctime,localtime,gmtime etc.
    // use gmtime()/localtime() to convert to broken down
    // time struct.

    time_t t = HTime::runIdToTime(runid,timezone,kFALSE);
    struct tm* t2 = gmtime(&t);



    year =  t2->tm_year     + 1900;   // since 1900
    month=  t2->tm_mon      + 1;      // 0-11
    day  =  t2->tm_mday;              // 1-31
    hour =  t2->tm_hour + timezone;   // 0-23
    min  =  t2->tm_min;               // 0-59
    sec  =  t2->tm_sec;               // 0-59

    if(print) {
	cout<<"runIdToBrokenTime : runID "<<runid<<" , timezone ="<<timezone
	    <<flush;
	printf(", %02i.%02i.%i, %02i:%02i:%02i\n",day,month,year,hour,min,sec);
    }
}


