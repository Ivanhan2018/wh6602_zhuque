#ifndef GLOBAL_CHANNEL_H
#define GLOBAL_CHANNEL_H

// 通向游戏服务端的通道，可以传递任何简易数据或成员指针
// CTGS 是意思是 channel_to_game_service
#define CTGS_MAIN_LOG 100		// 日志
#define CTGS_ASS_LOG_POINT 100	// 日志组件的指针


// @yexiuxuan add
#define CTGS_MAIN_TCP	101    // 通讯
#define CTCS_ASS_TCP_POINT 100 //通讯组件的指针

#define CTCS_MAIN_GOODRAND  102  //随机数
#define CTCS_MAIN_GOODRAND_POINT	100  // 随机数组件

// @yexiuxuan end

#endif