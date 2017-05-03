#include "optionxml.h"

OptionXML::OptionXML()
{
    if(m_settings.contains("db/ip"))
        m_dbIp = m_settings.value("db/ip").toString();
    if(m_settings.contains("db/port"))
        m_dbPort = m_settings.value("db/port").toInt();
    if(m_settings.contains("db/user"))
        m_dbUser = m_settings.value("db/user").toString();
    if(m_settings.contains("db/pass"))
        m_dbPass = m_settings.value("db/pass").toString();


    if(m_settings.contains("app/ip"))
        m_appIp = m_settings.value("app/ip").toString();
    if(m_settings.contains("app/port"))
        m_appPort = m_settings.value("app/port").toInt();


    if(m_settings.contains("win/port"))
        m_winPort = m_settings.value("win/port").toInt();
}

void OptionXML::persistConfig(QString type){
    if(type == "db"){
        m_settings.setValue(type+"/ip", m_dbIp);
        m_settings.setValue(type+"/port", m_dbPort);
        m_settings.setValue(type+"/user", m_dbUser);
        m_settings.setValue(type+"/pass", m_dbPass);
        emit dbConfigChanged();
    }

    else if(type == "app"){
        m_settings.setValue(type+"/ip", m_appIp);
        m_settings.setValue(type+"/port", m_appPort);
        emit appConfigChanged();
    }

    else if(type == "win"){
        m_settings.setValue(type+"/port", m_winPort);
        emit winConfigChanged();
    }

    else{
        qDebug() << "unknown type, only ip, port and user accepted";
    }
}

QVariant OptionXML::value(const QString &key, const QVariant &defaultValue){
    return m_settings.value(key,defaultValue);
}

void OptionXML::setValue(const QString &key, const QVariant &value){
    m_settings.setValue(key,value);
}
