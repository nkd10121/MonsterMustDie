#include "Rigidbody.h"

/// <summary>
/// コンストラクタ
/// </summary>
MyLib::Rigidbody::Rigidbody():
	m_pos(),
	m_nextPos(),
	m_dir(),
	m_velocity(),
	m_useGravity(true)
{
	// 処理なし
}

/// <summary>
/// 初期化
/// </summary>
void MyLib::Rigidbody::Init(bool m_useGravity)
{
	m_pos = Vec3();
	m_dir = Vec3();
	m_velocity = Vec3();
	this->m_useGravity = m_useGravity;
}

/// <summary>
/// velocityに力を加える
/// </summary>
void MyLib::Rigidbody::AddForce(const Vec3& force)
{
	m_velocity = m_velocity + force;
}

/// <summary>
/// VelocityのSetとDir自動計算
/// </summary>
void MyLib::Rigidbody::SetVelocity(Vec3 set, float mul)
{
	m_velocity = set * mul;
	if (m_velocity.SqLength() > 0)
	{
		m_dir = m_velocity.Normalize();
	}
}