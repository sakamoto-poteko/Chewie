/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include <QtGui>
#include <QTreeWidgetItem>
#include <QMdiArea>

#include <TDirectory.h>
#include <TDirectoryFile.h>
#include <TH1.h>
#include <TGraph.h>
#include <TKey.h>
#include <TIterator.h>
#include <TObject.h>
#include <TTree.h>
#include <TROOT.h>
#include <TFile.h>
#include <TStyle.h>
#include <TNtuple.h>

#include "hTreeBrowser.h"
#include "mainwindow.h"
#include "CanvasWidget.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

//=========================================================================
hTreeBrowser::hTreeBrowser(QWidget *parent, MainWindow * mainWindow)
    : QTreeWidget   (parent)
    , parent_       (parent->parentWidget())
    , theMainWindow_(mainWindow)
    , currentCanvas_(0)
    , canvasPosX_   (5)
    , canvasPosY_   (5)
    , canvasWitdh_  (800)
    , canvasHeight_ (600)
    , currentObject_(0)
{
    //    theHManager_   = theMainWindow_->getHManager()   ;
    serviceCanvas_.clear();
    cSw_.clear() ;

    QStringList labels;
    labels << tr("Folder Name") << tr("Object Type") << tr("Entries");
    this->setHeaderLabels(labels);
    this->header()->setResizeMode(0, QHeaderView::Interactive);
    this->header()->setResizeMode(1, QHeaderView::Interactive);
    this->header()->setResizeMode(2, QHeaderView::Interactive);

    this->header()->setDefaultSectionSize(200);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    QStyle *style = parent_->style();

    QString imagesDir = QString(getenv("CHEWIEDIR")) + "/images/";
    QPixmap closedFolderPM(imagesDir+"ClosedFolder.png");
    QPixmap openFolderPM  (imagesDir+"OpenFolder.png"  );
    QPixmap TH1IconPM     (imagesDir+"TH1Icon.png"     );
    QPixmap TH2IconPM     (imagesDir+"TH2Icon.png"     );
    QPixmap TTreePM       (imagesDir+"TreeIcon.png"    );
    QPixmap TGraphPM      (imagesDir+"TGraphIcon.png"  );
    QPixmap TProfilePM    (imagesDir+"TProfileIcon.png");

    folderIcon_  .addPixmap(closedFolderPM, QIcon::Normal, QIcon::Off      );
    folderIcon_  .addPixmap(openFolderPM,   QIcon::Normal, QIcon::On       );
    canvasIcon_  .addPixmap(style->standardPixmap(QStyle::SP_FileIcon     ));
    TH1Icon_     .addPixmap(TH1IconPM                                      );
    TH2Icon_     .addPixmap(TH2IconPM                                      );
    tTreeIcon_   .addPixmap(TTreePM                                        );
    tGraphIcon_  .addPixmap(TGraphPM                                       );
    tProfileIcon_.addPixmap(TProfilePM                                     );
    mainIcon_    .addPixmap(style->standardPixmap(QStyle::SP_DriveHDIcon  ));

    this->setAnimated(true);

    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QObject::connect(this, SIGNAL(customContextMenuRequested( const QPoint         &     )),
                     this, SLOT(  showContextMenu           ( const QPoint         &     ))) ;
    QObject::connect(this, SIGNAL(itemClicked               (       QTreeWidgetItem*, int)),
                     this, SLOT(  manipulateFolder          (       QTreeWidgetItem*, int))) ;
    //QObject::connect(this, SIGNAL(itemEntered               (       QTreeWidgetItem*, int)),
    //                 this, SLOT(  showHint                  (       QTreeWidgetItem*, int))) ;
}

//=========================================================================
hTreeBrowser::~hTreeBrowser(void)
{
    STDLINE("Dtor",ACRed) ;
}

//=========================================================================
void hTreeBrowser::cleanDestroy(void)
{
    serviceCanvas_.clear() ;
    cSw_.clear() ;
}

//=========================================================================
void hTreeBrowser::clear(void)
{
    QTreeWidget::clear();
    existingWItems_.clear();
}

