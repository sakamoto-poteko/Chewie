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

#include "mainwindow.h"
#include "hnavigator.h"
#include "analyzerdlg.h"

#include <mdisubwindow.h>
#include <QtGui>

//#include "MessageTools.h"

//===========================================================================
MainWindow::MainWindow() :
        theHNavigator_(0)
        ,theAnalyzer_(0)
{
    path_ = QString(getenv("CHEWIEDIR")) ;

    mdiArea_ = new QMdiArea;
    mdiArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea_);
    //    connect(mdiArea_, SIGNAL(subWindowActivated(QMdiSubWindow*)),
    //            this, SLOT(updateMenus()));
    windowMapper_ = new QSignalMapper(this);
    connect(windowMapper_, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("Chewie"));
    setUnifiedTitleAndToolBarOnMac(true);
}

//===========================================================================
MainWindow::~MainWindow()
{
    destroyActions();
    delete mdiArea_;
}

//===========================================================================
void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea_->closeAllSubWindows();
    if (mdiArea_->currentSubWindow())
    {
        event->ignore();
    } else
    {
        writeSettings();
        event->accept();
    }
}

//===========================================================================
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Chewie"), tr("<b>Chewie</b> is the testbeam analyzer."));
}

//===========================================================================
void MainWindow::updateMenus()
{
    //bool hasChild = (activeChildDlg() != 0);
    //    analyzerAct->setEnabled(hasChild);
}

//===========================================================================
void MainWindow::updateWindowMenu()
{
    windowMenu_->clear();
    //  windowMenu_->addAction(closeAct);

    QList<QMdiSubWindow*> windows = mdiArea_->subWindowList();
    separatorAct_->setVisible(!windows.isEmpty());

    /*
    for (int i = 0; i < windows.size(); ++i)
    {
        QDialog* child = qobject_cast<QDialog*>(windows.at(i)->widget());

        //QString text;
        //if (i < 9) {
        //    text = tr("&%1 %2").arg(i + 1)
        //                       .arg(child->userFriendlyCurrentFile());
        //} else {
        //    text = tr("%1 %2").arg(i + 1)
        //                      .arg(child->userFriendlyCurrentFile());
        //}
        //QAction *action  = windowMenu->addAction(text);
        //action->setCheckable(true);
        //action ->setChecked(child == activeMdiChild());
        //connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        //windowMapper->setMapping(action, windows.at(i));
    }
*/
}

//===========================================================================
void MainWindow::enableAnalyzerButton(bool enable)
{
    analyzerAct_->setEnabled(enable);
    if(enable)
        theAnalyzer_ = 0;
}

//===========================================================================
void MainWindow::enableHNavigatorButton(bool enable)
{
    hNavigatorAct_->setEnabled(enable);
    if(enable)
        theHNavigator_ = 0;
}

//===========================================================================
void MainWindow::createActions()
{
    QString iconPath ;

    //! [0]
    exitAct_ = new QAction(tr("E&xit"), this);
    exitAct_->setShortcuts(QKeySequence::Quit);
    exitAct_->setStatusTip(tr("Exit the application"));
    connect(exitAct_, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    //! [0]
    separatorAct_ = new QAction(this);
    separatorAct_->setSeparator(true);

    aboutAct_ = new QAction(tr("&About"), this);
    aboutAct_->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct_, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct_ = new QAction(tr("About &Qt"), this);
    aboutQtAct_->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct_, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    iconPath  = path_ ;
    iconPath += "/images/chewie.png" ;
    analyzerAct_ = new QAction(QIcon(iconPath), tr("&Open Analyzer"), this);
    analyzerAct_->setShortcuts(QKeySequence::New);
    analyzerAct_->setStatusTip(tr("Open a new analyzer"));
    connect(analyzerAct_, SIGNAL(triggered()), this, SLOT(openAnalyzer()));

    iconPath  = path_ ;
    iconPath += "/images/newCanvas.png" ;
    hNavigatorAct_ = new QAction(QIcon(iconPath), tr("Histogram Navigator"), this);
    hNavigatorAct_->setStatusTip(tr("Create a Histogram Navigator")) ;
    connect(hNavigatorAct_,   SIGNAL(triggered()), this, SLOT(buildHNavigator()));
}

//===========================================================================
void MainWindow::destroyActions()
{
    delete exitAct_;
    delete separatorAct_;
    delete aboutAct_;
    delete aboutQtAct_;
    delete analyzerAct_;
    delete hNavigatorAct_;
}

//===========================================================================
void MainWindow::createMenus()
{
    fileMenu_ = menuBar()->addMenu(tr("&File"));
    //    QAction *action = fileMenu->addAction(tr("Switch layout direction"));
    //    connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
    fileMenu_->addAction(exitAct_);

    windowMenu_ = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu_, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu_ = menuBar()->addMenu(tr("&Help"));
    helpMenu_->addAction(aboutAct_);
    helpMenu_->addAction(aboutQtAct_);
}

//===========================================================================
void MainWindow::createToolBars()
{
    fileToolBar_ = addToolBar(tr("Dialogs"));
    fileToolBar_->addAction(analyzerAct_);
    fileToolBar_->addAction(hNavigatorAct_) ;
}

//===========================================================================
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

//===========================================================================
void MainWindow::readSettings()
{
    QSettings settings("Trolltech", "Chewie Analyzer");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(1024, 620)).toSize();
    move(pos);
    resize(size);
}

//===========================================================================
void MainWindow::writeSettings()
{
    QSettings settings("Trolltech", "Chewie Analyzer");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

//===========================================================================
QDialog* MainWindow::activeChildDlg()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea_->activeSubWindow())
    {
        return qobject_cast<QDialog*>(activeSubWindow->widget());
    }
    return 0;
}

//===========================================================================
void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

//===========================================================================
void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea_->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

//===========================================================================
void MainWindow::openAnalyzer()
{
    if(!theAnalyzer_)
    {
        //    QDialog* child = new AnalyzerDlg(this) ;
        theAnalyzer_ = new AnalyzerDlg(this) ;
        QMdiSubWindow* subWindow = mdiArea_->addSubWindow(theAnalyzer_);
        enableAnalyzerButton(false) ;
        connect(subWindow, SIGNAL(destroyed()), this, SLOT(enableAnalyzerButton()));
        subWindow->setGeometry(5,5,1024,680);
        subWindow->show();
    }
}
//===========================================================================
void MainWindow::buildHNavigator()
{

    if( !theHNavigator_)
    {
        theHNavigator_ = new HNavigator(this) ;
        QMdiSubWindow* subWindow = (mdiSubWindow*)mdiArea_->addSubWindow(theHNavigator_) ;
        enableHNavigatorButton(false) ;
        connect(subWindow, SIGNAL(destroyed()), this, SLOT(enableHNavigatorButton()));
        subWindow->setGeometry(1026,5,theHNavigator_->width()+8,theHNavigator_->height()+40) ;
        subWindow->show() ;
    }
}
