#* Authors: Lorenzo Uplegger, Jennifer Ngadiuba
#*
#* Fermilab, I.N.F.N. Milan-Bicocca
#*
#****************************************************************************
include("$(ROOTSYS)/include/rootcint.pri")

QMAKE_CFLAGS_RELEASE = -O3
QMAKE_CXXFLAGS_RELEASE = -g

target.path          = ./
sources.path         = ./

HEADERS       = include/mainwindow.h                                 \
                include/MessageTools.h                               \
                include/ANSIColors.h                                 \
                include/AnalysisManager.h                            \
                include/Data.h                                       \
                include/Analysis.h                                   \
                include/Threader.h                                   \
                include/Efficiency.h                                 \
                include/EventManager.h                               \
                include/EventConverter.h                             \
                include/HistogramWindow.h                            \
                include/Window.h                                     \
                include/ThreadUtilities.h                            \
                include/CutsUtilities.h                              \
                include/CutsUtilities.h                              \
                include/Charge.h                                     \
                include/XmlParser.h                                  \
                include/analyzerdlg.h                                \
                include/hnavigator.h                                 \
                include/PlanesMapping.h                              \
                include/XmlAnalysis.h                                \
                include/XmlGeneral.h                                 \
                include/WindowsManager.h                             \
                include/hTreeBrowser.h                               \
                include/XmlWindow.h                                  \
                include/XmlPlane.h                                   \
                include/Resolution.h                                 \
                include/XmlConverter.h                               \
                include/Utilities.h                                  \
                include/XmlScan.h                                    \
                include/CanvasWidget.h                               \
                include/Tracks.h                                     \
                include/CalibrationsManager.h                        \
                ../Monicelli/include/EventHeader.h                   \
                ../Monicelli/include/Event.h                         \
                ../Monicelli/include/Geometry.h                      \
                ../Monicelli/include/Detector.h                      \
                ../Monicelli/include/ROC.h                           \
                ../Monicelli/include/mdisubwindow.h                  \
                ../Monicelli/plugins/customTextEdit/customTextEdit.h \
                ../Monicelli/plugins/customLineEdit/customLineEdit.h \
                ../Monicelli/plugins/customCheckBox/customCheckBox.h \
                ../Monicelli/plugins/customSpinBox/customSpinBox.h   \
                ../Monicelli/plugins/customTableWidget/customTableWidget.h \
    include/PixelMatrix.h \
    include/TracksAfter.h \
    include/fitParamManagerWidget.h \
    include/rectwindow.h




SOURCES       = src/EventManager.cpp              \
                src/EventConverter.cpp            \
                src/Data.cpp                      \
                src/AnalysisManager.cpp           \
                src/WindowsManager.cpp            \
                src/Window.cpp                    \
                src/HistogramWindow.cpp           \
                src/Analysis.cpp                  \
                src/Efficiency.cpp                \
                src/Charge.cpp                    \
                src/Threader.cpp                  \
                src/ThreadUtilities.cpp           \
                src/hTreeBrowser.cpp              \
                src/PlanesMapping.cpp             \               
                src/XmlAnalysis.cpp               \
                src/XmlGeneral.cpp                \
                src/analyzerdlg.cpp               \
                src/hnavigator.cpp                \
                src/mainwindow.cpp                \
                src/main.cpp                      \
                src/XmlWindow.cpp                 \
                src/XmlPlane.cpp                  \
                src/XmlParser.cpp                 \
                src/Resolution.cpp                \
                src/XmlConverter.cpp              \
                src/Utilities.cpp                 \
                src/CanvasWidget.cpp              \
                src/Tracks.cpp                    \
                src/CalibrationsManager.cpp       \
                tmp/EventDict.C                   \
                tmp/EventHeaderDict.C             \
                ../Monicelli/src/EventHeader.cpp  \
                ../Monicelli/src/Event.cpp        \
                ../Monicelli/src/Geometry.cpp     \
                ../Monicelli/src/Detector.cpp     \
                ../Monicelli/src/ROC.cpp          \
                ../Monicelli/src/mdisubwindow.cpp \
                src/XmlScan.cpp \
    src/PixelMatrix.cpp \
    src/TracksAfter.cpp \
    src/fitParamManagerWidget.cpp \
    src/rectwindow.cpp



RESOURCES     = Chewie.qrc

DEFINES              = USE_QT

INCLUDEPATH         += ./include                                \
                       ../Monicelli                             \
                       ../Monicelli/include                     \
                       ../Monicelli/plugins/customLineEdit      \
                       ../Monicelli/plugins/customTextEdit      \
                       ../Monicelli/plugins/customCheckBox      \
                       ../Monicelli/plugins/customSpinBox       \
                       ../Monicelli/plugins/customTableWidget   \
                       -pthread $(ROOTSYS)/include              \
                       $(QTDIR)/include/QtXml                   \
                       $(XERCESCINC)                            \
                       $(BOOSTINC)/include/boost/

LIBS                += -lTreePlayer                             \

LIBS                += -L$(BOOSTLIB)                            \
                       -lboost_regex                            \
#                       -lboost_system                           \
                       -lboost_filesystem


LIBS                += -L../Monicelli/plugins/libs              \
                       -lcustomLineEdit                         \
                       -lcustomTextEdit                         \
                       -lcustomCheckBox                         \
                       -lcustomSpinBox                          \
                       -lcustomTableWidget                      \


# install
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS Chewie.pro images

INSTALLS += target sources

header.depends       = ../Monicelli/include/EventHeader.h

header.target        = tmp/EventHeaderDict.C
header.commands      = rootcint -f tmp/EventHeaderDict.C                     \
                                -c ../Monicelli/include/EventHeader.h+

trees.depends        = ../Monicelli/include/Event.h                          \
                       ../Monicelli/include/Geometry.h                       \
                       ../Monicelli/include/Detector.h                       \
                       ../Monicelli/include/ROC.h

trees.target         = tmp/EventDict.C
trees.commands       = rootcint -f tmp/EventDict.C                           \
                                -c ../Monicelli/include/Event.h+             \
                                   ../Monicelli/include/Geometry.h+          \
                                   ../Monicelli/include/Detector.h+          \
                                   ../Monicelli/include/ROC.h+

QMAKE_EXTRA_TARGETS += trees
QMAKE_EXTRA_TARGETS += header



FORMS += analyzerdlg.ui \
    hnavigator.ui \
    canvaswidget.ui

QT *= xml

DEPENDPATH += . src include ../Monicelli/plugins/customTableWidget

unix:!macx:!symbian: LIBS += -lTreePlayer

OTHER_FILES +=
