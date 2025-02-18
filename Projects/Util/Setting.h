#pragma once
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
	virtual ~Setting();

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
	//マスターボリュームを設定する
	void SetMasterVolume(float volume) { m_data.masterVolume = volume; }
	//BGMボリュームを設定する
	void SetBGMVolume(float volume) { m_data.bgmVolume = volume; }
	//SEボリュームを設定する
	void SetSEVolume(float volume) { m_data.seVolume = volume; }
	//感度を設定する
	void SetSensitivity(float sensitivity) { m_data.sensitivity = sensitivity; }
	//フルスクリーンを設定する
	void SetIsFullScreen(bool isFullScreen) { m_data.isFullScreen = isFullScreen; }
	//操作説明を描画するかを設定する
	void SetIsDrawOperation(bool isDrawOperation) { m_data.isDrawOperation = isDrawOperation; }

	//マスターボリュームを取得する
	const float GetMasterVolume()const { return m_data.masterVolume; }
	//BGMボリュームを取得する
	const float GetBGMVolume()const { return m_data.bgmVolume; }
	//SEボリュームを取得する
	const float GetSEVolume()const { return m_data.seVolume; }
	//感度を取得する
	const float GetSensitivity()const { return m_data.sensitivity; }
	//フルスクリーンを取得する
	const bool GetIsFullScreen()const { return m_data.isFullScreen; }
	//操作説明を描画するかを取得する
	const bool GetIsDrawOperation()const { return m_data.isDrawOperation; }

	//オプションデータを読み込む
	void Load();
	//オプションデータを保存する
	void Save();

private:
	//オプションデータを全消去する
	void ClearData();
	//新しいオプションデータを生成する
	void CreateNewData();

private:
	struct Data
	{
		float masterVolume;
		float bgmVolume;
		float seVolume;

		float sensitivity;

		bool isFullScreen;
		bool isDrawOperation;
	};
private:
	Data m_data;
};

