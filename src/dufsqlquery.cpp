#include "dufsqlquery.h"
#include <QSqlError>
#include <QMessageBox>

DufSqlQuery::DufSqlQuery(const QString &query, QSqlDatabase db) :
    QSqlQuery(query,db)
{
}


DufSqlQuery::DufSqlQuery(QSqlDatabase db) :
    QSqlQuery(db)
{
}

DufSqlQuery::DufSqlQuery( QSqlResult * result ) :
    QSqlQuery(result)
{
}

DufSqlQuery::DufSqlQuery(const DufSqlQuery & other ) :
    QSqlQuery(other)
{
}

bool DufSqlQuery::verif()
{
    QSqlError erreur = this->lastError();
    if (erreur.type() == QSqlError::NoError) return true;
    if (erreur.type() == QSqlError::ConnectionError)
    {
        QMessageBox::critical(0,"Probleme de connexion","Attention, probleme d'acces au reseau\n\nVeuillez verifier votre connexion");
    }
    if (erreur.type() == QSqlError::StatementError)
    {
        QMessageBox::warning(0,"Erreur d'appel a la base de donnees","Il y a une erreur dans l'appel a la base de donnees.\n\nMessage de Sql :\n" + erreur.databaseText());
    }
    if (erreur.type() == QSqlError::TransactionError)
    {
        QMessageBox::warning(0,"Erreur d'echange avec la base de donnees","Il y a une erreur de connexion avec la base de donnes, veuillez essayer a nouveau un peu plus tard.");
    }
    if (erreur.type() == QSqlError::UnknownError)
    {
        QMessageBox::warning(0,"Erreur inconnue","Une erreur inconnue vient de se produire avec la base de donnes.\n\nErreur :\n" + erreur.databaseText());
    }
    if (erreur.type() == -1)
    {
        QMessageBox::warning(0,"Erreur indefinie","Une erreur inconnue vient de se produire avec la base de donnes.\n\nErreur :\n" + erreur.databaseText());
    }
    return false;
}
