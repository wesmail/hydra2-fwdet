#!/bin/sh
###############################################################################
#
#  Hades Software Environment Setup Script (Bourne shell family version)
#
#  Each Software Collection should have it dedicated verion of this script.
###############################################################################

. /misc/hadessoftware/etch32/admin/hsc-functions.sh
hsc_checkContext

# Root, and dependent packages
export ROOTSYS=/misc/hadessoftware/etch32/install/root-5.22.00a

# Global Hydra Location
export HADDIR=/misc/hadessoftware/etch32/install/hydra-dev

# Private Hydra Location - not used by default
#export MYHADDIR=/e.g./somewhere/in/my/home/directory

# Oracle
export ORACLE_HOME=/usr/local/oracle/product/10.2.0.1client-Deb4
export ORA_USER=hades_ana/hades@db-hades

# Qt GUI framework
export QTDIR=/misc/hadessoftware/etch32/install/qt-3.3.4

# CERNLIB - for HGeant
export CERN_ROOT=/misc/hadessoftware/etch32/install/cernlib_g77/2006

# RFIO support
export ADSM_BASE_NEW=/misc/hadessoftware/etch32/install/gstore-may07

hsc_setEnvironment
hsc_shortPrintout
hsc_finalizeScript
