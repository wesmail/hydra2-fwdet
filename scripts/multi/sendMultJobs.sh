#!/bin/bash

# this script executes multiple jobs in parallel on the same
# host. Each job will be executed via the jobScript.sh
# This jobScript supports up to 7 parameters (first param
# has to be the logfile). Edit the user specific part
# of the script according to your program.
#
# Input to the script is a filelist with 1 file per line.
# For each file a job is started on a separate xterm. With
# the parameter nFilesPerJob a comma separated filelist will
# be generated and and handed to the jobs script. This featute
# is usefull when running many small jobs. Each
# job has its own logfile. All needed directories for the
# logfiles will be created if non existing.
# The script keeps the number of executed jobs constant. In
# intervals of 1 second the number of running jobs is checked
# and new jobs are submitted if possible.




######################################################################
#   CONFIGURATION

scriptdir=$(pwd)

nJobs=10                              # number of parallel jobs
nFilesPerJob=100                      # number of files to be analyzed by 1 job
jobscript=${scriptdir}/jobScript.sh   # exec script (full path, call without dot, set it executable!)
par1="${scriptdir}/log/joblog_"       # log file (minimum required 1 par)
par2="/mypathenv/defall.sh"           # optional par2 : environment script
par3="/mypathprogram/analysis"        # optional par3 : executable
par4="no"                             # optional par4 : input file list
par5="/mypathoutput/outfile"          # optional par5 : outputfile (part number will be appended)
par6=100000000                        # optional par6
par7="no"                             # optional par7

filelist="all_files.list"             # 1 file including path per line
nFiles=$( cat $filelist | wc -l)

######################################################################


user=$(whoami)


ctF=0          # counter for file number
ctJ=0          # conunter for job number
partNumber=0   # counter for part number


declare -a jobarray
ct1=0
for file in $(cat $filelist)
do
   jobarray[$ct1]=$file
   ((ct1+=1))
done

for ((i=0; i<$nFiles; i++ ))
do
    echo $i ${jobarray[${i}]}
    
done


echo $nJobs $nJobs_1 $nFiles
while ((ctF<$nFiles))
do
     while ((ctJ<$nJobs)) && ((ctF<$nFiles))
     do
        echo "found njobs=${ctJ} : ${nJobs}"
        
        
        if [ $ctJ -lt $nJobs ]
        then
           
           infileList=${jobarray[${ctF}]} 
           ((ctF+=1))
           for (( ctList=1;ctList<$nFilesPerJob; ctList++ ))
           do
              if [ $ctF -le ${nFiles} ]
              then
                 infileList="${infileList},${jobarray[${ctF}]}"
                 ((ctF+=1))
              
              fi
           done

           ((partNumber+=1))
           
            par4=${infileList}
           
           ######################################################################
           #  SEND NEW JOB
           echo "add part ${partNumber}  last file ${ctF} of $nFiles ====> add new job ${par4}"
                                                                                      # log file               defall.sh prog  filelist outfile nev
           xterm  -iconic -g 110x30 -T "Job_${partNumber}" -bg white -e "${jobscript} ${par1}${partNumber}.out ${par2} ${par3} ${par4}  ${par5}_${partNumber}.root ${par6} ${par7}" &
           ######################################################################
        
           sleep 1
        fi
        
        ctJ=$(ps u -U "${user}" | grep "${jobscript}" | grep -v xterm |  grep -v grep | wc -l)
     done
     sleep 2  
     ctJ=$(ps u -U "${user}" | grep "${jobscript}" | grep -v xterm  | grep -v grep | wc -l)
     
done
