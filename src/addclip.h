#ifndef ADDCLIP_H
#define ADDCLIP_H

#include "ui_addclip.h"
#include <QStringList>

class AddClip : public QDialog, private Ui::AddClip
{
    Q_OBJECT

public:
    explicit AddClip(QList<QStringList> l, double f, QWidget *parent = 0);
    QStringList getClip();
    bool insert();
    int getInsertClip();

private slots:
    void on_listeBox_currentIndexChanged(int index);
    void on_afterButton_toggled(bool checked);
    void on_hours_valueChanged(int arg1);
    void on_minutes_valueChanged(int arg1);
    void on_seconds_valueChanged(int arg1);
    void on_frames_valueChanged(int arg1);
    void on_hours_2_valueChanged(int arg1);
    void on_minutes_2_valueChanged(int arg1);
    void on_seconds_2_valueChanged(int arg1);
    void on_frames_2_valueChanged(int arg1);

private:
    QList<QStringList> liste;
    void calcDuration();
    void calcTCOut();
    double fps;
    bool progra;
};

#endif // ADDCLIP_H
