#ifndef __BASE64_URL_HPP__
#define __BASE64_URL_HPP__

// Base64����, ����URL�����ַ��������滻
// �ο�: http://tools.ietf.org/html/rfc4648
//
// Author: Brianzhang
// Date: 2010-10-24
//
// Base64�Ļ���ԭ���ǽ�6-bit�Ķ�������ֵ�滻Ϊ64��8-bitASCII�ɴ�ӡ�ַ��е�һ��
// ���, ԭʼ��ֵ��ÿ3��8-bit�ַ�(��4��6-bit��ֵ)�ᱻ�滻Ϊ4��8-bitASCII��, ����󳤶�����1/3
// 
// Ϊ�˺ͱ�׼��Base64���벻ͬ, ���㷨������ASCII��Ĵ���, ���𵽷��ƽ������
// �����滻��ϵ��ʵ���ļ�Base64URL.cpp
//

// Base64URL����, �ɹ�����0
int Base64URLEncode(unsigned char* pszDstBuffer, int& iDstLen, const char* pszSrcBuffer, int iSrcLen);

// Base64URL����, �ɹ�����0
int Base64URLDecode(char* pszDstBuffer, int& iDstLen, const unsigned char* pszSrcBuffer, int iSrcLen);


#endif
