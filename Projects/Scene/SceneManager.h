#pragma once
#include <list>
#include <vector>
#include <memory>

class SceneBase;

/// <summary>
/// シーンを管理するクラス
/// </summary>
class SceneManager
{
private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneManager();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneManager();

	static SceneManager* m_instance;	//インスタンス
public:
	//コピーコンストラクタを禁止する
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator= (const SceneManager&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static SceneManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new SceneManager;
		}
		return *m_instance;
	}

	/// <summary>
	/// インスタンスを削除
	/// </summary>
	static void Destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init();
	/// <summary>
	/// 現在の先頭シーンの更新関数を呼び出す
	/// </summary>
	/// <returns>true : ゲーム終了, false : ゲーム続行</returns>
	bool Update();
	/// <summary>
	/// 持っているシーンの描画関数を呼び出す
	/// </summary>
	void Draw();

	/// <summary>
	/// 実行中のシーンを引数で指定したシーンに切り替える
	/// </summary>
	/// <param name="nextScene">遷移先のシーン</param>
	void SetNextScene(std::shared_ptr<SceneBase> nextScene);
	/// <summary>
	/// 現在先頭で実行中のシーンの上にシーンを乗っける
	/// </summary>
	/// <param name="scene">遷移先のシーン</param>
	void PushScene(std::shared_ptr<SceneBase> scene);
	/// <summary>
	/// 現在の先頭シーンを削除する
	/// </summary>
	void PopScene();

	/// <summary>
	/// 現在のシーンのsharedポインタを取得する
	/// </summary>
	/// <returns></returns>
	const std::shared_ptr<SceneBase> GetBackScenePointer()const { return m_pScene[m_pScene.size()-1]; }

	/// <summary>
	/// 最初のシーンがロードできないため応急処置
	/// </summary>
	void StartLoadThisScene();

	const int GetStageIdx()const { return m_stageIndex; }
	const void SetStageIdx(int idx) { m_stageIndex = idx; }
private:
	std::vector<std::shared_ptr<SceneBase>> m_pScene;		//現在のシーン
	std::shared_ptr<SceneBase> m_pNextScene;			//遷移予定のシーン

	int m_stageIndex;
};
