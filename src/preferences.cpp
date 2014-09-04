#include "preferences.h"

Preferences::Preferences(double v, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAEVersion(v);
}

void Preferences::setAEVersion(double v)
{
    if (v == 8) versionBox->setCurrentIndex(1);
    else if (v == 9) versionBox->setCurrentIndex(2);
    else if (v == 10) versionBox->setCurrentIndex(3);
    else if (v == 10.5) versionBox->setCurrentIndex(4);
    else if (v == 11) versionBox->setCurrentIndex(5);
    else if (v == 12) versionBox->setCurrentIndex(6);
    else if (v == 13) versionBox->setCurrentIndex(7);
    else versionBox->setCurrentIndex(0);
}

double Preferences::getAEVersion()
{
    if (versionBox->currentIndex() == 1) return 8;
    if (versionBox->currentIndex() == 2) return 9;
    if (versionBox->currentIndex() == 3) return 10;
    if (versionBox->currentIndex() == 4) return 10.5;
    if (versionBox->currentIndex() == 5) return 11;
    if (versionBox->currentIndex() == 6) return 12;
    if (versionBox->currentIndex() == 7) return 13;
    else return 0;
}
