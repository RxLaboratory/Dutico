#ifndef EDITLAYER_H
#define EDITLAYER_H

#include "ui_editlayer.h"

class EditLayer : public QDialog, private Ui::EditLayer
{
    Q_OBJECT

public:
    explicit EditLayer(QWidget *parent = 0);
    void setUiFileName(QString n);
    void setType(int t);
    void setName(QString n);
    void setPath(QString p);
    void setRecursive(bool r);
    void setFileName(QString f);
    void setSeq(bool s);
    int getType();
    QString getName();
    QString getPath();
    bool isRecursive();
    QString getFileName();
    bool isSeq();

private slots:
    void on_radioButton_toggled(bool checked);
    void on_radioButton_2_toggled(bool checked);
    void on_radioButton_3_toggled(bool checked);
    void on_browseButton_clicked();
};

#endif // EDITLAYER_H
