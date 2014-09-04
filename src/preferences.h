#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "ui_preferences.h"

class Preferences : public QDialog, private Ui::Preferences
{
    Q_OBJECT

public:
    explicit Preferences(double v = 0, QWidget *parent = 0);
    void setAEVersion(double v);
    double getAEVersion();
};

#endif // PREFERENCES_H