//=========================================================================
void hTreeBrowser::populate(TDirectory* currentDirectory)
{
    //    STDLINE(currentDirectory->GetName(),ACRed);
    QTreeWidgetItem* wItem = 0 ;
    if( existingWItems_.find(currentDirectory->GetName()) == existingWItems_.end())
    {
        existingWItems_[currentDirectory->GetName()] =  new QTreeWidgetItem(this);
    }
    //    STDLINE(currentDirectory->GetName(),ACRed);
    wItem = existingWItems_[currentDirectory->GetName()] ;
    //    STDLINE(currentDirectory->GetName(),ACPurple);
    wItem->setText(0, currentDirectory->GetName()) ;
    //    STDLINE(currentDirectory->GetName(),ACCyan);
    wItem->setIcon(0, mainIcon_                  ) ;
    this->populate(currentDirectory, wItem) ;
    //    STDLINE(currentDirectory->GetName(),ACGreen);
}

//=========================================================================
void hTreeBrowser::populate(TFolder* currentFolder)
{
    //    STDLINE(currentFolder->GetName(),ACRed);
    QTreeWidgetItem * wItem = 0 ;
    if( existingWItems_.find(currentFolder->GetName()) == existingWItems_.end())
    {
        existingWItems_[currentFolder->GetName()] =  new QTreeWidgetItem(this);
        //        STDLINE(currentFolder->GetName(),ACGreen);
    }
    //    STDLINE(currentFolder->GetName(),ACPurple);
    wItem = existingWItems_[currentFolder->GetName()] ;
    //    STDLINE(currentFolder->GetName(),ACPurple);
    wItem->setText(0, currentFolder->GetName()) ;
    wItem->setIcon(0, mainIcon_               ) ;

    this->populate(currentFolder,wItem) ;
    //    STDLINE(currentFolder->GetName(),ACCyan);
}

//=========================================================================
void hTreeBrowser::populate(TFolder* currentFolder, QTreeWidgetItem* parentWItem)
{
    //    STDLINE("populate(TFolder,QTreeWidgetItem)",ACRed);
    QTreeWidgetItem * wItem   = NULL  ;
    TObject         * obj     = NULL  ;
    std::string       objPath = ""    ;

    bool              create  = false ;

    TCollection* rootFoldersColl     = (TCollection *)currentFolder->GetListOfFolders() ;

    TIterator*   rootFoldersIterator = rootFoldersColl->MakeIterator() ;

    while((obj = rootFoldersIterator->Next()))
    {
        std::string folderName = obj->GetName() ;
        //STDLINE(folderName,ACGreen) ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Classes"))        continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Colors"))         continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("MapFiles"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Sockets"))        continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Canvases"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Styles"))         continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Functions"))      continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Tasks"))          continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Geometries"))     continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Browsers"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Specials"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Handlers"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("Cleanups"))       continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("StreamerInfo"))   continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("SecContexts"))    continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("PROOF Sessions")) continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("ROOT Memory"))    continue ;
        if( obj->IsA() == TFolder::Class() && folderName == std::string("ROOT Files"))     continue ;

        STDLINE(folderName + " unrecognized!",ACRed) ;
        exit(0);
        //if( folderName == "ROOT Files")
        //{

        objPath = std::string(currentFolder->GetName()) + std::string("-") + folderName ;

        if(existingWItems_.find(objPath) == existingWItems_.end())
        {
            /*
            // dario
            ss_.str(""); ss_ << currentFolder->GetName() << " || " << "objPath: " << objPath << " parentWItem: " << parentWItem; STDLINE(ss_.str(),ACWhite)
            bool alreadyThere = false ;
            for(existingWItemsDef::iterator pi=existingWItems_.begin(); pi!=existingWItems_.end(); ++pi)
            {
                if( (*pi).second == parentWItem) {alreadyThere = true ; break;}
                ss_.str(""); ss_ << "  " << (*pi).first << " --> " << (*pi).second << " alreadyThere: " << alreadyThere; STDLINE(ss_.str(),ACCyan)
            }
            // dario

            if( alreadyThere )
            {
              wItem = parentWItem ;
            }
            else
            {
              wItem = new QTreeWidgetItem(parentWItem) ;
              existingWItems_[objPath] = wItem ;
              create = true ;
              ss_.str(""); ss_  << "Created " << ACRed << ACBold << parentWItem ; STDLINE(ss_.str(),ACCyan)
            }
            create = true ;
*/
            wItem = new QTreeWidgetItem(parentWItem) ;
            existingWItems_[objPath] = wItem ;
            create = true ;
        }
        else
        {
            wItem  = existingWItems_[objPath] ;
            create = false ;
        }

        this->getObjectType(obj) ;

        if( obj->IsA() == TFolder::Class() )
        {
            if( create )
            {
                wItem->setText(0, tr(obj->GetName())) ;
                wItem->setText(1, tr("Folder")) ;
                wItem->setIcon(0, folderIcon_) ;
            }
            this->populate((TFolder*)obj,wItem) ;
        }
        else if( this->getObjectType(obj) == "TH1" )
        {
            if( !create ) continue ;
            ss_.str(""); ss_ << ((TH1*)obj)->GetEntries() ;
            wItem->setText(0, tr(obj->GetName())) ;
            wItem->setText(1, tr("TH1")) ;
            wItem->setText(2, tr(ss_.str().c_str())) ;
            wItem->setIcon(0, TH1Icon_) ;
        }
        else if( this->getObjectType(obj) == "TH2" )
        {
            if( !create ) continue ;
            ss_.str(""); ss_ << ((TH1*)obj)->GetEntries() ;
            wItem->setText(0, tr(obj->GetName())) ;
            wItem->setText(1, tr("TH2")) ;
            wItem->setText(2, tr(ss_.str().c_str())) ;
            wItem->setIcon(0, TH2Icon_);
        }
        else if( this->getObjectType(obj) == "TGraph" )
        {
            if( !create ) continue ;
            ss_.str(""); ss_ << ((TGraph*)obj)->GetN() ;
            wItem->setText(0, tr(obj->GetName())) ;
            wItem->setText(1, tr("TGraph")) ;
            wItem->setText(2, tr(ss_.str().c_str())) ;
            wItem->setIcon(0, tGraphIcon_);
        }
        else
        {
            STDLINE(this->getObjectType(obj),ACPurple);
        }
        //}
    }

}

