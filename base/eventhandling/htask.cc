#include "htask.h"

//*-- Author : Manuel Sanchez
//*-- Modified : 9/11/1998 by Manuel Sanchez
//*---Modified : 21/06/1999 D.Bertini <D.Bertini@gsi.de>

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////
//HTask
//
// This is the base class for a task (i.e. a transform data, take a decision
//depending on some parameters ...). Examples of tasks are the HReconstructor
//class which stands for a algorithm used to process event's data; the 
//HTaskSet class which corresponds to a set of generic tasks.
//
// The task is expected to be initialized using the init() function before the
//first call to the next() function. This last method (next()) performs the
//actual task and retrieves a pointer to the next task to be executed (or
//NULL if there is no next task).
//
// The finalize() method does some clean-up and in general will be called in
//the destructor. However it is provided as a separate function allowing the
//posibility to reset the task (two consecutive calls to finalize() and init())
//without destructing the object.
//
// The connectTask(aTask,n) function connects the task "aTask" as the next task
//to be performed. "n" is an optional integer and its meaning can be defined
//by each subclass.
//////////////////////////////////////////


ClassImp(HTask)
