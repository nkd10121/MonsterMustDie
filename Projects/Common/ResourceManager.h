#pragma once
#include <list>
#include <vector>
#include <string>
#include <memory>

class ResourceManager
{
public:
	enum class Kind : int
	{
		Image,		//画像
		Sound,		//音
		Model,		//モデル
		Effect,		//エフェクト
		Shader,		//シェーダー
	};

	//画像構造体
	struct Resource
	{
		std::string sceneName;	//読み込んだシーン
		Kind kind;				//種類
		std::string id;			//ID
		int handle;				//ハンドル
		bool isEternal;			//常駐フラグ
	};

	struct Sound : public  Resource
	{
		bool isBgm;
	};

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ResourceManager() {};
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ResourceManager();

	static ResourceManager* m_instance;	//インスタンス

public:
	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator= (const ResourceManager&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static ResourceManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new ResourceManager;
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
	/// 指定したパスをロードする
	/// </summary>
	/// <param name="data">情報群</param>
	void Load(std::string sceneName);

	/// <summary>
	/// ハンドルを取得する
	/// </summary>
	/// <param name="id">ID</param>
	/// <returns>-1 : エラー, -1以外 : ハンドル</returns>
	int GetHandle(std::string id);

	/// <summary>
	/// 常駐フラグがfalseのハンドルを全削除する
	/// </summary>
	void Clear(std::string sceneName);

	//すべてのハンドルを削除する
	void AllClear();

	/// <summary>
	/// ハンドルが読み込まれているかどうか確認
	/// </summary>
	/// <returns>false : 読み込み終えていない, true : 読み込み終了</returns>
	bool IsLoaded();

private:
	const void LoadResource(const std::vector<std::string>& data);
private:
	std::list<std::shared_ptr<Resource>> m_resources;
};

