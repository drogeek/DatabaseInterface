#ifndef WINMEDIANOTIFIER_H
#define WINMEDIANOTIFIER_H

#include <QDir>
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
};

#endif // WINMEDIANOTIFIER_H
