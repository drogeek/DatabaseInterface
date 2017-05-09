#ifndef QUERY2JSON_H
#define QUERY2JSON_H

#include <QObject>
#include <QSqlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>
#include "dberror.h"

class Query2Json
{
public:
    static const QJsonArray exec(const QByteArray sqlquery);
};
#endif // QUERY2JSON_H
