
export hydraScriptsDir=/u/slebedev/hades/hydra2/scripts/
export geomPath=${hydraScriptsDir}rich700/geom/
export nofTriggers=100000
export inputFile1=/input/file

for Z in 1 2 ; do
    export templateIniFile=${hydraScriptsDir}rich700/geom/hgeant_config_${Z}.dat
    export outputIniFile=${hydraScriptsDir}rich700/geom/hgeant_config_${Z}_output.dat
    export outputRootFile=/u/slebedev/hades/data/2018.wls.hgeant.e.${Z}.root

    if [ ${Z} = "1" ] ; then
        echo ${Z}
        export nofTriggers=200000
    elif [ ${Z} = "2" ]; then
        echo ${Z}
        export nofTriggers=100000
    fi

    xterm -hold -e ". ./run_one.sh"&

done
