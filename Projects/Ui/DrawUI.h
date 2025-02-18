#pragma once
#include "Vec2.h"
#include <functional>

class DrawUI
{
public:
	struct DrawRequest
	{
		std::function<void()> drawFunction;  //描画処理
		int zOrder;                          //描画順序（小さいほど手前）

		DrawRequest(const std::function<void()>& drawFunction, int zOrder = 0):
			drawFunction(drawFunction), zOrder(zOrder) {}
	};

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DrawUI() {};
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~DrawUI();

	static DrawUI* m_instance;	//インスタンス

public:
	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	DrawUI(const DrawUI&) = delete;
	DrawUI& operator=(const DrawUI&) = delete;
	DrawUI(DrawUI&&) = delete;
	DrawUI& operator= (const DrawUI&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static DrawUI& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new DrawUI;
		}

		return *m_instance;
	}

	/// <summary>
	/// 削除
	/// これをし忘れると普通にメモリリーク
	/// </summary>
	static void Destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

public:
	// 描画リクエストを登録
	void RegisterDrawRequest(const std::function<void()>& drawFunction, int zOrder = 0);
	// 登録されたリクエストを描画
	void Draw();

private:
	std::vector<DrawRequest> requests;
};