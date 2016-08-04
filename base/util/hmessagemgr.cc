//*-- Author : Joern Wuestenfeld
//*-- Modified : 15/04/2002 by Joern Wuestenfeld
//*-- Modified : 04/04/2002 by Joern Wuestenfeld
//*-- Modified : 28/03/2002 by Joern Wuestenfeld
//*-- Modified : 27/02/2003 by Joern Wuestenfeld
//*-- Modified : 07/05/2003 by Joern Wuestenfeld

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////
//
//  HMessageMgr
//
//  This class manages three different outputs for all kind of messages.
//  Besides the standart output, which is the default, one can also
//  write the messages to a file, or via a socket to a remote logging process.
//
//  The usage of this class should be done via the four macros that are defined
//  in the hmessagemgr.h file.
//  DEBUG_msg(level, det, text)
//  ERROR_msg(det, text)
//  WARNING_msg(level, det, text)
//  INFO_msg(level, det, text)
//
//  The meaning of the parameters is:
//  level - level of this message, never set it to zero because this means
//          no message at all will be printed
//  det   - detector which sends this message, can be one of:
//          DET_RICH DET_START DET_MDC DET_TOF DET_TOFINO DET_SHOWER
//          DET_TRIGGER
//  text  - text to print
//
/////////////////////////////////////////////////////
using namespace std;
#include "TSystem.h"
#include "hmessagemgr.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>


HMessageMgr::HMessageMgr(const Char_t *name, const Char_t *title)
    : TNamed(name,title)
{
    /// This is the default constructor inherited from TNamed.
    //
    //  Sets default logging to stdout and stderr.
    //  The default levels are: no debugging, but all of error, warning and info.
    //  param name Name of the object
    //  param title Title of the object




    msgFile = NULL;
    errorFile = NULL;
    msgSocket = NULL;
    errorSocket = NULL;
    file = NULL;

    debugLevel = 0;
    warningLevel = LEVELMAX;
    infoLevel = LEVELMAX;
    detectorMsg = DET_ALL;
    line = 0;
    screen = kFALSE;
    file = new char[2000];
}

HMessageMgr::~HMessageMgr(void)
{
    // Resets the message logging to ist default.
    //  Closses all loggfiles or open sockets, and loggs further messages to
    //  stdout and stderr.
    //  Also the loglevels are set to the same values as in the constructor.
    setDefault();
    delete [] file;
    file = NULL;
}

Bool_t HMessageMgr::setDefault(void)
{
    //  Closses all loggfiles or open sockets, and loggs further messages to
    //  stdout and stderr.
    //  Also the loglevels are set to the same values as in the constructor.
    if(msgFile)
    {
	delete msgFile;
    }
    if(errorFile)
    {
	delete errorFile;
    }
    if(msgSocket)
    {
	msgSocket->Close();
	delete msgSocket;
	msgSocket = NULL;
    }
    if(errorSocket)
    {
	errorSocket->Close();
	delete errorSocket;
	errorSocket = NULL;
    }

    debugLevel = 0;
    warningLevel = LEVELMAX;
    infoLevel = LEVELMAX;
    detectorMsg = DET_ALL;
    screen = kFALSE;

    return kTRUE;
}

