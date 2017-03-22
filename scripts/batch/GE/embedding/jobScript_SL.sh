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
    par8=$(echo $input | cut -d " " -f8)
    par9=$(echo $input | cut -d " " -f9)
    par10=$(echo $input | cut -d " " -f10)
    par11=$(echo $input | cut -d " " -f11)
    par12=$(echo $input | cut -d " " -f12)

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
    echo "par8 = ${par8}"
    echo "par9 = ${par9}"
    echo "par10 = ${par10}"
    echo "par11 = ${par11}"
    echo "par12 = ${par12}"
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


envscript=${par1}
outdir=${par2}
particle=${par3}
dstfile=${par4}
dstsuffix=${par5}
template=${par6}
geompath=${par7}
hgeant=${par8}
nevents=${par9}
hldpath=${par10}
deltaelectronfile=${par11}
sourceType=${par12}
#----------------------
# evironment 
  echo "==> running enironment script ${envscript}"
. ${envscript}
#----------------------

filename=$(basename ${dstfile})


vertexfile=$(echo ${filename} | sed 's/.root$/_vertex.root/')
plutofile=$(echo ${filename} | sed 's/.root$//')
geantfile=$(echo ${filename} | sed "s/.root$/_geant_${particle}_/")

hldfile=$(echo ${filename} | sed 's/_[1-9].root/.root/') # take care of split files
hldfile=$(echo ${hldfile}  | sed "s/${dstsuffix}//")


#---------------------------------------------
# we have to collect all split files
splitname=$(echo $dstfile | sed 's/.root$//')
files=$(ls -1 ${splitname}_*.root)

dstlist="${dstfile}"
for item in $files
do
    dstlist="${dstlist},${item}"
done
#---------------------------------------------

doVertex=1
doPluto=1
doGeant=1
doDST=1
status=0


if [ $doVertex -eq 1 ]
then
  echo "==> execute vertex program "
  echo ./extract_vertex ${dstlist} ${outdir}/vertex/${vertexfile} ${nevents}

./extract_vertex ${dstlist} ${outdir}/vertex/${vertexfile} ${nevents}

fi

if [ $doPluto -eq 1 ]
then

  echo "==> execute pluto program "
  
  
  ldd  ./pluto_embedded
  
  echo ./pluto_embedded  ${outdir}/pluto ${plutofile} ${particle} ${nevents} ${sourceType} ${outdir}/vertex/${vertexfile}
./pluto_embedded  ${outdir}/pluto ${plutofile} ${particle} ${nevents} ${sourceType} ${outdir}/vertex/${vertexfile}
   status=$?
fi

if [ $doGeant -eq 1 ]
then

  echo "==> create ini.dat"
  echo ./replaceIniDat.pl -t ${template} -d ${outdir}/input/${geantfile}.dat -n ${nevents} -i ${outdir}/pluto/${plutofile}_pluto_${particle}.evt -p ${geompath} -o ${outdir}/geant/${geantfile}.root

./replaceIniDat.pl -t ${template} -d ${outdir}/input/${geantfile}.dat -n ${nevents} -i ${outdir}/pluto/${plutofile}_pluto_${particle}.evt -p ${geompath} -o ${outdir}/geant/${geantfile}.root

  echo "==> execute hgeant "

  mydir=$(pwd)
  
  mkdir sub_${myline}
  cd    sub_${myline}

  mysubdir=$(pwd)

  echo created subdir $mysubdir

  echo ${hgeant} -b -c -f ${outdir}/input/${geantfile}.dat
  ${hgeant} -b -c -f ${outdir}/input/${geantfile}.dat
  status=$?

  cd ${mydir}

  if [ -d sub_${myline} ]
  then 
    echo remove subdir sub_${myline}
    rm -rf sub_${myline}
  fi
  
  
  
  
fi

#---------------------------------------------
# we have to collect all split files
#geantlist=${outdir}/geant/${geantfile}1.root
#splitname=$(echo $geantlist | sed 's/.root$//')
#files=$(ls -1 ${splitname}_*.root)
#
#for item in $files
#do
#    geantlist="${geantlist},${item}"
#done
#---------------------------------------------

geantlist=${outdir}/geant/${geantfile}1.root

if [ $doDST -eq 1 ]
then

  echo "==> execute embedding dst "
if [ "${deltaelectronfile}" == "no" ]
then
   echo ./analysisDST_embedding ${hldpath}/${hldfile} ${geantlist} ${outdir}/dst ${nevents} 0
  ./analysisDST_embedding ${hldpath}/${hldfile} ${geantlist} ${outdir}/dst ${nevents} 0
else
  echo ./analysisDST_embedding ${hldpath}/${hldfile} ${geantlist},${deltaelectronfile} ${outdir}/dst ${nevents} 0
  ./analysisDST_embedding ${hldpath}/${hldfile} ${geantlist},${deltaelectronfile} ${outdir}/dst ${nevents} 0
fi
  status=$?


echo "------------------------------------"
echo "MOVING OUTPUTFILES:"

   if [ $status -eq 0 ]
   then
     
     echo "EXIT OF PROGRAM OK"
     result=$(ls -1 ${outdir}/dst/${hldfile}*_dst*.root 2>/dev/null | wc -l)
     echo "FOUND $result OUTPUTFILES"
     if [ $result -gt 0 ]
     then 
        mv -v ${outdir}/dst/${hldfile}*_dst*.root  ${outdir}/dst/root
     fi
   else  
     echo "EXIT OF PROGRAM NOT OK"
   
   fi
#---------------------------------------------------------------------

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
    echo "par8 = ${par8}"  
    echo "par9 = ${par9}"
    echo "par10 = ${par10}"
    echo "par11 = ${par11}"
    echo "par12 = ${par12}"
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

    
    mv ${pathoutputlog}/slurm-${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out ${pathoutputlog}/${hldfile}.log
 
    
