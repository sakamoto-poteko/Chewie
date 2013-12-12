/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class HNavigator ;
class AnalyzerDlg;

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QSignalMapper;
class QDialog;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    QMdiArea      * getMdiArea            (void                    ) {return mdiArea_      ;}
    HNavigator    * getHNavigator         (void                    ) {return theHNavigator_;}
    AnalyzerDlg   * getAnalyzer           (void                    ) {return theAnalyzer_  ;}
protected:
    void            closeEvent            (QCloseEvent  * event    );

private slots:
    void            openAnalyzer          (void                    );
    void            about                 (void                    );
    void            buildHNavigator       (void                    );
    void            updateMenus           (void                    );
    void            updateWindowMenu      (void                    );
    void            enableAnalyzerButton  (bool enable=true        );
    void            enableHNavigatorButton(bool enable=true        );
    void            switchLayoutDirection (void                    );
    void            setActiveSubWindow    (QWidget *window         );


private:
    void            createActions         (void                    );
    void            destroyActions        (void                    );
    void            createMenus           (void                    );
    void            createToolBars        (void                    );
    void            createStatusBar       (void                    );
    void            readSettings          (void                    );
    void            writeSettings         (void                    );

    QDialog*        activeChildDlg        (void                    );

    QMdiArea*       mdiArea_       ;
    QSignalMapper*  windowMapper_  ;

    HNavigator*     theHNavigator_ ;
    AnalyzerDlg*    theAnalyzer_   ;

    QMenu*          fileMenu_      ;
    QMenu*          editMenu_      ;
    QMenu*          windowMenu_    ;
    QMenu*          helpMenu_      ;

    QToolBar*       fileToolBar_   ;
    QToolBar*       editToolBar_   ;

    QString         path_          ;

    QAction*        exitAct_       ;
    QAction*        separatorAct_  ;
    QAction*        aboutAct_      ;
    QAction*        aboutQtAct_    ;
    QAction*        analyzerAct_   ;
    QAction*        hNavigatorAct_ ;
};

#endif
