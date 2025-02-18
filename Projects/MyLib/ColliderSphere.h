#pragma once
#include "ColliderBase.h"

namespace MyLib
{
	/// <summary>
	/// 旧型の当たり判定
	/// </summary>
	class ColliderSphere : public ColliderBase
	{
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="isTrigger">トリガーかどうか</param>
		ColliderSphere(bool isTrigger);

		/// <summary>
		/// 半径を設定
		/// </summary>
		/// <param name="radius"></param>
		void SetRadius(float radius) { m_radius = radius; }
	public:
		float m_radius;	//半径
	};
}
