#!/bin/bash

cd /u/slebedev/hades/hydra2/
. ./defall.sh
cd -

hydraScriptsDir=/u/slebedev/hades/hydra2/scripts/
templateIniFile=${hydraScriptsDir}rich700/geom/hgeant_config_1.dat
outputIniFile=${hydraScriptsDir}rich700/geom/hgeant_config_1_output.dat
inputFile1=/input/file
geomPath=${hydraScriptsDir}rich700/geom/
outputRootFile=/u/slebedev/hades/data/hgeant.e.root
nofTriggers=1000

perl ../../scripts/batch/GE/hgeant/replaceIniDat.pl -t ${templateIniFile} -d ${outputIniFile} -i ${inputFile1} -p ${geomPath} -o ${outputRootFile} -n ${nofTriggers}


 /u/slebedev/hades/hgeant2/install/hgeant -b -c -f ${outputIniFile}


  #/u/slebedev/hades/hgeant2/install/hgeant -f ${outputIniFile}
