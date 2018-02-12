#!/bin/bash

cd /u/slebedev/hades/hydra2/
. ./defall.sh
cd -

echo ${templateIniFile}

 perl ../../scripts/batch/GE/hgeant/replaceIniDat.pl -t ${templateIniFile} -d ${outputIniFile} -i ${inputFile1} -p ${geomPath} -o ${outputRootFile} -n ${nofTriggers}


  /u/slebedev/hades/hgeant2/install/hgeant -b -c -f ${outputIniFile}

 # /u/slebedev/hades/hgeant2/install/hgeant -f ${outputIniFile}
