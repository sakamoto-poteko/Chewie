#ifndef ANALYZERDLG_H
#define ANALYZERDLG_H

#include <QDialog>
#include <QString>
#include <QFuture>
#include <QFutureWatcher>
#include <QListWidgetItem>

#include <sstream>
#include <string>

class EventReader        ;
class EventManager       ;
class AnalysisManager    ;
class HistogramWindow    ;
class XmlParser          ;
class TFile              ;

namespace Ui {
class AnalyzerDlg;
}

class AnalyzerDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AnalyzerDlg (QWidget *parent = 0);
            ~AnalyzerDlg (void               );

    std::map<std::string,TFile*> getOpenFiles (void        );

    AnalysisManager * getAnalysisManager () {return theAnalysisManager_;}

    static QString      getEnvPath      (QString environmentName);

private:
    void         setInnerGeometry(void                   );
    void         openGeoFile     (QString fileName       );
    void         initXml         (void                   );
    static void* threaderMethod  (void* myClass,
                                  void* whatToRun,
                                  int   arg0=0           );


    void         setPrioritiesFromUi ();

    Ui::AnalyzerDlg             *ui                     ;
    QTimer                      *theTimer_              ;
    QFuture<void*>              *theFuture_             ;
    QFutureWatcher<void>        *theWatcher_            ;
    XmlParser                   *theXmlParser_          ;
    EventManager                *theEventManager_       ;
    AnalysisManager             *theAnalysisManager_    ;
    QString                      whatToRun_             ;
    std::map<std::string,TFile*> openFileList_          ;

private slots:
    void on_calibrationLoadGeoFilePB_clicked   (void                 );
    void on_openCalibrationsFilePB_clicked     (void                 );
    void on_openRootFilePB_clicked             (void                 );
    void on_openNtupleFilePB_clicked           (void                 );
    void on_runAnalysisPB_clicked              (void                 );
    void on_saveOutputFilePB_clicked           (void                 );
    void displayFinishedBox                    (void                 );
    void advanceProgressBar                    (void                 );
    void updateFileName                        (void                 );
    void on_abortPB_clicked                    (void                 );
    void on_calibParsCB_stateChanged           (int              arg1);
    void on_calibDispersionCB_stateChanged     (int              arg1);
    void on_dutsListLW_2_itemSelectionChanged  (void                 );
    void on_makeCalibPlotsPB_clicked           (void                 );
    void on_openCalibrationPlotsPB_clicked     (void                 );
    void on_runConverterPB_clicked             (void                 );
    void on_openConfigurationFilePB_clicked    (void                 );
    void on_saveConfigurationFile_clicked      (void                 );
    void on_useCalibrationsCB_stateChanged     (int              arg1);
    void on_resetRemovedPxlsDUT1PB_clicked     (void                 );
    void on_resetRemovedPxlsDUT0PB_clicked     (void                 );
    void on_removePxlDUT1PB_clicked            (void                 );
    void on_removePxlDUT0PB_clicked            (void                 );
    void on_loadFileToScanListPB_clicked       (void                 );
    void on_scanListTW_cellChanged             (int row, int column  );
    void on_resetScanListPB_clicked            (void                 );
    void on_runScanPB_clicked                  (void                 );

    //////////cuts//////////////////////////////////////////////////
    void on_chi2EfficiencyCutCB_stateChanged   (int              arg1);
    void on_nTrackEfficiencyCutCB_stateChanged (int              arg1);
    void on_xErrEfficiencyCutCB_stateChanged   (int              arg1);
    void on_yErrEfficiencyCutCB_stateChanged   (int              arg1);
    void on_xEfficiencyCutCB_stateChanged      (int              arg1);
    void on_yEfficiencyCutCB_stateChanged      (int              arg1);
    void on_hitsEfficiencyCutSB_valueChanged   (const QString&   arg1);

    void on_chi2ChargeCutCB_stateChanged       (int              arg1);
    void on_nTrackChargeCutCB_stateChanged     (int              arg1);
    void on_xErrChargeCutCB_stateChanged       (int              arg1);
    void on_yErrChargeCutCB_stateChanged       (int              arg1);
    void on_xChargeCutCB_stateChanged          (int              arg1);
    void on_yChargeCutCB_stateChanged          (int              arg1);
    void on_hitsChargeCutSB_valueChanged       (const QString&   arg1);

    void on_chi2ResolutionCutCB_stateChanged   (int              arg1);
    void on_nTrackResolutionCutCB_stateChanged (int              arg1);
    void on_xErrResolutionCutCB_stateChanged   (int              arg1);
    void on_yErrResolutionCutCB_stateChanged   (int              arg1);
    void on_hitsResolutionCutSB_valueChanged   (const QString&   arg1);
    void on_resetListPB_clicked();
    void on_convertResetListPB_clicked();
};

#endif // ANALYZERDLG_H
