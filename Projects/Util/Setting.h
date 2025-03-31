#pragma once

/// <summary>
/// オプションの設定
/// </summary>
class Setting
{
private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Setting() {};
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Setting() {};

	static Setting* m_instance;	//インスタンス

public:
	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	Setting(const Setting&) = delete;
	Setting& operator=(const Setting&) = delete;
	Setting(Setting&&) = delete;
	Setting& operator= (const Setting&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static Setting& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new Setting;
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
	/// マスターボリュームを設定する
	/// </summary>
	/// <param name="volume">マスターボリューム</param>
	void SetMasterVolume(float volume) { m_data.masterVolume = volume; }
	/// <summary>
	/// BGMボリュームを設定する
	/// </summary>
	/// <param name="volume">BGMボリューム</param>
	void SetBGMVolume(float volume) { m_data.bgmVolume = volume; }
	/// <summary>
	/// SEボリュームを設定する
	/// </summary>
	/// <param name="volume">SEボリューム</param>
	void SetSEVolume(float volume) { m_data.seVolume = volume; }
	/// <summary>
	/// カメラ感度を設定する
	/// </summary>
	/// <param name="sensitivity">カメラ感度</param>
	void SetSensitivity(float sensitivity) { m_data.sensitivity = sensitivity; }
	/// <summary>
	/// フルスクリーンを設定する
	/// </summary>
	/// <param name="isFullScreen">フルスクリーンフラグ</param>
	void SetIsFullScreen(bool isFullScreen) { m_data.isFullScreen = isFullScreen; }
	/// <summary>
	/// 操作説明を描画するかを設定する
	/// </summary>
	/// <param name="isDrawOperation">描画フラグ</param>
	void SetIsDrawOperation(bool isDrawOperation) { m_data.isDrawOperation = isDrawOperation; }

	/// <summary>
	/// マスターボリュームを取得する
	/// </summary>
	/// <returns>マスターボリューム</returns>
	const float GetMasterVolume()const { return m_data.masterVolume; }
	/// <summary>
	/// BGMボリュームを取得する
	/// </summary>
	/// <returns>BGMボリューム</returns>
	const float GetBGMVolume()const { return m_data.bgmVolume; }
	/// <summary>
	/// SEボリュームを取得する
	/// </summary>
	/// <returns>SEボリューム</returns>
	const float GetSEVolume()const { return m_data.seVolume; }
	/// <summary>
	/// カメラ感度を取得する
	/// </summary>
	/// <returns>カメラ感度</returns>
	const float GetSensitivity()const { return m_data.sensitivity; }
	/// <summary>
	/// フルスクリーンを取得する
	/// </summary>
	/// <returns>フルスクリーンフラグ</returns>
	const bool GetIsFullScreen()const { return m_data.isFullScreen; }
	/// <summary>
	/// 操作説明を描画するかを取得する
	/// </summary>
	/// <returns>描画フラグ</returns>
	const bool GetIsDrawOperation()const { return m_data.isDrawOperation; }

	/// <summary>
	/// オプションデータを読み込む
	/// </summary>
	void Load();
	/// <summary>
	/// オプションデータを保存する
	/// </summary>
	void Save();

private:
	/// <summary>
	/// オプションデータをリセットする
	/// </summary>
	void ResetData();
	/// <summary>
	/// 新しいオプションデータを生成する
	/// </summary>
	void CreateNewData();

private:
	//オプションデータをまとめた構造体
	struct Data
	{
		float masterVolume;
		float bgmVolume;
		float seVolume;

		float sensitivity;

		bool isFullScreen;
		bool isDrawOperation;
	};

	Data m_data;	//データ
};

