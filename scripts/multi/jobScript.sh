#!/bin/bash

# This is the generic jobscript to run a parallel jobs
# on a standard computer with n cores. This script
# is not called direct but by the script senMultJobs.sh
#
# The script supports up to 7 parameters. The first parameter
# is the logfile for the job. Directories for the logfile
# will be created if non existing.
#
# The user specific part of the script is indicated below.
# Any output of standard out and error out will be visible on
# the screen and inside the logfile. It is recommended that
# the user follows this strategy when editing the user specific
# part using tee.



par1="no"
par2="no"
par3="no"
par4="no"
par5="no"
par6="no"
par7="no"

if [ $# -lt 1 ]
then
        echo "Usage : jobScript.sh par1(logfile) [par2] [par2] [par3] [par4] [par5] [par6] [par7]"
        sleep 3
        exit
fi

case "$#" in

1)  par1=$1
    ;;
2)  par1=$1
    par2=$2
    ;;
3)  par1=$1
    par2=$2
    par3=$3
    ;;
4)  par1=$1
    par2=$2
    par3=$3
    par4=$4
    ;;
5)  par1=$1
    par2=$2
    par3=$3
    par4=$4
    par5=$5
    ;;
6)  par1=$1
    par2=$2
    par3=$3
    par4=$4
    par5=$5
    par6=$6
    ;;
7)  par1=$1
    par2=$2
    par3=$3
    par4=$4
    par5=$5
    par6=$6
    par7=$7
    ;;
*) echo "Unsupported number of arguments" 
   echo "Usage : jobScript.sh par1(logfile) [par2] [par2] [par3] [par4] [par5] [par6] [par7]"
   exit
   ;;
esac

    echo "par1 = ${par1}"
    echo "par2 = ${par2}"
    echo "par3 = ${par3}"
    echo "par4 = ${par4}"
    echo "par5 = ${par5}"
    echo "par6 = ${par6}"
    echo "par7 = ${par7}"

#----------------------
# clean up log file
if [ -f ${par1} ]
then
   echo "==> removing existing log file ${par1}" 2>&1 | tee -a ${par1}
   rm -f ${par1}                                 2>&1 | tee -a ${par1}
fi
#----------------------

#----------------------
# create log dir if not existing
logdir=$(dirname ${par1})
if [ ! -d ${logdir} ]
then
    mkdir -p $logdir                            2>&1 | tee -a ${par1}
    echo "==> creating logdir ${logdir}"        2>&1 | tee -a ${par1}
fi
#----------------------


    
    date 2>&1 | tee -a ${par1}
    echo ""               | tee -a ${par1}
    echo "--------------------------------" | tee -a ${par1}
    echo "par1 = ${par1}" 2>&1 | tee -a ${par1}
    echo "par2 = ${par2}" 2>&1 | tee -a ${par1}
    echo "par3 = ${par3}" 2>&1 | tee -a ${par1}
    echo "par4 = ${par4}" 2>&1 | tee -a ${par1}
    echo "par5 = ${par5}" 2>&1 | tee -a ${par1}
    echo "par6 = ${par6}" 2>&1 | tee -a ${par1}
    echo "par7 = ${par7}" 2>&1 | tee -a ${par1}
    echo "--------------------------------" | tee -a ${par1}
    echo ""               | tee -a ${par1}



    echo ""               | tee -a ${par1}
    echo "--------------------------------"      2>&1  | tee -a ${par1}
    echo " DEBUG INFO"                           2>&1  | tee -a ${par1}
    echo "==> Kernel version information :"      2>&1  | tee -a ${par1}
    uname -a                                     2>&1  | tee -a ${par1}
    echo "==> C compiler that built the kernel:" 2>&1  | tee -a ${par1} 
    cat /proc/version                            2>&1  | tee -a ${par1}
    echo "==> load on this node:"                2>&1  | tee -a ${par1}
    mpstat -P ALL                                2>&1  | tee -a ${par1}
    echo "==> actual compiler is"                2>&1  | tee -a ${par1}
    gcc -v                                       2>&1  | tee -a ${par1}
    echo "--------------------------------"      2>&1  | tee -a ${par1}
    echo ""               | tee -a ${par1}








###################################################################
###################################################################
#   EDIT THIS PART TO EXECUTE YOUR JOB!



#----------------------
# evironment 
  echo "==> running enironment script ${par2}"      2>&1 | tee -a ${par1}
. ${par2}  2>&1 | tee -a ${par1}  
#----------------------



  echo "==> execute program "        2>&1 | tee -a ${par1}


  echo "==> $par3 $par4 $par5 $par6"      2>&1 | tee -a ${par1}
  time  $par3 $par4 $par5 $par6           2>&1 | tee -a ${par1}

#---------------------------------------------------------------------



#   END EDIT YOUR EXECUT JOB!
###################################################################
###################################################################

    





    echo ""               | tee -a ${par1}
    echo "--------------------------------" | tee -a ${par1}
    echo "Job with params " 2>&1 | tee -a ${par1}
    echo "par1 = ${par1}"   2>&1 | tee -a ${par1}
    echo "par2 = ${par2}"   2>&1 | tee -a ${par1}
    echo "par3 = ${par3}"   2>&1 | tee -a ${par1}
    echo "par4 = ${par4}"   2>&1 | tee -a ${par1}
    echo "par5 = ${par5}"   2>&1 | tee -a ${par1}
    echo "par6 = ${par6}"   2>&1 | tee -a ${par1}
    echo "par7 = ${par7}"   2>&1 | tee -a ${par1}
    echo "finsished!"       2>&1 | tee -a ${par1}
    echo "--------------------------------" | tee -a ${par1}
    echo ""                    | tee -a ${par1}
    date 2>&1 | tee -a ${par1}

    sleep 2

