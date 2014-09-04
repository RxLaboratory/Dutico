#include "addclip.h"
#include "timecodemanager.h"

AddClip::AddClip(QList<QStringList> l, double f, QWidget *parent) :
    QDialog(parent)
{
    progra = true;
    setupUi(this);

    liste = l;
    fps = f;
    //lister les plans
    for (int i=0;i<liste.count();i++)
    {
        listeBox->addItem(liste[i][0]);
    }
    progra = false;
}

void AddClip::on_listeBox_currentIndexChanged(int index)
{
    tcinText->setText(liste[index][1]);
}

void AddClip::on_afterButton_toggled(bool checked)
{
    listeBox->setEnabled(checked);
}

void AddClip::calcDuration()
{
    if (progra) return;

    progra = true;
    int framesIn = TimeCodeManager::tcToFrames(tcinText->text(),fps);
    QList<int> tcOut;
    tcOut << hours->value() << minutes->value() << seconds->value() << frames->value();
    int framesOut = TimeCodeManager::tcToFrames(tcOut,fps);
    int dur = framesOut-framesIn;
    QList<int> duration = TimeCodeManager::framesToTcN(dur,fps);
    hours_2->setValue(duration[0]);
    minutes_2->setValue(duration[1]);
    seconds_2->setValue(duration[2]);
    frames_2->setValue(duration[3]);
    progra = false;
}

void AddClip::calcTCOut()
{
    if (progra) return;

    progra = true;
    int framesIn = TimeCodeManager::tcToFrames(tcinText->text(),fps);
    QList<int> duration;
    duration << hours_2->value() << minutes_2->value() << seconds_2->value() << frames_2->value();
    int dur = TimeCodeManager::tcToFrames(duration,fps);
    int framesOut = framesIn + dur;
    QList<int> tcout = TimeCodeManager::framesToTcN(framesOut,fps);
    hours->setValue(tcout[0]);
    minutes->setValue(tcout[1]);
    seconds->setValue(tcout[2]);
    frames->setValue(tcout[3]);
    progra = false;

}

void AddClip::on_hours_valueChanged(int arg1)
{
    calcDuration();
}

void AddClip::on_minutes_valueChanged(int arg1)
{
    calcDuration();
}

void AddClip::on_seconds_valueChanged(int arg1)
{
    calcDuration();
}

void AddClip::on_frames_valueChanged(int arg1)
{
    calcDuration();
}

void AddClip::on_hours_2_valueChanged(int arg1)
{
    calcTCOut();
}

void AddClip::on_minutes_2_valueChanged(int arg1)
{
    calcTCOut();
}

void AddClip::on_seconds_2_valueChanged(int arg1)
{
    calcTCOut();
}

void AddClip::on_frames_2_valueChanged(int arg1)
{
    calcTCOut();
}

QStringList AddClip::getClip()
{
    QStringList c;
    c << nomText->text();
    c << tcinText->text();
    QList<int> tcout;
    tcout << hours->value() << minutes->value() << seconds->value() << frames->value();
    c << TimeCodeManager::tcNtoTc(tcout);
    QList<int> dur;
    dur << hours_2->value() << minutes_2->value() << seconds_2->value() << frames_2->value();
    c << TimeCodeManager::tcNtoTc(dur);
    c << effectText->text();
    c << comText->toPlainText();
    QList<int> sourcetcin;
    sourcetcin << hours_3->value() << minutes_3->value() << seconds_3->value() << frames_3->value();
    c << TimeCodeManager::tcNtoTc(sourcetcin);
    QList<int> sourcetcout;
    sourcetcout << hours_4->value() << minutes_4->value() << seconds_4->value() << frames_4->value();
    c << TimeCodeManager::tcNtoTc(sourcetcout);
    return c;
}

bool AddClip::insert()
{
    return insertBox->isChecked();
}

int AddClip::getInsertClip()
{
    if (startButton->isChecked()) return -1;
    else if (endButton->isChecked()) return liste.count()-1;
    else return listeBox->currentIndex();
}
