#!/bin/bash

# Before running this scripts read HOW_TO_INSTALL.txt
#
# This script install almost the full HADES software tree
# The script should be run from inside the packages dir 
# (which is created by untaring hades_tarball.tar.gz).
# The ORACLE client software has to be installed before.
# It is recommended to install step by step the software
# parts setting yes/no in the ACTIVATE INSTALLATIONS
# section. In special the ROOT installation requires the
# installation of developper headers/libs etc. Once ROOT
# works the rest will work fine.
# This script can and should be used to install new HYDRA/HGEANT
# versions from the corresponding svn repositories by running
# the script after changing the corresponding
# HYDRA/HGEANT directives.


#-------------------------------------------------------
# DEPENDENCIES
# ROOT needs gsl
# HYDRA needs ROOT + gsl + ORACLE client
# HGEANT needs CERNLIB,  ROOT + gsl + ORACLE client
# GARFIELD needs CERNLIB
#
# The dependencies you can get by running the
# provided . ./makeDependList.sh pathtolibs
#
#-------------------------------------------------------




#-------------------------------------------------------
# The installation software tree will have the
# following layout:

# /${myinstalldir}/admin

# /${myinstalldir}/cernlib_gfortran/2005/
# /${myinstalldir}/garfield

# /${myinstalldir}/gsl-${gslversion}

# /${myinstalldir}/root-${rootversion}
# /${myinstalldir}/${rootversion}/${hydraname}
# /${myinstalldir}/${rootversion}/${hgeantname}

# /${mybuilddir}/gsl-${gslversion}    # can be removed
# /${mybuilddir}/${rootversion}/${hydraname}
# /${mybuilddir}/${rootversion}/${hgeantname}
#-------------------------------------------------------


#-------------------------------------------------------
# CONFIGURATION

# THIS SOFTWARE REPOSITORY
hadestarball=$(pwd)

nthreads=2                   # number of parallel threads for make
                             # compiling on small memory machines
                             # number of threads should small, since
                             # otherwise ROOT will need too much mem
                             # resulting in internal compiler crash!

# INSTALL NAD BUILD PATH
hadessoft=install
hadesbuild=build
myinstalldir=~/hadessoftware/${hadessoft}   # ROOT of software tree binaries
mybuilddir=~/hadessoftware/${hadesbuild}    # ROOT of software tree build

# GSL
gslversion=1.15              # gsl version has to match the tar file
gsltar=gsl-1.15.tar.gz

# ROOT
rootversion=5.34.01          # root version according to tar file
roottar=root_v5.34.01.source.tar.gz

# CERNLIB
cerntar=cernlib_patched.tar.gz       # cenlib2005 including all patches for gfortran and 64bit

# GARFIELD
garfieldtar=garfield.tar.gz  # all garfield .cra , car , patchy etc files

# URQMD 3.3
urqmd3_3=3.3p1              # urqmd version has to match the tar file
urqmd3_3tar=urqmd-3.3p1_patched.tar.gz

# URQMD CONVERTER
urqmdconvert=1.3.6              #  version has to match the tar file
urqmdconverttar=convert-urqmd_1.3.6.tar.gz

# HYDRA
hydraversion=trunk           # which version to checkout
hydraname=hydra-dev          # dirname in the tree for the installed version
#hydraversion=tags/hydra2-3.4  # which version to checkout
#hydraname=hydra2-3.4          # dirname in the tree for the installed version

# HGEANT
hgeantversion=trunk          # which version to checkout
hgeantname=hgeant-dev        # dirname in the tree for the installed version
#hgeantversion=tags/hgeant2-3.4 # which version to checkout
#hgeantname=hgeant2-3.4        # dirname in the tree for the installed version

plutoversion=tags/5.42  # which version to checkout
plutoname=v5_42          # dirname in the tree for the installed version


