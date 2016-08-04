#!/bin/bash

if [ $# -ne 1 ]
then

    echo "usage :  . ./makeHadesTarBall.sh destination_dir"
    echo "         destination_dir will be created if not existing"

else

    olddir=$(pwd)

    outdir=$1
    
    stamp=$(date +%d%m%Y)
    
    filename=hades_packages_${stamp}.tar.gz
    
    
    echo "------------------------------------------------------"
    echo "tarball will be created in ${outdir}"
    
    
    if [ ! -d ${outdir} ]
    then
    
       echo "${outdir} does not exist and will be created"
       mkdir -p ${outdir}
    fi
    
       
    cd ${outdir}
    outdir=$(pwd)  # fix relative path
    
    if [ -e ${filename} ]
    then
        echo "file ${filename} exists and will be overridden!"
        rm  ${filename}
    fi

    if [ -d packages ]
    then
        echo "dir packages exists in ${outdir} and will be overridden!"
        rm -rf packages
    fi
       
    cp -r ${olddir}/packages ${outdir}
    cd packages
    cp admin/defall.sh .
           
       
    cp /misc/hadessoftware/etch32/repos/*.gz .
    cp /misc/hadessoftware/etch32/repos/*.pdf .
           
    . ./clean.sh
    
    cd ${outdir}

    echo "------------------------------------------------------"
    echo "content in packages:"
    tar -cvvzf  ${filename}  packages
    chmod 755   ${filename}

    echo "------------------------------------------------------"
    echo "tarball:"
    ls -ltrh  ${filename}

    cd ${olddir}

fi
