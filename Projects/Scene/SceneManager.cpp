#include "SceneManager.h"
#include "SceneTitle.h"

SceneManager* SceneManager::m_instance = nullptr;

/// <summary>
/// コンストラクタ
/// </summary>
SceneManager::SceneManager()
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneManager::~SceneManager()
{

}

/// <summary>
/// 初期化
/// </summary>
void SceneManager::Init()
{

}

/// <summary>
/// 現在の先頭シーンの更新関数を呼び出す
/// </summary>
bool SceneManager::Update()
{
	Input::GetInstance().Update();

	//末尾のみ実行
	m_pScene.back()->UpdateAll();

	if (m_pScene.back()->GetIsGameEnd())
	{
		return true;
	}

	if (m_pScene.back()->IsSceneEnd())
	{
		//次に遷移するシーンが決まっている時
		if (m_pNextScene != nullptr)
		{
			if (m_pScene.back()->GetIsPushNextScene())
			{
				//末尾に追加
				m_pScene.push_back(m_pNextScene);
				m_pNextScene.reset();
				m_pNextScene = nullptr;

				m_pScene.back()->StartLoad();
			}
			else
			{
				for (auto& s : m_pScene)
				{
					s->End();
				}

				//シーンをリセット
				m_pScene.clear();
				//末尾に追加
				m_pScene.push_back(m_pNextScene);
				m_pNextScene.reset();
				m_pNextScene = nullptr;

				m_pScene.back()->StartLoad();
			}
		}
		//次に遷移するシーンが決まっていない時
		else
		{
			PopScene();

			m_pScene.back()->StartFadeIn();
		}

	}

	//基本終了しない
	return false;
}

/// <summary>
/// 持っているシーンの描画関数を呼び出す
/// </summary>
void SceneManager::Draw()
{
	//先頭から順に描画
	//最後に積んだものが最後に描画される
	for (auto& m_pScene : m_pScene)
	{
		m_pScene->DrawAll();
	}
}

/// <summary>
/// 実行中のシーンを引数で指定したシーンに切り替える
/// </summary>
void SceneManager::SetNextScene(std::shared_ptr<SceneBase> nextScene)
{
	//リストが空っぽだったら入れ替えずに
	if (m_pScene.empty())
	{
		//末尾に追加
		m_pScene.push_back(nextScene);
	}
	else
	{
		m_pNextScene = nextScene;
	}
}

/// <summary>
/// 現在先頭で実行中のシーンの上にシーンを乗っける
/// </summary>
void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	m_pScene.push_back(scene);
}

/// <summary>
/// 現在の先頭シーンを削除する
/// </summary>
void SceneManager::PopScene()
{
	m_pScene.pop_back();
}

void SceneManager::StartLoadThisScene()
{
	m_pScene.back()->StartLoad();
}