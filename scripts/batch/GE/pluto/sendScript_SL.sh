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

type=pi0 # p,d,pi-,pi+,lambda,K-,K+,K0S,phiKK,Xi-,    pi0,w,phi,rho0,D+,wdalitz      ( pi- pi+ e+ e- white )
nFiles=100
sourceSwitch=0   # 0 = thermal, 1 white embedding
         
         
         
#     pi0      1*10e6
#     eta      1*10e6
#     w        1*10e6
#     phi      1*10e6
#     rho0     2*10e6
#     wdalitz  3*10e6
#     D+       3*10e6

#     K-     100*10^6 evts
#     K+      50*10^6 evts
#     K0S     50*10^6 evts
#     lambda 100*10^6 evts
#     Xi-     20*10^6 evts
#     phiKK   10*10^6 evts
#     d      100*10^6 evts
#     pi-     50*10^6 evts
#     pi+     50*10^6 evts

#     e-      10*10^6 evts
#     e+      10*10^6 evts

submmissionbase=/lustre/nyx/hades/user/${user}/sub/apr12/gen8
submissiondir=${submmissionbase}/pluto
 nFilesPerJob=100                                 # number of files to be analyzed by 1 job (default==1)
    jobscript=${submissiondir}/jobScript_SL.sh       # exec script (full path, call without dot, set it executable!)
    outputdir=/lustre/nyx/hades/dstsim/apr12/pluto/${type} # outputdir for files AND logFiles
pathoutputlog=${outputdir}/out                    # protocol from batch farm for each file
     filename=$type                               # filename of log file if nFilesPerJob > 1 (partnumber will be appended)
par1=/cvmfs/hades.gsi.de/install/5.34.34/hydra2-4.9m/defall.sh  # optional par1 : environment script
par2=${submissiondir}/run_pluto                                # optional par2 : executable
par3=${outputdir}                                              # optional par3 : outputdir
par4=""                                                        # optional par4 : outputfile (from file list)
par5=$type                                                     # optional par5 : particle type
par6=1000                                                      # 4000 optional par6 : number of events
par7=${sourceSwitch}                                           # optional par7
resources="--mem=2000 --time=0-2:00:00"                          # runtime < 10h, mem < 2GB
jobarrayFile="pluto_jobarray_${type}_${sourceSwitch}.dat"

filelist=${currentDir}/all_files_${type}_100.list  # file list in local dir! not in submissiondir!!!


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

  nFiles=$( cat $jobarrayFile | wc -l)
  arrayoffset=0;
  ctsend=0
  block=500
  while ((${ctsend} * ${block} < ${nFiles}))
  do
     ((start=${ctsend}*${block}))
     ((rest=${nFiles}-${start}))
     if [ $rest -le $block ]
     then
        ((stop=$rest))
     else
        ((stop=$block))
     fi
     ((arrayoffset=${ctsend} * ${block}))
     command="--array=1-${stop} ${resources} -D ${submissiondir}  --output=${pathoutputlog}/slurm-%A_%a.out ${jobscript} ${submissiondir}/${jobarrayFile} ${pathoutputlog} ${arrayoffset}"
     #echo $command
     sbatch $command

     ((ctsend+=1))
  done

  echo "${nFiles} jobs for type ${type} submitted"
fi