//=========================================================================
std::string hTreeBrowser::getObjectType(TObject* obj)
{
    //    STDLINE("",ACRed);
    std::string objectType = "Unknown" ;
    TKey* keyH = 0 ;

    TIter bases(obj->IsA()->GetListOfBases());
    int count = 0 ;
    while((keyH = (TKey*)bases()))
    {
        if( count++ == 0 )
            objectType = keyH->GetName() ;
    }
    return objectType ;
}

//=========================================================================
void hTreeBrowser::populate(TDirectory* currentDirectory, QTreeWidgetItem* parentWItem)
{
    //    STDLINE("populate(TDirectory,QTree)",ACRed);
    QTreeWidgetItem * wItem   = NULL;
    bool              create  = false ;

    //currentDirectory->ls() ;

    TKey * keyH = NULL ;
    TIter hList(currentDirectory->GetListOfKeys());

    while((keyH = (TKey*)hList()))
    {
        //If you uncomment this line you will get a
        //Error in <TBufferFile::ReadClassBuffer>: class: TUUID, attempting to access a wrong version
        //Every time it tries to read a TFolder or TDirectory object
        //this->getObjectType(keyH->ReadObj());
        std::string objPath = std::string(currentDirectory->GetName()) + std::string("-") +  std::string(keyH->GetName());
        //std::string objectName = keyH->GetName() ;
        //        STDLINE(keyH->GetName(),ACPurple);
        if( existingWItems_.find(objPath) == existingWItems_.end())
        {
            wItem = new QTreeWidgetItem(parentWItem) ;
            existingWItems_[objPath] = wItem ;
            create = true ;
        }
        else
        {
            wItem  = existingWItems_[objPath] ;
            create = false ;
        }

        if( keyH->IsFolder() )
        {
            if(std::string(keyH->GetClassName()) == "TTree")
            {
                if( !create ) continue ;
                wItem->setText(0, keyH->GetName()) ;
                wItem->setText(1, "TTree") ;
                wItem->setIcon(0, tTreeIcon_);
            }
            else if (std::string(keyH->GetClassName()) == "TFolder" )
            {
                TFolder * aFolder = (TFolder*)(currentDirectory->GetKey(keyH->GetName())->ReadObj()) ; // Get the folder in memory (this is the trick!)
                if( create )
                {
                    wItem->setText(0, tr(keyH->GetName())) ;
                    wItem->setText(1, tr(keyH->GetClassName())) ;
                    wItem->setIcon(0, folderIcon_);
                }
                this->populate(aFolder,wItem) ;
            }
            else
            {
                currentDirectory->cd(keyH->GetName());
                if( create )
                {
                    wItem->setText(0, tr(gDirectory->GetName())) ;
                    wItem->setText(1, tr(keyH->GetClassName())) ;
                    wItem->setIcon(0, folderIcon_);
                }
                this->populate(gDirectory,wItem) ;
            }
        }
        else
        {
            if( !create ) continue ;
            wItem->setText(0, tr(keyH->GetName())) ;
            wItem->setText(1, tr(keyH->GetClassName())) ;
            std::string className = keyH->GetClassName();
            if(className == "TCanvas" )
            {
                wItem->setIcon(0, canvasIcon_);
            }
            else if( className.find("TH1") != std::string::npos )
            {
                wItem->setIcon(0, TH1Icon_);
            }
            else if( className.find("TH2") != std::string::npos )
            {
                wItem->setIcon(0, TH2Icon_);
            }
            else if( className.find("TGraph") != std::string::npos )
            {
                wItem->setIcon(0, tGraphIcon_);
            }
            else if( className == "TProfile" )
            {
                wItem->setIcon(0, tProfileIcon_);
            }
            else
            {
                //STDLINE(className + "-" + this->getObjectType(keyH->ReadObj()),ACRed);
                wItem->setIcon(0, fileIcon_);
            }
        }
    }

}

