#!/bin/sh
###############################################################################
#
#  Hades Software Environment Setup Script (Bourne shell family version)
#
#  Each Software Collection should have it dedicated verion of this script.
#  They can be distinguished e.g. by the used paths.
#
#  Author: Simon Lang, GSI, 30.09.2006
#
###############################################################################

. SCRIPTSADMIN/hsc-functions.sh

hsc_checkContext

# Root, and dependent packages
export ROOTSYS=

# Global Hydra Location
export HADDIR=
export ROOT_INCLUDE_PATH=${HADDIR}/include

# Private Hydra Location - not used by default
#export MYHADDIR=/e.g./somewhere/in/my/home/directory
#export ROOT_INCLUDE_PATH=${MYHADDIR}/include:${HADDIR}/include

# Oracle
export ORACLE_HOME=
export ORA_USER=

# CERNLIB - for HGeant
export CERN_ROOT=

# PLUTO
export PLUTODIR=

# RFIO support
export ADSM_BASE_NEW=

hsc_setEnvironment
hsc_shortPrintout

hsc_finalizeScript
