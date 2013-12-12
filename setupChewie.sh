
export BASEDATADIR=/home/akrzywda/Programming/2013_11_November
export STORAGEDIR=/media/96c057c5-5e1d-400c-b99f-a1aaab8d6ff8

  export MonicelliDir=../Monicelli

  #===== Local directories
  export CHEWIEDIR=`pwd`
  export CHEWIEDATADIR=$STORAGEDIR/MonicelliOutput
  export CHEWIEINPUTDIR=$STORAGEDIR/ChewieInput
  export CHEWIEOUTPUTDIR=$STORAGEDIR/ChewieOutput
  export CHEWIEXMLDIR=$CHEWIEDIR/xml
  
  #===== Location of the ROOT components
  export ROOTSYS=/opt/local/root

  #===== Location of the Qt components
  export QTDIR=/usr/local/qt
  export QTCREATORDIR=/usr/local/qtcreator
  export QTROOTSYSDIR=`pwd`/qtrootsys
  export QTROOT_INCLUDE_PATH=$ROOTSYS/include

  #===== Location of the BOOST components
  export BOOSTINC=/usr/include
  export BOOSTLIB=/usr/lib64

  #===== Location of the XERCES components
  export XERCESCINC=/opt/local/xerces/include
  export XERCESCLIB=/opt/local/xerces/lib
  

#===== Final PATH definitions
export PATH=$ROOTSYS/bin:$QTDIR/bin:$QTCREATORDIR/bin:$PATH
export LD_LIBRARY_PATH=$ROOTSYS/lib:$QTDIR/lib:$QTCREATORDIR/lib:$QTROOTSYSDIR/lib:$XERCESCLIB/:$CHEWIEDIR/../Monicelli/plugins/libs/

#----------------------------------------------------------------------------------------------------
