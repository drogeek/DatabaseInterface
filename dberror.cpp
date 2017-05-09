#include "dberror.h"


dbError::dbError():
    m_msg ("There is a problem with the database")
{
}


const char * dbError::what() const throw(){
    return m_msg.c_str();
}
