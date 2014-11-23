#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "editlayer.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include "preferences.h"
#include <QDesktopServices>
#include "dufsqlquery.h"
#include <QDateTime>
#include "about.h"
#include "addclip.h"
#include "timecodemanager.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ratio = 16.0/9.0;
    ratioQT = 10;
    progra = true;
    setupUi(this);
    tabs->setCurrentIndex(0);
    exportTabs->setCurrentIndex(0);
    edlFile = new QFichier("");
    progra = false;
    saveFile = "";
}

bool MainWindow::on_actionNouveau_triggered()
{
    int rep = QMessageBox::question(this,"Nouveau Projet","Voulez-vous enregistrer le projet en cours ?",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (rep == QMessageBox::Yes) on_actionEnregistrer_triggered();
    else if (rep != QMessageBox::No) return false;
    saveFile = "";
    this->setWindowTitle("Dubab");
    tabs->setCurrentIndex(0);
    edlList->clearContents();
    edlList->setRowCount(0);
    editLayerButton->setEnabled(false);
    disableLayerButton->setEnabled(false);
    removeLayerButton->setEnabled(false);
    layersList->clearContents();
    layersList->setRowCount(0);
    exportNameEdit->setText("");
    exportNameEdit_2->setText("");
    durationBox->clear();
    durationBox->addItem("Timecodes");
    resolutionBox->clear();
    resolutionBox->addItem("Personnalisée");
    soundBox->clear();
    soundBox->addItem("Pas de son");
    bitcNameBox->setChecked(true);
    bitcShotsTCBox->setChecked(true);
    bitcTCBox->setChecked(false);
    renderBox->setChecked(true);
    aeButton->setChecked(false);
    tabAE->setEnabled(false);
    qtButton->setChecked(false);
    tabQT->setEnabled(false);
    edlButton->setChecked(false);
    acpButton->setChecked(false);
    precomposeButton->setChecked(true);
    qtWidthSpinner->setValue(200);
    qtHeightSpinner->setValue(20);
    proportionsButton_2->setChecked(true);
    qtTextSizeSpinner->setValue(12);
    qtCounterBox->setChecked(false);
    qtCounterBox1->setChecked(true);
    enableExportButton();
    exportTabs->setCurrentIndex(0);
    return true;
}

void MainWindow::on_actionQuitter_triggered()
{
    this->close();
}

void MainWindow::on_browseButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Open timecode file", QString(), "All Files (*.*);;Text Files (Avid Caption or Quicktime) (*.txt);;EDL (*.edl);;Dutico (*.dtc);;JSON (*.js)");
    if (file == "") return;
    edlFile->setFileName(file);
    QString fName = edlFile->fileName();
    exportNameEdit_2->setText(fName.section("/",-1).section(".",0,-2));
    int type = findType(edlFile);
    if (type == 0)
    {
        QMessageBox::warning(this,"Type de fichier inconnu","Le type de fichier à importer n'a pas été reconnu.\nCette version de Dutico prend en charge les fichiers de type suivant :\n\nEDL\nAvid Caption Plugin\nQuicktime text\nDutico DTC");
        return;
    }
    QList<QStringList> contenu;
    if (edlFile->exists())
    {
        contenu = loadFile(edlFile,type);
        display(contenu);
    }
}

void MainWindow::on_fusionButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Open timecode file", QString(), "All Files (*.*);;Text Files (Avid Caption or Quicktime) (*.txt);;EDL (*.edl);;Dutico (*.dtc);;JSON (*.js)");
    if (file == "") return;
    edlFile->setFileName(file);
    QString fName = edlFile->fileName();
    exportNameEdit_2->setText(fName.section("/",-1).section(".",0,-2));
    int type = findType(edlFile);
    if (type == 0)
    {
        QMessageBox::warning(this,"Type de fichier inconnu","Le type de fichier à importer n'a pas été reconnu.\nCette version de Dutico prend en charge les fichiers de type suivant :\n\nEDL\nAvid Caption Plugin\nQuicktime text");
        return;
    }
    QList<QStringList> contenu;
    if (edlFile->exists())
    {
        contenu = loadFile(edlFile,type);
        display(contenu,false);
    }
}

void MainWindow::on_removeClipButton_clicked()
{
    if (edlList->selectionModel()->selectedRows().count() == 0) return;
    if (QMessageBox::question(this,"Suppression de clips","Êtes vous sûrs de vouloir supprimer les clips sélectionnés ?") != QMessageBox::Yes) return;
    for(int i = edlList->selectionModel()->selectedRows().count()-1;i>=0;i--)
    {
        int row = edlList->selectionModel()->selectedRows()[i].row();
        edlList->removeRow(row);
    }
}

void MainWindow::on_actionCharger_un_EDL_triggered()
{
    on_browseButton_clicked();
}

int MainWindow::findType(QFichier *testFile)
{
    int r = 0;
    QString ligne1 = testFile->getLine(0);
    //0 = Inconnu
    //1 = EDL
    //2 = QuickTime text
    //3 = Avid CP
    //4 = DTC
    if (ligne1.startsWith("TITLE:"))
    {
        r = 1;
    }
    else if (ligne1.toLower().startsWith("{qttext}"))
    {
        r = 2;
    }
    else if (ligne1.startsWith("@"))
    {
        r = 3;
    }
    else
    {
        testFile->open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(testFile);
        QString testFileS = in.readAll();
        testFile->close();
        QJsonDocument jstest = QJsonDocument::fromJson(testFileS.toStdString().c_str());
        if (jstest.object().contains("clips")) r = 4;
    }

    if (r == 0 && testFile->fileName().toLower().endsWith(".edl"))
    {
        r = 1;
    }

    return r;
}

