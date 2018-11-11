#ifndef AUDIO_PLAYER_HEAD_FILE
#define AUDIO_PLAYER_HEAD_FILE

#include "stdafx.h"
#include <mmsystem.h>

class CGameClientViewDX;

#define MAX_SYN_SOUND					8								//异步个数

//声音播放
class CAudioPlayer :public CWnd
{
	//变量定义
private:
	UINT							m_nDeviceType;							//设备类型
	//CGameFrameDlg					* m_pGameFrameDlg;						//游戏框架
	TCHAR							m_szAudioName[MAX_PATH];				//语音名称

	//函数定义
public:
	//构造函数
	CAudioPlayer();
	//析构函数
	virtual ~CAudioPlayer();

	//配置接口
public:
	//设置框架
	//void SetGameFrameDlg(CGameFrameDlg *pGameFrameDlg) {m_pGameFrameDlg=pGameFrameDlg;}

	//功能函数
public:
	//重置语音
	bool Resume();
	//暂停语音
	bool Pause();
	//停止语音
	bool Stop();
	//关闭语音
	bool Close();
	//播放语音
	bool Play(LPCTSTR pszFileName, bool bLoop=false);
	//重播语音
	bool RePlay();
	//返回类型
	UINT GetDeviceType() {return m_nDeviceType;}
	//获取名称
	LPCTSTR GetAudioName() {return m_szAudioName;}

	//消息映射
private:
	//语音回调
	LRESULT OnMCINotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#endif