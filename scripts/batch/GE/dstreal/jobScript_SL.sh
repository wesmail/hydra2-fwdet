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

    echo ""               
    echo "--------------------------------"
    echo "SLURM_JOBID        : " $SLURM_JOBID
    echo "SLURM_ARRAY_JOB_ID : " $SLURM_ARRAY_JOB_ID
    echo "SLURM_ARRAY_TASK_ID: " $SLURM_ARRAY_TASK_ID
    echo "--------------------------------"
    echo ""

    arrayoffset=$par3 
    pathoutputlog=$par2 
    jobarrayFile=$par1

    ((myline=${SLURM_ARRAY_TASK_ID}+${arrayoffset}))
    # map back params for the job
    input=$(awk "NR==${myline}" $jobarrayFile)   # get all params for this job
    
    
    par1=$(echo $input | cut -d " " -f1)
    par2=$(echo $input | cut -d " " -f2)
    par3=$(echo $input | cut -d " " -f3)
    par4=$(echo $input | cut -d " " -f4)
    par5=$(echo $input | cut -d " " -f5)
    par6=$(echo $input | cut -d " " -f6)
    par7=$(echo $input | cut -d " " -f7)

    echo "input: $input"
    echo "par1 = ${par1}"
    echo "par2 = ${par2}"
    echo "par3 = ${par3}"
    echo "par4 = ${par4}"
    echo "par5 = ${par5}"
    echo "par6 = ${par6}"
    echo "par7 = ${par7}"


    format='+%Y/%m/%d-%H:%M:%S'

    host=$(hostname)

    date $format
    echo ""               
    echo "--------------------------------"
    echo "RUNNING ON HOST : " $host
    echo "WORKING DIR     : " $(pwd)
    echo "USER is         : " $USER
    echo "DISK USAGE /tmp :"
    df -h /tmp
    echo "--------------------------------"
    
    
    echo ""               
    echo "--------------------------------"
    echo "par1 = ${par1}"
    echo "par2 = ${par2}"
    echo "par3 = ${par3}"
    echo "par4 = ${par4}"
    echo "par5 = ${par5}"
    echo "par6 = ${par6}"
    echo "par7 = ${par7}"
    echo "--------------------------------"
    echo ""



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



#----------------------
# evironment 
  echo "==> running enironment script ${par1}"
. ${par1}
#----------------------

  echo "==> ldd ${par2}"
  ldd $par2

  echo "==> execute program "


  echo "==> $par2 $par3 $par4 $par5"
  time  $par2 $par3 $par4 $par5

  status=$?


echo "------------------------------------"
echo "MOVING OUTPUTFILES:"

   outdir=$par4
   inputfile=${par3}
   filename=$(basename ${inputfile} | sed 's/.root$//')


   befilename=$(basename ${inputfile} | sed 's/.hld$//')

   if [ ! -d ${outdir}/root ]
   then
      mkdir ${outdir}/root
   fi
     
   if [ ! -d ${outdir}/filter ]
   then
      mkdir ${outdir}/filter
   fi

   if [ ! -d ${outdir}/crash ]
   then
      mkdir ${outdir}/crash
   fi


   if [ $status -ne 0 ]
   then
     echo "JOB: $JOB_ID CRASHED ON HOST: $host WITH FILE $befilename"
     echo "JOB: $JOB_ID CRASHED ON HOST: $host WITH FILE $befilename" > ${outdir}/crash/${befilename}_crash.txt
     
     
     result=$(ls -1 ${outdir}/qa/${filename}*_hist.root 2>/dev/null | wc -l)
     
     
     if [ $result -gt 0 ]
     then
        mv -v ${outdir}/qa/${filename}*_Tree.root     ${outdir}/crash
        mv -v ${outdir}/qa/${filename}*_hist.root     ${outdir}/crash
        mv -v ${outdir}/qa/${filename}*.pdf           ${outdir}/crash
        mv -v ${outdir}/qa/${filename}*.txt           ${outdir}/crash
     fi
     
     result=$(ls -1 ${outdir}/${filename}*_dst*.root 2>/dev/null | wc -l)
     
     if [ $result -gt 0 ]
     then 
        mv -v ${outdir}/${filename}*_dst*.root        ${outdir}/crash
     fi
     
     result=$(ls -1 ${outdir}/${filename}*_filter_tree.root 2>/dev/null | wc -l)
     
     if [ $result -gt 0 ]
     then 
        mv -v ${outdir}/${filename}*_filter_tree.root ${outdir}/crash
     fi
     
   else
     
     result=$(ls -1 ${outdir}/${filename}*_dst*.root 2>/dev/null | wc -l)
     
     if [ $result -gt 0 ]
     then 
        mv -v ${outdir}/${filename}*_dst*.root        ${outdir}/root
     fi
     
     result=$(ls -1 ${outdir}/${filename}*_filter_tree.root 2>/dev/null | wc -l)
     
     if [ $result -gt 0 ]
     then 
        mv -v ${outdir}/${filename}*_filter_tree.root ${outdir}/filter
     fi
     
   fi
#---------------------------------------------------------------------



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
    
    
    echo ""               
    echo "--------------------------------"
    echo "MONITOR ENVIRONMENT:"
    echo "SLURM_JOBID        : " $SLURM_JOBID
    echo "SLURM_ARRAY_JOB_ID : " $SLURM_ARRAY_JOB_ID
    echo "SLURM_ARRAY_TASK_ID: " $SLURM_ARRAY_TASK_ID
    echo "RUNNING ON HOST    : " $(hostname)
    echo "WORKING DIR        : " $(pwd)
    echo "USER is            : " $USER
    echo "DISK USAGE /tmp    :------------"
    df -h /tmp 
    echo "--------------------------------"
    
    date $format


    sleep 2

    infile=$(basename $par3)

    
    mv ${pathoutputlog}/slurm-${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out ${pathoutputlog}/${infile}.log

    
