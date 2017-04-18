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
private:
    void notification(QString target,QJsonValue value) override;
    void query(QString target,QJsonValue value) override;
    void send(QJsonValue value, QString type, QString target);
    static const QString QUERY;
};

#endif // WINMEDIANOTIFIER_H
