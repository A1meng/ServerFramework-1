
#ifndef __SESSION_DEF_HPP__
#define __SESSION_DEF_HPP__



const int NETHEAD_V2_SIZE = 24;

typedef struct tagNetHead_V2
{ 
	unsigned int	m_uiSocketFD; //!<�׽���
	unsigned int	m_uiSocketTime;	//!<�׽��ִ���ʱ��
	unsigned int	m_uiSrcIP; //!<Դ��ַ
	unsigned short	m_ushSrcPort; //!<Դ�˿�
	unsigned short	m_ushReservedValue01; //!<�ֽڶ��룬δ��
	unsigned int	m_uiCodeTime; //!<��Ϣʱ��
	unsigned int	m_uiCodeFlag; //!<��Ϣ��־������ʵ���׽��ֿ���
}TNetHead_V2;



#endif