QList<QStringList> MainWindow::loadFile(QFichier *f, int t)
{

    QList<QStringList> tc;

    QStringList contenu = f->getLines();

    //si Avid Caption Plugin
    if (t == 3)
    {
        bool dedans = false;
        //regexp pour timecode
        QRegularExpression re("^\\d\\d:\\d\\d:\\d\\d:\\d\\d \\d\\d:\\d\\d:\\d\\d:\\d\\d");
        for (int i = 0;i < contenu.count();i++) //pour chaque ligne du fichier
        {
            QString ligne = contenu[i];

            if (ligne.toLower() == "<begin subtitles>") dedans = true; //si on est après la balise
            if (ligne.toLower() == "<end subtitles>") break; //si on est avant
            if (dedans)
            {
                //si on est sur une ligne de timecode
                if (re.match(ligne).hasMatch())
                {
                    QString TCIn = ligne.left(11);
                    QString TCOut = ligne.mid(12,11);
                    QString name = contenu[i+1];
                    QStringList l;
                    l << name << TCIn << TCOut << "" << "" << "" << "" << "placeholder";
                    tc << l;
                }
            }
        }
    }

    //si EDL
    else if (t == 1)
    {
        qDebug() << "edl";
        //regexp pour le numéro de ligne
        QRegularExpression rel("^\\d\\d\\d");
        //le format
        QString format = "";

        for (int i = 0;i < contenu.count();i++) //pour chaque ligne du fichier
        {
            //découper selon les espaces
            QString ligne = contenu[i];
            qDebug() << ligne;
            QStringList blocs = ligne.split(" ",QString::SkipEmptyParts);

            if (blocs.count() == 0) continue; //si ligne vide

            QString name = "";
            QString TCIn = "";
            QString TCOut = "";
            QString comment = "";
            QString effect = "";
            QString sourceTCIn = "";
            QString sourceTCOut = "";

            //si ligne de titre
            if (blocs[0].toUpper() == "TITLE:" && format == "")
            {
                qDebug() << "TITLE:";
                //chercher le bloc "FORMAT:"
                for(int j = 1;j<blocs.count();j++)
                {
                    if (blocs[j].toUpper() == "FORMAT:")
                    {
                        format = blocs[j+1].toUpper();
                        if (format != "SMPTE" && format != "CMX3600") QMessageBox::information(this,"EDL Format","Please be aware that only these formats have been tested:\n- SMPTE\n- CMX3600\n\nYou may want to export EDL from your editing software in one of these format.") ;
                        break;
                    }
                }
            }
            //si ligne de format
            if (blocs[0].toUpper() == "FORMAT:" && format == "")
            {
                format = blocs[1].toUpper();
                if (format != "SMPTE" && format != "CMX3600") QMessageBox::information(this,"EDL Format","Please be aware that only these formats have been tested:\n- SMPTE\n- CMX3600\n\nYou may want to export EDL from your editing software in one of these format.") ;
                break;
            }
            //si ligne de timecodes video
            if (blocs.count() > 7)
            {
                if (rel.match(blocs[0]).hasMatch() && (blocs[2].contains("V",Qt::CaseInsensitive) || blocs[2].contains("B",Qt::CaseInsensitive)))
                {
                    if (blocs[3] == "CUT" || blocs[3] == "C")
                    {
                        TCIn = blocs[6];
                        TCOut = blocs[7];
                        sourceTCIn = blocs[4];
                        sourceTCOut = blocs[5];
                        name = blocs[1];
                    }
                    else if (blocs[3] == "DIS" || blocs[3] == "D")
                    {
                        TCIn = blocs[7];
                        TCOut = blocs[8];
                        sourceTCIn = blocs[5];
                        sourceTCOut = blocs[6];
                        name = blocs[1];
                        effect = "DISSOLVE | " + blocs[4];
                    }
                    //on fait les lignes suivantes jusqu'à la ligne de timecodes suivante (ou fin de fichiers)
                    //pour les autres infos (comments, effets,...)
                    for (int j = i+1; j < contenu.count(); j++)
                    {
                        QString sousLigne = contenu[j];
                        QStringList sousBlocs = sousLigne.split(" ",QString::SkipEmptyParts);
                        //si ligne vide
                        if (sousBlocs.count() == 0)
                        {
                            continue;
                        }
                        //si on a atteint les timecodes suivant
                        if (rel.match(sousBlocs[0]).hasMatch()) break;
                        //si ligne de commentaires
                        if (sousBlocs[0].toUpper() == "COMMENT:")
                        {
                            //virer le premier
                            sousBlocs.removeFirst();
                            comment == "" ? comment = sousBlocs.join(" ") : comment += "\n" + sousBlocs.join(" ");
                        }
                    }
                    QStringList l;
                    l << name << TCIn << TCOut << effect << comment << sourceTCIn << sourceTCOut << "placeholder";
                    tc << l;
                }
            }
        }
    }

    //si Quicktime
    else if (t == 2)
    {
        //regexp pour timecode
        QRegularExpression re("^\\[\\d\\d:\\d\\d:\\d\\d\\.\\d\\d\\]");
        //il faut commencer par regarder si les TC se terminent en frames ou en fraction de seconde...
        //(on sait pas trop avec les QTText)
        bool isFrames = true;
        //d'abord chercher si on a une info de fps
        for (int i = 0;i < contenu.count();i++) //pour chaque ligne du fichier
        {
            QString ligne = contenu[i];
            //si fps
            if (ligne.toLower().contains("timescale:"))
            {
                QString fpsS = ligne.toLower().section("timescale:",-1);
                fpsS = fpsS.left(fpsS.indexOf("}"));
                if (fpsS.toDouble() != 0)
                {
                    fps->setValue(fpsS.toDouble());
                    fpsBox->setCurrentIndex(0);
                }
                break;
            }
        }

        //puis regarder dans tous les timecodes si on en a plus grand que le fps demandé
        for (int i = 0;i < contenu.count();i++) //pour chaque ligne du fichier
        {
            QString ligne = contenu[i];
            if (re.match(ligne).hasMatch())
            {
                QString fin = ligne.mid(1,11).replace(".",":").section(":",-1);
                if (fin.toDouble()>fps->value())
                {
                    isFrames = false;
                    break;
                }
            }
        }

        //et maintenant qu'on sait, on charge
        for (int i = 0;i < contenu.count();i++) //pour chaque ligne du fichier
        {
            QString ligne = contenu[i];
            //si fps
            if (ligne.toLower().contains("timescale:"))
            {
                QString fpsS = ligne.toLower().section("timescale:",-1);
                fpsS = fpsS.left(fpsS.indexOf("}"));
                if (fpsS.toDouble() != 0)
                {
                    fps->setValue(fpsS.toDouble());
                    fpsBox->setCurrentIndex(0);
                }
            }
            //sinon si ligne de timecode
            if (re.match(ligne).hasMatch())
            {
                QString TCIn = ligne.mid(1,11);
                //si on termine pas par des frames, convertir en frames
                if (!isFrames)
                {
                    int frames = TCIn.section(":",0,0).toInt()*3600*fps->value() + TCIn.section(":",1,1).toInt()*60*fps->value() + TCIn.section(":",2).toDouble()*fps->value();
                    TCIn = TimeCodeManager::framesToTc(frames,fps->value());
                }
                else
                {
                    TCIn = TCIn.replace(".",":");
                }
                QString test = "";
                QString name = "";
                int j = 1;
                //la ligne suivante c'est le nom, jusqu'au timecode suivant
                while (!re.match(test).hasMatch())
                {
                    if (name == "") name += "\r\n";
                    name += test;
                    if (i+j < contenu.count()) test = contenu[i+j];
                    else break;
                    j++;
                }
                //si on a un nom, on enregistre le tout
                if (re.match(contenu[i+j-1]).hasMatch() && name.trimmed() != "")
                {
                    QString TCOut = contenu[i+j-1].mid(1,11);
                    //si on termine pas par des frames, convertir en frames
                    if (!isFrames)
                    {
                        int frames = TCOut.section(":",0,0).toInt()*3600*fps->value() + TCOut.section(":",1,1).toInt()*60*fps->value() + TCOut.section(":",2).toDouble()*fps->value();
                        TCOut = TimeCodeManager::framesToTc(frames,fps->value());
                    }
                    else
                    {
                        TCOut = TCOut.replace(".",":");
                    }
                    QStringList l;
                    l << name << TCIn << TCOut << "" << "" << "" << "" << "placeholder";
                    tc << l;
                    if(i+j == contenu.count()) break;
                }
            }
        }
    }

    //si DTC
    else if (t == 4)
    {
        QMessageBox::information(this,"Erreur","Cette version de Dutico ne prend pas encore en charge l'importation de DTC");
    }

    return tc;
}

void MainWindow::display(QList<QStringList> c,bool replace)
{
    if (replace)
    {
        edlList->clearContents();
        edlList->setRowCount(0);
    }

    foreach(QStringList cc,c)
    {
        edlList->setRowCount(edlList->rowCount()+1);
        QTableWidgetItem *itemName = new QTableWidgetItem(cc[0]);
        edlList->setItem(edlList->rowCount()-1,0,itemName);
        QTableWidgetItem *itemTCIn = new QTableWidgetItem(cc[1]);
        edlList->setItem(edlList->rowCount()-1,1,itemTCIn);
        QTableWidgetItem *itemTCOut = new QTableWidgetItem(cc[2]);
        edlList->setItem(edlList->rowCount()-1,2,itemTCOut);
        int tcout = TimeCodeManager::tcToFrames(cc[2],fps->value());
        int tcin = TimeCodeManager::tcToFrames(cc[1],fps->value());
        QTableWidgetItem *itemDuration = new QTableWidgetItem(TimeCodeManager::framesToTc( tcout - tcin,fps->value()));
        edlList->setItem(edlList->rowCount()-1,3,itemDuration);
        QTableWidgetItem *itemEffect = new QTableWidgetItem(cc[3]);
        edlList->setItem(edlList->rowCount()-1,4,itemEffect);
        QTableWidgetItem *itemComment = new QTableWidgetItem(cc[4]);
        edlList->setItem(edlList->rowCount()-1,5,itemComment);
        QTableWidgetItem *itemSTCIn = new QTableWidgetItem(cc[5]);
        edlList->setItem(edlList->rowCount()-1,6,itemSTCIn);
        QTableWidgetItem *itemSTCOut = new QTableWidgetItem(cc[6]);
        edlList->setItem(edlList->rowCount()-1,7,itemSTCOut);
        QTableWidgetItem *itemFile = new QTableWidgetItem(cc[7]);
        edlList->setItem(edlList->rowCount()-1,8,itemFile);
    }
    edlList->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

}

