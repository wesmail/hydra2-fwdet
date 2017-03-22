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

  mydir=$(pwd)
  
  mkdir sub_${myline}
  cd    sub_${myline}

  mysubdir=$(pwd)

  echo created subdir $mysubdir
  echo create link to input
  ln -s ../input  input

#  echo "==> ${par2} -b -c -f ${par3}"
#  time  $par2 -b -c -f $par3
  files=$(echo $par3 | sed 's/,/ /g')
  for file in $files
  do
     
     entr=$(cat /proc/sys/kernel/random/entropy_avail)
     
     echo check entropy : ${entr} 
     date

     filenumber=0
     if [[ $file =~ _([0-9]*).dat$ ]];
     then
        filenumber=${BASH_REMATCH[1]}
        echo "filenumber is ${filenumber}"
     fi
     
     replace=$(cat ../replaceMask.txt)
     replace=$(echo $replace | sed "s/XXX/$filenumber/g")
     echo "Setting up replace : $replace"
     
     echo "==> Execute replace"
     $replace
     
     echo "==> ${par2} -b -c -f ${file}"
     time  $par2 -b -c -f $file
     date

     infile=$(../extract_outfile.pl -i ${file} )  # output file of hgeant
     outdir=$(dirname $infile)
     outfile=$(basename $infile)
     
     ../testFiles2 $infile     # return 0 if ok
     
     good=$?
     
     if [ $good -ne 0 ]
     then
        echo "FILE $infile is corrupted, will be removed!"
        rm -f $infile
        par6="no" # do not do vertex ectract
     
        if [ ! -d  $outdir/crash ]
        then
          mkdir -p $outdir/crash
        fi
        echo "write log $outfile ==> $outdir/crash/${outfile}.txt"
        echo  $outfile > $outdir/crash/${outfile}.txt
     fi
     
     if [ $par6 != "no" ]
     then
     
        outdir=${outdir}/vertex
     
        echo  "==> ../${par6} $infile $outdir -1"
        time ../${par6} $infile $outdir -1
        
     fi
     
  done

  cd ${mydir}

  if [ -d sub_${myline} ]
  then 
    echo remove subdir sub_${myline}
    rm -rf sub_${myline}
  fi

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

    filename=""
    ct=0
    files=$(echo $par3 | sed 's/,/ /g')
    for file in $files
    do
     if [ $ct -eq 0 ]
     then
       filename=$(basename $file)
       filename=$(echo $filename | sed 's/.log//')
     fi
     ((ct+=1))
    done

    if [ $ct -gt 1 ]
    then
       filename=${filename}_part
    fi
    
    mv ${pathoutputlog}/slurm-${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out ${pathoutputlog}/${filename}.log

    