Bool_t HMessageMgr::setFile(const Char_t *dir)
{
    // Sets the logfile.
    // Input is the directory to where to log to.
    // The filename is derived from the name of this object, that is given
    // when creating it.
    // Two files are created. One with extension .msg for all messages of
    // type info, warning and debug.
    // The other one with extension .err fo the error messages.
    // param dir Directory where to store the logfiles.
    // retval setFile Returns kTRUE if file could be opened, otherwise kFALSE.

    Char_t *fname;

    fname = (Char_t *)calloc(1,2048);
    if(msgFile)
    {
	delete msgFile;
	msgFile = NULL;
    }
    if(errorFile)
    {
	delete errorFile;
	errorFile  = NULL;
    }
    if(!msgFile)
    {
	if((UInt_t)strlen(dir) > (UInt_t)(2048 - fName.Length() + 5))
	{
	    free(fname);
	    return kFALSE;
	}
	else
	{
	    sprintf(fname,"%s%s%s",dir,fName.Data(),".msg");
	}
	msgFile = new ofstream(fname,ios::out);
	if(!msgFile)
	{
	    free(fname);
	    return kFALSE;
	}
    }
    else
    {
	free(fname);
	return kFALSE;
    }
    if(!errorFile)
    {
	if((UInt_t )strlen(dir) > (UInt_t)(2048 - fName.Length() + 5))
	{
	    free(fname);
	    return kFALSE;
	}
	else
	{
	    sprintf(fname,"%s%s%s",dir,fName.Data(),".err");
	}
	errorFile = new ofstream(fname,ios::out);
	if(!errorFile)
	{
	    free(fname);
	    if(msgFile)
	    {
		msgFile->close();
		msgFile = NULL;
	    }
	    return kFALSE;
	}
    }
    else
    {
	free(fname);
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HMessageMgr::setPort(const Char_t *host, Int_t port)
{
    // Opens two socket connections to a logging server on the specified host.
    // The passed port is used for the message logging. Errors are send to
    // the portaddress port + 10.
    // eventually opened logfiles are closed, if the connection could be opened.
    // Otherwise the current logging stays active.
    // param host Name of the host to send the logmessages to.
    // param port Port to use for communication with remote process.
    // retval setPort Returns kTRUE if remote process could be contacted, otherwise
    // kFALSE is returned.

    msgSocket = new TSocket(host,port);
    errorSocket = new TSocket(host,port+10);
    thisHost = new TString(gSystem->HostName());
    if(msgSocket->IsValid() && errorSocket->IsValid())
    {
	if(msgFile)
	{
	    msgFile->close();
	    msgFile = NULL;
	}
	if(errorFile)
	{
	    errorFile->close();
	    errorFile = NULL;
	}
	msgSocket->Send(thisHost->Data());
	errorSocket->Send(thisHost->Data());
	return kTRUE;
    }
    else
    {
	delete msgSocket;
	delete errorSocket;
	msgSocket = NULL;
	errorSocket = NULL;
	return kFALSE;
    }
}

Bool_t HMessageMgr::switchOff(void)
{
    // Switches of the logging completly by setting the output to /dev/null.
    // Could probably done also by redirecting stdout and stderr to /dev/null.
    // Currently opened files or sockets are closed.
    // retval switchOff Returns kTRUE on succsess, kFALSE otherwise.

    Char_t *fname;

    fname = (Char_t *)calloc(1,2048);
    setDefault();
    if(msgFile)
    {
	delete msgFile;
	msgFile = NULL;
    }
    if(errorFile)
    {
	delete errorFile;
	errorFile  = NULL;
    }
    if(!msgFile)
    {
	sprintf(fname,"%s","/dev/null");
    }
    msgFile = new ofstream(fname,ios::out);
    if(!msgFile)
    {
	free(fname);
	return kFALSE;
    }
    else
    {
	free(fname);
	return kFALSE;
    }
    if(!errorFile)
    {
	sprintf(fname,"%s","/dev/null");
    }
    errorFile = new ofstream(fname,ios::out);
    if(!errorFile)
    {
	free(fname);
	return kFALSE;
    }
    else
    {
	free(fname);
	return kFALSE;
    }

    return kTRUE;
}

void HMessageMgr::setDebugLevel(Char_t level)
{
    // Sets the level of debug messages to level.
    // param level Level of debugging, max 10 min 0.

    if(level > 10)
	debugLevel = 10;
    else
	if(level <= 0)
	    debugLevel = 1;
	else
	    debugLevel = level;
}

void HMessageMgr::setInfoLevel(Char_t level)
{
    // Sets the level of info messages to level.
    // param level Level of information, max 10 min 0.

    if(level > 1)
	infoLevel = 10;
    else
	if(level <= 0)
	    infoLevel = 1;
	else
	    infoLevel = level;
}

void HMessageMgr::setWarningLevel(Char_t level)
{
    // Sets the level of warning messages to level.
    //  param level Level of warning, max 10 min 0.
    if(level > 10)
	warningLevel = 10;
    else
	if(level <= 0)
	    warningLevel = 1;
	else
	    warningLevel = level;
}

/// Selects for which detector messages should be displayed.
/**
 \param det An or'd pattern of the DET_* constants.
 */
void HMessageMgr::enableDetectorMsg(Int_t det)
{
    // Selects for which detector messages should be displayed.
    // param det An or'd pattern of the DET_* constants.
    detectorMsg = det;
}

void HMessageMgr::messageSeperator(const Char_t *seperator, Int_t num)
{
    // This function prints a line with character seperator.
    // The seperator character is repeated num times on this line.
    // param seperator character to be used as seperator
    // param num How often the character is to be repeated on the same line.
    Int_t i;

    msg1 = (Char_t *)calloc(1,num+2);
    for(i=0;i<num;i++)
    {
	strcat(msg1,seperator);
    }
    write(msg1);
    free(msg1);
}

void HMessageMgr::hydraErrorHandler(Int_t level, Bool_t abort, const Char_t *location, const Char_t *msg)
{
    // Message handler for ROOT. Can be installed via gSystem->SetErrorHandler()
    // Used by all ROOT classes to do message logging.
    // Gives a common message layout.
    // param level Level of logging
    // param abort Wether to abort the execution after the printing of the message
    // param location Location of the message
    // param msg Text to be printed.
    if (level < gErrorIgnoreLevel)
	return;

    if((level >= kError) | (level >= kSysError) | (level >= kFatal))
    {
	gHades->getMsg()->error(10,DET_ALL,location,msg);
    }
    else
	if(level >= kWarning)
	{
	    gHades->getMsg()->warning(10,DET_ALL,location,msg);
	}
	else
	    if(level >= kInfo)
	    {
		gHades->getMsg()->info(10,DET_ALL,location,msg);
	    }
}

/// This function should never be called directly, use the DEBUG macro instead.
/**
 This function writes a debug message to the output.
 It looks like this example:<br>
 debug  <Hades          >:No input specified<p>
 \param level Level of message logging
 \param det One of the DET_* constants that defines the detector printing this message.
 \param className Name of the class that prints this message.
 \param text Text to 'print.
 */
void HMessageMgr::debug(Char_t level, Int_t det, const Char_t *className, const Char_t *text)
{
    // This function should never be called directly, use the DEBUG macro instead.
    // This function writes a debug message to the output.
    // It looks like this example:<br>
    // debug  <Hades          >:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param className Name of the class that prints this message.
    // param text Text to 'print.
    if(level <= 0)
	level = 1;
    if((level <= debugLevel) && (detectorMsg & det))
    {
	msg1 = (Char_t*)calloc(1,strlen((Char_t *)text)+30);
	if(msg1)
	{
	    if(line == 0)
		sprintf(msg1,"%7s%s%s%s%s","debug","<",className,">: ",text);
	    else
		sprintf(msg1,"%7s%s%s %s%s%d%s%s","debug","<",className,file,":",line,">:",text);
	    write(msg1);
	    free(msg1);
	}
    }
}

void HMessageMgr::error(Char_t level, Int_t det, const Char_t *className, const Char_t *text)
{
    // This function should never be called directly, use the ERROR macro instead.
    // This function writes a error message to the output.
    // It looks like this example:<br>
    // error  <Hades          >:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param className Name of the class that prints this message.
    // param text Text to print.
    if(level <= 0)
	level = 1;
    if(detectorMsg & det)
    {
	msg1 = (Char_t*)calloc(1,strlen((Char_t *)text)+30);
	if(msg1)
	{
	    sprintf(msg1,"%7s%s%s%s%s","error","<",className,">: ",text);
	    ewrite(msg1);
	    free(msg1);
	}
    }
}

void HMessageMgr::warning(Char_t level, Int_t det, const Char_t *className, const Char_t *text)
{
    // This function should never be called directly, use the WARNING macro instead.
    // This function writes a warning message to the output.
    // It looks like this example:<br>
    // warning<Hades          >:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param className Name of the class that prints this message.
    // param text Text to print.
    if(level <= 0)
	level = 1;
    if((level <= warningLevel)  && (detectorMsg & det))
    {
	msg1 = (Char_t*)calloc(1,strlen((Char_t *)text)+strlen(className)+15);
	if(msg1)
	{
	    sprintf(msg1,"%7s%s%s%s%s","warning","<",className,">: ",text);
	    write(msg1);
	    free(msg1);
	}
    }
}

void HMessageMgr::info(Char_t level, Int_t det, const Char_t *className, const Char_t *text)
{
    // This function should never be called directly, use the INFO macro instead.
    // This function writes a info message to the output.
    //  It looks like this example:<br>
    //  info<         Hades>:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param className Name of the class that prints this message.
    // param text Text to print.
    if(level <= 0)
	level = 1;
    if((level <= infoLevel)  && (detectorMsg & det))
    {
	msg1 = (Char_t*)calloc(1,strlen((Char_t *)text)+30);
	if(msg1)
	{
	    sprintf(msg1,"%7s%s%s%s%s","info","<",className,">: ",text);
	    write(msg1);
	    free(msg1);
	}
    }
}

void HMessageMgr::debug(Int_t level, Int_t det, const Char_t *className, const Char_t *text, ...)
{
    // This function put out an error message including some variable arguments.
    // The text parameter takes the standart parameters as in printf().
    // The output looks like:<br>
    // debug<         Hades>:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param className Name of the class that prints this message.
    // param text Text to print.
    // param ... Variable number of arguments, that will be used to fill the text
    va_list ap;

    if(level <= 0)
	level = 1;
    if((level <= debugLevel)  && (detectorMsg & det))
    {
	msg1 = (Char_t *)calloc(1,2048);
	msg2 = (Char_t *)calloc(1,2048);
	if(line == 0)
	    sprintf(msg1,"%7s%s%s%s","debug","<",className,">:");
	else
	    sprintf(msg1,"%7s%s%s %s%s%d%s","debug","<",className,file,":",line,">: ");
	va_start(ap,text);
	vsprintf(msg2,text,ap);
	va_end(ap);
	strcat(msg1,msg2);
	write(msg1);
	free(msg1);
	free(msg2);
    }
}

void HMessageMgr::error(Int_t level, Int_t det, const Char_t *className, const Char_t *text, ...)
{
    // This function put out an error message including some variable arguments.
    // The text parameter takes the standart parameters as in printf().
    //  The output looks like:<br>
    //  error<         Hades>:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param className Name of the class that prints this message.
    // param text Text to print.
    // param ... Variable number of arguments, that will be used to fill the text
    va_list ap;

    if(level <= 0)
	level = 1;
    if(detectorMsg & det)
    {
	msg1 = (Char_t *)calloc(1,2048);
	msg2 = (Char_t *)calloc(1,2048);
	sprintf(msg1,"%7s%s%s%s","error","<",className,">: ");
	va_start(ap,text);
	vsprintf(msg2,text,ap);
	va_end(ap);
	strcat(msg1,msg2);
	ewrite(msg1);
	free(msg1);
	free(msg2);
    }
}

void HMessageMgr::warning(Int_t level, Int_t det, const Char_t *className, const Char_t *text, ...)
{
    // This function put out an warning message including some variable arguments.
    // The text parameter takes the standart parameters as in printf().
    // The output looks like:<br>
    // warning<         Hades>:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param className Name of the class that prints this message.
    // param text Text to print.
    // param ... Variable number of arguments, that will be used to fill the text
    va_list ap;
    UInt_t size1,size2;

    if(level <= 0)
	level = 1;
    if((level <= warningLevel)  && (detectorMsg & det))
    {
	size2 = strlen(text)+2048;
	size1 = strlen(className) + size2 + 15;
	msg2 = (Char_t *)calloc(1,size2);
	msg1 = (Char_t *)calloc(1,size1);
	sprintf(msg1,"%7s%s%s%s","warning","<",className,">: ");
	va_start(ap,text);
	vsnprintf(msg2,size2,text,ap);
	va_end(ap);
	strcat(msg1,msg2);
	write(msg1);
	free(msg1);
	free(msg2);
    }
}

void HMessageMgr::info(Int_t level, Int_t det, const Char_t *className, const Char_t *text, ...)
{
    // This function put out an info message including some variable arguments.
    // The text parameter takes the standart parameters as in printf().
    //  The output looks like:<br>
    //  info<         Hades>:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param className Name of the class that prints this message.
    // param text Text to print.
    // param ... Variable number of arguments, that will be used to fill the text

    va_list ap;

    if(level <= 0)
	level = 1;
    if((level <= infoLevel)  && (detectorMsg & det))
    {


	msg1 = (Char_t *)calloc(1,2048);
	msg2 = (Char_t *)calloc(1,2048);
	sprintf(msg1,"%7s%s%s%s","info","<",className,">: ");
	va_start(ap,text);
	vsprintf(msg2,text,ap);
	va_end(ap);
	strcat(msg1,msg2);
	write(msg1);
	free(msg1);
	free(msg2);
    }
}

void HMessageMgr::debugB(Char_t level, Int_t det, const Char_t *text, ...)
{
    // This function put out an error message including some variable arguments.
    // The text parameter takes the standart parameters as in printf().
    // The output looks like:<br>
    // debug<         Hades>:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param className Name of the class that prints this message.
    // param file Filename that prints the message.
    // param line Linenumber from where the message is printed.
    // param text Text to print.
    // param ... Variable number of arguments, that will be used to fill the text
    va_list ap;

    if(level <= 0)
	level = 1;
    if((level <= debugLevel)  && (detectorMsg & det))
    {
	msg1 = (Char_t *)calloc(1,2048);
	msg2 = (Char_t *)calloc(1,2048);
	sprintf(msg1,"%7s%s%s %s%d%s","debug","<",file,":",line,">: ");
	va_start(ap,text);
	vsprintf(msg2,text,ap);
	va_end(ap);
	strcat(msg1,msg2);
	write(msg1);
	free(msg1);
	free(msg2);
    }
}

void HMessageMgr::errorB(Char_t level, Int_t det, const Char_t *text, ...)
{
    // This function put out an error message including some variable arguments.
    // The text parameter takes the standart parameters as in printf().
    // The output looks like:<br>
    // error<         Hades>:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param text Text to print.
    // param ... Variable number of arguments, that will be used to fill the text
    va_list ap;

    if(level <= 0)
	level = 1;
    if(detectorMsg & det)
    {
	msg1 = (Char_t *)calloc(1,2048);
	msg2 = (Char_t *)calloc(1,2048);
	sprintf(msg1,"%7s","error :");
	va_start(ap,text);
	vsprintf(msg2,text,ap);
	va_end(ap);
	strcat(msg1,msg2);
	ewrite(msg1);
	free(msg1);
	free(msg2);
    }
}

void HMessageMgr::warningB(Char_t level, Int_t det, const Char_t *text, ...)
{
    // This function put out an warning message including some variable arguments.
    // The text parameter takes the standart parameters as in printf().
    // The output looks like:<br>
    // warning<         Hades>:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param text Text to print.
    // param ... Variable number of arguments, that will be used to fill the text
    va_list ap;
    UInt_t size1,size2;

    if(level <= 0)
	level = 1;
    if((level <= warningLevel)  && (detectorMsg & det))
    {
	size2 = strlen(text)+2048;
	size1 = size2 + 15;
	msg2 = (Char_t *)calloc(1,size2);
	msg1 = (Char_t *)calloc(1,size1);
	sprintf(msg1,"%7s","warning : ");
	va_start(ap,text);
	vsnprintf(msg2,size2,text,ap);
	va_end(ap);
	strcat(msg1,msg2);
	write(msg1);
	free(msg1);
	free(msg2);
    }
}

void HMessageMgr::infoB(Char_t level, Int_t det, const Char_t *text, ...)
{
    // This function put out an info message including some variable arguments.
    // The text parameter takes the standart parameters as in printf().
    // The output looks like:<br>
    // info<         Hades>:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param text Text to print.
    // param ... Variable number of arguments, that will be used to fill the text
    va_list ap;

    if(level <= 0)
	level = 1;
    if((level <= infoLevel)  && (detectorMsg & det))
    {
	msg1 = (Char_t *)calloc(1,2048);
	msg2 = (Char_t *)calloc(1,2048);
	sprintf(msg1,"%7s","info : ");
	va_start(ap,text);
	vsprintf(msg2,text,ap);
	va_end(ap);
	strcat(msg1,msg2);
	write(msg1);
	free(msg1);
	free(msg2);
    }
}

void HMessageMgr::debug(Int_t level, Int_t det, const Char_t * className, const Char_t *file, Int_t line, const Char_t *text, ...)
{
    // This function put out an error message including some variable arguments.
    // The text parameter takes the standart parameters as in printf().
    // The output looks like:<br>
    // debug<         Hades>:No input specified<p>
    // param level Level of message logging
    // param det One of the DET_* constants that defines the detector printing this message.
    // param file Filename that prints the message.
    // param line Linenumber from where the message is printed.
    // param text Text to print.
    // param ... Variable number of arguments, that will be used to fill the text
    va_list ap;

    if(level <= 0)
	level = 1;
    if((level <= debugLevel)  && (detectorMsg & det))
    {
	msg1 = (Char_t *)calloc(1,2048);
	msg2 = (Char_t *)calloc(1,2048);
	sprintf(msg1,"%7s%s%s %s%s%d%s","debug","<",className,file,":",line,">: ");
	va_start(ap,text);
	vsprintf(msg2,text,ap);
	va_end(ap);
	strcat(msg1,msg2);
	write(msg1);
	free(msg1);
	free(msg2);
    }
}


void HMessageMgr::write(const Char_t *text)
{
    // Internal function used to write to the correct output.
    // Used for info, warning and debug messages.
    // Adds the name of the host the code is running on, if output is send via
    // socket to a remote logging process.
    // If screen is set to kTRUE the output is writen to stdout in addition to
    // the logfile.
    // param text Text to be writen to specified target.
    Char_t *msg;

    if(msgFile)
    {
	msgFile->write(text,strlen(text));
	if(screen)
	    cout << text << endl;
    }
    else
    {
	if(msgSocket)
	{
	    msg =(Char_t *)calloc(1,strlen(text) + 20);
	    sprintf(msg,"<%10s>%s",thisHost->Data(),text);
	    msgSocket->Send(msg);
	    free(msg);
	}
	else
	{
	    cout << text << endl;
	}
    }
}

void HMessageMgr::ewrite(const Char_t *text)
{
    // Internal function used to write to the correct output.
    // Used for error messages.
    // Adds the name of the host the code is running on, if output is send via
    // socket to a remote logging process.
    // If screen is set to kTRUE the output is writen to stdout in addition to
    // the logfile.
    // param text Text to be writen to specified target.
    Char_t *msg;

    if(errorFile)
    {
	errorFile->write(text,strlen(text));
	if(screen)
	    cout << text << endl;
    }
    else
    {
	if(errorSocket)
	{
	    msg =(Char_t *)calloc(1,strlen(text) + 20);
	    sprintf(msg,"<%10s>%s",thisHost->Data(),text);
	    errorSocket->Send(msg);
	    free(msg);
	}
	else
	{
	    cerr << text << endl;
	}
    }
}

void HMessageMgr::setF(const Char_t *name)
{
    // Sets the filename for the debug messages.
    // The filenames are printed when the debug level is set.
    // param name Name of the file that wants to print a message soon.
    if(name && (strlen(name) != 0))
	strncpy(file, name,1999);
    else strcpy(file, "");
}

void HMessageMgr::setL(Int_t num)
{
    // Sets the linenumber for the debug message.
    // The linenumber is printed when the debug level is set.
    // param num Linenumber of message.
    line = num;
}

ClassImp(HMessageMgr)
