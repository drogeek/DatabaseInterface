#ifndef WINMEDIANOTIFIER_H
#define WINMEDIANOTIFIER_H

#include <QDir>
#include <QSqlError>
#include "query2json.h"
#include "abstractnotifier.h"

class ServerNotifier : public AbstractNotifier
{
public:
    ServerNotifier();
    ServerNotifier(QSharedPointer<QTcpSocket> sock);
    void send(QJsonValue value, QString type, QString target);
private:
    void notification(QString target,QJsonValue value);
    void query(QString target,QJsonValue value);
    static const QString QUERY;
};

#endif // WINMEDIANOTIFIER_H
