
//
// 2009
// DhLj
//

#ifndef _ROLES_H_
#define _ROLES_H_

#include <afxwin.h> 
#include <afxext.h> 
#include <afxdisp.h>
#include "TagRoles.h"


//转换牌类
class Roles
{
public:
	Roles( void );
	virtual ~Roles( void );

	CARDSTATE m_Lard;												// dz状态,是不是地主
	CARDSTATE m_Push;												// 出牌状态,是不是该出牌
	AIPOS m_pos;													//自己的位置

	void SetCards( IVector& ivIn );							// 排序			
	bool PushCard( const RCVector rcIn, const CARDCLASS cc );		// 出牌
	void ChooseCards( const int iNum );								// 点击取牌
	void DeleteChoose( void );										// 清空选牌
	inline CVector GetCards( void ) { return m_Cards; }				// 取牌面
	inline RCVector GetRCards( void ) { return m_RCards; }			// 取选牌(转换)
	inline IVector GetChCards( void ) { return m_ivTemp; }			// 取选牌
	inline CARDCLASS GetCardClass( void ) { return m_ccTemp; }		// 取选牌的类型

	//附加
	CARDCLASS GetCardRCVector(const RCVector& rcVec);
	bool ChangeCardToIvVertor(const IVector& ivIn, RCVector& rcVec);
	bool FindLinkTemp(const RCVector& rcVec, const int iOr, const int iF, const int iL );

private:		
	CVector m_Cards;												// 牌面.size()牌的数量
	IVector m_ivTemp;												// 选中牌
	RCVector m_RCards;												// 选中牌转换
	CARDCLASS m_ccTemp;												// 选中牌类

	void ChangeCard( void );										// 转换牌
	CARDCLASS SelectClass( void );									// 牌面分类
	bool FindLink( const int iOr, const int iF, const int iL );		// 找顺牌
	bool GetChoose( void );											// 取选中牌
	void DeleteCard( void );										// 删牌
	bool CompareCard( RCVector rcIn, CARDCLASS cc ) const ;			// 比较牌面
};


#endif