#ifndef __GAME_PROTOCOL_DEF_HPP__
#define __GAME_PROTOCOL_DEF_HPP__

#include "TdrKeywords.h"
#include "S_GameConfig.h"

//ս����λ·������������30����
const int MAX_POSITION_NUMBER_IN_PATH = 30;

//ս���ϵ�λ��λ��������Ϣ
struct TUNITPOSITION
{
    int iPosX;          //ս���ϵ�λ��X����
    int iPosY;          //ս���ϵ�λ��Y����

    TUNITPOSITION()
    {
        memset(this, 0, sizeof(*this));
    };

    TUNITPOSITION(int iX, int iY)
    {
        iPosX = iX;
        iPosY = iY;
    };

    TUNITPOSITION(const TUNITPOSITION& stPos)
    {
        iPosX = stPos.iPosX;
        iPosY = stPos.iPosY;
    };
};

//ս���ϵ�λ��·����Ϣ
struct TUNITPATH
{
    int iPosNum;                                        //·���ϵĵ����Ŀ
    TUNITPOSITION astPos[MAX_POSITION_NUMBER_IN_PATH];  //��λ·���ϵĵ���Ϣ

    TUNITPATH()
    {
        memset(this, 0, sizeof(*this));
    };
};

#endif