void MainWindow::on_fpsBox_currentIndexChanged(int index)
{
    if (index == 0) fps->setEnabled(true);
    else
    {
        fps->setEnabled(false);
        QString fpsText = fpsBox->currentText();
        fps->setValue(fpsText.left(fpsText.length()-4).toDouble());
    }
}

void MainWindow::on_applyOffsetButton_clicked()
{
     //convertir l'offset en frames
    int os = hours->value()*60*60*fps->value() + minutes->value()*60*fps->value() + seconds->value()*fps->value() + frames->value();
    //appliquer
    offset(os,true);
}

void MainWindow::on_applyOffsetSButton_clicked()
{
    //convertir l'offset en frames
   int os = hoursS->value()*60*60*fps->value() + minutesS->value()*60*fps->value() + secondsS->value()*fps->value() + framesS->value();
   //appliquer
   offset(os,true,true);
}

void MainWindow::offset(int os,bool all,bool sources,QList<int> rows)
{
    int indexIn = 1;
    int indexOut = 2;
    if (sources)
    {
        indexIn = 6;
        indexOut = 7;
    }

    if (all)
    {
        for (int r = 0; r<edlList->rowCount();r++)
        {
            QString tcin = edlList->item(r,indexIn)->text();
            QString tcout = edlList->item(r,indexOut)->text();
            int newTcin = TimeCodeManager::tcToFrames(tcin,fps->value()) + os;
            int newTcout = TimeCodeManager::tcToFrames(tcout,fps->value()) + os;
            edlList->item(r,indexIn)->setText(TimeCodeManager::framesToTc(newTcin,fps->value()));
            edlList->item(r,indexOut)->setText(TimeCodeManager::framesToTc(newTcout,fps->value()));
        }
    }
    else if (rows.count() > 0)
    {
        foreach(int r,rows)
        {
            QString tcin = edlList->item(r,indexIn)->text();
            QString tcout = edlList->item(r,indexOut)->text();
            int newTcin = TimeCodeManager::tcToFrames(tcin,fps->value()) + os;
            int newTcout = TimeCodeManager::tcToFrames(tcout,fps->value()) + os;
            edlList->item(r,indexIn)->setText(TimeCodeManager::framesToTc(newTcin,fps->value()));
            edlList->item(r,indexOut)->setText(TimeCodeManager::framesToTc(newTcout,fps->value()));
        }
    }
}

void MainWindow::on_edlNextButton_clicked()
{
    tabs->setCurrentIndex(1);
}

void MainWindow::on_previousLayersButton_clicked()
{
    tabs->setCurrentIndex(0);
}

void MainWindow::on_nextLayersButton_clicked()
{
    tabs->setCurrentIndex(2);
}

void MainWindow::on_previousExportButton_clicked()
{
    tabs->setCurrentIndex(1);
}

void MainWindow::on_addLayerButton_clicked()
{
    EditLayer el;

    //chopper le nom du media de la première ligne de l'EDL pour exemple
    if (nameColumnBox->currentIndex() == 0)
    {
        if (edlList->rowCount()>0) el.setUiFileName(edlList->item(0,0)->text());
    }
    else
    {
        //virer les retour à la ligne
        QStringList comments = edlList->item(0,5)->text().split("\n");
        if (comments.count() > 0)
        {
            if (lineCommentSpinner->value() <= comments.count())
            {
                el.setUiFileName(comments[lineCommentSpinner->value()-1]);
            }
            else
            {
                el.setUiFileName(comments[0]);
            }
        }
    }

    //afficher l'éditeur
    if (el.exec())
    {
        //si pas de chemin donné, annuler
        if (el.getPath().trimmed() == "")
        {
            QMessageBox::warning(this,"Path missing","You must provide a valid path to find medias.");
            return;
        }

        int type = 0;

        layersList->setRowCount(layersList->rowCount()+1);
        QTableWidgetItem *itemType = new QTableWidgetItem();
        if (el.getType() == 1)
        {
            type = 1;
            itemType->setText("Video");
            itemType->setIcon(QIcon(":/icons/video.png"));
        }
        if (el.getType() == 2)
        {
            type = 2;
            itemType->setText("Shots");
            itemType->setIcon(QIcon(":/icons/video2.png"));
        }
        if (el.getType() == 3)
        {
            type = 3;
            itemType->setText("Still");
            itemType->setIcon(QIcon(":/icons/Pictures.png"));
        }
        layersList->setItem(layersList->rowCount()-1,0,itemType);

        QTableWidgetItem *itemName = new QTableWidgetItem(el.getName());
        layersList->setItem(layersList->rowCount()-1,1,itemName);

        QTableWidgetItem *itemPath = new QTableWidgetItem(el.getPath());
        layersList->setItem(layersList->rowCount()-1,2,itemPath);

        QTableWidgetItem *itemRec = new QTableWidgetItem();
        el.isRecursive() ? itemRec->setText("yes") : itemRec->setText("no");
        if (type == 0) itemRec->setText("");
        layersList->setItem(layersList->rowCount()-1,3,itemRec);

        QTableWidgetItem *itemFileName = new QTableWidgetItem(el.getFileName());
        if (type == 0) itemFileName->setText("");
        layersList->setItem(layersList->rowCount()-1,4,itemFileName);

        QTableWidgetItem *itemSeq = new QTableWidgetItem();
        el.isSeq() ? itemSeq->setText("yes") : itemSeq->setText("no");
        if (type == 0 || type == 3) itemSeq->setText("");
        layersList->setItem(layersList->rowCount()-1,5,itemSeq);

        QTableWidgetItem *itemActive = new QTableWidgetItem("yes");
        layersList->setItem(layersList->rowCount()-1,6,itemActive);

        layersList->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    }

    //ajouter les noms des layers dans l'onglet export
    updateLayers();
}

void MainWindow::on_actionAjouter_une_couche_triggered()
{
    on_addLayerButton_clicked();
}

QList<QTableWidgetItem*> MainWindow::takeLayersRow(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int col = 0; col < layersList->columnCount(); ++col)
    {
        rowItems << layersList->takeItem(row, col);
    }
    return rowItems;
}

void MainWindow::setLayersRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int col = 0; col < layersList->columnCount(); ++col)
    {
        layersList->setItem(row, col, rowItems.at(col));
    }
}

void MainWindow::on_upButton_clicked()
{
    if (layersList->selectedItems().count() == 0) return;
    if (layersList->selectedItems()[0]->row() == 0) return;

    QList<QTableWidgetItem*> cr = takeLayersRow(layersList->currentRow());
    QList<QTableWidgetItem*> pr = takeLayersRow(layersList->currentRow()-1);

    setLayersRow(layersList->currentRow()-1, cr);
    setLayersRow(layersList->currentRow(), pr);

    layersList->setCurrentItem(cr[0]);
}

void MainWindow::on_downButton_clicked()
{
    if (layersList->selectedItems().count() == 0) return;
    if (layersList->selectedItems()[0]->row() == layersList->rowCount()-1) return;

    QList<QTableWidgetItem*> cr = takeLayersRow(layersList->currentRow());
    QList<QTableWidgetItem*> nr = takeLayersRow(layersList->currentRow()+1);

    setLayersRow(layersList->currentRow()+1, cr);
    setLayersRow(layersList->currentRow(), nr);

    layersList->setCurrentItem(cr[0]);
}

