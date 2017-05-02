#ifndef DATABASEACCESS_H
#define DATABASEACCESS_H

#include <QObject>
#include <QSqlDatabase>
#include <QTimer>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

class DataBaseAccess: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    static const QString DRIVER;
    static const QString NAME;
    static const QString IP;
public:
    DataBaseAccess();
    DataBaseAccess(QString ip,qint16 port,QString user, QString pass);
    void setDatabase(QString ip,qint16 port,QString user, QString pass);
    bool connected(){ return m_connected; }

public slots:
    void open();

private:
    QSqlDatabase m_db;
    QTimer m_timer;
    bool m_connected;
    void setConnected(bool connected){
        m_connected = connected;
        emit connectedChanged();
    }
signals:
    void error(QString err);
    void configChanged();
    void connectedChanged();
};

#endif // DATABASEACCESS_H
