#!/usr/local/bin/bash

############################################################
# defall.sh
# set ALL basic environment variables for hydra
# Date: 18 Feb 2004 J.Markert
############################################################
## --------------------------------------------------
##  checking for right method to call the script
## --------------------------------------------------

if [ `basename $0` = defall.sh ]
then
    echo
    echo use a leading " . " to call the `basename $0` script!
    echo -e "exiting ...\n"
    exit
fi
# added 06 May 2004 P.Zumbruch
############################################################

myvers=dev
mybase=/misc/halo
mypath=${mybase}/anal

export PATH=${mybase}/valgrind/bin:$PATH  # valgrind: memory debugging tool
export CVSROOT=${mybase}/repos/anal
. rootlogin 400-03
. oraenv
. ${mypath}/${myvers}/hadenv.sh -H ${mypath} -M ${mypath}/${myvers} -l "none" ${myvers}
. ${mypath}/${myvers}/onlinenv
. ${mypath}/${myvers}/rfioenv.sh
#. ${mypath}/${myvers}/distenv.sh     # distributed compiling
