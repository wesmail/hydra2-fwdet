#!/bin/bash

# Submission script for GridEngine (GE). Each job will 
# be executed via the jobScript.sh
# This jobScript supports up to 7 parameters. Edit 
# the user specific part of the script according to 
# your program.
#
# Input to the script is a filelist with 1 file per line.
# For each file a job is started. With the parameter 
# nFilesPerJob a comma separated filelist will be 
# generated and handed to the job script. This feature
# is usefull when running many small jobs. Each
# job has its own logfile. All needed directories for the
# logfiles will be created if non existing.
#
# IMPORTANT: the hera/prometheus cluster jobs will only
# see the /hera file system. All needed scripts, programs
# and parameters have to be located on /hera or the job
# will crash. This script syncs your working dir to the submission
# dir on /hera . Make sure your scripts use the submission dir!
# Software should be taken from /cvmfs/hades.gsi.de/install/
#
# job log files will be named like inputfiles. If nFilesPerJob > 1
# the log files will contain the partnumber.
#
######################################################################
#   CONFIGURATION

user=$(whoami)
currentDir=$(pwd | xargs -i basename {})
currentDir=../$currentDir

type=Xi- # pi-,pi+,lambda,K-,K+,K0S,phiKK,Xi-,    pi0,w,phi,rho0,D+,wdalitz      ( pi- pi+ e+ e- white )
nFiles=100000
sourceSwitch=0   # 0 = thermal, 1 white embedding
         
         
submmissionbase=/hera/hades/user/${user}/sub/apr12/gen8
submissiondir=${submmissionbase}/pluto
 nFilesPerJob=20                                  # number of files to be analyzed by 1 job (default==1)
    jobscript=${submissiondir}/jobScript.sh       # exec script (full path, call without dot, set it executable!)
    outputdir=/hera/hades/dstsim/apr12/pluto/${type} # outputdir for files AND logFiles
pathoutputlog=${outputdir}/out                    # protocol from batch farm for each file
     filename=$type                               # filename of log file if nFilesPerJob > 1 (partnumber will be appended)
par1=/cvmfs/hades.gsi.de/install/5.34.01/hydra2-4.9f/defall.sh  # optional par1 : environment script
par2=${submissiondir}/run_pluto                                # optional par2 : executable
par3=${outputdir}                                              # optional par3 : outputdir
par4=""                                                        # optional par4 : outputfile (from file list)
par5=$type                                                     # optional par5 : particle type
par6=1000                                                      # 4000 optional par6 : number of events
par7=${sourceSwitch}                                           # optional par7
resources="-P hades -l h_rt=10:0:0,h_vmem=2G"                           # runtime < 10h, mem < 2GB
jobarrayFile="pluto_jobarray_${type}_${sourceSwitch}.dat"

filelist=${currentDir}/all_files_${type}.list  # file list in local dir! not in submissiondir!!!


createList=no   # (yes/no) use this to create files list with generic names (for simulation, testing)
                # use "no" if you have a filelist available


######################################################################


#---------------------------------------------------------------------
# create a file list for submission (simulation, testing etc.)
# for real data you will have a filelist with real filenames
if [ "$createList" == "yes" ]
then
   if [ -f $filelist ]
   then
     echo "===> REMOVING EXISTING FILELIST : $filelist"
     rm -f $filelist 
   fi

   echo "===> CREATE FILELIST : $filelist"
   for ((ct=1;ct<=$nFiles;ct++))
   do
      echo ${filename}_${ct} >> $filelist
   done
fi
#---------------------------------------------------------------------



nFiles=$( cat $filelist | wc -l)

#---------------------------------------------------------------------
# create needed dirs
if [ ! -d $submmissionbase ]
then
    echo "===> CREATE SUBMISSIONBASEDIR : $submmissionbase"
    mkdir -p $submmissionbase
else
    echo "===> USE SUBMISSIONBASEDIR : $submmissionbase"
fi

