#include "MyLib.h"
#include "DxLib.h"

std::list<MyLib::DebugDraw::SphereInfo>	MyLib::DebugDraw::m_sphereInfo;
std::list<MyLib::DebugDraw::CupsuleInfo> MyLib::DebugDraw::m_cupsuleInfo;

/// <summary>
/// 描画情報の削除
/// </summary>
void MyLib::DebugDraw::Clear()
{
	//球体の描画情報リストの中身を全削除
	m_sphereInfo.clear();
	m_cupsuleInfo.clear();
}

/// <summary>
/// 描画
/// </summary>
void MyLib::DebugDraw::Draw3D()
{
#ifdef _DEBUG	//デバッグ描画
	//球体の描画情報リストにある情報分描画する
	for (auto& sphere : m_sphereInfo)
	{
		DrawSphere3D(sphere.center.ToVECTOR(), sphere.radius, 8, sphere.color, sphere.color, false);
	}

	//カプセルの描画情報リストにある情報分描画する
	for (auto& cupsule : m_cupsuleInfo)
	{
		DrawCapsule3D(cupsule.pos1.ToVECTOR(), cupsule.pos2.ToVECTOR(), cupsule.radius, 4, cupsule.color, cupsule.color, false);
	}
#endif
}

/// <summary>
/// 球体の描画リストに追加する
/// </summary>
void MyLib::DebugDraw::AddDrawSphere(const Vec3& center, const float& radius, const unsigned int& color)
{
	//球体の描画リストに追加する
	SphereInfo addInfo;
	addInfo.center = center;
	addInfo.radius = radius;
	addInfo.color = color;
	m_sphereInfo.emplace_back(addInfo);
}

/// <summary>
/// カプセルの描画リストに追加する
/// </summary>
void MyLib::DebugDraw::AddDrawCupsule(const Vec3& center, const float& size, const float& radius, const unsigned int& color)
{
	CupsuleInfo addInfo;
	addInfo.pos1 = Vec3(center.x, center.y - size, center.z);
	addInfo.pos2 = Vec3(center.x, center.y + size, center.z);
	addInfo.radius = radius;
	addInfo.color = color;
	m_cupsuleInfo.emplace_back(addInfo);
}
