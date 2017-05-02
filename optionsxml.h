#ifndef OPTIONSXML_H
#define OPTIONSXML_H

#include <QObject>
#include <QDomElement>
#include <QDomDocument>
#include <QFile>
#include <QDebug>

class OptionsXML : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serverIp MEMBER m_serverIp NOTIFY serverIpChanged)
    Q_PROPERTY(qint16 serverPort MEMBER m_serverPort NOTIFY serverPortChanged)
public:
    OptionsXML();
    QString serverIp(){
        return m_serverIp;
    }
    qint16 serverPort(){
        return m_serverPort;
    }
    void setServerIp(QString ip){
        m_serverIp = ip;
        emit serverIpChanged();
    }
    void setServerPort(qint16 port){
        m_serverPort = port;
        emit serverPortChanged();
    }

public slots:
    void readFromFile();
    void writeToFile();
signals:
    void serverPortChanged();
    void serverIpChanged();
    void configChanged();
private:
    static const QString DELEGATEIP;
    static const QString DELEGATEPORT;
    static const QString WINMEDIAIP;
    static const QString WINMEDIAPORT;
    static const QString APPIP;
    static const QString APPPORT;
    QString m_Ip;
    qint16 m_serverPort;
    QDomDocument m_doc;

};

#endif // OPTIONSXML_H
