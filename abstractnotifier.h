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
    Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)
    struct NotifType{
        const QByteArray CELLHASCHANGED = "CELL";
        const QByteArray LOOPHASCHANGED = "LOOP";
        const QByteArray TABHASCHANGED = "TAB";
    };
protected:
    QSharedPointer<QTcpSocket> m_sock;

    static const QString CMDFILE;
    static const QString JSON_TARGET;

//    virtual void notification(QString target,QJsonValue value) = 0;
//    virtual void query(QString target,QJsonValue value) = 0;
public:
    static const NotifType notifType;
    static const QString JSON_DATA;
    static const QString JSON_TYPE;
    static const QString TYPE_NOTIFY;
    static const QString TYPE_DB;
    static const QString TYPE_RAMI;
    static const QString TARGET_PANEL;
    static const QString TARGET_CARTRIDGE;
    static const QString RAMI_COLUMN;
    static const QString RAMI_ROW;
    static const QString RAMI_STATE;
    AbstractNotifier();
    AbstractNotifier(QSharedPointer<QTcpSocket> sock);
    void send(QJsonValue value, QString type,QString target);
    void setSocket(QSharedPointer<QTcpSocket> sock);
    void setConnected(bool connected){
        m_connected = connected;
        emit connectedChanged();
    }
    bool connected(){ return m_connected; }
    virtual ~AbstractNotifier(){}

signals:
    void newNotification(QString target,QJsonValue value);
    void newQuery(QString target,QJsonValue value);
    void newRami(QJsonValue value);
    void commandReceived(QVariantMap params);
    void connectedChanged();
//    void connected();
//    void disconnected();

private slots:
    void parse();
    void parseRami(QJsonValue value);
public slots:
    void sendRami(int row, int column, bool state);
    void sendRami(QVariantMap params);
private:
    QJsonDocument wrapWithType(QJsonValue data,QString type,QString target);
    bool m_connected;

};

#endif // NOTIFIER_H