# SCRIPTS : hsc-functions.sh, hsc-print, hsc-check, hsc-clean in scripts dir
tagscripts=SCRIPTSADMIN      # this tag will be replaced in the scripts by the real path to script dir
tagshadessoft=HADESSOFTWARE  # this tag will be replaced in the scripts by the real hadessoftware/install

# ENVIRONMENT location definition for defall.sh (upto the client dir, path should end my_patch_to_oracle/client )
#ORACLE_HOME=/cvmfs/hades.gsi.de/ora/10.2.0.1_linux_x86_64/client  # squeeze64
ORACLE_HOME=/cvmfs/it.gsi.de/oracle/product/11.2.0/client_x86_64_1  # squeeze64+wheezy64
ORA_USER=hades_ana/hades@db-hades
ADSM_BASE_NEW=/misc/hadessoftware/etch32/install/gstore-may07   # needed for USES_RFIO (etch32,outdated)


#-------------------------------------------------------
# ACTIVATE INSTALLATIONS

doRFIO=no   # switch use of RFIO on/off  (default is off)
doORA=yes   # switch use ORA off/on

INSTALLGSL=no
INSTALLROOT=no
INSTALLCERN=no
CLEANCERN=no
INSTALLGARFIELD=no
INSTALLSCRIPTS=no
INSTALLHYDRA=no
INSTALLHGEANT=no
INSTALLPLUTO=no
INSTALLHZIP=no
INSTALLHADD=no
INSTALLHISTMAPADD=no
INSTALLGEMON=no

INSTALLURQMD3_3=no
INSTALLURQMDCONVERT=no
#-------------------------------------------------------


export HADESGSL="${myinstalldir}/gsl-${gslversion}"
echo  "HADESGSL = ${HADESGSL}"


#-------------------------------------------------------


#-------------------------------------------------------
if [ -e ${myinstalldir} ]
then
   echo "INSTALLDIR = ${myinstalldir} EXISTS ALREADY AND WILL BE USED!"
else 
   echo "CREATE INSTALLDIR = ${myinstalldir}!"
   mkdir -p ${myinstalldir} || exit -1
fi

if [ -e ${mybuilddir} ]
then
   echo "BUILDDIR = ${mybuilddir} EXISTS ALREADY AND WILL BE USED!"
else 
   echo "CREATE BUILDDIR = ${mybuilddir}!"
   mkdir -p ${mybuilddir} || exit -1
fi
#-------------------------------------------------------


#-------------------------------------------------------
if [ "${INSTALLGSL}" == "yes" ]
then
    echo "INSTALLING GSL"
    
    cd ${mybuilddir}
    
    tar -zxf  ${hadestarball}/${gsltar}
    cd gsl-${gslversion}

    ./configure  --prefix="${myinstalldir}/gsl-${gslversion}"
    make -s -j${nthreads} || exit -1
    # make check > log_test.txt 2>&1  # run test suite (optional)
    make install || exit -1
fi
#-------------------------------------------------------

cd ${myinstalldir}


#-------------------------------------------------------
if [ "${INSTALLROOT}" == "yes" ]
then
    echo "INSTALLING ROOT"

    
    if [ -e ${myinstalldir}/root ]
    then
       rm -rf ${myinstalldir}/root
    fi  

    if [ -e ${myinstalldir}/root-${rootversion} ]
    then
       rm -rf ${myinstalldir}/root-${rootversion}
    fi  
    
    
    tar -zxf ${hadestarball}/${roottar}
    mv root root-${rootversion}
    
    cd  root-${rootversion}

    export ROOTSYS=$(pwd)

    arch=$(uname -m)
   
    target=linux
   
    if [ "$arch" == "x86_64" ]
    then
       target=linuxx8664gcc
    fi

    ./configure ${target} --enable-soversion \
                        --enable-table \
                        --enable-asimage \
                        --enable-opengl \
                        --enable-minuit2 \
                        --enable-mathmore \
                        --enable-roofit \
                        --enable-xml \
                        --enable-builtin-pcre \
                        --enable-builtin-ftgl \
                        --enable-builtin-glew \
                        --disable-mysql \
                        --disable-pgsql \
                        --disable-monalisa \
                        --disable-globus \
                        --disable-xrootd \
                        --disable-explicitlink \
                        --disable-rpath \
                        --disable-pythia6 \
                        --disable-python \
 			--with-gsl-incdir=${myinstalldir}/gsl-${gslversion}/include/ \
 			--with-gsl-libdir=${myinstalldir}/gsl-${gslversion}/lib

    make -s -j${nthreads} || exit -1
    
