#include "timecodemanager.h"

TimeCodeManager::TimeCodeManager(QObject *parent) :
    QObject(parent)
{
}

int TimeCodeManager::tcToFrames(QString t,double fps)
{
    QStringList timeCode = t.split(":");
    int f = timeCode[0].toInt()*60*60*fps + timeCode[1].toInt()*60*fps + timeCode[2].toInt()*fps + timeCode[3].toInt();
    return f;
}

QString TimeCodeManager::framesToTc(int f, double fps)
{
    int h = f/fps/60/60;
    int m = f/fps/60-(h*60);
    int s = f/fps-(m*60)-(h*60*60);
    int i = f-(s*fps)-(m*60*fps)-(h*60*60*fps);
    QString hS;
    QString mS;
    QString sS;
    QString iS;
    if (h < 0) hS = "00";
    else if (h<10) hS = "0" + QString::number(h);
    else hS = QString::number(h);
    if (m < 0) mS = "00";
    else if (m<10) mS = "0" + QString::number(m);
    else mS = QString::number(m);
    if (s < 0) sS = "00";
    else if (s<10) sS = "0" + QString::number(s);
    else sS = QString::number(s);
    if (i < 0) iS = "00";
    else if (i<10) iS = "0" + QString::number(i);
    else iS = QString::number(i);

    QString tc = hS + ":" + mS + ":" + sS + ":" + iS;

    return tc;

}

QList<int> TimeCodeManager::framesToTcN(int f, double fps)
{
    int h = f/fps/60.0/60;
    int m = f/fps/60.0-(h*60);
    int s = f/fps-(m*60)-(h*60*60);
    int i = f-(s*fps)-(m*60*fps)-(h*60*60*fps);
    QList<int> tcn;
    tcn << h << m << s << i;

    return tcn;

}

int TimeCodeManager::tcToFrames(QList<int> tcn, double fps)
{
    int f = tcn[0]*60*60*fps + tcn[1]*60*fps + tcn[2]*fps + tcn[3];
    return f;
}

QString TimeCodeManager::tcNtoTc(QList<int> tcn)
{
    int h = tcn[0];
    int m = tcn[1];
    int s = tcn[2];
    int i = tcn[3];
    QString hS;
    QString mS;
    QString sS;
    QString iS;
    if (h < 0) hS = "00";
    else if (h<10) hS = "0" + QString::number(h);
    else hS = QString::number(h);
    if (m < 0) mS = "00";
    else if (m<10) mS = "0" + QString::number(m);
    else mS = QString::number(m);
    if (s < 0) sS = "00";
    else if (s<10) sS = "0" + QString::number(s);
    else sS = QString::number(s);
    if (i < 0) iS = "00";
    else if (i<10) iS = "0" + QString::number(i);
    else iS = QString::number(i);

    QString tc = hS + ":" + mS + ":" + sS + ":" + iS;

    return tc;
}
