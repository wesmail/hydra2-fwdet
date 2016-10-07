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
# rotate rich : 097,103,109,115,121. 
# embedding e+,e- 105, 110, 115, 120, 125 
user=$(whoami)
currentDir=$(pwd | xargs -i basename {})
currentDir=../$currentDir

day=108
useDelta="no"  # no:do not use  yes: use $deltaelectronlist
particle="pi0" # pi-,e+,e- PLUTO names!    source 1 lambda K0S K- K+ phiKK pi- pi+ p    source 0 : e+ e-
sourceType=1   # 0 white 1 thermal


submmissionbase=/lustre/nyx/hades/user/${user}/sub/apr12/gen8a_embdst_source1
submissiondir=${submmissionbase}/embedding
 nFilesPerJob=1                                 # number of files to be analyzed by 1 job (default==1)
    jobscript=${submissiondir}/jobScript_SL.sh     # exec script (full path, call without dot, set it executable!)
    outputdir=/lustre/nyx/hades/dst/apr12/gen8a/embedding/test/${particle}/${day}     # outputdir for files AND logFiles
pathoutputlog=${outputdir}/out                    # protocol from batch farm for each file
     filename=embedded_dst_${type}               # filename of log file if nFilesPerJob > 1 (partnumber will be appended)
 
envscript=/cvmfs/hades.gsi.de/install/5.34.34/hydra2-4.9o/defall.sh
template=au123au.dat
geompath=${submissiondir}/geom_rich
#geompath=${submissiondir}/geom
hgeant=/cvmfs/hades.gsi.de/install/5.34.34/hgeant2-4.9o/hgeant
nevents=1000
dstsuffix=_dst_apr12.root
hldpath=/lustre/nyx/hades/raw/apr12/$day
par1="no"                                                      # optional par1 : environment script
par2="no"                                                      # optional par2 : executable
par3=""                                                        # optional par3 : input file hld
par4="no"                                                      # optional par4 : input file geant
par5="no"                                                      # optional par4 : outputdir 
par6="no"                                                      # optional par5 : number of events
par7="no"                                                      # optional par7


resources="--mem=2000 --time=0-8:00:00"                            # runtime < 10h, mem < 2GB
jobarrayFile="gen8_day_${day}_${particle}_jobarray.dat"

deltaelectronlist=delta_long_1500.list     # contains 1500 files , 100000 evts each delta electrons


#filelist=${currentDir}/lists/day_${day}.list  # file list in local dir! not in submissiondir!!!
filelist=${currentDir}/lists/day_${day}_test.list  # file list in local dir! not in submissiondir!!!


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

#---------------------------------------------------------------------
# output dirs

if [ ! -d $pathoutputlog ]
then
   echo "===> CREATE LOGDIR : $pathoutputlog"
   mkdir -p $pathoutputlog
else
   echo "===> USE LOGDIR : $pathoutputlog"
fi

if [ ! -d $outputdir ]
then
   echo "===> CREATE OUTPUTDIR : $outputdir"
   mkdir -p $outputdir
else
   echo "===> USE OUTPUTDIR : $outputdir"
fi




if [ ! -d $outputdir/dst ]
then
   echo "===> CREATE DSTDIR : $outputdir/dst"
   mkdir -p $outputdir/dst
fi

if [ ! -d $outputdir/dst/root ]
then
   echo "===> CREATE ROOTDIR : $outputdir/dst/root"
   mkdir -p $outputdir/dst/root
fi

if [ ! -d $outputdir/dst/qa ]
then
   echo "===> CREATE QADIR : $outputdir/dst/qa"
   mkdir -p $outputdir/dst/qa
fi

if [ ! -d $outputdir/geant ]
then
   echo "===> CREATE GEANTDIR : $outputdir/geant"
   mkdir -p $outputdir/geant
fi

if [ ! -d $outputdir/input ]
then
   echo "===> CREATE INPUTDIR : $outputdir/input"
   mkdir -p $outputdir/input
fi

if [ ! -d $outputdir/pluto ]
then
   echo "===> CREATE PLUTODIR : $outputdir/pluto"
   mkdir -p $outputdir/pluto
fi

if [ ! -d $outputdir/vertex ]
then
   echo "===> CREATE VERTEXDIR : $outputdir/vertex"
   mkdir -p $outputdir/vertex
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
# read the delta files list into an array
declare -a deltaarray
nDelta=0
if [ "${useDelta}" == "yes" ]
then

   for file in $(cat $deltaelectronlist)
   do
      deltaarray[$nDelta]=$file
      ((nDelta+=1))
   done
fi   
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
     
     ######################################################################
     #  SEND NEW JOB (USER SPECIFIC)
     
     par3=${infileList}
        
     if [ "${useDelta}" == "no" ]
     then
        deltafile="no"
     else
        ctdelta=0;
        ((ctdelta=${ctF}%${nDelta}))
        deltafile=${deltaarray[${ctdelta}]}
     fi
         
         #defall.sh outdir particle dstfile dstsuffix template geompath hgeant nev hldpath deltaelectronfile
     
     echo "${envscript} ${outputdir} ${particle} ${par3} ${dstsuffix} ${template} ${geompath} ${hgeant} ${nevents} ${hldpath} ${deltafile} ${sourceType}" >>  $jobarrayFile
     

     ######################################################################
     
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

  echo "${nFiles} jobs for day ${day} submitted"
fi
