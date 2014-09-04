#ifndef DUFSQLQUERY_H
#define DUFSQLQUERY_H

#include <QSqlQuery>

class DufSqlQuery : public QSqlQuery
{
public:
    explicit DufSqlQuery(const QString & query = QString(), QSqlDatabase db = QSqlDatabase());
    explicit DufSqlQuery(QSqlDatabase db = QSqlDatabase());
    explicit DufSqlQuery( QSqlResult * result );
    explicit DufSqlQuery(const DufSqlQuery & other );
    bool verif();
    
    
};

#endif // DUFSQLQUERY_H
