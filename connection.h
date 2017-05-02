#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QWeakPointer>
#include "ramiProtocol.h"

class Connection : public QObject
{

    Q_OBJECT
    Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)
public:
    Connection();
    void receive();
    void setSocket(QSharedPointer<QTcpSocket> socket);
    void setConnected(bool connected){
        m_connected = connected;
        emit connectedChanged();
    }
    bool connected(){ return m_connected; }

public slots:
    void send(int row, int column, bool state);
    void disconnect();
signals:
    void commandReceived(QVariantMap params);
    void connectedChanged();
private:
    QSharedPointer<QTcpSocket> m_sock;
    bool m_connected;
};

#endif // CONNECTION_H
