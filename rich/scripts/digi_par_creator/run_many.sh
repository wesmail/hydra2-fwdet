
export hydraScriptsDir=/u/slebedev/hades/hydra2/scripts/
export geomPath=${hydraScriptsDir}rich700/geom/
export nofTriggers=250000
export inputFile1=/input/file
export templateIniFile=${hydraScriptsDir}rich700/geom/hgeant_config_single_electron.dat
export runWithScript=yes

for Z in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39; do

    export outputIniFile=${hydraScriptsDir}rich700/geom/hgeant_config_single_electron_output_${Z}.dat
    export outputGeantRootFile=/lustre/nyx/cbm/users/slebedev/hades/2018.rich_params.hgeant.e.${Z}.root
    export dstInputFile=/lustre/nyx/cbm/users/slebedev/hades/2018.rich_params.hgeant.e.${Z}1.root
    export dstOutputFile=/lustre/nyx/cbm/users/slebedev/hades/2018.rich_params.dst.e.${Z}.root

    xterm -hold -e ". ./run_one.sh"&

done
