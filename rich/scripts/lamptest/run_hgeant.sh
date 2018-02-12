#!/bin/bash

cd /u/slebedev/hades/hydra2/
. ./defall.sh
cd -

hydraScriptsDir=/u/slebedev/hades/hydra2/rich/scripts/lamptest/
templateIniFile=${hydraScriptsDir}geom/hgeant_config_lamptest.dat
outputIniFile=${hydraScriptsDir}geom/hgeant_config_lamptest_output.dat
inputFile1=/u/slebedev/hades/hydra2/rich/scripts/lamptest/lamp_sim_photons.evt
geomPath=${hydraScriptsDir}geom/
outputRootFile=/u/slebedev/hades/data/hgeant_lamp.e.root
nofTriggers=10000

perl /u/slebedev/hades/hydra2/scripts/batch/GE/hgeant/replaceIniDat.pl -t ${templateIniFile} -d ${outputIniFile} -i ${inputFile1} -p ${geomPath} -o ${outputRootFile} -n ${nofTriggers}


# /u/slebedev/hades/hgeant2/install/hgeant -b -c -f ${outputIniFile}


  /u/slebedev/hades/hgeant2/install/hgeant -f ${outputIniFile}
