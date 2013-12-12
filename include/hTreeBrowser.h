/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef HTREEBROWSER_H
#define HTREEBROWSER_H

#include "map"
#include <sstream>
#include <vector>

#include <QIcon>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <TDirectory.h>
#include <TFolder.h>
#include <TFile.h>

#include "hnavigator.h"
#include "MessageTools.h"

class MainWindow   ;
class CanvasWidget ;
class QMdiSubWindow;
class HManager     ;
class HNavigator   ;
class TObject      ;

class hTreeBrowser : public QTreeWidget
{
    Q_OBJECT

public:
    typedef std::vector<TObject*                 > tFileVDef          ;
    typedef std::map<std::string, tFileVDef      > selectedObjectsDef ;
    typedef std::map<int,         CanvasWidget * > canvasWidgetDef    ;

    hTreeBrowser    (QWidget         * parent,
                     MainWindow      * mainWindow      ) ;
    ~hTreeBrowser    (void                              ) ;

    void               populate        (TDirectory      * currentDirectory) ;
    void               populate        (TFolder         * currentFolder   ) ;
    void               populate        (TDirectory      * currentDirectory,
                                        QTreeWidgetItem * wItem           ) ;
    void               populate        (TFolder         * currentFolder,
                                        QTreeWidgetItem * wItem           ) ;
    selectedObjectsDef getSelectedItems(void                              ) ;
    void               setCurrentCanvas(int               currentCanvas   ) {currentCanvas_ = currentCanvas;}
    void               setCanvasSize   (std::string       canvasSize      ) ;
    int                getCurrentCanvas(void                              ) {return currentCanvas_         ;}
    canvasWidgetDef    getCanvases     (void                              ) {return serviceCanvas_         ;}
    void               unZoom          (void                              ) ;
    void               setRange        (float xMin = 0, float xMax = 0
                                       ,float yMin = 0, float yMax = 0
                                       ,float zMin = 0, float zMax = 0) ;

    void               setCurrentFile  (TFile           * currentFile     ) {file_ = currentFile           ;}
    void               clear           (void                              ) ;

    TObject * getCurrentObject () {return currentObject_;} //only for fitting in hnavigator
    const char * getFileName () {return file_->GetName();}
    TFile * getFile () {return file_;}

public slots:
    void       showContextMenu  (const QPoint                           & pos            ) ;

protected:

private slots:

    void       cleanDestroy     (void                                                    ) ;
    void       resizeEvent      (      QResizeEvent                     *                ) ;
    void       manipulateFolder (      QTreeWidgetItem                  *,
                                       int                                               ) ;
//    void       showHint         (      QTreeWidgetItem                  *,
//                                       int                                               ) ;
private:
    typedef std::vector<std::string>                dirPathDef         ;
    typedef std::map<int,         QMdiSubWindow  *> cSwDef             ;
    typedef std::map<std::string, QTreeWidgetItem*> existingWItemsDef  ;

    dirPathDef  getFullPath      (      QTreeWidgetItem                  * wItem          ) ;
    void        collectItems     (      QTreeWidgetItem                  * wItem,
                                        hTreeBrowser::selectedObjectsDef & selectedObjects) ;
    void        getAllItems      (      QTreeWidgetItem                  * wItem          ) ;
    void        selectAllChildren(      QTreeWidgetItem                  * wItem,
                                        bool                                              ) ;
    std::string getObjectType    (      TObject                          * obj            ) ;

    std::stringstream   ss_               ;


    QWidget           * parent_           ;
    MainWindow        * theMainWindow_    ;
    int                 currentCanvas_    ;
    canvasWidgetDef     serviceCanvas_    ;
    cSwDef              cSw_              ;
    HManager          * theHManager_      ;
    HNavigator        * theHNavigator_    ;
    existingWItemsDef   existingWItems_   ;

    QIcon               canvasIcon_       ;
    QIcon               folderIcon_       ;
    QIcon               fileIcon_         ;
    QIcon               mainIcon_         ;
    QIcon               TH1Icon_          ;
    QIcon               TH2Icon_          ;
    QIcon               tTreeIcon_        ;
    QIcon               tGraphIcon_       ;
    QIcon               tProfileIcon_     ;
    dirPathDef          dirPath_          ;

    TFile             * file_             ;

    int                 canvasPosX_       ;
    int                 canvasPosY_       ;
    int                 canvasWitdh_      ;
    int                 canvasHeight_     ;

    TObject * currentObject_;
};

#endif // HTREEBROWSER_H
