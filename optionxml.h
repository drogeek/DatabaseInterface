#ifndef OPTIONXML_H
#define OPTIONXML_H

#include <QObject>
#include <QDebug>
#include <QVariant>
#include <QSettings>

class OptionXML : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString dbIp MEMBER m_dbIp READ dbIp WRITE setDbIp NOTIFY dbIpChanged)
    Q_PROPERTY(quint16 dbPort MEMBER m_dbPort READ dbPort WRITE setDbPort NOTIFY dbPortChanged)
    Q_PROPERTY(QString dbUser MEMBER m_dbUser READ dbUser WRITE setDbUser  NOTIFY dbUserChanged)
    Q_PROPERTY(QString dbPass MEMBER m_dbPass READ dbPass WRITE setDbPass  NOTIFY dbPassChanged)

    Q_PROPERTY(QString appIp MEMBER m_appIp READ appIp WRITE setAppIp  NOTIFY appIpChanged)
    Q_PROPERTY(quint16 appPort MEMBER m_appPort READ appPort WRITE setAppPort  NOTIFY appPortChanged)

    Q_PROPERTY(quint16 winPort MEMBER m_winPort READ winPort WRITE setWinPort  NOTIFY winPortChanged)

public:
    //Accessors
    /* *db* */
    OptionXML();
    QVariant value(const QString &key, const QVariant &defaultValue=QVariant());
    void setValue(const QString &key, const QVariant &value);

    QString dbIp(){ return m_dbIp; }
    void setDbIp(QString ip){ m_dbIp = ip; emit dbIpChanged(); }

    quint16 dbPort(){ return m_dbPort; }
    void setDbPort(quint16 port){ m_dbPort = port; emit dbPortChanged(); }

    QString dbUser(){ return m_dbUser; }
    void setDbUser(QString user){ m_dbUser = user; emit dbUserChanged(); }

    QString dbPass(){ return m_dbPass; }
    void setDbPass(QString pass){ m_dbPass = pass; emit dbPassChanged(); }

    /* *app* */
    QString appIp(){ return m_appIp; }
    void setAppIp(QString ip){ m_appIp = ip; emit appIpChanged(); }

    quint16 appPort(){ return m_appPort; }
    void setAppPort(quint16 port){ m_appPort = port; emit appPortChanged(); }


    /* *win* */
    quint16 winPort(){ return m_winPort; }
    void setWinPort(quint16 port){ m_winPort = port; emit winPortChanged(); }


public slots:
    void persistConfig(QString type);
signals:
    void dbIpChanged();
    void dbPortChanged();
    void dbUserChanged();
    void dbPassChanged();

    void winPortChanged();

    void appIpChanged();
    void appPortChanged();

    void dbConfigChanged();
    void appConfigChanged();
    void winConfigChanged();
private:
    QString m_dbIp;
    quint16 m_dbPort;
    QString m_dbUser;
    QString m_dbPass;

    quint16 m_winPort;

    QString m_appIp;
    quint16 m_appPort;
    QSettings m_settings;
};

#endif // OPTIONXML_H