void MainWindow::on_editLayerButton_clicked()
{
    if (layersList->selectedItems().count() == 0) return;

    int row = layersList->currentRow();

    EditLayer el;
    if (nameColumnBox->currentIndex() == 0)
    {
        if (edlList->rowCount()>0) el.setUiFileName(edlList->item(0,0)->text());
    }
    int type = 1;
    if (layersList->item(row,0)->text() == "Video") type = 1;
    else if (layersList->item(row,0)->text() == "Shots") type = 2;
    else if (layersList->item(row,0)->text() == "Still") type = 3;
    el.setType(type);
    el.setName(layersList->item(row,1)->text());
    el.setPath(layersList->item(row,2)->text());
    el.setRecursive(layersList->item(row,3)->text() == "yes");
    el.setFileName(layersList->item(row,4)->text());
    el.setSeq(layersList->item(row,5)->text() == "yes");

    if (el.exec())
    {
        if (el.getPath().trimmed() == "")
        {
            QMessageBox::warning(this,"Path missing","You must provide a valid path to find medias.");
            return;
        }

        int type = 0;

        if (el.getType() == 1)
        {
            type = 1;
            layersList->item(row,0)->setText("Video");
            layersList->item(row,0)->setIcon(QIcon(":/icons/video.png"));
        }
        if (el.getType() == 2)
        {
            type = 2;
            layersList->item(row,0)->setText("Shots");
            layersList->item(row,0)->setIcon(QIcon(":/icons/video2.png"));
        }
        if (el.getType() == 3)
        {
            type = 3;
            layersList->item(row,0)->setText("Still");
            layersList->item(row,0)->setIcon(QIcon(":/icons/Pictures.png"));
        }

        layersList->item(row,1)->setText(el.getName());

        layersList->item(row,2)->setText(el.getPath());

        el.isRecursive() ? layersList->item(row,3)->setText("yes") : layersList->item(row,3)->setText("no");
        if (type == 0) layersList->item(row,3)->setText("");
        if (type == 0) layersList->item(row,3)->setText("");

        layersList->item(row,4)->setText(el.getFileName());

        el.isSeq() ? layersList->item(row,5)->setText("yes") : layersList->item(row,5)->setText("no");
        if (type == 0 || type == 3) layersList->item(row,5)->setText("");

        layersList->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

        updateLayers();
    }
}

void MainWindow::on_disableLayerButton_clicked()
{
    if (layersList->selectedItems().count() == 0) return;

    int row = layersList->currentRow();
    if (layersList->item(row,6)->text() == "yes")
    {
        for (int col = 0;col<layersList->columnCount();col++)
        {
            layersList->item(row,col)->setForeground(QColor(125,125,125));
        }
        layersList->item(row,6)->setText("no");
        disableLayerButton->setText("Activer");
        disableLayerButton->setIcon(QIcon(":/icons/layers.png"));
    }
    else
    {
        for (int col = 0;col<layersList->columnCount();col++)
        {
            layersList->item(row,col)->setForeground(QColor(0,0,0));
        }
        layersList->item(row,6)->setText("yes");
        disableLayerButton->setText("Désactiver");
        disableLayerButton->setIcon(QIcon(":/icons/layersdisable.png"));
    }
}

void MainWindow::on_removeLayerButton_clicked()
{
    if (layersList->selectedItems().count() == 0) return;
    if (QMessageBox::question(this,"Confirm","Are you sure you want to remove selected layer?") == QMessageBox::Yes)
        layersList->removeRow(layersList->currentRow());


    updateLayers();
}

void MainWindow::updateLayers()
{
    //activer les boutons pour manier les layers
    upButton->setEnabled(layersList->rowCount()>0);
    downButton->setEnabled(layersList->rowCount()>0);
    editLayerButton->setEnabled(layersList->rowCount()>0);
    disableLayerButton->setEnabled(layersList->rowCount()>0);
    removeLayerButton->setEnabled(layersList->rowCount()>0);


    resolutionBox->clear();
    durationBox->clear();
    soundBox->clear();
    resolutionBox->addItem("Personnalisée");
    durationBox->addItem("Timecodes");
    soundBox->addItem("Pas de son");
    for (int row=0;row<layersList->rowCount();row++)
    {
        resolutionBox->addItem(layersList->item(row,1)->text());
        if (layersList->item(row,0)->text() == "Shots" && layersList->item(row,5)->text() != "Yes" || layersList->item(row,0)->text() == "Video")
        {
            soundBox->addItem(layersList->item(row,1)->text());
        }
        if (layersList->item(row,0)->text() == "Shots")
        {
            durationBox->addItem(layersList->item(row,1)->text());
        }
    }
}

void MainWindow::on_exportBrowseButton_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, "Destination");
    if (file != "")
    {
        exportNameEdit->setText(file);
    }
}

void MainWindow::on_exportButton_clicked()
{
    //trier
    edlList->sortItems(1);
    if (exportNameEdit_2->text().trimmed() == "" || !QDir(exportNameEdit->text()).exists())
    {
        QMessageBox::warning(this,"Destination manquante","Vous devez d'abord indiquer une destination et un nom valides pour votre(s) export(s).");
        return;
    }
    if (aeButton->isChecked()) exportToAE();
    if (qtButton->isChecked()) exportToQT();
    if (edlButton->isChecked()) exportToEDL();
    if (acpButton->isChecked()) exportToACP();

}

