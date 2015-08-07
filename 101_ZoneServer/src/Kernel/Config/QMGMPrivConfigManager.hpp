#ifndef __QM_GM_PRIV_CONFIG_MANAGER_HPP__
#define __QM_GM_PRIV_CONFIG_MANAGER_HPP__

//GMȨ�޿������ù�����

//ӵ��GMȨ�޵�IP�Ŷ�
struct GMIPSection
{
    unsigned uIPBegin;
    unsigned uIPEnd;

    GMIPSection()
    {
        memset(this, 0, sizeof(*this));
    }
};

//���֧�ֵ�GM��ЧIP�Ŷε�����
const int MAX_VALID_GM_IP_SECTION = 30;

//ӵ��GMȨ�޵�IP�б�
struct GMValidIPList
{
    int iIPSectionNum;
    GMIPSection stIPSections[MAX_VALID_GM_IP_SECTION];

    GMValidIPList()
    {
        memset(this, 0, sizeof(*this));
    };
};

//���֧�ֵ�GM��Чuin������
const int MAX_VALID_GM_USER_NUM = 1000;

//ӵ��GMȨ�޵�����б�
struct GMValidUserList
{
    int iValidUserNum;
    unsigned int uValidUin[MAX_VALID_GM_USER_NUM];

    GMValidUserList()
    {
        memset(this, 0, sizeof(*this));
    };
};

//GM���Ȩ������
struct QMGMUserPrivConfig
{
    GMValidIPList stValidIpSection;
    GMValidUserList stValidUsers;

    QMGMUserPrivConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

class CQMGMPrivConfigManager
{
public:
    CQMGMPrivConfigManager();
    ~CQMGMPrivConfigManager();

    //����GMȨ�������ļ�
    int LoadGMPrivConfig();

    //�ж��Ƿ�����Ч��GM�û���IP
    bool CheckIsGMIP(unsigned int uClientIP);

    //�ж��Ƿ�����Ч��GM�û�uin
    bool CheckIsGMUin(unsigned int uin);
    
private:
    static QMGMUserPrivConfig m_stGMPrivConfig;
};

#endif
