#ifndef DBERROR_H
#define DBERROR_H

#include <exception>
#include <QSqlError>

class dbError: public std::exception
{
public:
    dbError();
private:
    const std::string m_msg;
    virtual const char* what() const throw() override;
};

#endif // DBERROR_H