void MainWindow::exportToAE()
{

    //1 créer le JSON
    //créer l'objet principal
    QJsonObject dtc;

    //paramètres after
    QJsonObject aeParams;
    aeParams.insert("compName",this->windowTitle().section(".",0,0));
    aeParams.insert("paramsFromClip",resolutionBox->currentIndex()!=0);
    aeParams.insert("width",widthBox->value());
    aeParams.insert("height",heightBox->value());
    aeParams.insert("pix",1);
    aeParams.insert("precompose",precomposeButton->isChecked());
    aeParams.insert("handles",0);
    aeParams.insert("video",true);
    aeParams.insert("singleTextLayer",true);
    aeParams.insert("fps",fps->value());
    aeParams.insert("multilayers",true);
    if (resolutionBox->currentIndex() != 0) aeParams.insert("paramsFromLayer",resolutionBox->currentText());
    else aeParams.insert("paramsFromLayer","");
    if (durationBox->currentIndex() != 0) aeParams.insert("useLayerTC",durationBox->currentText());
    else aeParams.insert("useLayerTC","");
    if (soundBox->currentIndex() != 0) aeParams.insert("sound",soundBox->currentText());
    else aeParams.insert("sound","");
    aeParams.insert("bitcName",bitcNameBox->isChecked());
    aeParams.insert("bitcShotsTC",bitcShotsTCBox->isChecked());
    aeParams.insert("bitcTC",bitcTCBox->isChecked());
    QString renderDir = exportNameEdit->text();
    renderDir = renderDir.replace("\\","/");
    if (!renderDir.endsWith("/")) renderDir += "/";
    aeParams.insert("renderFile",renderDir + exportNameEdit_2->text());
    aeParams.insert("render",renderBox->isChecked());
    aeParams.insert("newAEP",newAEPBox->isChecked());
    aeParams.insert("shotAEP",shotAEPBox->isChecked());
    aeParams.insert("saveAEP",saveAEPBox->isChecked());
    aeParams.insert("saveFile",renderDir + exportNameEdit_2->text());

    dtc.insert("aeParams",aeParams);

    //parcourir le tableau et construire la liste des fichiers et timecodes
    QJsonArray clips;
    for (int r = 0;r<edlList->rowCount();r++)
    {
        //l'objet clip
        QJsonObject clip;
        clip.insert("TCIn",edlList->item(r,1)->text());
        clip.insert("TCOut",edlList->item(r,2)->text());
        clip.insert("sourceTCIn",edlList->item(r,6)->text());
        clip.insert("sourceTCOut",edlList->item(r,7)->text());
        clip.insert("name",edlList->item(r,0)->text());
        clip.insert("comment",edlList->item(r,5)->text());
        clip.insert("file","");
        clip.insert("sequence",false);

        //le tableau des effets
        QJsonArray effects;
        if (edlList->item(r,4)->text() != "")
        {
            QJsonObject effect;
            QString effectName = "";
            int effectDuration = 0;
            QStringList fx = edlList->item(r,4)->text().split(" | ");
            if (fx.count() > 0)
            {
                effectName = fx[0];
                if (fx.count() > 1) effectDuration = fx[1].toInt();
            }
            effect.insert("name",effectName);
            effect.insert("duration",effectDuration);
            effects.insert(effects.count(),effect);
        }
        clip.insert("effects",effects);

        //les couches
        QJsonArray layers;
        for (int l=layersList->rowCount()-1;l>=0;l--)
        {
            if (layersList->item(l,0)->text() == "Video" || layersList->item(l,6)->text() == "no") continue;
            QJsonObject layer;
            layer.insert("type",layersList->item(l,0)->text());
            layer.insert("name",layersList->item(l,1)->text());
            layer.insert("sequence",layersList->item(l,5)->text() == "yes");
            //Trouver le fichier pour chaque couche
            //chopper le nom du media
            QString mediaName = "";
            if (nameColumnBox->currentIndex() == 0)
            {
                mediaName = edlList->item(r,0)->text();
            }
            else
            {
                //virer les retour à la ligne
                QStringList comments = edlList->item(r,5)->text().split("\n");
                if (comments.count() > 0)
                {
                    if (lineCommentSpinner->value() <= comments.count())
                    {
                        mediaName = comments[lineCommentSpinner->value()-1];
                    }
                    else
                    {
                        mediaName = comments[0];
                    }
                }
            }
            layer.insert("file",findMedia(mediaName,layersList->item(l,0)->text(),layersList->item(l,2)->text(),layersList->item(l,3)->text() == "yes",layersList->item(l,4)->text(),layersList->item(l,5)->text() == "yes"));
            layers.insert(layers.count(),layer);
        }
        clip.insert("layers",layers);

        //et on met tout dans le tabeau des clips
        clips.insert(clips.count(),clip);
    }

    dtc.insert("clips",clips);

    //les couches video
    QJsonArray videos;
    for (int l=layersList->rowCount()-1;l>=0;l--)
    {
        if (layersList->item(l,0)->text() != "Video" || layersList->item(l,6)->text() == "no") continue;
        QJsonObject video;
        video.insert("name",layersList->item(l,1)->text());
        video.insert("file",layersList->item(l,2)->text());
        videos.insert(videos.count(),video);
    }
    dtc.insert("videos",videos);

    //écrire le jsondocument dans un fichier
    QString dtcString = QJsonDocument(dtc).toJson();
    QString chemin = exportNameEdit->text();
    if (!chemin.endsWith("/") || !chemin.endsWith("\\")) chemin += "/";
    QFile *dtcFile = new QFile(renderDir + exportNameEdit_2->text() + ".dtc");
    dtcFile->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(dtcFile);
    out << dtcString;
    dtcFile->close();

    //4 si possible lancer dans After Effects le jsx
    QFile *aeversion = new QFile(QDir::currentPath() + "/AEVersion.jsxinc");
    aeversion->open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inAEVersion(aeversion);
    QString aeVersionS = inAEVersion.readAll();
    aeversion->close();
    //le jsx
    QFile *jsx= new QFile(QDir::currentPath() + "/Dutico_Launch.jsx");
    jsx->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream outJsx(jsx);
    outJsx << aeVersionS + "\r\n";
    outJsx << "var dtcFile = new File(\"" + renderDir + exportNameEdit_2->text() + ".dtc\");\r\n";
    outJsx << "#include \"" + QDir::currentPath() + "/jsonParser.jsxinc\"\r\n";
    outJsx << "#include \"" + QDir::currentPath() + "/dtc.jsxinc\"\r\n";
    outJsx << "#include \"" + QDir::currentPath() + "/duticoLauncher.jsxinc\"";
    jsx->close();
    QUrl cmd = QUrl::fromLocalFile(QDir::currentPath() + "/Dutico_Launch.jsx");
    QDesktopServices::openUrl(cmd);
}

void MainWindow::exportToQT()
{
    QString file = exportNameEdit->text();
    if (!file.endsWith("/") || !file.endsWith("\\")) file += "/";
    file += exportNameEdit_2->text() + "_QT.txt";

    QStringList output;
    QString opt = "{Qttext}{font:Arial}{plain}{size:";
    opt += QString::number(qtTextSizeSpinner->value());
    opt += "}{justify:center}{width:";
    opt += QString::number(qtWidthSpinner->value());
    opt += "}{height:";
    opt += QString::number(qtHeightSpinner->value());
    opt += "}{timeStamps:absolute}{timeScale:";
    opt += QString::number(fps->value());
    opt += + "}";
    output << opt;
    //Vérifier si on doit ajouter un frameCounter
    if (!qtCounterBox->isChecked())
    {
        for (int r = 0; r<edlList->rowCount();r++)
        {
            QString tc = "[" + edlList->item(r,1)->text().left(8) + "." + edlList->item(r,1)->text().right(2) + "]";
            output << tc;
            //chopper le nom du plan
            QString mediaName = "";
            if (nameColumnBox->currentIndex() == 0)
            {
                mediaName = edlList->item(r,0)->text();
            }
            else
            {
                //virer les retour à la ligne
                QStringList comments = edlList->item(r,5)->text().split("\n");
                if (comments.count() > 0)
                {
                    if (lineCommentSpinner->value() <= comments.count())
                    {
                        mediaName = comments[lineCommentSpinner->value()-1];
                    }
                    else
                    {
                        mediaName = comments[0];
                    }
                }
            }
            output << mediaName;
            if (r < edlList->rowCount()-1)
            {
                if (edlList->item(r,2)->text() != edlList->item(r+1,1)->text())
                {
                    output << "[" + edlList->item(r,2)->text().left(8) + "." + edlList->item(r,2)->text().right(2) + "]";
                    output << " ";
                }
            }
            else
            {
                output << "[" + edlList->item(r,2)->text().left(8) + "." + edlList->item(r,2)->text().right(2) + "]";
            }
        }
    }
    //sinon créer le compteur
    else
    {
        int fcStart = 0;
        if (qtCounterBox1->isChecked()) fcStart = 1;
        for (int r = 0; r<edlList->rowCount();r++)
        {
            int dur = TimeCodeManager::tcToFrames(edlList->item(r,2)->text(),fps->value()) - TimeCodeManager::tcToFrames(edlList->item(r,1)->text(),fps->value());
            int d = 0;
            while(d<dur)
            {
                int tc = TimeCodeManager::tcToFrames(edlList->item(r,1)->text(),fps->value()) + d;
                output << "[" + TimeCodeManager::framesToTc(tc,fps->value()).left(8) + "." + TimeCodeManager::framesToTc(tc,fps->value()).right(2) + "]";
                output << QString::number(d+fcStart) + "f";
                d++;
            }
        }
    }

    QFichier saveFile(file);
    saveFile.setCodec("UTF-8");
    saveFile.clear();
    saveFile.appendLines(output);

}

void MainWindow::exportToEDL()
{
    QMessageBox::information(this,"Export EDL","Cette version de Dutico ne prend pas encore en charge l'export vers EDL.");
}

