#ifndef __ACCOUNT_UIN_OBJ_HPP__
#define __ACCOUNT_UIN_OBJ_HPP__

#include "RegAuthPublic.hpp"
#include "ObjAllocator.hpp"
#include "HashMap_KString.hpp"

using namespace ServerLib;

//todo jasonxiong ���������ַ����洢��Ҫ���ݺ���ʵ���ڴ�ʹ�õ���������Ż�
//todo jasonxiong �������ݽ��벻ͬ��ƽ̨�ʺţ�������Ҫ������֮�����������֤��ʽ

class CAccountUinObj : public CObj
{
private:
    //todo jasonxiong TDataString ���滺����ַ����ĳ��ȿ��Ը��ݺ������ʺ�ƽ̨��ʵ����Ҫ�����Ż��������˷��ڴ�
    TDataString m_stAccountID;

    //��ҵ�uin
    unsigned int m_uiUin;

    //��ҵ�password�Ļ��棬����ÿ����֤����Ҫ�������ݿ�
    //todo jasonxiong PASSWORD����󳤶Ⱥ��ʺŵ���󳤶ȿ��ܲ�һ��������Ҳ��Ҫ��ʵ�ʵ��ַ��������Ż�
    TDataString m_stPassword;

public:
    CAccountUinObj();
    virtual ~CAccountUinObj();

public:
    virtual int Initialize();

public:
    const TDataString& GetKey() const;
    void SetKey(const TDataString& stKey);

    const TDataString& GetPassword() const;
    void SetPassword(const TDataString& stPassword);

    const unsigned int GetUin() const;
    void SetUin(unsigned int uiUin);

    DECLARE_DYN
};

#endif // __ROLE_NUMBER_OBJ_HPP__

