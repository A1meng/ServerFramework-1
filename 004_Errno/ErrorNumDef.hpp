#ifndef __ERROR_NUM_DEF_HPP__
#define __ERROR_NUM_DEF_HPP__

#define T_SERVER_SUCESS   0x0

//˵���� ��ӡ��������ʹ�� "0x%"
//����������Ϊ app(����������, 8λ) + module(ģ�����ͣ�12λ) + error(�����룬12λ)

#define MAKE_ERRNO(app, module, error) (((app&0xFF)<<24) | ((module&0xFFF)<<12)| (error&0xFFF))

enum EN_SERVER_DEF
{
    EN_SERVER_COMMON    = 0x01,                //ͨ�����͵Ĵ�����

    EN_SERVER_ACCOUNT   = 0x02,                //��¼������
    EN_SERVER_GATEWAY   = 0x03,                //���������
    EN_SERVER_DATABASE  = 0x04,                //���ݿ������
    EN_SERVER_ZONE      = 0x05,                 //��������
    EN_SERVER_WORLD     = 0x06,                 //���������
    EN_SERVER_ROLEDB    = 0x07,                 //��ɫ���ݿ���·�����
    EN_SERVER_REGAUTH   = 0x08,                 //ע�����֤������
    EN_SERVER_ACCOUNTDB = 0x09,                 //�ʺ����ݿ������
    EN_SERVER_NAMEDB    = 0x0a,                 //�������ݿ������
};

enum EN_COMMON_MODULE_DEF
{
    EN_COMMON_MODULE_SYS = 0x001,               //ϵͳ������
};

#define TSVR_SYSTEM_BASE MAKE_ERRNO(EN_SERVER_COMMON, EN_COMMON_MODULE_SYS, 0)
enum COMMON_SYSTEM_ERR_DEF
{
    T_COMMON_SYSTEM_PARA_ERR        = TSVR_SYSTEM_BASE + 0x01,      //��������
    T_COMMON_SYSTEM_FUNC_ERR        = TSVR_SYSTEM_BASE + 0x02,      //ϵͳִ�д���

    T_COMMON_LOG_CHKLEVEL_ERR       = TSVR_SYSTEM_BASE + 0x03,      //��ӡ��־�ȼ�����
    T_COMMON_LOG_OPENFILE_ERR       = TSVR_SYSTEM_BASE + 0x04,      //����־�ļ�����

    T_COMMON_FILE_GETSTAT_ERR       = TSVR_SYSTEM_BASE + 0x05,      //��ȡ�ļ���״̬����
    T_COMMON_FILE_REMOVE_ERR        = TSVR_SYSTEM_BASE + 0x06,      //ɾ���ļ�����
    T_COMMON_FILE_RENAME_ERR        = TSVR_SYSTEM_BASE + 0x07,      //�������ļ�����

    T_COMMON_ZMQBUS_CONNECT_ERR     = TSVR_SYSTEM_BASE + 0x08,      //Zmq bug���Ӵ���
};

#endif
