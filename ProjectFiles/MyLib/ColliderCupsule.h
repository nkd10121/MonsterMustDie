#pragma once
#include "ColliderBase.h"
#include "Vec3.h"

namespace MyLib
{
	/// <summary>
	/// カプセル型の当たり判定
	/// </summary>
	class ColliderCupsule : public ColliderBase
	{
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="isTrigger">トリガーかどうか</param>
		ColliderCupsule(bool isTrigger);

	public:
		float m_size;	//大きさ
		float m_radius;	//半径
	};
}


