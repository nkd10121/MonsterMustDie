#pragma once
#include <List>
#include "Vec3.h"

namespace MyLib
{
/// <summary>
/// デバッグ用の描画情報をまとめ、後で表示するためのクラス
/// </summary>
class DebugDraw
{
public:
	/// <summary>
	/// 描画情報の削除
	/// </summary>
	static void Clear();
	/// <summary>
	/// 描画
	/// </summary>
	static void Draw3D();

	/// <summary>
	/// 球体の描画リストに追加する
	/// </summary>
	/// <param name="center">中心座標</param>
	/// <param name="radius">半径</param>
	/// <param name="color">描画色</param>
	static void AddDrawSphere(const Vec3& center,const float& radius,const unsigned int& color);
	/// <summary>
	/// カプセルの描画リストに追加する
	/// </summary>
	/// <param name="center">中心座標</param>
	/// <param name="size">大きさ</param>
	/// <param name="radius">半径</param>
	/// <param name="color">描画色</param>
	static void AddDrawCupsule(const Vec3& center,const float& size,const float& radius,const unsigned int& color);
private:
	//球体の描画情報
	struct SphereInfo
	{
		Vec3 center;		//中心座標
		float radius;		//半径
		unsigned int color; //色
	};

	//カプセルの描画情報
	struct CupsuleInfo
	{
		Vec3 pos1;			//座標1
		Vec3 pos2;			//座標2
		float size;			//大きさ
		float radius;		//半径
		unsigned int color;	//色
	};

	static std::list<SphereInfo> m_sphereInfo;		//球体の描画情報リスト
	static std::list<CupsuleInfo> m_cupsuleInfo;	//カプセルの描画情報リスト
};
}