#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "qfichier.h"
#include <QDir>
#include <QCloseEvent>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    //ACTIONS
    bool on_actionNouveau_triggered();
    void on_actionQuitter_triggered();
    void on_actionCharger_un_EDL_triggered();
    void on_actionAjouter_une_couche_triggered();
    //BOUTONS
    void on_browseButton_clicked();
    void on_applyOffsetButton_clicked();
    void on_fpsBox_currentIndexChanged(int index);
    void on_edlNextButton_clicked();
    void on_previousLayersButton_clicked();
    void on_nextLayersButton_clicked();
    void on_previousExportButton_clicked();
    void on_addLayerButton_clicked();
    void on_upButton_clicked();
    void on_downButton_clicked();
    void on_editLayerButton_clicked();
    void on_disableLayerButton_clicked();
    void on_removeLayerButton_clicked();
    void on_exportBrowseButton_clicked();
    void on_exportButton_clicked();
    void on_resolutionBox_currentIndexChanged(int index);
    void on_actionPr_f_rences_triggered();
    void on_widthBox_valueChanged(int arg1);
    void on_heightBox_valueChanged(int arg1);
    void on_detectOffsetButton_clicked();
    void on_nameColumnBox_activated(int index);
    bool on_actionEnregistrer_sous_triggered();
    bool on_actionEnregistrer_triggered();
    void on_actionOuvrir_triggered();
    void closeEvent(QCloseEvent *event);
    void on_aeButton_clicked(bool checked);
    void on_qtButton_clicked(bool checked);
    void on_acpButton_clicked(bool checked);
    void on_edlButton_clicked(bool checked);
    void on_qtCounterBox_clicked(bool checked);
    void on_qtWidthSpinner_valueChanged(int arg1);
    void on_qtHeightSpinner_valueChanged(int arg1);
    void on_actionAbout_Qt_triggered();
    void on_actionAbout_triggered();
    void on_actionAvid_Caption_Plugin_triggered(bool checked);
    void on_actionAfter_Effects_triggered(bool checked);
    void on_actionQuicktime_text_triggered(bool checked);
    void on_actionEdit_Decision_List_EDL_triggered(bool checked);
    void on_layersList_cellDoubleClicked(int row, int column);
    void on_layersList_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_fusionButton_clicked();
    void on_actionFusionner_un_fichier_de_Timecodes_triggered();
    void on_removeClipButton_clicked();
    void on_newAEPBox_toggled(bool checked);
    void on_applyOffsetSButton_clicked();
    void on_shotAEPBox_toggled(bool checked);

private:
    int findType(QFichier *testFile); //trouve à quel type de timecode on a affaire
    //0 = Inconnu
    //1 = EDL
    //2 = QuickTime text
    //3 = Avid Caption Plugin
    QList<QStringList> loadFile(QFichier *f, int t); //charge le contenu d'un fichier
    void display(QList<QStringList> c, bool replace = true); //affiche les timecodes dans le tableau
    QFichier *edlFile; //le fichier de timecodes chargé
    QList<QTableWidgetItem*> takeLayersRow(int row);
    void setLayersRow(int row, const QList<QTableWidgetItem*>& rowItems);
    void updateLayers();
    QString findMedia(QString mediaName,QString type,QString path,bool recursive,QString fileName,bool seq);
    QString searchSourceFile(bool r,QDir sf,QStringList f);
    float ratio; //résolution, à garder de coté pour connaitre les proportions (bouton garder les proportions)
    float ratioQT; //taille bloc qt, à garder de coté pour connaitre les proportions (bouton garder les proportions)
    bool progra;//pour bloquer les signaux quand initié programmatiquement
    QString saveFile; //fichier où est enregistré le projet
    bool save(); //enregistre le projet courant dans saveFile
    void enableExportButton();
    void exportToAE(); //export after effects
    void exportToQT();//export quicktime text
    void exportToEDL();//export edl
    void exportToACP();//export acp
    void offset(int os, bool all = true, bool sources = false, QList<int> rows = QList<int>());//offset les timecodes
};

#endif // MAINWINDOW_H