void MainWindow::exportToACP()
{
    QString file = exportNameEdit->text();
    if (!file.endsWith("/") || !file.endsWith("\\")) file += "/";
    file += exportNameEdit_2->text() + "_ACP.txt";

    QStringList output;
    //créer l'entete
    output << "@ This file was created with DuTico, more information on www.duduf.net";
    output << "";
    output << "<begin subtitles>";

    for (int r = 0; r<edlList->rowCount();r++)
    {
        QString tc = edlList->item(r,1)->text() + " " + edlList->item(r,2)->text();
        output << tc;
        //chopper le nom du plan
        QString mediaName = "";
        if (nameColumnBox->currentIndex() == 0)
        {
            mediaName = edlList->item(r,0)->text();
        }
        else
        {
            //virer les retour à la ligne
            QStringList comments = edlList->item(r,5)->text().split("\n");
            if (comments.count() > 0)
            {
                if (lineCommentSpinner->value() <= comments.count())
                {
                    mediaName = comments[lineCommentSpinner->value()-1];
                }
                else
                {
                    mediaName = comments[0];
                }
            }
        }
        output << mediaName;
        output << "";
    }

    output << "<end subtitles>";

    QFichier saveFile(file);
    saveFile.setCodec("UTF-8");
    saveFile.clear();
    saveFile.appendLines(output);
}

QString MainWindow::findMedia(QString mediaName,QString type,QString path,bool recursive,QString fileName,bool seq)
{
    QString media = "";
    QDir searchFolder(path);
    QString prefix = fileName.section("%",0,0);
    QString suffix = fileName.section("%",-1);
    QStringList filter(prefix + mediaName + suffix);
    return searchSourceFile(recursive,searchFolder,filter);
}

