#ifndef PUBLIC_DEF_H
#define PUBLIC_DEF_H

#pragma once


//模块定义
#ifdef _DEBUG
    #define GAME_FRAME_DLL_NAME	TEXT("GameFrameD.dll")	//组件 DLL 名字
#else
    #define GAME_FRAME_DLL_NAME	TEXT("GameFrame.dll")	//组件 DLL 名字
#endif

#endif