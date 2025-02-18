#pragma once
#include <DxLib.h>
#include <cmath>

struct Vec3
{
public:
	float x, y, z;	//座標

public:
	/// <summary>
	/// 引数なしコンストラクタ
	/// </summary>
	Vec3();
	/// <summary>
	/// 引数ありコンストラクタ
	/// </summary>
	/// <param name="inX">X座標</param>
	/// <param name="inY">Y座標</param>
	/// <param name="inZ">Z座標</param>
	Vec3(float inX, float inY, float inZ);
	/// <summary>
	/// 引数ありコンストラクタ
	/// </summary>
	/// <param name="vec">DxLibのVECTOR型</param>
	Vec3(DxLib::VECTOR vec);

	/// <summary>
	/// 右ベクトル
	/// </summary>
	/// <returns>(1, 0, 0)</returns>
	static Vec3 Right();
	/// <summary>
	/// 左ベクトル
	/// </summary>
	/// <returns>(-1, 0, 0)</returns>
	static Vec3 Left();
	/// <summary>
	/// 上ベクトル
	/// </summary>
	/// <returns>(0, 1, 0)</returns>
	static Vec3 Up();
	/// <summary>
	/// 下ベクトル
	/// </summary>
	/// <returns>(0, -1, 0)</returns>
	static Vec3 Down();
	/// <summary>
	/// 正面ベクトル
	/// </summary>
	/// <returns>(0, 0, 1)</returns>
	static Vec3 Front();
	/// <summary>
	/// 背面ベクトル
	/// </summary>
	/// <returns>(0, 0, -1)</returns>
	static Vec3 Back();

	/* 演算 */
	/// <summary>
	/// 各値に-をかける
	/// </summary>
	Vec3 operator-() const;					
	/// <summary>
	/// 足し算
	/// </summary>
	Vec3 operator+(const Vec3& val) const;
	/// <summary>
	/// 足し算
	/// </summary>
	void operator+=(const Vec3& val);
	/// <summary>
	/// 引き算
	/// </summary>
	Vec3 operator-(const Vec3& val) const;
	/// <summary>
	/// 引き算
	/// </summary>
	void operator-=(const Vec3& val);
	/// <summary>
	/// 掛け算
	/// </summary>
	Vec3 operator*(float scale) const;
	/// <summary>
	/// 掛け算
	/// </summary>
	void operator*=(float scale);
	/// <summary>
	/// 割り算
	/// </summary>
	Vec3 operator/(float scale) const;
	/// <summary>
	/// 割り算
	/// </summary>
	void operator/=(float scale);
	/// <summary>
	/// 比較
	/// </summary>
	bool operator==(const Vec3& val)const;
	/// <summary>
	/// 比較
	/// </summary>
	bool operator!=(const Vec3& val)const;

	/// <summary>
	/// 長さを取得
	/// </summary>
	/// <returns>長さ</returns>
	float Length()const;

	/// <summary>
	/// 長さの二乗を取得
	/// </summary>
	/// <returns>長さの二乗</returns>
	float SqLength()const;

	/// <summary>
	/// 正規化ベクトルを取得
	/// </summary>
	/// <returns>正規化ベクトル</returns>
	Vec3 Normalize()const;

	/// <summary>
	/// DxLibのVECTOR型に変換
	/// </summary>
	/// <returns></returns>
	VECTOR ToVECTOR()const;
};

/// <summary>
/// 内積
/// </summary>
/// <param name="item1">ベクトル１</param>
/// <param name="item2">ベクトル２</param>
/// <returns>内積結果</returns>
float Dot(const Vec3& item1, const Vec3& item2);
/// <summary>
/// 外積
/// </summary>
/// <param name="item1">ベクトル１</param>
/// <param name="item2">ベクトル２</param>
/// <returns>外積結果</returns>
Vec3 Cross(const Vec3& item1, const Vec3& item2);

Vec3 Abs(const Vec3& item);