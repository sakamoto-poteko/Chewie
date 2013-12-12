#!/bin/sh
#----------------------------------------------------------------------------------------------------
# Author: J. Ngadiuba, L. Uplegger

# The following script sets up the environment to run qt and Monicelli on local machines.
# Once you have fetched this copy from the cvs repository, make a copy (name it something
# like mySetupQt.csh) and modify the copy to suit your needs. Please do NOT put bac in cvs  
# a modified copy of the original setupQt.csh if you have by mistake accidentally edited it.
#
# Before running Monicelli, source YOUR own, PRIVATE **copy** of this file:

export BASEDATADIR /data/TestBeam/2013_09_September

#===============   ftbftracker02.fnal.gov   ==============================================
if(${HOSTNAME} == "ftbftracker02.fnal.gov") then

  export MonicelliDir              ../Monicelli
  
  #===== Local directories
  export CHEWIEDIR=`pwd`
  export CHEWIEDATADIR=${BASEDATADIR}/MonicelliOutput
  export CHEWIEINPUTDIR=${BASEDATADIR}/ChewieInput
  export CHEWIEOUTPUTDIR=${BASEDATADIR}/ChewieOutput
  export CHEWIEXMLDIR=$CHEWIEDIR/xml
  
  #===== Location of the ROOT components
  export ROOTSYS=/opt/local/root

  #===== Location of the Qt components
  export QTDIR=/opt/local/qt
  export QTCREATORDIR=/opt/local/qtcreator
  export QTROOTSYSDIR=`pwd`/qtrootsys
  export QTROOT_INCLUDE_PATH=$ROOTSYS/include

  #===== Location of the BOOST components
  export BOOSTINC=/usr/include
  export BOOSTLIB=/usr/lib64

  #===== Location of the XERCES components
  export XERCESCINC=/opt/local/xerces/include
  export XERCESCLIB=/opt/local/xerces/lib
  
  export LD_LIBRARY_PATH=/usr/lib64
  
endif

#===== Final PATH definitions
export PATH=$ROOTSYS/bin:$QTDIR/bin:$QTCREATORDIR/bin:$PATH
export LD_LIBRARY_PATH=$ROOTSYS/lib:$QTDIR/lib:$QTCREATORDIR/lib:$QTROOTSYSDIR/lib:$XERCESCLIB/:$CHEWIEDIR/../Monicelli/plugins/libs/:$LD_LIBRARY_PATH

#----------------------------------------------------------------------------------------------------
