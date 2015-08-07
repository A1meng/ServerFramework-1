/**
*@file ErrorNOUtility.hpp
*@author jasonxiong
*@date 2009-11-11
*@version 1.0
*@brief �����빤���࣬���������ģ����������������������Ӧ������Ϣ
*
*
*/

#ifndef __ERROR_NO_UTIL_HPP__
#define __ERROR_NO_UTIL_HPP__

namespace ServerLib
{
//!ÿһ��ģ�����������Ĵ���Ÿ���
const int MAX_ERROR_NO_INFO_PER_MODULE = 64;

const int MAX_ERROR_MODULE_NUMBER = 128;

class CErrorNOUtility
{
    //!����������������Ӧ��Ϣ
    typedef struct tagErrorNOInfo
    {
        int m_iErrorNO;
        char* m_pszErrorMsg;
    } TErrorNOInfo;

    //!����һ��ģ������д�����Ϣ�Ľṹ
    typedef struct tagErrorModuleInfo
    {
        int m_iModuleID; //!<ģ��ID
        int m_iErrorNONum; //!<��ģ�鵱ǰ����Ĵ���Ÿ���
        TErrorNOInfo m_aszErrorNOInfo[MAX_ERROR_NO_INFO_PER_MODULE]; //!<������Ϣ
    } TErrorModuleInfo;

public:
    CErrorNOUtility();
    ~CErrorNOUtility();

public:


};

}

#endif //__ERROR_NO_UTIL_HPP__
///:~
