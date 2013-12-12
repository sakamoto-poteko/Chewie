#----------------------------------------------------------------------------------------------------
# Author: J. Ngadiuba, L. Uplegger

# The following script sets up the environment to run qt and Monicelli on local machines.
# Once you have fetched this copy from the cvs repository, make a copy (name it something
# like mySetupQt.csh) and modify the copy to suit your needs. Please do NOT put bac in cvs  
# a modified copy of the original setupQt.csh if you have by mistake accidentally edited it.
#
# Before running Monicelli, source YOUR own, PRIVATE **copy** of this file:

setenv BASEDATADIR /home/nfs/cmstestbeam/Analysis

#===============   ftbftracker02.fnal.gov   ==============================================
if(${HOSTNAME} == "ftbftracker02.fnal.gov") then

  setenv MonicelliDir              ../Monicelli
  
  #===== Local directories
  setenv CHEWIEDIR		   `pwd`
  setenv CHEWIEDATADIR		   /data/Akrzywda/MonicelliOutputAkrzywda
  setenv CHEWIEINPUTDIR            /data/Akrzywda/ChewieInputAkrzywda
  setenv CHEWIEOUTPUTDIR           /data/Akrzywda/ChewieOutputAkrzywda
  setenv CHEWIEXMLDIR              $CHEWIEDIR/xml
  
  #===== Location of the ROOT components
  setenv ROOTSYS		  /opt/local/root

  #===== Location of the Qt components
  setenv QTDIR  		  /opt/local/qt
  setenv QTCREATORDIR		  /opt/local/qtcreator
  setenv QTROOTSYSDIR		  `pwd`/qtrootsys
  setenv QTROOT_INCLUDE_PATH	  $ROOTSYS/include

  #===== Location of the BOOST components
  setenv BOOSTINC		  /usr/include
  setenv BOOSTLIB		  /usr/lib64

  #===== Location of the XERCES components
  setenv XERCESCINC		  /opt/local/xerces/include
  setenv XERCESCLIB		  /opt/local/xerces/lib
  
  setenv LD_LIBRARY_PATH          /usr/lib64
  
endif

#===== Final PATH definitions
setenv PATH			$ROOTSYS/bin:$QTDIR/bin:$QTCREATORDIR/bin:$PATH
setenv LD_LIBRARY_PATH          $ROOTSYS/lib:$QTDIR/lib:$QTCREATORDIR/lib:$QTROOTSYSDIR/lib:$XERCESCLIB/:$CHEWIEDIR/../Monicelli/plugins/libs/:$LD_LIBRARY_PATH

#----------------------------------------------------------------------------------------------------