if [ ! -d $pathoutputlog ]
then
    echo "===> CREATE LOGDIR : $pathoutputlog"
    mkdir -p $pathoutputlog
else
    echo "===> USE LOGDIR : $pathoutputlog"
fi
#---------------------------------------------------------------------


ctF=0          # counter for file number
ctJ=0          # counter for job number
partNumber=0   # counter for part number

if [ -f $jobarrayFile ]
then
  rm -f $jobarrayFile
fi
echo "===> CREATING JOB ARRAY FILE"


#---------------------------------------------------------------------
# read the files list into an job array
declare -a jobarray
ct1=0
for file in $(cat $filelist)
do
   jobarray[$ct1]=$file
   ((ct1+=1))
done
#---------------------------------------------------------------------


#---------------------------------------------------------------------
# loop over the job array and submit parts with
# nFilesPerJob to GE

while ((ctF<$nFiles))
do
     #---------------------------------------------------------------------
     # build comma separated file list
     # per job
     if [ $nFilesPerJob -gt 1 ]
     then
        infileList=${jobarray[${ctF}]}
        ((ctF+=1))
        for (( ctList=1;ctList<$nFilesPerJob; ctList++ ))
        do   	
            if [ $ctF -lt ${nFiles} ]
            then
               infileList="${infileList},${jobarray[${ctF}]}"
               ((ctF+=1))
            fi
        done
     else 
        infileList=${jobarray[${ctF}]}
        ((ctF+=1))
     fi
     #---------------------------------------------------------------------
     
     ((partNumber+=1))

     logfile="${pathoutputlog}/${filename}_${partNumber}.log"

     if [ $nFilesPerJob -eq 1 ]
     then
        file=$(basename ${infileList})
        logfile="${pathoutputlog}/${file}.log"
     fi
     
     if [ -f ${logfile} ]
     then
        rm -f ${logfile}
     fi
           
     
#     echo "-----------------------------------------------------------------------------"
#     echo "add part ${partNumber}  last file ${ctF} of $nFiles ====> add new job ${infileList}"

     ######################################################################
     #  SEND NEW JOB (USER SPECIFIC)
     
     par4=${infileList}

     command="-j y -wd ${submissiondir} ${resources} -o ${logfile} \
     ${jobscript}  ${par1}   ${par2} ${par3}  ${par4} ${par5} ${par6} ${par7}"
     #jobscript.sh defall.sh prog    outdir   outfile type     nev    sourceSwitch
     
     echo "${par1} ${par2} ${par3} ${par4} ${par5} ${par6} ${par7}" >>  $jobarrayFile
     
done
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# sync the local modified stuff 
# to the submission dir
echo "===> SYNC CURENTDIR TO SUBMISSIONDIR : rsync  -vHaz $currentDir ${submmissionbase}"
rsync  -vHaz $currentDir ${submmissionbase}/

syncStat=$?

if [ ! $syncStat -eq 0 ]
then
     echo "===> ERROR : SYNCHRONIZATION ENCOUNTERED PROBLEMS"
else

  echo "-------------------------------------------------"

#  qsub -t 1-${nFiles}  -j y -wd ${submissiondir} ${resources}  -o ${pathoutputlog} ${jobscript} ${submissiondir}/${jobarrayFile} ${pathoutputlog}
  nFiles=$( cat $jobarrayFile | wc -l)
  ctsend=0
  block=2000
  while ((${ctsend} * ${block} < ${partNumber}))
  do
     ((start=${ctsend}*${block}+1))
     ((stop= ${start}+${block}-1))
     ((rest=${partNumber}-${start}))
     if [ $rest -le $block ]
     then
        ((stop=$start+$rest))
     fi

   qsub -t ${start}-${stop} -tc 500 -j y -wd ${submissiondir} ${resources}  -o ${pathoutputlog} ${jobscript} ${submissiondir}/${jobarrayFile} ${pathoutputlog}

     ((ctsend+=1))
  done

  echo "${nFiles} jobs for type ${type} submitted"
fi


