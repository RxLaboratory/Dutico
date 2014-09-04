#include "editlayer.h"
#include <QFileDialog>

EditLayer::EditLayer(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

void EditLayer::on_radioButton_toggled(bool checked)
{
    detailsWidget->setEnabled(!checked);
    recursiveButton->setEnabled(!checked);
    seqWidget->setEnabled(radioButton_2->isChecked());
}

void EditLayer::on_radioButton_2_toggled(bool checked)
{
    detailsWidget->setEnabled(!radioButton->isChecked());
    recursiveButton->setEnabled(!radioButton->isChecked());
    seqWidget->setEnabled(checked);
}

void EditLayer::on_radioButton_3_toggled(bool checked)
{
    detailsWidget->setEnabled(!radioButton->isChecked());
    recursiveButton->setEnabled(!radioButton->isChecked());
    seqWidget->setEnabled(radioButton_2->isChecked());
}

void EditLayer::on_browseButton_clicked()
{
    if (radioButton->isChecked())
    {
        QString file = QFileDialog::getOpenFileName(this, "Open video file", QString(), "All Files (*.*);;Quicktime (*.mov);;H.264 (*.mp4);;AVC (*.mts);;MXF (*.mxf);;Audio Video Interleave (*.avi)");
        if (file != "")
        {
            pathEdit->setText(file);
            if (nameEdit->text().trimmed() == "") nameEdit->setText(file.section("/",-1));
        }
    }
    else
    {
        QString dossier = QFileDialog::getExistingDirectory(this, "Choose folder containing shots");
        if (dossier != "")
        {
            pathEdit->setText(dossier);
            if (nameEdit->text().trimmed() == "") nameEdit->setText(dossier.section("/",-1));
        }
    }
}

void EditLayer::setUiFileName(QString n)
{
    uiFileName->setText(n);
}

int EditLayer::getType()
{
    if (radioButton->isChecked()) return 1;
    else if (radioButton_2->isChecked()) return 2;
    else if (radioButton_3->isChecked()) return 3;
    else return 0;
}

QString EditLayer::getName()
{
    return nameEdit->text();
}

QString EditLayer::getPath()
{
    return pathEdit->text();
}

bool EditLayer::isRecursive()
{
    return recursiveButton->isChecked();
}

QString EditLayer::getFileName()
{
    QString ext = uiExtension->text();
    if (ext == "") ext = "*";
    return uiPrefix->text() + "%" + uiSuffix->text() + "." + ext;
}

bool EditLayer::isSeq()
{
    return seqButton->isChecked();
}

void EditLayer::setType(int t)
{
    if (t == 1) radioButton->setChecked(true);
    else if (t == 2) radioButton_2->setChecked(true);
    else if (t == 3) radioButton_3->setChecked(true);
}

void EditLayer::setName(QString n)
{
    nameEdit->setText(n);
}

void EditLayer::setPath(QString p)
{
    pathEdit->setText(p);
}

void EditLayer::setRecursive(bool r)
{
    recursiveButton->setChecked(r);
}

void EditLayer::setFileName(QString f)
{
    uiPrefix->setText(f.section("%",0,0));
    uiSuffix->setText(f.split("%")[1].section(".",0,0));
    uiExtension->setText(f.section(".",-1));
}

void EditLayer::setSeq(bool s)
{
    seqButton->setChecked(s);
    videoButton->setChecked(!s);
}
