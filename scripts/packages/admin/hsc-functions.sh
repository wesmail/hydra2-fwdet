#!/bin/bash
###############################################################################
#
#  Hades Software Environment Scripting Library (Bourne shell family version)
#
#  Usually this script is included using the '.' or 'source' instruction
#  wihtin other scripts.
#  With this, it provides a set of commonly used functions for Hades Software
#  collection management or setup scripts.
#
#  Author: Simon Lang, GSI, 16.02.2007
#
#  $Id$
#
###############################################################################


#### code which is executed anyway ############################################


# for AT&T KornShell compartibility
case $0 in
*ksh)
   alias local='typeset'
   ;;
*)
   ;;
esac

# no errors yet
hsc_Error=0


#### function definitions ####################################################


hsc_checkContext()
{
   # was the script executed in the current shell context?
   case $0 in
   */*)
# TODO: In some cases this check fails, thus skip it for the moment
#
#      echo "ERROR: Invoke this script in the current shell contest!"
#      echo "ERROR: Start it with a leading '.' or the 'source' command!"
#      exit 2
      ;;
   *)
      ;;
   esac
}


hsc_debugScripts()
{
   # echos every executed line of the script
   declare -x PS4='$LINENO : '
   set -x
}


hsc_removeFromPath()   # removeFromPath <path variable name> <dir pattern>
{
   local path_name="$1"
   local pattern="$2"
   local new_path=""
   local old_ifs=$IFS
   local dir
   
   # dereference path_name and save to path
   eval "local path=\$$path_name"

   IFS=':'
   for dir in $path; do
      case $dir in
      $pattern)
	 ;;
      *)
	 new_path="$new_path:$dir"
	 ;;
      esac
   done
   IFS=$old_ifs

   # save result back to variable referenced by path_name
   eval "$path_name=${new_path#:}"
}


hsc_cleanPath()   # cleanPath <path variable name>
{
   local path_name="$1"

   # dereference path_name and save to path
   eval "local path=\$$path_name"

   path=$(echo $path | sed -e 's/::/:/g')
   path=${path%:}
   path=${path#:}

   # save result back to variable referenced by path_name
   eval "$path_name=$path"
}


hsc_resetEnvironment()
{
   export PATH=/bin:/usr/bin:/usr/local/bin:/usr/X11R6/bin
   unset  LD_LIBRARY_PATH
   unset  CLASSPATH

   unset ROOTSYS
   unset ORACLE_HOME ORA_USER
   unset QTDIR
   unset CERN_ROOT
   unset ADSM_BASE_NEW
   unset HADDIR MYHADDIR PLUTODIR
}


hsc_printVariables()   # printVariable <variable name> [variable name ...]
{
   local var_name var_value max_length
   
   max_length=0
   for var_name in $@; do
      if [[ ${#var_name} -gt $max_length ]]; then
	 max_length=${#var_name}
      fi
   done

   for var_name in $@; do
      eval "var_value=\$$var_name"
      printf "%-${max_length}s : %s\n" $var_name ${var_value:--unset-}
   done
}


hsc_printPath()   # printPath <path variable name>
{
   local path_name="$1"
   local old_ifs=$IFS
            
   # dereference path_name and save to path
   eval "local path=\$$path_name"

   echo -e "$path_name is set to:"
   if [[ -z "$path" ]]; then
      echo "   -unset-"
   fi
   
   IFS=':'
   for dir in $path; do
      echo "   $dir"
   done
   IFS=$old_ifs
}
           

hsc_shortPrintout()
{
   # print the most interesting environment variables
   echo
   hsc_printVariables ROOTSYS HADDIR MYHADDIR
   echo
   echo "Use SCRIPTSADMIN/hsc-print to print the environment."
   echo "Use SCRIPTSADMIN/hsc-check to check the environment."
   echo "Use SCRIPTSADMIN/hsc-clean to reset the environment."
   echo
}


hsc_longPrintout()
{
   echo
   hsc_printPath PATH
   hsc_printPath LD_LIBRARY_PATH
   hsc_printPath CLASSPATH
   echo
   hsc_printVariables MYHADDIR HADDIR PLUTODIR ROOTSYS \
      ORACLE_HOME ORA_USER QTDIR CERN_ROOT ADSM_BASE_NEW
   echo
}


hsc_checkPath()   # checkPath <path variable name>
{
   local old_ifs=$IFS
   local path_name="$1"
   eval "local path_value=\$$path_name"

   if [[ -z $path_value ]]; then
      echo "HINT:  Variable $path_name not set."
      return
   fi

   IFS=':'
   for dir in $path_value; do
      if [[ ! -d $dir ]]; then
	 echo -e -n "\033[1mERROR:\033[0m "
	 echo "Variable $path_name contains a non-existing path:"
	 echo -e -n "\033[1mERROR:\033[0m "
	 echo "  $dir"
	 hsc_Error=1
      fi
   done
   IFS=$old_ifs
}


hsc_checkVariable()   # checkVariable <variable name>
{
   local var_name="$1"
   eval "local var_value=\$$var_name"

   if [[ -z $var_value ]]; then
      echo "HINT: Variable $var_name not set."
      return
   fi
   
   if [[ ! -d $var_value ]]; then
      echo -e -n "\033[1mERROR:\033[0m "
      echo "Variable $var_name points to a non-existing directory:"
      echo -e -n "\033[1mERROR:\033[0m "
      echo "  $var_value"
      hsc_Error=1
   fi
}


hsc_checkEnvironment()
{
   for path in PATH LD_LIBRARY_PATH; do
      hsc_checkPath $path
   done

   for var in MYHADDIR HADDIR PLUTODIR ROOTSYS ORACLE_HOME \
	 QTDIR CERN_ROOT ADSM_BASE_NEW; do
      hsc_checkVariable $var
   done
}


hsc_setEnvironment()
{
   # remove old elements from paths 
   hsc_removeFromPath PATH            "*/HADESSOFTWARE/*"
   hsc_removeFromPath PATH            "*/oracle/*"
   hsc_removeFromPath CLASSPATH       "*/oracle/*"
   hsc_removeFromPath LD_LIBRARY_PATH "*/HADESSOFTWARE/*"
   hsc_removeFromPath LD_LIBRARY_PATH "*/oracle/*"
   hsc_removeFromPath LD_LIBRARY_PATH "$HOME/*"

   # prepend new elements to paths - last assignment is first search location
   if [ ! -z "$CERN_ROOT" ]; then
      PATH=$CERN_ROOT/bin:$PATH
   fi
   if [ ! -z "$ORACLE_HOME" ]; then
      PATH=$ORACLE_HOME/bin:$PATH
   fi
   if [ ! -z "$ROOTSYS" ]; then
      PATH=$ROOTSYS/bin:$PATH
   fi

   if [ ! -z "$ORACLE_HOME" ]; then
      CLASSPATH=$ORACLE_HOME/jdbc/lib/classes12.zip:$CLASSPATH
   fi

   if [ ! -z "$CERN_ROOT" ]; then
      LD_LIBRARY_PATH=$CERN_ROOT/lib:$LD_LIBRARY_PATH
   fi
   if [ ! -z "$ORACLE_HOME" ]; then
      LD_LIBRARY_PATH=$ORACLE_HOME/lib:$LD_LIBRARY_PATH
   fi
   if [ ! -z "$ADSM_BASE_NEW" ]; then
      LD_LIBRARY_PATH=$ADSM_BASE_NEW/lib:$LD_LIBRARY_PATH
   fi
   if [ ! -z "$ROOTSYS" ]; then
      LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
   fi
   if [ ! -z "$PLUTODIR" ]; then
      LD_LIBRARY_PATH=$PLUTODIR/lib:$LD_LIBRARY_PATH
   fi
   if [ ! -z "$HADDIR" ]; then
      LD_LIBRARY_PATH=$HADDIR/lib:$LD_LIBRARY_PATH
   fi
   if [ ! -z "$MYHADDIR" ]; then
      LD_LIBRARY_PATH=$MYHADDIR/lib:$LD_LIBRARY_PATH
   fi
   
   # remove crap like '::' from paths and export the paths
   hsc_cleanPath PATH
   hsc_cleanPath CLASSPATH
   hsc_cleanPath LD_LIBRARY_PATH
   export PATH LD_LIBRARY_PATH CLASSPATH
}


hsc_finalizeScript()
{
   # TODO: In some cases the check on the shell context fails,
   # thus skip exit/return statements

   case $0 in
   */*)
      # just exit - any function or variable lost with the shell process
#      exit $hsc_Error
      ;;
   *)
      # remove local variables and functions introduced here and return
      unset hsc_checkContext
      unset hsc_debugScripts
      unset hsc_removeFromPath
      unset hsc_cleanPath
      unset hsc_resetEnvironment
      unset hsc_printVariables
      unset hsc_printPath
      unset hsc_shortPrintout
      unset hsc_longPrintout
      unset hsc_checkPath
      unset hsc_checkVariable
      unset hsc_checkEnvironment
      unset hsc_setEnvironment
      unset hsc_finalizeScript
      if [ $hsc_Error -eq 0 ]; then
         unset hsc_Error
#	 return 0
      else
         unset hsc_Error
#	 return 1
      fi
      ;;
   esac
}
