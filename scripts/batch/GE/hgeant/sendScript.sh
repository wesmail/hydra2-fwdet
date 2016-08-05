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

# Au+Au
impact=bmax10
#impact=bmax16



nevents=1000
nFiles=10000
         # pi-,lambda,K-,K+,K0S,phiKK,Xi-,w,phi,rho0,pi0,eta,D+,wdalitz deltaelectron
         #                   PLUTO    offset  UrQMD
         # pi0      1*10e6   1-10000  0        1-10000              ok1
         # eta      1*10e6   1-10000  0        1-10000              ok1
         # w        1*10e6   1-10000  0        1-10000              ok1
         # phi      1*10e6   1-10000  10000    10001-20000          ok1
         # rho0     2*10e6   1-20000  20000    20001-40000          ok1
         # wdalitz  3*10e6   1-30000  40000    40001-70000          ok1
         # D+       3*10e6   1-30000  70000    70001-100000         ok1
         
#     no_enhancement 10*10^6 ok      ok1
#     minbias        10*10^6         ok-1
#     K-     100*10^6 evts   ok      ok1
#     K+      50*10^6 evts   ok      ok1
#     K0S     50*10^6 evts   o       ok1
#     lambda 100*10^6 evts   ok      ok1
#     Xi-     20*10^6 evts   o       ok1
#     phiKK   10*10^6 evts   o       ok1
#     pi-     50*10^6 evts   ok
#     pi+     50*10^6 evts   ok
#     p       10*10^6 evts   ok
#     d      100*10^6 evts   ok


#   white          
#     pi-   100*10^6 evts   ok        ok1 
#     pi+   100*10^6 evts   ok        ok1
#     e-   10*10^6 evts              ok1
#     e+   10*10^6 evts              ok1
         
part=phiKK
offset=0

submmissionbase=/hera/hades/user/${user}/sub/apr12/gen8/cocktail/hgeant/${part}
submissiondir=${submmissionbase}/hgeant
 nFilesPerJob=1                               # number of files to be analyzed by 1 job (default==1)
    jobscript=${submissiondir}/jobScript.sh     # exec script (full path, call without dot, set it executable!)
    outputdir=/hera/hades/dstsim/apr12/hgeant/${impact}/new/${part}     # outputdir for files AND logFiles
pathoutputlog=${outputdir}/out                  # protocol from batch farm for each file
     filename=testrun                           # filename of log file if nFilesPerJob > 1 (partnumber will be appended)
     replace=${currentDir}/replaceIniDat.pl
     template=au123au.dat
      input1=/hera/hades/dstsim/apr12/pluto/${part}/${part}
      input3=/hera/hades/dstsim/apr12/urqmd/${impact}/new/all/evt/Au_Au_1230MeV_1000evts      # bmax10
      output=/hera/hades/dstsim/apr12/hgeant/${impact}/new/${part}/Au_Au_1230MeV_1000evts

par1=/cvmfs/hades.gsi.de/install/5.34.34/hydra2-4.9m/defall.sh  # optional par1 : environment script
par2=/cvmfs/hades.gsi.de/install/5.34.34/hgeant2-4.9m/hgeant    # optional par2 : executable
par3=""                                                        # optional par3 : geant inidatfile
par4="no"                                                      # optional par4 : 
par5="no"                                                      # optional par5 : number of events
par6="no"                                                      # optional par6
par7="no"                                                      # optional par7
resources="-P hadeshighprio -l h_rt=20:0:0,h_vmem=2G"                           # runtime < 10h, mem < 2GB

jobarrayFile="gen8_${part}_jobarray.dat"

filelist=${currentDir}/all_files_${part}_1files_${nFiles}.list  # file list in local dir! not in submissiondir!!!

createList=no  # (yes/no) use this to create files list with generic names (for simulation, testing)
                 # use "no" if you have a filelist available

createInput=yes  # (yes/no) use this to create inidatfiles
                 # use "no" if input files are already created

#nFiles=$(cat $filelist | wc -l)
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
   for ((ct=1+$offset;ct<=+$offset+$nFiles;ct++))
   do
      echo ./input/au123au_${part}_${ct}.dat >> $filelist
   done
fi
#---------------------------------------------------------------------

mkdir -p $submissiondir/input

#---------------------------------------------------------------------
# create input inidat for geant
if [ "$createInput" == "yes" ]
then
   #if [ -d $submisiondir/input ]
   #then
   #  echo "===> REMOVING EXISTING INPUTFILES"
   #  rm -f $submissiondir/input/*.dat
   #fi

   echo "===> CREATE INPUTFILES"
   for ((ct=1+$offset;ct<=$offset+$nFiles;ct++))
   do
      ct_pluto=0
      ((ct_pluto=${ct}-${offset}))
      iniDat=${submissiondir}/input/au123au_${part}_${ct}.dat
      #$replace -t $template -d $iniDat -n $nevents -i ${input1}_${ct_pluto}.evt -s ${input2}_${ct}.evt -u ${input3}_${ct}.evt -p ${submissiondir}/geom_rich -o ${output}_${ct}_.root   # pluto lep + lambda + urqmd
      $replace -t $template -d $iniDat -n $nevents -i ${input1}_${ct_pluto}.evt -u ${input3}_${ct}.evt -p ${submissiondir}/geom_rich   -o ${output}_${ct}_.root   # pluto  + urqmd
      #$replace -t $template -d $iniDat -n $nevents -i ${input1}_${ct}.evt -p ${submissiondir}/geom_rich -o ${output}_${ct}_.root   # pluto lep only
      #$replace -t $template -d $iniDat -n $nevents -i ${input3}_${ct}.evt -p ${submissiondir}/geom_rich -o ${output}_${ct}_.root   # urqmd
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

#---------------------------------------------------------------------
# output dirs

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
     
     #echo "-----------------------------------------------------------------------------"
     #echo "add part ${partNumber}  last file ${ctF} of $nFiles ====> add new job ${infileList}"

     ######################################################################
     #  SEND NEW JOB (USER SPECIFIC)
     
     par3=${infileList}

           #defall.sh prog filelist outdir  nev
     echo "${par1} ${par2} ${par3} ${par4} ${par5} ${par6} ${par7}" >>  $jobarrayFile
     

     ######################################################################
     
done
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# sync the local modified stuff 
# to the submission dir
echo "===> SYNC CURENTDIR TO SUBMISSIONDIR : rsync  -vHa $currentDir ${submmissionbase}"
rsync  -vHa $currentDir ${submmissionbase}/

syncStat=$?

if [ ! $syncStat -eq 0 ]
then
     echo "===> ERROR : SYNCHRONIZATION ENCOUNTERED PROBLEMS"
else

  echo "-------------------------------------------------"


  nFiles=$( cat $jobarrayFile | wc -l)
  ctsend=0
  block=500
  while ((${ctsend} * ${block} < ${nFiles}))
  do
     ((start=${ctsend}*${block}+1))
     ((stop= ${start}+${block}-1))
     ((rest=${nFiles}-${start}))
     if [ $rest -le $block ]
     then
        ((stop=$start+$rest))
     fi

     qsub -t ${start}-${stop}  -j y -wd ${submissiondir} ${resources}  -o ${pathoutputlog} ${jobscript} ${submissiondir}/${jobarrayFile} ${pathoutputlog}

     ((ctsend+=1))
  done

  echo "${nFiles} jobs for day ${day} submitted"
fi
