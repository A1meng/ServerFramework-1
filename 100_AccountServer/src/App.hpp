#ifndef __APP_HPP__
#define __APP_HPP__

// Ӧ�ó��������
class CApp
{
protected:
    CApp() {}

public:
    virtual ~CApp() {}

    // ��صĳ�ʼ����bResume��ʾ�Ƿ�ֱ��attach�������ϴ��˳���δɾ���Ĺ����ڴ�
    // ����ֵΪ0��ʾ�ɹ���������ʾʧ��
    virtual int Initialize(bool bResume, int iWorldID)
    {
        return 0;
    }

    // ���У�һ��Ϊ��ѭ��: ����code->����->���ʹ�����
    virtual void Run() = 0;
};

#endif // __APP_HPP__

