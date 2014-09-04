#ifndef TIMECODEMANAGER_H
#define TIMECODEMANAGER_H

#include <QObject>
#include <QStringList>

class TimeCodeManager : public QObject
{
    Q_OBJECT
public:
    explicit TimeCodeManager(QObject *parent = 0);
    static QString framesToTc(int f, double fps);
    static QList<int> framesToTcN(int f, double fps);
    static int tcToFrames(QString t, double fps);
    static int tcToFrames(QList<int> tcn, double fps);
    static QString tcNtoTc(QList<int> tcn);

signals:

public slots:

};

#endif // TIMECODEMANAGER_H