QString MainWindow::searchSourceFile(bool r,QDir sf,QStringList f)
{
    QString foundFile = "";
    //chercher dans le dossier directement
    QFileInfoList results = sf.entryInfoList(f,QDir::Files);
    if (results.count() > 0)
    {
        //chercher le plus récent
        QFileInfo found = results[0];
        foreach(QFileInfo fi,results)
        {
            if (fi.lastModified() > found.lastModified()) found = fi;
        }

        foundFile = found.absoluteFilePath();
        return foundFile;
    }
    if (r)
    {
        QFileInfoList dirs = sf.entryInfoList(QStringList("*"),QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
        if (dirs.count() > 0)
        {
            foreach(QFileInfo fi,dirs)
            {
                foundFile = searchSourceFile(r,QDir(fi.absoluteFilePath()),f);
                if (foundFile != "") return foundFile;
            }
        }
    }
    return foundFile;
}

void MainWindow::on_resolutionBox_currentIndexChanged(int index)
{

    widthBox->setEnabled(index == 0);
    heightBox->setEnabled(index == 0);
    proportionsButton->setEnabled(index == 0);

}

void MainWindow::on_actionPr_f_rences_triggered()
{
    //chercher la version d'After
    QFichier jsx(QDir::currentPath() + "/AEVersion.jsxinc");
    QString vS = jsx.getLine("#target aftereffects");
    double v = vS.section("-",-1).toDouble();
    Preferences p(v,this);
    if (p.exec())
    {
        v = p.getAEVersion();
        if (v!=0) jsx.setLine("#target aftereffects","-" + QString::number(v));
        else jsx.setLine("#target aftereffects","");
    }
}

void MainWindow::on_widthBox_valueChanged(int arg1)
{
    if (!progra)
    {
        progra = true;
        if (proportionsButton->isChecked())
        {
            heightBox->setValue(int(arg1/ratio));
        }
        else
        {
            ratio = float(arg1)/float(heightBox->value());
        }
        progra = false;
    }
}

void MainWindow::on_heightBox_valueChanged(int arg1)
{
    if (!progra)
    {
        progra = true;
        if (proportionsButton->isChecked())
        {
            widthBox->setValue(int(arg1*ratio));
        }
        else
        {
            ratio = float(widthBox->value())/float(arg1);
        }
        progra = false;
    }
}

void MainWindow::on_detectOffsetButton_clicked()
{
    //prendre le premier TCIn
    int first = TimeCodeManager::tcToFrames(edlList->item(0,1)->text(),fps->value());
    //parcourir tous les TCIn pour trouver le plus petit
    for (int i = 1;i<edlList->rowCount();i++)
    {
        int newTc = TimeCodeManager::tcToFrames(edlList->item(i,1)->text(),fps->value());
        if (first > newTc) first = newTc;
    }
    QString firstS = TimeCodeManager::framesToTc(first,fps->value());
    hours->setValue(-firstS.section(":",0,0).toInt());
    minutes->setValue(-firstS.section(":",1,1).toInt());
    seconds->setValue(-firstS.section(":",2,2).toInt());
    frames->setValue(-firstS.section(":",3,3).toInt());
}

void MainWindow::on_nameColumnBox_activated(int index)
{
    lineCommentLabel->setEnabled(index == 1);
    lineCommentSpinner->setEnabled(index == 1);
}

bool MainWindow::on_actionEnregistrer_sous_triggered()
{
    saveFile = QFileDialog::getSaveFileName(this,"Enregistrement","","Dutico Project (*.dtcp);;SQLite (*.s3db,*.db);;Tous les fichiers (*.*)");
    if (saveFile == "") return false;

    if (save()) return true;
    else return false;


}

bool MainWindow::save()
{
    QFile sf(saveFile);
    if (sf.exists())
    {
        if (!sf.remove())
        {
            QMessageBox::warning(this,"Erreur d'écriture","Le fichier n'a pas pu être écrit.\nPeut-être est-il utilisé par une autre application ?");
            return false;
        }
    }
    this->setWindowTitle(saveFile.section("/",-1) + " - Dubab");
    QFile::copy(QDir::currentPath() + "/blankProject.dtcp",saveFile);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","save");
    db.setDatabaseName(saveFile);
    db.setHostName("localhost");
    db.open();
    QString q = "";
    //insérer les timecodes
    for (int i=0;i<edlList->rowCount();i++)
    {
        q = "INSERT INTO TimeCodes(name,TCIn,TCOut,duration,effect,comment,sourceTCIn,sourceTCOut) VALUES (";
        q += "'" + edlList->item(i,0)->text() + "',";
        q += "'" + edlList->item(i,1)->text() + "',";
        q += "'" + edlList->item(i,2)->text() + "',";
        q += "'" + edlList->item(i,3)->text() + "',";
        q += "'" + edlList->item(i,4)->text() + "',";
        q += "'" + edlList->item(i,5)->text() + "',";
        q += "'" + edlList->item(i,6)->text() + "',";
        q += "'" + edlList->item(i,7)->text() + "'";
        q += ");";
        if (!DufSqlQuery(q,db).verif()) break;
    }
    //insérer les layers
    for (int i=0;i<layersList->rowCount();i++)
    {
        q = "INSERT INTO Layers(ind,type,name,path,recursive,files,sequences,active) VALUES (";
        q += QString::number(i) + ",";
        q += "'" + layersList->item(i,0)->text() + "',";
        q += "'" + layersList->item(i,1)->text() + "',";
        q += "'" + layersList->item(i,2)->text() + "',";
        q += "'" + layersList->item(i,3)->text() + "',";
        q += "'" + layersList->item(i,4)->text() + "',";
        q += "'" + layersList->item(i,5)->text() + "',";
        q += "'" + layersList->item(i,6)->text() + "'";
        q += ");";
        if (!DufSqlQuery(q,db).verif()) break;
    }
    //MAJ les options
    q = "UPDATE Options SET val = " + QString::number(fps->value()) + " WHERE name = 'fps';";
    DufSqlQuery(q,db);
    q = "UPDATE Options SET val = " + QString::number(nameColumnBox->currentIndex()) + " WHERE name = 'nameColumn';";
    DufSqlQuery(q,db);
    q = "UPDATE Options SET val = " + QString::number(durationBox->currentIndex()) + " WHERE name = 'duration';";
    DufSqlQuery(q,db);
    q = "UPDATE Options SET val = " + QString::number(soundBox->currentIndex()) + " WHERE name = 'sound';";
    DufSqlQuery(q,db);
    q = "UPDATE Options SET val = " + QString::number(resolutionBox->currentIndex()) + " WHERE name = 'resolution';";
    DufSqlQuery(q,db);
    q = "UPDATE Options SET val = " + QString::number(widthBox->value()) + " WHERE name = 'width';";
    DufSqlQuery(q,db);
    q = "UPDATE Options SET val = " + QString::number(heightBox->value()) + " WHERE name = 'height';";
    DufSqlQuery(q,db);
    QString bitcNameS = "0";
    if (bitcNameBox->isChecked()) bitcNameS = "1";
    q = "UPDATE Options SET val = " + bitcNameS + " WHERE name = 'bitcName';";
    DufSqlQuery(q,db);
    QString bitcShotsTCS = "0";
    if (bitcShotsTCBox->isChecked()) bitcShotsTCS = "1";
    q = "UPDATE Options SET val = " + bitcShotsTCS + " WHERE name = 'bitcShotsTC';";
    DufSqlQuery(q,db);
    QString bitcTCS = "0";
    if (bitcTCBox->isChecked()) bitcTCS = "1";
    q = "UPDATE Options SET val = " + bitcTCS + " WHERE name = 'bitcTC';";
    DufSqlQuery(q,db);
    QString renderS = "0";
    if (renderBox->isChecked()) renderS = "1";
    q = "UPDATE Options SET val = " + renderS + " WHERE name = 'render';";
    DufSqlQuery(q,db);
    QString precomposeS = "0";
    if (precomposeButton->isChecked()) precomposeS = "1";
    q = "UPDATE Options SET val = " + precomposeS + " WHERE name = 'precompose';";
    DufSqlQuery(q,db);

    QString aeS = "0";
    if (aeButton->isChecked()) aeS = "1";
    q = "UPDATE Options SET val = " + aeS + " WHERE name = 'exportAE';";
    DufSqlQuery(q,db);
    QString qtS = "0";
    if (qtButton->isChecked()) qtS = "1";
    q = "UPDATE Options SET val = " + qtS + " WHERE name = 'exportQT';";
    DufSqlQuery(q,db);
    QString edlS = "0";
    if (edlButton->isChecked()) edlS = "1";
    q = "UPDATE Options SET val = " + edlS + " WHERE name = 'exportEDL';";
    DufSqlQuery(q,db);
    QString acpS = "0";
    if (acpButton->isChecked()) acpS = "1";
    q = "UPDATE Options SET val = " + acpS + " WHERE name = 'exportACP';";
    DufSqlQuery(q,db);

    QString qtCounterS = "0";
    if (qtCounterBox->isChecked()) qtCounterS = "1";
    q = "UPDATE Options SET val = " + qtCounterS + " WHERE name = 'qtCounter';";
    DufSqlQuery(q,db);
    QString qtCounter1S = "0";
    if (qtCounterBox1->isChecked()) qtCounter1S = "1";
    q = "UPDATE Options SET val = " + qtCounter1S + " WHERE name = 'qtCounter1';";
    DufSqlQuery(q,db);
    q = "UPDATE Options SET val = " + QString::number(qtWidthSpinner->value()) + " WHERE name = 'qtBlocWidth';";
    DufSqlQuery(q,db);
    q = "UPDATE Options SET val = " + QString::number(qtHeightSpinner->value()) + " WHERE name = 'qtBlocHeight';";
    DufSqlQuery(q,db);
    q = "UPDATE Options SET val = " + QString::number(qtTextSizeSpinner->value()) + " WHERE name = 'qtTextSize';";
    DufSqlQuery(q,db);

    //MAJ l'export
    q = "UPDATE Export SET val = '" + exportNameEdit->text() + "' WHERE name = 'path';";
    DufSqlQuery(q,db);
    q = "UPDATE Export SET val = '" + exportNameEdit_2->text() + "' WHERE name = 'name';";
    DufSqlQuery(q,db);
    db.close();
    return true;
}

bool MainWindow::on_actionEnregistrer_triggered()
{
    if (saveFile != "" && QFile(saveFile).exists())
    {
        if (!save())
        {
            return false;
        }
    }
    else if (!on_actionEnregistrer_sous_triggered())
    {
        return false;
    }
    else return true;
}

void MainWindow::on_actionOuvrir_triggered()
{
    progra = true;
    //réinitialiser
    if (!on_actionNouveau_triggered()) return;
    //et charger
    QString openFile = QFileDialog::getOpenFileName(this,"Ouvrir un projet","","Dutico Project (*.dtcp);;SQLite (*.s3db,*.db);;Tous les fichiers (*.*)");
    if (openFile == "" || !QFile(openFile).exists()) return;
    else saveFile = openFile;
    this->setWindowTitle(saveFile.section("/",-1) + " - Dutico");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","open");
    db.setDatabaseName(saveFile);
    db.setHostName("localhost");
    db.open();
    //charger les timecodes
    QString q = "SELECT name,TCIn,TCOut,duration,effect,comment,sourceTCIn,sourceTCOut FROM TimeCodes ORDER BY TCIn;";
    DufSqlQuery queryTC(q,db);
    queryTC.verif();
    edlList->clearContents();
    edlList->setRowCount(0);
    while(queryTC.next())
    {
        edlList->setRowCount(edlList->rowCount()+1);
        QTableWidgetItem *itemName = new QTableWidgetItem(queryTC.value(0).toString());
        edlList->setItem(edlList->rowCount()-1,0,itemName);
        QTableWidgetItem *itemTCIn = new QTableWidgetItem(queryTC.value(1).toString());
        edlList->setItem(edlList->rowCount()-1,1,itemTCIn);
        QTableWidgetItem *itemTCOut = new QTableWidgetItem(queryTC.value(2).toString());
        edlList->setItem(edlList->rowCount()-1,2,itemTCOut);
        QTableWidgetItem *itemDuration = new QTableWidgetItem(queryTC.value(3).toString());
        edlList->setItem(edlList->rowCount()-1,3,itemDuration);
        QTableWidgetItem *itemEffect = new QTableWidgetItem(queryTC.value(4).toString());
        edlList->setItem(edlList->rowCount()-1,4,itemEffect);
        QTableWidgetItem *itemComment = new QTableWidgetItem(queryTC.value(5).toString());
        edlList->setItem(edlList->rowCount()-1,5,itemComment);
        QTableWidgetItem *itemSTCIn = new QTableWidgetItem(queryTC.value(6).toString());
        edlList->setItem(edlList->rowCount()-1,6,itemSTCIn);
        QTableWidgetItem *itemSTCOut = new QTableWidgetItem(queryTC.value(7).toString());
        edlList->setItem(edlList->rowCount()-1,7,itemSTCOut);
        QTableWidgetItem *itemFile = new QTableWidgetItem("");
        edlList->setItem(edlList->rowCount()-1,8,itemFile);
    }
    edlList->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    //charger les layers
    q = "SELECT ind,type,name,path,recursive,files,sequences,active FROM Layers ORDER BY ind;";
    DufSqlQuery queryLayers(q,db);
    queryLayers.verif();
    layersList->clearContents();
    layersList->setRowCount(0);
    while(queryLayers.next())
    {
        layersList->setRowCount(layersList->rowCount()+1);
        QTableWidgetItem *itemType = new QTableWidgetItem(queryLayers.value(1).toString());
        if (queryLayers.value(1).toString() == "Video")
        {
            itemType->setIcon(QIcon(":/icons/video.png"));
        }
        if (queryLayers.value(1).toString() == "Shots")
        {
            itemType->setIcon(QIcon(":/icons/video2.png"));
        }
        if (queryLayers.value(1).toString() == "Still")
        {
            itemType->setIcon(QIcon(":/icons/Pictures.png"));
        }
        layersList->setItem(layersList->rowCount()-1,0,itemType);
        QTableWidgetItem *itemName = new QTableWidgetItem(queryLayers.value(2).toString());
        layersList->setItem(layersList->rowCount()-1,1,itemName);
        QTableWidgetItem *itemPath = new QTableWidgetItem(queryLayers.value(3).toString());
        layersList->setItem(layersList->rowCount()-1,2,itemPath);
        QTableWidgetItem *itemRec = new QTableWidgetItem(queryLayers.value(4).toString());
        layersList->setItem(layersList->rowCount()-1,3,itemRec);
        QTableWidgetItem *itemFiles = new QTableWidgetItem(queryLayers.value(5).toString());
        layersList->setItem(layersList->rowCount()-1,4,itemFiles);
        QTableWidgetItem *itemSeq = new QTableWidgetItem(queryLayers.value(6).toString());
        layersList->setItem(layersList->rowCount()-1,5,itemSeq);
        QTableWidgetItem *itemAct = new QTableWidgetItem(queryLayers.value(7).toString());
        layersList->setItem(layersList->rowCount()-1,6,itemAct);
    }
    updateLayers();
    //charger les options
    q = "SELECT name,val FROM Options;";
    DufSqlQuery queryOptions(q,db);
    queryOptions.verif();
    while(queryOptions.next())
    {
        int val = queryOptions.value(1).toInt();
        QString name = queryOptions.value(0).toString();
        if (name == "fps")
        {
            fps->setValue(val);
            fpsBox->setCurrentIndex(0);
        }
        if (name == "nameColumn") nameColumnBox->setCurrentIndex(val);
        if (name == "duration") durationBox->setCurrentIndex(val);
        if (name == "sound") soundBox->setCurrentIndex(val);
        if (name == "resolution") resolutionBox->setCurrentIndex(val);
        if (name == "width") widthBox->setValue(val);
        if (name == "height") heightBox->setValue(val);
        if (name == "bitcName") bitcNameBox->setChecked(val == 1);
        if (name == "bitcShotsTC") bitcShotsTCBox->setChecked(val == 1);
        if (name == "bitcTC") bitcTCBox->setChecked(val == 1);
        if (name == "render") renderBox->setChecked(val == 1);
        if (name == "precompose") precomposeButton->setChecked(val == 1);

        if (name == "exportAE")
        {
            aeButton->setChecked(val == 1);
            tabAE->setEnabled(val == 1);
        }
        if (name == "exportEDL") edlButton->setChecked(val == 1);
        if (name == "exportQT")
        {
            qtButton->setChecked(val == 1);
            tabQT->setEnabled(val == 1);
        }
        if (name == "exportACP") acpButton->setChecked(val == 1);

        if (name == "qtBlocHeight") qtHeightSpinner->setValue(val);
        if (name == "qtBlocWidth") qtWidthSpinner->setValue(val);
        if (name == "qtTextSize") qtTextSizeSpinner->setValue(val);
        if (name == "qtCounter") qtCounterBox->setChecked(val == 1);
        if (name == "qtCounter1") qtCounterBox1->setChecked(val == 1);
    }
    enableExportButton();
    //charger l'export
    q = "SELECT name,val FROM Export;";
    DufSqlQuery queryExport(q,db);
    queryExport.verif();
    while(queryExport.next())
    {
        QString val = queryExport.value(1).toString();
        QString name = queryExport.value(0).toString();
        if (name == "path") exportNameEdit->setText(val);
        if (name == "name") exportNameEdit_2->setText(val);
    }
    db.close();
    progra = false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int rep = QMessageBox::question(this,"Quitter","Voulez-vous enregistrer le projet en cours avant de quitter ?",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (rep == QMessageBox::Yes)
    {
        if (on_actionEnregistrer_triggered()) event->accept();
        else event->ignore();
    }
    else if (rep != QMessageBox::No)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::enableExportButton()
{
    if (aeButton->isChecked() || qtButton->isChecked() || edlButton->isChecked() || acpButton->isChecked())
    {
        exportButton->setEnabled(true);
    }
    else
    {
        exportButton->setEnabled(false);
    }
}

void MainWindow::on_aeButton_clicked(bool checked)
{
    if (checked)
    {
        tabAE->setEnabled(true);
        exportTabs->setCurrentIndex(0);
    }
    else
    {
        tabAE->setEnabled(false);
    }
    actionAfter_Effects->setChecked(checked);
    enableExportButton();
}

void MainWindow::on_qtButton_clicked(bool checked)
{
    if (checked)
    {
        tabQT->setEnabled(true);
        exportTabs->setCurrentIndex(1);
    }
    else
    {
        tabQT->setEnabled(false);
    }
    actionQuicktime_text->setChecked(checked);
    enableExportButton();
}

void MainWindow::on_acpButton_clicked(bool checked)
{
    actionAvid_Caption_Plugin->setChecked(checked);
    enableExportButton();
}

void MainWindow::on_edlButton_clicked(bool checked)
{
    actionEdit_Decision_List_EDL->setChecked(checked);
    enableExportButton();
}

void MainWindow::on_qtCounterBox_clicked(bool checked)
{
    qtCounterBox1->setEnabled(checked);
}

void MainWindow::on_qtWidthSpinner_valueChanged(int arg1)
{
    if (!progra)
    {
        progra = true;
        if (proportionsButton_2->isChecked())
        {
            qtHeightSpinner->setValue(int(arg1/ratioQT));
        }
        else
        {
            ratioQT = float(arg1)/float(qtHeightSpinner->value());
        }
        progra = false;
    }
}

void MainWindow::on_qtHeightSpinner_valueChanged(int arg1)
{
    if (!progra)
    {
        progra = true;
        if (proportionsButton_2->isChecked())
        {
            qtWidthSpinner->setValue(int(arg1*ratioQT));
        }
        else
        {
            ratioQT = float(qtWidthSpinner->value())/float(arg1);
        }
        progra = false;
    }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAbout_triggered()
{
    About a;
    a.exec();
}

void MainWindow::on_actionAvid_Caption_Plugin_triggered(bool checked)
{
    acpButton->setChecked(checked);
    on_acpButton_clicked(checked);
}

void MainWindow::on_actionAfter_Effects_triggered(bool checked)
{
    aeButton->setChecked(checked);
    on_aeButton_clicked(checked);
}

void MainWindow::on_actionQuicktime_text_triggered(bool checked)
{
    qtButton->setChecked(checked);
    on_qtButton_clicked(checked);
}

void MainWindow::on_actionEdit_Decision_List_EDL_triggered(bool checked)
{
    edlButton->setChecked(checked);
    on_edlButton_clicked(checked);
}

void MainWindow::on_layersList_cellDoubleClicked(int row, int column)
{
    on_editLayerButton_clicked();
}

void MainWindow::on_layersList_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    int r = current->row();
    if(layersList->item(r,6)->text() == "yes")
    {
        disableLayerButton->setText("Désactiver");
        disableLayerButton->setIcon(QIcon(":/icons/layersdisable.png"));
    }
    else
    {
        disableLayerButton->setText("Activer");
        disableLayerButton->setIcon(QIcon(":/icons/layers.png"));
    }
}

void MainWindow::on_actionFusionner_un_fichier_de_Timecodes_triggered()
{
    on_fusionButton_clicked();
}

void MainWindow::on_newAEPBox_toggled(bool checked)
{
    saveAEPBox->setEnabled(checked);
}

void MainWindow::on_shotAEPBox_toggled(bool checked)
{
    bitcFrame->setEnabled(!checked);
    precomposeButton->setEnabled(!checked);
    renderBox->setEnabled(!checked);
    if (checked)
    {
        precomposeButton->setChecked(false);
        bitcNameBox->setChecked(false);
        bitcShotsTCBox->setChecked(false);
        bitcTCBox->setChecked(false);
        renderBox->setChecked(false);
    }
}

