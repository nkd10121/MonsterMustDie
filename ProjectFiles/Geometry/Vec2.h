#pragma once
#include <DxLib.h>
#include <cmath>

struct Vec2
{
public:
	float x, y;	//座標

public:
	/// <summary>
	/// 引数なしコンストラクタ
	/// </summary>
	Vec2();
	/// <summary>
	/// 引数ありコンストラクタ
	/// </summary>
	/// <param name="inX">X座標</param>
	/// <param name="inY">Y座標</param>
	Vec2(float inX, float inY);

	/// <summary>
	/// 右ベクトル
	/// </summary>
	/// <returns>(1, 0, 0)</returns>
	static Vec2 Right();
	/// <summary>
	/// 左ベクトル
	/// </summary>
	/// <returns>(-1, 0, 0)</returns>
	static Vec2 Left();
	/// <summary>
	/// 上ベクトル
	/// </summary>
	/// <returns>(0, 1, 0)</returns>
	static Vec2 Up();
	/// <summary>
	/// 下ベクトル
	/// </summary>
	/// <returns>(0, -1, 0)</returns>
	static Vec2 Down();

	/// <summary>
	/// 0ベクトル
	/// </summary>
	static Vec2 Zero();

	/* 演算 */
	/// <summary>
	/// 各値に-をかける
	/// </summary>
	Vec2 operator-() const;
	/// <summary>
	/// 足し算
	/// </summary>
	Vec2 operator+(const Vec2& val) const;
	/// <summary>
	/// 足し算
	/// </summary>
	void operator+=(const Vec2& val);
	/// <summary>
	/// 引き算
	/// </summary>
	Vec2 operator-(const Vec2& val) const;
	/// <summary>
	/// 引き算
	/// </summary>
	void operator-=(const Vec2& val);
	/// <summary>
	/// 掛け算
	/// </summary>
	Vec2 operator*(float scale) const;
	/// <summary>
	/// 掛け算
	/// </summary>
	void operator*=(float scale);
	/// <summary>
	/// 割り算
	/// </summary>
	Vec2 operator/(float scale) const;
	/// <summary>
	/// 割り算
	/// </summary>
	void operator/=(float scale);
	/// <summary>
	/// 比較
	/// </summary>
	bool operator==(const Vec2& val)const;
	/// <summary>
	/// 比較
	/// </summary>
	bool operator!=(const Vec2& val)const;

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
	Vec2 Normalize()const;
};