#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "Vec3.h"
#include "DxLib.h"

/// <summary>
/// エフェクトを管理するクラス
/// </summary>
class EffectManager
{
private:
	/// <summary>
	/// エフェクト構造体
	/// </summary>
	struct Effect
	{
		int handle;		//エフェクトハンドル
		float x, y, z;	//座標
		int frame;		//フレーム数
		bool isExist;	//存在フラグ
	};

	/// <summary>
	/// エフェクト生成構造体
	/// </summary>
	struct EffectEmitter
	{
		int emitterHandle = -1;			//エフェクト生成機のハンドル
		const char* effectPath = "";	//エフェクトのパス
		std::vector<Effect> effects;	//エフェクト配列
		int endFrame;					//エフェクトの終了フレーム
		bool isEternal;					//常駐フラグ
	};


private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EffectManager() {};
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EffectManager();

	static EffectManager* m_instance;	//インスタンス

public:
	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
	EffectManager(EffectManager&&) = delete;
	EffectManager& operator= (const EffectManager&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static EffectManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new EffectManager;
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
	/// <summary>
	/// エフェクトをロード
	/// </summary>
	/// <param name="name">エフェクト名</param>
	/// <param name="path">パス</param>
	/// <param name="endFrame">終了フレーム数</param>
	/// <param name="scale">拡大率</param>
	void Load(std::string name, std::string path, int endFrame,bool isEternal);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 常駐フラグがfalseのハンドルを削除する
	/// </summary>
	void Clear();

	/// <summary>
	/// エフェクトを生成
	/// </summary>
	/// <param name="name">エフェクト名</param>
	/// <param name="pos">描画座標</param>
	/// <param name="rot">回転角度</param>
	void CreateEffect(std::string name, Vec3 pos, Vec3 rot = Vec3());

	/// <summary>
	/// 指定した名前のすべてのエフェクトの座標を移動させる
	/// </summary>
	/// <param name="name">移動させたいエフェクトの名前</param>
	/// <param name="pos">移動先座標</param>
	void SetPos(std::string name, Vec3 pos);

	/// <summary>
	/// 指定した名前のすべてのエフェクトを回転させる
	/// </summary>
	/// <param name="name">エフェクト名</param>
	/// <param name="rot">回転角度</param>
	void SetRotation(std::string name, Vec3 rot);

private:
	std::map<std::string, std::shared_ptr<EffectEmitter>> m_effect;	//エフェクトハンドル
};

