#!/bin/bash

# This is the generic jobscript to run jobs on GridEngine
#
# The script supports up to 7 parameters.
#
# The user specific part of the script is indicated below.



par1="no"
par2="no"
par3="no"
par4="no"
par5="no"
par6="no"
par7="no"

if [ $# -lt 1 ]
then
        echo "Usage : jobScript.sh par1 [par2] [par2] [par3] [par4] [par5] [par6] [par7]"
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
   echo "Usage : jobScript.sh par1 [par2] [par2] [par3] [par4] [par5] [par6] [par7]"
   exit
   ;;
esac

    pathoutputlog=$par2 
    jobarrayFile=$par1

    # map back params for the job
    input=$(awk "NR==$SGE_TASK_ID" $jobarrayFile)   # get all params for this job
    
    
    par1=$(echo $input | cut -d " " -f1)
    par2=$(echo $input | cut -d " " -f2)
    par3=$(echo $input | cut -d " " -f3)
    par4=$(echo $input | cut -d " " -f4)
    par5=$(echo $input | cut -d " " -f5)
    par6=$(echo $input | cut -d " " -f6)
    par7=$(echo $input | cut -d " " -f7)

    echo ""               
    echo "--------------------------------"
    echo "input: $input"
    echo "par1 = ${par1}"
    echo "par2 = ${par2}"
    echo "par3 = ${par3}"
    echo "par4 = ${par4}"
    echo "par5 = ${par5}"
    echo "par6 = ${par6}"
    echo "par7 = ${par7}"
    echo "--------------------------------"
    echo ""


    format='+%Y/%m/%d-%H:%M:%S'

    date $format
    echo ""               
    echo "--------------------------------"
    echo "RUNNING ON HOST : " $(hostname)
    echo "WORKING DIR     : " $(pwd)
    echo "USER is         : " $USER
    echo "JOBID           : $JOB_ID"
    echo "DISK USAGE /tmp :"
    df -h /tmp
    echo "--------------------------------"
    
    
    echo ""
    echo "--------------------------------"
    echo " DEBUG INFO"                     
    echo "==> Kernel version information :"
    uname -a                               
    echo "==> C compiler that built the kernel:"
    cat /proc/version                           
    echo "==> load on this node:"               
    mpstat -P ALL                               
    echo "==> actual compiler is"               
    gcc -v                                      
    echo "--------------------------------"     
    echo ""               








###################################################################
###################################################################
#   EDIT THIS PART TO EXECUTE YOUR JOB!


     urqmd=$par1
 converter=$par2
  filename=$par3
    outdir=$par4
   nevents=$par5
      seed=$par6
     input=$par7
 
 
 olddir=$(pwd)


 
 echo "==> ldd ${urqmd}"
 ldd ${urqmd}
 echo "	==> ldd ${converter}"
 ldd ${converter}


 if [ ! -d ${outdir}/evt ]
 then
    mkdir -p ${outdir}/evt
 fi

 cd ${outdir}

 #######################################
 # because of the random seed we need to 
 # clone the input
 config=$(basename ${input})
 cfg=${config}_${seed}
 
 cat ${input} | sed 's/^xxx/#/' >  $cfg
 echo rsd $seed                 >> $cfg
 echo xxx                       >> $cfg
 #######################################

 export ftn09=${cfg}
 export ftn14=${filename}

 echo "==> execute program "


 echo "==> RUNNING URQMD:"
 time  ${urqmd}
  
 if [ $? -ne 0 ]
 then
    echo "ERROR: urqmd did not succeed!"
 fi

 if [ -f $cfg ]
 then
    rm -f $cfg
 fi

 echo "RUNNING CONVERT:"
 echo  "$convert $filename $events"
 ls -ltrh ${filename}

 if [ -f ${filename} ]
 then
   infile=$(basename ${filename})
   time ${converter} ${infile} ${nevents}
   mv ${filename} ${filename}.f14
 else
   echo "ERROR: file = ${infile} not found!"
 fi

 if [ -f ${filename}.evt ]
 then
   mv ${filename}.evt evt/
   rm ${filename}.f14
 else
   echo "ERROR: file = ${filename}.evt not found!"
 fi


 cd -
 ls -ltrh ${outdir}/evt/${filename}.evt

 cd ${olddir}

#   END EDIT YOUR EXECUT JOB!
###################################################################
###################################################################

    





    echo ""               
    echo "--------------------------------"
    echo "Job with params "
    echo "par1 = ${par1}"  
    echo "par2 = ${par2}"  
    echo "par3 = ${par3}"  
    echo "par4 = ${par4}"  
    echo "par5 = ${par5}"  
    echo "par6 = ${par6}"  
    echo "par7 = ${par7}"  
    echo "finsished!"      
    echo "--------------------------------"
    echo ""
    
    host=$(hostname)
    
    echo ""               
    echo "--------------------------------"
    echo "MONITOR ENVIRONMENT:"
    echo "JOB EXE   :---------------------"
    qstat -j $JOB_ID
    #echo "HOST LOAD :---------------------"
    #qhost -h $host
    #echo "OTHER JOBS ON HOST:-------------"
    #qhost -h $host -j
    echo "DISK USAGE /tmp :---------------"
    df -h /tmp 
    #echo "MEM/CPU USAGE :-----------------"
    #qstat -j $JOB_ID | grep usage
    echo "--------------------------------"
   
    
    
    
    date $format


    sleep 2

    infile=$(basename $par3)

    
    mv ${pathoutputlog}/${JOB_NAME}.o${JOB_ID}.${SGE_TASK_ID} ${pathoutputlog}/${infile}.log

