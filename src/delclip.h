#ifndef DELCLIP_H
#define DELCLIP_H

#include "ui_delclip.h"

class DelClip : public QDialog, private Ui::DelClip
{
    Q_OBJECT

public:
    explicit DelClip(QWidget *parent = 0);
};

#endif // DELCLIP_H
