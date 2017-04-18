#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QTcpSocket>
#include <QSharedPointer>
#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
#include <QSharedPointer>

/* To use, subclass this class, implement "query" and "notification" methods
 * without forgetting to call wrapWithType to wrap the message if you send it
 * on the network via m_sock */
class AbstractNotifier: public QObject
{
    Q_OBJECT

    struct NotifType{
        const QByteArray CELLHASCHANGED = "CELL";
        const QByteArray LOOPHASCHANGED = "LOOP";
        const QByteArray TABHASCHANGED = "TAB";
    };
protected:
    QSharedPointer<QTcpSocket> m_sock;

    static const QString CMDFILE;
    static const QString JSON_TARGET;

    virtual void notification(QString target,QJsonValue value) = 0;
    virtual void query(QString target,QJsonValue value) = 0;
public:
    static const NotifType notifType;
    static const QString JSON_DATA;
    static const QString JSON_TYPE;
    static const QString NOTIFY;
    static const QString DB;
    static const QString PANEL;
    static const QString CARTRIDGE;
    AbstractNotifier();
    AbstractNotifier(QSharedPointer<QTcpSocket> sock);
    void send(QJsonValue value, QString type,QString target);
    void setSocket(QSharedPointer<QTcpSocket> sock);
    virtual ~AbstractNotifier(){}

signals:
    void newNotification(QString target,QJsonValue value);
    void newQuery(QString target,QJsonValue value);

private slots:
    void parse();
private:
    QJsonDocument wrapWithType(QJsonValue data,QString type,QString target);

};

#endif // NOTIFIER_H
