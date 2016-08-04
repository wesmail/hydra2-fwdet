#ifndef HMESSAGEMGR__H
#define HMESSAGEMGR__H

#include "TObject.h"
#include "TString.h"
#include "TSocket.h"
#include "TError.h"
#include "hades.h"

using namespace std;
#include <fstream>
#include <iostream>

#define LEVELMAX 10

#if HDL > 0
/*!
	Macro that prints out a debug message, containing filename and linenumber
	if HDL set to a nonzero value
 */ 
#define DEBUG_msg(level, det, text) \
  ((HMessageMgr *)gHades->getMsg())->setL(__LINE__); \
  ((HMessageMgr *)gHades->getMsg())->setF(__FILE__); \
  ((HMessageMgr *)gHades->getMsg())->debug(level,det, this->GetName(), text);
#else
#define DEBUG_msg(level, det, text) \
  ((HMessageMgr *)gHades->getMsg())->setL(0); \
  ((HMessageMgr *)gHades->getMsg())->setF(""); \
  ((HMessageMgr *)gHades->getMsg())->debug(level,det, this->GetName(), text);
#endif // HDL > 0

/*!
	Macro that prints out an error message via HMessageMgr if set otherwise
	expands to fprintf()
 */
#define ERROR_msg(det, text) \
  if(((HMessageMgr *)gHades->getMsg()) != NULL) \
  ((HMessageMgr *)gHades->getMsg())->error(1, det, this->GetName(), text); \
  else cerr << text << endl;

/*!
	Macro that prints out a warning message via HMessageMgr if set otherwise
	expands to fprintf()
 */
#define WARNING_msg(level, det, text) \
  if(((HMessageMgr *)gHades->getMsg()) != NULL) \
  ((HMessageMgr *)gHades->getMsg())->warning(level,det, this->GetName(), text); \
  else cout << text << endl;

/*!
	Macro that prints out an info message via HMessageMgr if set otherwise
	expands to fprintf()
*/
#define INFO_msg(level, det, text) \
  if(((HMessageMgr *)gHades->getMsg()) != NULL) \
  ((HMessageMgr *)gHades->getMsg())->info((level),(det), (this->GetName()), (text)); \
  else cout << text << endl;

/*!
 Macro that prints out an seperator line via HMessageMgr if set otherwise
	expands to fprintf()
*/
#define SEPERATOR_msg(sep, num) \
  if(((HMessageMgr *)gHades->getMsg()) != NULL) \
  ((HMessageMgr *)gHades->getMsg())->messageSeperator(sep,num); \
  else for(Int_t i=0;i<num;i++) \
    cout << sep; \
    cout << endl;

/// Message manager class to send messages to different outputs.
/*!
	This class manages three different outputs for all kind of messages.
	Besides the standart output, which is the default, one can also
	write the messages to a file, or via a socket to a remote logging process.<p>
	The usage of this class should be done via the four macros that are defined
	in the hmessagemgr.h file.<p>
	DEBUG_msg(level, det, text)<br>
	ERROR_msg(det, text)<br>
	WARNING_msg(level, det, text)<br>
	INFO_msg(level, det, text)<br>
	SEPERATOR_msg(sep,num)<p>
	The meaning of the parameters is:<br>
	level - level of this message, never set it to zero because this means 
	no message at all will be printed<br>
	det   - detector which sends this message, can be one of:<br>
	DET_RICH DET_START DET_MDC DET_TOF DET_TOFINO DET_SHOWER
	DET_TRIGGER DET_KICKPLANE DET_RUNTIMEDB DET_QA<br>
	text  - text to print
 */
class HMessageMgr : public TNamed
{
 public:
  HMessageMgr(const Char_t *name, const Char_t *title);
  ~HMessageMgr(void);

  Bool_t setDefault(void);
  Bool_t setFile(const Char_t *dir);
  Bool_t setPort(const Char_t *host, Int_t port);
  Bool_t switchOff(void);

  void setDebugLevel(Char_t level);
  void setInfoLevel(Char_t level);
  void setWarningLevel(Char_t level);
  void enableDetectorMsg(Int_t det);

  void messageSeperator(const Char_t *seperator, Int_t num);

  static void hydraErrorHandler(Int_t level, Bool_t abort, const Char_t *location, const Char_t *msg);

	// normal version including classname
  void debug(Char_t level, Int_t det, const Char_t *className, const Char_t *text);
  void error(Char_t level, Int_t det, const Char_t *className, const Char_t *text);
  void warning(Char_t level, Int_t det, const Char_t *className, const Char_t *text);
  void info(Char_t level, Int_t det, const Char_t *className, const Char_t *text);

  // version with variable arguments list and classname
  void debug(Int_t level, Int_t det, const Char_t *className, const Char_t *text, ...);
  void error(Int_t level, Int_t det, const Char_t *className, const Char_t *text, ...);
  void warning(Int_t level, Int_t det, const Char_t *className, const Char_t *text, ...);
  void info(Int_t level, Int_t det, const Char_t *className, const Char_t *text, ...);
  
	// version with variable arguments but without classname
  void debugB(Char_t level, Int_t det, const Char_t *text, ...);
  void errorB(Char_t level, Int_t det, const Char_t *text, ...);
  void warningB(Char_t level, Int_t det, const Char_t *text, ...);
  void infoB(Char_t level, Int_t det, const Char_t *text, ...);


  //version with variable argument list including filename and line number
  void debug(Int_t level, Int_t det, const Char_t *className, const Char_t *file, Int_t line, const Char_t *text, ...);

  void setF(const Char_t *name);
  void setL(Int_t num);

	/// Enable output to stdout even if writen to file.
  void setScreenOutput(Bool_t f = kTRUE){screen = f;};

	enum Detector {
			DET_RICH       = 0x001,
			DET_START      = 0x002,
			DET_MDC        = 0x004,
			DET_TOF        = 0x008,
			DET_TOFINO     = 0x010,
			DET_SHOWER     = 0x020,
			DET_TRIGGER    = 0x040,
			DET_RUNTIMEDB  = 0x080,
			DET_KICKPLANE  = 0x100,
			DET_QA         = 0x200,
			DET_ALL        = 0x3ff
	};

 protected:
  void write(const Char_t *text);
  void ewrite(const Char_t *text);

 private:
  Char_t debugLevel;       //!< Level up to which the messages wil be printed.
  Char_t warningLevel;     //!< Level up to which the messages wil be printed.
  Char_t infoLevel;        //!< Level up to which the messages wil be printed.

  ofstream *msgFile;       //!< File to write messages to (debug,warning,info)
  ofstream *errorFile;     //!< File to write error messages to

  TSocket *msgSocket;      //!< Socket to send messages to (debug,warning,info)
  TSocket *errorSocket;    //!< Socket to send error messages to
  TString *thisHost;       //!< Name of this host

  Int_t detectorMsg;       //!< Bitmask fo which detector to print messages
  Char_t *msg1;            //!< internal message variable
	Char_t *msg2;            //!< internal message variable 
  Char_t *file;            //!< pointer to store name of file where message comes from
  Int_t line;              //!< line in file where message comes from
  Bool_t screen;           //!< write to stdout in addition to file or socket

  ClassDef(HMessageMgr,0)  // Message logging facility
};

#endif