//=========================================================================
hTreeBrowser::dirPathDef hTreeBrowser::getFullPath(QTreeWidgetItem * wItem)
{
    std::string fullPath = wItem->text(0).toStdString();
    //    STDLINE(fullPath,ACPurple);
    if( wItem->parent() )
    {
        this->getFullPath(wItem->parent());
        dirPath_.push_back(fullPath) ;
        return dirPath_ ;
    }

    dirPath_.clear() ;
    dirPath_.push_back(fullPath) ;
    return dirPath_ ;
}

//=========================================================================
void hTreeBrowser::showContextMenu(const QPoint &)
{
    //STDLINE("Must plot now!",ACRed);
    theHNavigator_ = theMainWindow_->getHNavigator() ;

    // Create suitable canvas (a singleton)
    if( serviceCanvas_.find(currentCanvas_) == serviceCanvas_.end() )
    {
        serviceCanvas_[currentCanvas_] = new CanvasWidget(parent_) ;
        ss_.str(""); ss_ << "Current canvas: " << currentCanvas_ ;
        serviceCanvas_[currentCanvas_]->setTitle(ss_.str()) ;
        connect( serviceCanvas_[currentCanvas_], SIGNAL(destroyed()    ),
                 this                          , SLOT  (cleanDestroy())) ;
    }
    else
    {
        serviceCanvas_[currentCanvas_]->clear() ;
    }

    // Add the canvas to the main window
    if( cSw_.find(currentCanvas_) == cSw_.end())
    {
        cSw_[currentCanvas_] = theMainWindow_->getMdiArea()->addSubWindow(serviceCanvas_[currentCanvas_]) ;
        cSw_[currentCanvas_]->setGeometry(canvasPosX_,canvasPosY_,canvasWitdh_,canvasHeight_) ;
        canvasPosX_   += 5;
        canvasPosY_   += 5;
    }

    // Determine name -> full-path association for the selected items
    selectedObjectsDef selectedObjects = this->getSelectedItems() ;

    int numberOfPlots = 0 ;
    for (selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
    {
        for(tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
        {
            if( (*jt)->IsA() == TFolder::Class()        ) continue ;
            if( (*jt)->IsA() == TDirectory::Class()     ) continue ;
            if( (*jt)->IsA() == TDirectoryFile::Class() ) continue ;
            if( (*jt)->IsA() == TCanvas::Class()        ) continue ;
            numberOfPlots++ ;
        }
    }
    if(numberOfPlots == 0) return;

    if( theHNavigator_->plotStatBox() )
        gStyle->SetOptStat(111111) ;
    else
        gStyle->SetOptStat(0) ;

    if( theHNavigator_->plotFitBox() )
    {
        gStyle->SetOptFit(111111) ;
        STDLINE("OptFit",ACRed);
    }
    else
        gStyle->SetOptFit(0) ;

    int nx = 1;
    int ny = 1;
    if(theHNavigator_->canvasSplitType() == "X")
        nx=numberOfPlots;
    else if(theHNavigator_->canvasSplitType() == "Y")
        ny=numberOfPlots;
    else if(theHNavigator_->canvasSplitType() == "XY")
    {
        if     ( numberOfPlots ==  1 ) {nx=1;ny=1;}
        else if( numberOfPlots ==  2 ) {nx=2;ny=1;}
        else if( numberOfPlots ==  3 ) {nx=3;ny=1;}
        else if( numberOfPlots ==  4 ) {nx=2;ny=2;}
        else if( numberOfPlots ==  5 ) {nx=3;ny=2;}
        else if( numberOfPlots ==  6 ) {nx=3;ny=2;}
        else if( numberOfPlots ==  7 ) {nx=3;ny=3;}
        else if( numberOfPlots ==  8 ) {nx=3;ny=3;}
        else if( numberOfPlots ==  9 ) {nx=3;ny=3;}
        else if( numberOfPlots == 10 ) {nx=4;ny=3;}
        else if( numberOfPlots == 11 ) {nx=4;ny=3;}
        else if( numberOfPlots == 12 ) {nx=4;ny=3;}
        else if( numberOfPlots == 13 ) {nx=4;ny=4;}
        else if( numberOfPlots == 14 ) {nx=4;ny=4;}
        else if( numberOfPlots == 15 ) {nx=4;ny=4;}
        else if( numberOfPlots == 16 ) {nx=4;ny=4;}
        else
        {
            ny = ceil(sqrt(numberOfPlots));
            nx = ny-1;
            if(nx*ny<numberOfPlots)
                ++nx;
        }
    }
    serviceCanvas_[currentCanvas_]->divide(nx, ny) ;
    serviceCanvas_[currentCanvas_]->setLogX(theHNavigator_->logX());
    serviceCanvas_[currentCanvas_]->setLogY(theHNavigator_->logY());
    serviceCanvas_[currentCanvas_]->setLogZ(theHNavigator_->logZ());


    std::string options = "" ;

    int pad = 1 ;
    for (selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
    {
        for(tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
        {
            if( (*jt)->IsA() == TDirectory::Class()     ) continue ;
            if( (*jt)->IsA() == TFolder::Class()        ) continue ;
            if( (*jt)->IsA() == TDirectoryFile::Class() ) continue ;
            if( (*jt)->IsA() == TCanvas::Class() )
            {
                (*jt)->Draw() ;
                continue ;
            }
            serviceCanvas_[currentCanvas_]->cd(pad++) ;
            if(this->getObjectType(*jt).find("TH1") != std::string::npos)
            {
                (*jt)->Draw() ;
 //               if (currentObject_) delete currentObject_;
                currentObject_ = (*jt);
            }
            else if(this->getObjectType(*jt).find("TH2") != std::string::npos)
            {
                options += theHNavigator_->twoDOption() ;
                (*jt)->Draw(options.c_str()) ;
//                if (currentObject_) delete currentObject_;
                currentObject_ = (*jt);
            }
            else if(this->getObjectType(*jt).find("TGraph") != std::string::npos)
            {
                options += "ACL" ;
                (*jt)->Draw(options.c_str()) ;
//                if (currentObject_) delete currentObject_;
                currentObject_ = (*jt);
            }
            else
            {
                STDLINE(std::string("Don't know how to plot object of type: ") + this->getObjectType(*jt),ACRed);
            }
        }
    }
    serviceCanvas_[currentCanvas_]->flush() ;

    cSw_[currentCanvas_]->show() ;
    cSw_[currentCanvas_]->raise() ;
}

//===========================================================================
void hTreeBrowser::resizeEvent(QResizeEvent * )
{
}

//===========================================================================
void hTreeBrowser::unZoom()
{
    if(serviceCanvas_.find(currentCanvas_) == serviceCanvas_.end()) return;
    // Determine name -> full-path association for the selected items
    selectedObjectsDef selectedObjects = this->getSelectedItems() ;

    for (selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
    {
        for(tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
        {
            if(      (*jt)->IsA() == TDirectory    ::Class() ) continue ;
            else if( (*jt)->IsA() == TFolder       ::Class() ) continue ;
            else if( (*jt)->IsA() == TDirectoryFile::Class() ) continue ;
            else if( (*jt)->IsA() == TCanvas       ::Class() ) continue ;
            else if(    this->getObjectType(*jt).find("TH1") != std::string::npos
                     || this->getObjectType(*jt).find("TH2") != std::string::npos )
            {
                ((TH1*)(*jt))->GetXaxis()->UnZoom() ;
                ((TH1*)(*jt))->GetYaxis()->UnZoom() ;
                ((TH1*)(*jt))->GetZaxis()->UnZoom() ;
            }
            else if( this->getObjectType(*jt) == "TGraph" )
            {
                ((TGraph*)(*jt))->GetXaxis()->UnZoom() ;
                ((TGraph*)(*jt))->GetYaxis()->UnZoom() ;
            }
        }
    }
    serviceCanvas_[currentCanvas_]->flush() ;
}

//===========================================================================
void hTreeBrowser::setRange(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
{
    if(serviceCanvas_.find(currentCanvas_) == serviceCanvas_.end()) return;
    selectedObjectsDef selectedObjects = this->getSelectedItems() ;

    for (selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
    {
        for(tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
        {
            if(      (*jt)->IsA() == TDirectory    ::Class() ) continue ;
            else if( (*jt)->IsA() == TFolder       ::Class() ) continue ;
            else if( (*jt)->IsA() == TDirectoryFile::Class() ) continue ;
            else if( (*jt)->IsA() == TCanvas       ::Class() ) continue ;
            else if(    this->getObjectType(*jt).find("TH1") != std::string::npos
                     || this->getObjectType(*jt).find("TH2") != std::string::npos )
            {
                if(xMin < xMax)
                    ((TH1*)(*jt))->GetXaxis()->SetRangeUser(xMin,xMax);
                if(yMin < yMax)
                    ((TH1*)(*jt))->GetYaxis()->SetRangeUser(yMin,yMax);
                if(zMin < zMax)
                    ((TH1*)(*jt))->GetZaxis()->SetRangeUser(zMin,zMax);
            }
            else if( this->getObjectType(*jt) == "TGraph" )
            {
                STDLINE((*jt),ACPurple);
                if(xMin < xMax)
                    ((TGraph*)(*jt))->GetXaxis()->SetRangeUser(xMin,xMax);
                if(yMin < yMax)
                    ((TGraph*)(*jt))->GetYaxis()->SetRangeUser(yMin,yMax);
            }
        }
    }
    serviceCanvas_[currentCanvas_]->flush() ;
}

//===========================================================================
hTreeBrowser::selectedObjectsDef hTreeBrowser::getSelectedItems()
{
    //STDLINE("hTreeBrowser::getSelectedItems()",ACRed);
    hTreeBrowser::selectedObjectsDef selectedObjects ;
    QList<QTreeWidgetItem *> selectedItems = this->selectedItems() ;

    for (int item = 0; item < selectedItems.size(); ++item)
        this->collectItems(selectedItems[item],selectedObjects) ;

    return selectedObjects ;
}

//===========================================================================
void hTreeBrowser::collectItems(QTreeWidgetItem                  * wItem,
                                hTreeBrowser::selectedObjectsDef & selectedObjects)
{
    //    STDLINE("hTreeBrowser::collectItems()",ACRed);
    hTreeBrowser::dirPathDef dirPath = this->getFullPath(wItem) ;
    if(dirPath.size() < 2) return;

    std::string dir = "";
    for(unsigned int i=1; i<dirPath.size()-1; i++)
    {
        dir += dirPath[i];
        if( i != dirPath.size()-2) dir += "/" ;
    }

    if( dirPath[0] == "root") // Selected object is in memory only
    {
        TFolder* targetFolder = (TFolder *)(gROOT->GetRootFolder()->FindObjectAny(dir.c_str())) ;
        TObject* obj          = targetFolder->FindObject(wItem->text(0).toStdString().c_str()) ;
        selectedObjects[dir].push_back(obj) ;
    }
    else // Selected object resides on files
    {
        //        TFile * file  = theHManager_->getFileHandle(dirPath[0]) ;
        TFile*   file = file_ ;
        TObject* obj  = file->FindObjectAny(dirPath[1].c_str()) ;

        if( obj->IsA() == TFolder::Class() )
        {
            for(hTreeBrowser::dirPathDef::iterator ni=dirPath.begin()+2; ni!=dirPath.end(); ++ni)
            {
                if( ni == dirPath.end() - 1 ) // Last is the histogram name
                {
                    selectedObjects[dir].push_back(((TFolder*)obj)->FindObjectAny((*ni).c_str())) ;
                }
                else
                {
                    obj = ((TFolder*)obj)->FindObjectAny((*ni).c_str()) ;
                }
            }
        }
        else// if(obj->IsA() == TDirectoryFile::Class())
        {
            file->cd(dir.c_str()) ;
            //STDLINE("Reading obj",ACGreen);
            //            selectedObjects[dir].push_back(gDirectory->GetKey(wItem->text(0).toStdString().c_str())->ReadObj());
            selectedObjects[dir].push_back(gDirectory->Get(wItem->text(0).toStdString().c_str()));
            //STDLINE(gDirectory->Get(wItem->text(0).toStdString().c_str()),ACGreen);
            //          //            STDLINE("done Reading dir",ACRed);

            if(selectedObjects[dir].back()->IsA() == TDirectoryFile::Class())
            {
                for(int i=0; i<wItem->childCount(); ++i)
                {
                    this->collectItems(wItem->child(i),selectedObjects) ;
                }
            }
        }
        //else
        //{
        //    STDLINE(obj->IsA()->GetImplFileName(),ACYellow);
        //}
    }
}

//===========================================================================
void hTreeBrowser::manipulateFolder( QTreeWidgetItem * wItem, int )
{
    //    STDLINE("hTreeBrowser::manipulateFolder()",ACRed);
    /*
   Selection strategy for the current item:
   1) if it is a leaf just add it to the list of already selected items
   2) if it is a folder, deselect any explicitly selected child but add the
      folder to the list of already selected items.
   */

    if( wItem->childCount() == 0 )
    {
        wItem->setSelected(true);
    }
    else
    {
        this->selectAllChildren(wItem, wItem->isSelected()) ;
    }
}


//===========================================================================
//void hTreeBrowser::showHint( QTreeWidgetItem * wItem, int )
//{
    //STDLINE("This method is doing nothing",ACRed);
    //if( wItem->childCount() > 0 )
    //{
        //        STDLINE(wItem->text(0).toStdString(),ACYellow) ;
    //}
    //else
    //{
        //        STDLINE(wItem->text(0).toStdString(),ACWhite) ;
    //}
//}

//===========================================================================
void hTreeBrowser::selectAllChildren(QTreeWidgetItem * wItem, bool select)
{
    //    STDLINE("selectAllChildren",ACRed);
    for(int i=0; i < wItem->childCount(); ++i)
    {
        wItem->child(i)->setSelected(select);
        this->selectAllChildren(wItem->child(i), select);
    }
}

//===========================================================================
void hTreeBrowser::getAllItems(QTreeWidgetItem * )
{
}

//===========================================================================
void hTreeBrowser::setCanvasSize(std::string canvasSize)
{
    if(serviceCanvas_.find(currentCanvas_) == serviceCanvas_.end()) return;
    //    STDLINE("",ACRed);
    if(      canvasSize == std::string("Small" ) )
    {
        canvasWitdh_  = 400;
        canvasHeight_ = 300;
    }
    else if( canvasSize == std::string("Medium") )
    {
        canvasWitdh_  = 600;
        canvasHeight_ = 450;
    }
    else if( canvasSize == std::string("Large") )
    {
        canvasWitdh_  = 800;
        canvasHeight_ = 600;
    }
    else if( canvasSize == std::string("Huge") )
    {
        canvasWitdh_  = 1200;
        canvasHeight_ = 800;
    }

    if( cSw_.find(currentCanvas_) != cSw_.end())
    {
        cSw_[currentCanvas_]->setGeometry(canvasPosX_,canvasPosY_,canvasWitdh_,canvasHeight_) ;
        serviceCanvas_[currentCanvas_]->update() ;
    }

}
