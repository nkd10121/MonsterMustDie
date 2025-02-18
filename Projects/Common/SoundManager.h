#pragma once
#include "DxLib.h"
#include <string>
#include <list>
#include <memory>

struct Sound;

/// <summary>
/// サウンド関係を管理するクラス
/// </summary>
class SoundManager
{
private:
	/// <summary>
	/// サウンド情報
	/// </summary>
	struct Sound
	{
		std::string id;		//サウンドID
		std::string path;	//パス
		int handle;			//ハンドル
		bool isEteral;		//常駐フラグ
	};
private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SoundManager() {};
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SoundManager();

	static SoundManager* m_instance;	//インスタンス
public:

	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator= (const SoundManager&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static SoundManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new SoundManager;
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
	/// サウンドを読み込む
	/// </summary>
	/// <param name="id">サウンドID</param>
	/// <param name="path">サウンドパス</param>
	/// <param name="isBGM">true = BGM,false = SE</param>

	/// <summary>
	/// サウンドを読み込む
	/// </summary>
	/// <param name="id">サウンドID</param>
	/// <param name="path">サウンドパス</param>
	/// <param name="isBGM">true : BGM,false : SE</param>
	/// <param name="isEternal">常駐するかどうか true:する,false:しない</param>
	void Load(std::string id, std::string path, bool isBGM,bool isEternal);

	/// <summary>
	/// すべてのリソースの読み込みが終了しているかどうかを取得
	/// </summary>
	/// <returns>true:終了,false:まだ</returns>
	const bool IsLoaded()const;

	/// <summary>
	/// 常駐フラグがfalseのハンドルを全削除する
	/// </summary>
	void Clear();

	/// <summary>
	/// 指定したIDのBGMを流す
	/// </summary>
	/// <param name="id">流したいBGMのID</param>
	/// <param name="isFromStart">true = 最初から,false = 途中から</param>
	void PlayBGM(std::string id, bool isFromStart);

	/// <summary>
	///	指定したIDのSEを流す
	/// </summary>
	/// <param name="id">流したいSEのID</param>
	void PlaySE(std::string id);

	/// <summary>
	/// BGMをフェードアウトさせる
	/// </summary>
	/// <param name="id">フェードアウトさせたいBGMのID</param>
	/// <param name="fadeFrame"></param>
	void FadeOutBGM(std::string id, int fadeFrame);

	/// <summary>
	/// 指定したIDのBGMを止める
	/// </summary>
	/// <param name="id">止めたいBGMのID</param>
	void StopBGM(std::string id);

	/// <summary>
	/// 指定したサウンドが流れているどうか
	/// </summary>
	/// <param name="id">サウンドネーム</param>
	/// <returns>true = 流れている,false = 流れていない</returns>
	bool isPlayingSound(std::string id);

	void BGMChangeVolume();

private:
	/// <summary>
	/// 指定したハンドルのサウンドが流れているかどうか
	/// </summary>
	/// <param name="handle">チェックしたいハンドル</param>
	/// <returns>true = 流れている,false = 流れていない</returns>
	bool CheckPlaying(int handle) { return CheckSoundMem(handle); }

private:
	Sound m_BGM;	//BGM配列
	std::list<std::shared_ptr<Sound>> m_SE;		//SE配列

	float m_BGMvolume = 0.5f;	//bgmの音量　min:0.0f,max:1.0f
	float m_SEvolume = 0.5f;	//seの音量　min:0.0f,max:1.0f

	int m_fadeFrameCount = 0;
};