fi
#-------------------------------------------------------

cd ${myinstalldir}

#-------------------------------------------------------
if [ "${INSTALLCERN}" == "yes" ]
then
    echo "INSTALLING CERNLIB"
#-------------------------------------------
# Install CERNLIB
#
# uses gfortran to install cernlib including blas
# 64bit on 64bit OS , 32bit on 32bit OS
# To make 32bit code on 64bit OS some edits in
# installcern64.sh have to be done
# it produces cernenv.sh inside the given installdir
# which has to be used to run the garfield installation
# later.

     echo "Checking for CERNLIB requirements"

     echo "------------------------------"
     echo "check imake"
     result=$(which imake)

     if [ "$result" != "" ]
     then
        echo "imake  : ok"
     else
        echo "imake  : not found"
        echo "install xutils-dev"
        exit -1
     fi


     echo "------------------------------"
     echo "check gmake"
     result=$(which gmake)

     if [ "$result" != "" ]
     then
        echo "gmake  : ok"
     else
        echo "gmake  : not found"
        echo "         add a softlink  ln -s /bin/make /bin/gmake (or somewhere in PATH)"
        exit -1
     fi

     echo "------------------------------"
     echo "check ed"
     result=$(which imake)

     if [ "$result" != "" ]
     then
        echo "ed     : ok"
     else
        echo "ed     : not found"
        exit -1
     fi


  echo "------------------------------"
  echo "check openmotiv-dev"

    if [ -e /usr/include/Xm/Xm.h ]
    then
       echo "opemotif-dev  : ok"
    else
       echo "openmotif-dev : not found"
       echo "install libmotif-dev"
       exit -1
    fi


    echo "------------------------------"
    echo "check libxt-dev-dev"

    if [ -e /usr/include/X11/StringDefs.h ]
    then
     echo "libxt-dev  : ok"
    else
       echo "libxt-dev : not found"
       echo "install libxt-dev"
       exit -1
    fi

    echo "------------------------------"
    echo "check libfreetype2"

    if [ -e /usr/include/freetype2 ]
    then
       echo "libfreetype2  : ok"
    else
       echo "libfreetype2 : not found"
       exit -1
    fi


    echo "------------------------------"
    echo "check tcsh"

    if [ -e /bin/tcsh ]
    then
       echo "tcsh  : ok"
    else
       echo "tcsh : not found"
       echo "install tcsh" 
       echo "because of problems with csh and cernlib"
       echo "make csh a softlink to the compatible tcsh"
     
       echo "add a softlink : ln -s /usr/bin/tcsh /usr/bin/csh"
       echo "remove any other csh link in /bin/csh"
       exit -1
    fi

    if [ -e  ${myinstalldir}/cernlib_tarball ]
    then
       rm -rf ${myinstalldir}/cernlib_tarball
    fi

    if [ -e  ${myinstalldir}/cernlib_gfortran ]
    then
       rm -rf ${myinstalldir}/cernlib_gfortran
    fi

    tar -xzf ${hadestarball}/${cerntar}
    cd cernlib_tarball

    . ./installcern64.sh ${myinstalldir}/cernlib_gfortran

    if [ "${CLEANCERN}" == "yes" ]
    then
    rm -rf ${myinstalldir}/cernlib_tarball
    rm -f  ${myinstalldir}/cernlib_gfortran/*.tgz
    rm -f  ${myinstalldir}/cernlib_gfortran/Install_*
    rm -f  ${myinstalldir}/cernlib_gfortran/src_*
    rm -f  ${myinstalldir}/cernlib_gfortran/*.gz
    rm -f  ${myinstalldir}/cernlib_gfortran/*.contents
    rm -rf ${myinstalldir}/cernlib_gfortran/2005/build
    rm -f  ${myinstalldir}/cernlib_gfortran/LAPACK/SRC/*.o
    rm -f  ${myinstalldir}/cernlib_gfortran/LAPACK/BLAS/SRC/*.o
    fi
fi
#-------------------------------------------------------

cd ${myinstalldir}

#-------------------------------------------------------
if [ "${INSTALLGARFIELD}" == "yes" ]
then
    echo "INSTALLING GARFIELD"

    if [ -e  ${myinstalldir}/garfield_tarball ]
    then
       rm -rf ${myinstalldir}/garfield_tarball
    fi

    tar -xzf ${hadestarball}/${garfieldtar}
    cd garfield_tarball

    . ./installgarfield.sh ${myinstalldir}/garfield  ${myinstalldir}/cernlib_gfortran/cernenv.sh
    rm -rf ${myinstalldir}/garfield_tarball
fi
#-------------------------------------------------------

cd ${myinstalldir}

#-------------------------------------------------------
if [ "${INSTALLSCRIPTS}" == "yes" ]
then
    echo "INSTALLING SCRIPTS"

    mkdir -p ${myinstalldir}/admin
    
    ${hadestarball}/replace.pl -i ${hadestarball}/admin/hsc-print -o ${myinstalldir}/admin/hsc-print -k "${tagscripts}#${myinstalldir}/admin"
    ${hadestarball}/replace.pl -i ${hadestarball}/admin/hsc-check -o ${myinstalldir}/admin/hsc-check -k "${tagscripts}#${myinstalldir}/admin"
    ${hadestarball}/replace.pl -i ${hadestarball}/admin/hsc-clean -o ${myinstalldir}/admin/hsc-clean -k "${tagscripts}#${myinstalldir}/admin"
    ${hadestarball}/replace.pl -i ${hadestarball}/admin/hsc-functions.sh -o ${myinstalldir}/admin/hsc-functions.sh -k "${tagshadessoft}#${hadessoft},${tagscripts}#${myinstalldir}/admin"
   
    chmod 755 ${myinstalldir}/admin/hsc-print
    chmod 755 ${myinstalldir}/admin/hsc-check
    chmod 755 ${myinstalldir}/admin/hsc-clean
    chmod 755 ${myinstalldir}/admin/hsc-functions.sh

fi
#-------------------------------------------------------

cd ${myinstalldir}

#-------------------------------------------------------
if [ "${INSTALLHYDRA}" == "yes" ]
then
    echo "INSTALLING HYDRA"
    cd ${mybuilddir}
    mkdir -p ${mybuilddir}/${rootversion}
    cd ${mybuilddir}/${rootversion}
    
    if [ -e ${mybuilddir}/${rootversion}/${hydraname} ]
    then
        rm -rf ${mybuilddir}/${rootversion}/${hydraname}
    fi

    if [ -e ${myinstalldir}/${rootversion}/${hydraname} ]
    then
        rm -rf ${myinstalldir}/${rootversion}/${hydraname}
    fi
    
    mkdir -p ${myinstalldir}/${rootversion}/${hydraname}
    
    svn checkout https://subversion.gsi.de/hades/hydra2/${hydraversion} ${hydraname}
    
    cd ${hydraname}
    
    cp admin/*.mk .
   
    if [ "${doRFIO}" == yes ] && [ "${doORA}" == yes ]
    then
    ${hadestarball}/replace.pl \
    -i ${mybuilddir}/${rootversion}/${hydraname}/admin/Makefile \
    -o ${mybuilddir}/${rootversion}/${hydraname}/Makefile  \
    -l "export USES_RFIO#export USES_RFIO?=yes,export USES_ORACLE#export USES_ORACLE?=yes,INSTALL_DIR#INSTALL_DIR=${myinstalldir}/${rootversion}/${hydraname}"
    fi
    
    
    if [ "${doRFIO}" == no ] && [ "${doORA}" == yes ]
    then
    ${hadestarball}/replace.pl \
    -i ${mybuilddir}/${rootversion}/${hydraname}/admin/Makefile \
    -o ${mybuilddir}/${rootversion}/${hydraname}/Makefile  \
    -l "export USES_RFIO#export USES_RFIO?=no,export USES_ORACLE#export USES_ORACLE?=yes,INSTALL_DIR#INSTALL_DIR=${myinstalldir}/${rootversion}/${hydraname}"
    fi
    
    if [ "${doRFIO}" == yes ] && [ "${doORA}" == no ]
    then
    ${hadestarball}/replace.pl \
    -i ${mybuilddir}/${rootversion}/${hydraname}/admin/Makefile \
    -o ${mybuilddir}/${rootversion}/${hydraname}/Makefile  \
    -l "export USES_RFIO#export USES_RFIO?=yes,export USES_ORACLE#export USES_ORACLE?=no,INSTALL_DIR#INSTALL_DIR=${myinstalldir}/${rootversion}/${hydraname}"
    fi
    
    
    if [ "${doRFIO}" == no ] && [ "${doORA}" == no ]
    then
    ${hadestarball}/replace.pl \
    -i ${mybuilddir}/${rootversion}/${hydraname}/admin/Makefile \
    -o ${mybuilddir}/${rootversion}/${hydraname}/Makefile  \
    -l "export USES_RFIO#export USES_RFIO?=no,export USES_ORACLE#export USES_ORACLE?=no,INSTALL_DIR#INSTALL_DIR=${myinstalldir}/${rootversion}/${hydraname}"
    fi
    
    rootsys=${myinstalldir}/root-${rootversion}
    haddir=${myinstalldir}/${rootversion}/${hydraname}
    cernroot=${myinstalldir}/cernlib_gfortran/2005
   
    ${hadestarball}/replace.pl \
    -i ${hadestarball}/defall.sh \
    -o ${mybuilddir}/${rootversion}/${hydraname}/defall.sh \
    -k "${tagscripts}#${myinstalldir}/admin,export ROOTSYS=#export ROOTSYS=${rootsys},export HADDIR=#export HADDIR=${haddir},export ORACLE_HOME=#export ORACLE_HOME=${ORACLE_HOME},export ORA_USER=#export ORA_USER=${ORA_USER},export CERN_ROOT=#export CERN_ROOT=${cernroot},export ADSM_BASE_NEW=#export ADSM_BASE_NEW=${ADSM_BASE_NEW}"

    chmod 755 ./defall.sh
   
    . ./defall.sh
    
    time make -s -j${nthreads} || exit -1
    make install  || exit -1

    cp ./defall.sh ${myinstalldir}/${rootversion}/${hydraname}
    cp ./defall.sh ${myinstalldir}/admin/defall_${hydraname}.sh
fi
#-------------------------------------------------------

cd ${myinstalldir}

#-------------------------------------------------------
if [ "${INSTALLHGEANT}" == "yes" ]
then
    echo "INSTALLING HGEANT"
    cd ${mybuilddir} 
    mkdir -p ${mybuilddir}/${rootversion}
    cd ${mybuilddir}/${rootversion}

    if [ -e ${mybuilddir}/${rootversion}/${hgeantname} ]
    then
        rm -rf ${mybuilddir}/${rootversion}/${hgeantname}
    fi

    if [ -e ${myinstalldir}/${rootversion}/${hgeantname} ]
    then
        rm -rf ${myinstalldir}/${rootversion}/${hgeantname}
    fi
     
    mkdir -p ${myinstalldir}/${rootversion}/${hgeantname}

    echo svn checkout https://subversion.gsi.de/hades/hgeant2/${hgeantversion} ${hgeantname}
    svn checkout https://subversion.gsi.de/hades/hgeant2/${hgeantversion}  ${hgeantname}

    cd ${hgeantname}

    if [ "${doRFIO}" == yes ] && [ "${doORA}" == yes ]
    then
    ${hadestarball}/replace.pl \
    -i ${mybuilddir}/${rootversion}/${hgeantname}/adm/Makefile \
    -o ${mybuilddir}/${rootversion}/${hgeantname}/Makefile \
    -l "export USES_ORACLE#export USES_ORACLE?=yes,export USES_RFIO#export USES_RFIO?=yes,INSTALL_DIR#INSTALL_DIR=${myinstalldir}/${rootversion}/${hgeantname}"
    fi
    
    if [ "${doRFIO}" == no ] && [ "${doORA}" == yes ]
    then
    ${hadestarball}/replace.pl \
    -i ${mybuilddir}/${rootversion}/${hgeantname}/adm/Makefile \
    -o ${mybuilddir}/${rootversion}/${hgeantname}/Makefile \
    -l "export USES_ORACLE#export USES_ORACLE?=yes,export USES_RFIO#export USES_RFIO?=no,INSTALL_DIR#INSTALL_DIR=${myinstalldir}/${rootversion}/${hgeantname}"
    fi
    
    if [ "${doRFIO}" == yes ] && [ "${doORA}" == no ]
    then
    ${hadestarball}/replace.pl \
    -i ${mybuilddir}/${rootversion}/${hgeantname}/adm/Makefile \
    -o ${mybuilddir}/${rootversion}/${hgeantname}/Makefile \
    -l "export USES_ORACLE#export USES_ORACLE?=no,export USES_RFIO#export USES_RFIO?=yes,INSTALL_DIR#INSTALL_DIR=${myinstalldir}/${rootversion}/${hgeantname}"
    fi
    
    if [ "${doRFIO}" == no ] && [ "${doORA}" == no ]
    then
    ${hadestarball}/replace.pl \
    -i ${mybuilddir}/${rootversion}/${hgeantname}/adm/Makefile \
    -o ${mybuilddir}/${rootversion}/${hgeantname}/Makefile \
    -l "export USES_ORACLE#export USES_ORACLE?=no,export USES_RFIO#export USES_RFIO?=no,INSTALL_DIR#INSTALL_DIR=${myinstalldir}/${rootversion}/${hgeantname}"
    fi
    
    
    
    
    rootsys=${myinstalldir}/root-${rootversion}
    haddir=${myinstalldir}/${rootversion}/${hydraname}
    cernroot=${myinstalldir}/cernlib_gfortran/2005
   
    ${hadestarball}/replace.pl \
    -i ${hadestarball}/defall.sh \
    -o ${mybuilddir}/${rootversion}/${hgeantname}/defall.sh \
    -k "${tagscripts}#${myinstalldir}/admin,export ROOTSYS=#export ROOTSYS=${rootsys},export HADDIR=#export HADDIR=${haddir},export ORACLE_HOME=#export ORACLE_HOME=${ORACLE_HOME},export ORA_USER=#export ORA_USER=${ORA_USER},export CERN_ROOT=#export CERN_ROOT=${cernroot},export ADSM_BASE_NEW=#export ADSM_BASE_NEW=${ADSM_BASE_NEW}"

    chmod 755 ./defall.sh

    . ./defall.sh
    
    time make -s -j${nthreads} || exit -1
    make install  || exit -1
    
    cp ./defall.sh ${myinstalldir}/${rootversion}/${hgeantname}
    cp ./defall.sh ${myinstalldir}/admin/defall_${hgeantname}.sh

fi
#-------------------------------------------------------

cd ${myinstalldir}

#-------------------------------------------------------
if [ "${INSTALLPLUTO}" == "yes" ]
then
    echo "INSTALLING PLUTO"

    if [ -e ${myinstalldir}/${rootversion}/pluto/${plutoname} ]
    then
        rm -rf ${myinstalldir}/${rootversion}/pluto/${plutoname}
    fi
   
    mkdir -p ${myinstalldir}/${rootversion}/pluto
    cd ${myinstalldir}/${rootversion}/pluto

    echo svn checkout https://subversion.gsi.de/hades/pluto/${plutoversion} ${plutoname}
    svn checkout https://subversion.gsi.de/hades/pluto/${plutoversion}  ${plutoname}

    cd ${plutoname}

    
    rootsys=${myinstalldir}/root-${rootversion}
    
    export ROOTSYS=${rootsys}
    
    time make -s -j${nthreads} || exit -1
    
    mkdir -p ${myinstalldir}/${rootversion}/pluto/${plutoname}/include
    
    find ${myinstalldir}/${rootversion}/pluto/${plutoname} -name "*.h" | xargs -i cp {} {myinstalldir}/${rootversion}/pluto/${plutoname}/include/
    
fi
#-------------------------------------------------------

cd ${myinstalldir}

#-------------------------------------------------------
if [ "${INSTALLHZIP}" == "yes" ]
then
    
    echo "INSTALLING HZIP"
    cd ${mybuilddir} 
    mkdir -p ${mybuilddir}/${rootversion}
    cd ${mybuilddir}/${rootversion}

    if [ -e ${mybuilddir}/${rootversion}/hzip_make ]
    then
        rm -rf ${mybuilddir}/${rootversion}/hzip_make
    fi
    
    if [ -e ${myinstalldir}/root-${rootversion} ]
    then
       if [ -e ${myinstalldir}/${rootversion}/${hydraname} ]
       then
          
          svn checkout https://subversion.gsi.de/hades/hydra2/trunk/programs/hzip  hzip_make
          
          cd ${mybuilddir}/${rootversion}/hzip_make
          cp ${mybuilddir}/${rootversion}/${hydraname}/defall.sh .
          . ./defall.sh
       
          make || exit -1
          
          cp ${mybuilddir}/${rootversion}/hzip_make/build/hzip ${myinstalldir}/root-${rootversion}/bin
          
       else
         echo "ERROR: specified hydra version : ${myinstalldir}/${rootversion}/${hydraname} not found!"
       fi
    else
       echo "ERROR: specified root version : ${myinstalldir}/root-${rootversion} not found!"
    fi
fi
#-------------------------------------------------------

cd ${myinstalldir}

#-------------------------------------------------------
if [ "${INSTALLHADD}" == "yes" ]
then
    
    echo "INSTALLING HADD"
    
    if [ -e ${myinstalldir}/root-${rootversion} ]
    then
       
       svn export https://subversion.gsi.de/hades/hydra2/trunk/scripts/tools/hadd.pl  ${myinstalldir}/root-${rootversion}/bin/hadd.pl
          
    else
       echo "ERROR: specified root version : ${myinstalldir}/root-${rootversion} not found!"
       exit -1
    fi
fi
#-------------------------------------------------------


#-------------------------------------------------------
if [ "${INSTALLHISTMAPADD}" == "yes" ]
then
    
    echo "INSTALLING HISTMAPADD"
    cd ${mybuilddir} 
    mkdir -p ${mybuilddir}/${rootversion}
    cd ${mybuilddir}/${rootversion}

    if [ -e ${mybuilddir}/${rootversion}/histmapadd_make ]
    then
        rm -rf ${mybuilddir}/${rootversion}/histmapdd_make
    fi
    
    if [ -e ${myinstalldir}/root-${rootversion} ]
    then
       if [ -e ${myinstalldir}/${rootversion}/${hydraname} ]
       then
          
          svn checkout https://subversion.gsi.de/hades/hydra2/trunk/programs/histmapadd  histmapadd_make
          
          cd ${mybuilddir}/${rootversion}/histmapadd_make
          cp ${mybuilddir}/${rootversion}/${hydraname}/defall.sh .
          . ./defall.sh
       
          make || exit -1
          
          cp ${mybuilddir}/${rootversion}/histmapadd_make/build/histmapadd ${myinstalldir}/root-${rootversion}/bin
	  svn export https://subversion.gsi.de/hades/hydra2/trunk/programs/histmapadd/histmapadd.pl  ${myinstalldir}/root-${rootversion}/bin/histmapadd.pl          
       else
         echo "ERROR: specified hydra version : ${myinstalldir}/${rootversion}/${hydraname} not found!"
       fi
    else
       echo "ERROR: specified root version : ${myinstalldir}/root-${rootversion} not found!"
    fi
fi
#-------------------------------------------------------

#-------------------------------------------------------
if [ "${INSTALLGEMON}" == "yes" ]
then
    if [ -e ${myinstalldir}/root-${rootversion} ]
    then
       svn export https://subversion.gsi.de/hades/hydra2/trunk/scripts/tools/GE_mon.pl  ${myinstalldir}/root-${rootversion}/bin/GE_mon.pl
    else
       echo "ERROR: specified root version : ${myinstalldir}/root-${rootversion} not found!"
    fi
fi
#-------------------------------------------------------


#-------------------------------------------------------
if [ "${INSTALLURQMD3_3}" == "yes" ]
then
    echo "INSTALLING URQMD3_3"
    
    cd ${mybuilddir}

    if [ -e ${mybuilddir}/urqmd-${urqmd3_3} ]
    then
        rm -rf ${mybuilddir}/urqmd-${urqmd3_3}
    fi

    if [ -e ${myinstalldir}/urqmd-${urqmd3_3} ]
    then
        rm -rf ${myinstalldir}/urqmd-${urqmd3_3}
    fi

    mkdir -p ${myinstalldir}/urqmd-${urqmd3_3}
    
    tar -zxf  ${hadestarball}/${urqmd3_3tar}
    cd urqmd-${urqmd3_3}

    OS=$(uname)
    TYPE=$(uname -m)
    make || exit -1

    cp urqmd.${TYPE} ${myinstalldir}/urqmd-${urqmd3_3}/urqmd
    cp ${hadestarball}/urqmd-${urqmd3_3}.pdf ${myinstalldir}/urqmd-${urqmd3_3}
fi
#-------------------------------------------------------

#-------------------------------------------------------
if [ "${INSTALLURQMDCONVERT}" == "yes" ]
then
    echo "INSTALLING URQMDCONVERT"
    
    cd ${mybuilddir}

    if [ -e ${mybuilddir}/convert-urqmd_${urqmdconvert} ]
    then
        rm -rf ${mybuilddir}/convert-urqmd_${urqmdconvert}
    fi

    if [ -e ${myinstalldir}/convert-urqmd_${urqmdconvert} ]
    then
        rm -rf ${myinstalldir}/convert-urqmd_${urqmdconvert}
    fi

    mkdir -p ${myinstalldir}/convert-urqmd_${urqmdconvert}

    
    tar -zxf  ${hadestarball}/${urqmdconverttar}
    
    cd  ${mybuilddir}/convert-urqmd_${urqmdconvert}
    
    . ./build-converter.sh
    
    cp convert-urqmd_${urqmdconvert} ${myinstalldir}/convert-urqmd_${urqmdconvert}
    cp README ${myinstalldir}/convert-urqmd_${urqmdconvert}
fi
#-------------------------------------------------------


cd ${hadestarball}


