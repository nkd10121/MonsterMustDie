#include "SceneBase.h"
#include "Game.h"

#include "EffectManager.h"
#include "SoundManager.h"
#include "MapManager.h"
#include "TrapManager.h"

#include "ResourceManager.h"
#include "FontManager.h"

#include "SceneDebug.h"

#include<chrono>
#include<ctime>
#include<cassert>



namespace
{
	// フェードにかかるフレームデフォルト
	constexpr int kFadeFrameDefault = 30;
	// 1フレーム当たりのフェード速度
	constexpr int kFadeSpeed = 255 / kFadeFrameDefault;

	//明るさの最大
	constexpr int kBrightMax = 255;
	//明るさの最低
	constexpr int kBrightMin = 0;

	//ロード中に描画する文字列配列
	const std::string kLoadingText[] =
	{
		"L",
		"o",
		"a",
		"d",
		"i",
		"n",
		"g",
		".",
		".",
		".",
	};

	const std::string kLoadingImage[] =
	{
		"I_01",
		"I_02",
		"I_03",
		"I_04",
		"I_05",
	};
}

/// <summary>
/// コンストラクタ
/// </summary>
SceneBase::SceneBase(std::string name) :
	m_isInit(false),
	m_isThisSceneEnd(false),
	m_isPushNextScene(false),
	m_isGameEnd(false),
	m_loadingFrame(0),
	m_fadeAlpha(kBrightMax),
	m_fadeSpeed(0),
	m_fadeColor(0x000000),
	m_sceneName(name),
	m_isDrawOperation(false),
	m_angle(0.0f),
	m_loadingIdx(0)
#ifdef DISP_PROCESS
	, m_updateTime(0),
	m_drawTime(0)
#endif
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneBase::~SceneBase()
{

}

/// <summary>
/// 現在のシーンを終了する準備をする
/// </summary>
/// <param name="isPushScene"></param>
void SceneBase::EndThisScene(bool isPushScene)
{
	if (isPushScene)
	{
		m_isPushNextScene = isPushScene;
	}
	m_isThisSceneEnd = true;

	StartFadeOut();
}

///// <summary>
///// リソースの仕分けとロード開始をする
///// </summary>
//void SceneBase::AssortAndLoadResourse(std::list<LoadCSV::ResourceData> data)
//{
//	for (auto& d : data)
//	{
//		//モデルデータなら
//		if (d.extension == ".mv1")
//		{
//			//モデルをロードする
//			auto path = d.path + d.extension;
//			ModelManager::GetInstance().Load(d.id, path, d.isEternal);
//		}
//		//音声データなら
//		else if (d.extension == ".mp3")
//		{
//			//サウンドをロードする
//			auto path = d.path + d.extension;
//			SoundManager::GetInstance().Load(d.id, path, d.isBGM, d.isEternal);
//		}
//		//エフェクトデータなら
//		else if (d.extension == ".efk")
//		{
//			//MEMO:Effekseerのロードは非同期ロード対応してないらしいのでここは非同期ロードじゃない
//			SetUseASyncLoadFlag(false);
//			//エフェクトをロードする
//			auto path = d.path + d.extension;
//			EffectManager::GetInstance().Load(d.id, path, 30, d.isEternal);
//			//非同期ロードをONに戻す
//			SetUseASyncLoadFlag(true);
//		}
//		//画像データなら
//		else if (d.extension == ".png")
//		{
//			auto path = d.path + d.extension;
//			ImageManager::GetInstance().Load(d.id, path, d.isEternal);
//		}
//		//ピクセルシェーダーなら
//		else if (d.extension == ".pso")
//		{
//			auto path = d.path + d.extension;
//			ShaderManager::GetInstance().Load(d.id, path, true, d.isEternal);
//		}
//		//頂点シェーダーなら
//		else if (d.extension == ".vso")
//		{
//			auto path = d.path + d.extension;
//			ShaderManager::GetInstance().Load(d.id, path, false, d.isEternal);
//		}
//		//今まで以外なら
//		else
//		{
//			assert(0 && "想定されていない拡張子です");
//		}
//	}
//}

/// <summary>
/// 派生先の初期化とシーン共通で必要な初期化を行う
/// </summary>
bool SceneBase::InitAll()
{
	//ゲームシーンを開始しようとしていたら
	if (m_sceneName == "SCENE_GAME")
	{

		if (Input::GetInstance().IsTriggered("OK"))
		{
			m_isDrawOperation = false;
		}
		else
		{
			return false;
		}
	}

	// フェードアウト状態から開始
	m_fadeAlpha = kBrightMax;
	StartFadeIn();
	// 継承先シーンの初期化処理
	Init();

	return true;
}

/// <summary>
/// 派生先の更新とシーン共通で必要な更新を行う
/// </summary>
void SceneBase::UpdateAll()
{
#ifdef DISP_PROCESS
	//更新処理を始めた時点の値
	LONGLONG start = GetNowHiPerformanceCount();

	printf("---------------------------------------\n");
	printf("現在のシーン : %s\n", m_sceneName.c_str());

	// 現在時刻をsystem_clockを用いて取得
	auto now = std::chrono::system_clock::now();

	// 現在時刻をtime_t形式に変換
	std::time_t t = std::chrono::system_clock::to_time_t(now);

	//現在時刻を表示
	printf("現在時刻 : %d", (static_cast<int>(t) / 3600 + 9) % 24);//時
	printf(":%d", static_cast<int>(t) / 60 % 60);//分
	printf(":%d\n", static_cast<int>(t) % 60);//秒
#endif

	//継承先のシーンのリソースのロードが終わっているか確認
	if (!IsLoaded())
	{
		//ロード中なら更新処理何も行わない
		m_loadingFrame++;
		return;
	}

	//初期化関数を呼んだか確認
	if (!m_isInit)
	{
		//呼んでいなかったら初期化関数を呼ぶ
		m_isInit = InitAll();

		if (!m_isInit)
		{
			return;
		}
	}

	//フェードの更新
	UpdateFade();
	// 継承先のシーンの更新処理
	Update();
	//次のシーンを選ぶ更新
	SelectNextSceneUpdate();

#ifdef DISP_PROCESS
	//更新処理を終えた時点の値
	m_updateTime = GetNowHiPerformanceCount() - start;
#endif
}

/// <summary>
/// 派生先の描画とシーン共通で必要な描画を行う
/// </summary>
void SceneBase::DrawAll()
{
#ifdef DISP_PROCESS
	//描画処理を始めた時点の値
	LONGLONG start = GetNowHiPerformanceCount();
#endif

#ifdef _DEBUG	//グリッドを描画する
	for (int x = -50; x <= 50; x += 10)
	{
		DrawLine3D(VGet(static_cast<float>(x), 0, -50), VGet(static_cast<float>(x), 0, 50), 0xffff00);
	}
	for (int z = -50; z <= 50; z += 10)
	{
		DrawLine3D(VGet(-50, 0, static_cast<float>(z)), VGet(50, 0, static_cast<float>(z)), 0xff0000);
	}
#endif



	//継承先のシーンの描画処理
	Draw();
	//フェードの描画
	DrawFade();

	if (m_isDrawOperation)
	{
		if (Input::GetInstance().IsTriggered("RIGHT"))
		{
			m_loadingIdx = min(m_loadingIdx++, 4);
		}
		if (Input::GetInstance().IsTriggered("LEFT"))
		{
			m_loadingIdx = max(m_loadingIdx--, 0);
		}
		DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 2, 1.0f, 0.0f, ResourceManager::GetInstance().GetHandle(kLoadingImage[m_loadingIdx]), true);
		//DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 2, 1.0f, 0.0f, ResourceManager::GetInstance().GetHandle("I_BIGWINDOW3"), true);
		//DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 2, 1.0f, 0.0f, ResourceManager::GetInstance().GetHandle("I_OPERATION"), true);
		//DrawRotaGraph(Game::kWindowWidth/2, Game::kWindowHeight/2,1.0f,0.0f, ResourceManager::GetInstance().GetHandle("I_GAME01"), true);

		for (int i = 0; i < 5; i++)
		{
			bool isFill = false;
			if (m_loadingIdx == i)
			{
				isFill = true;
			}
			DrawCircle(Game::kWindowWidth / 2 - 160 + 80 * i, 680, 4, 0xffffff, isFill);
		}

		m_angle += 0.125f;

		if (IsLoaded())
		{
			DrawRotaGraph(990, 685, 0.5f + sinf(m_angle) / 100, 0.0f, ResourceManager::GetInstance().GetHandle("I_A"), true);
			FontManager::GetInstance().DrawCenteredExtendText(1100,680,"でスタート",0xffffff,32,0x000000,1.0f + sinf(m_angle) / 100);

			//DrawString(580, 660, "Aボタンでスタート", 0x000000);
		}
	}

	//ロード中画面の描画
	DrawLoading();
#ifdef _DEBUG	//デバッグ描画
#ifdef TRUE
	//描画処理を終えた時点の値
	m_drawTime = GetNowHiPerformanceCount() - start;

	// 説明 
	DrawString(0, Game::kWindowHeight - 32, "処理:", 0xffffff, 0x000000);
	DrawString(0, Game::kWindowHeight - 16, "描画:", 0xffffff, 0x000000);

	/* 処理バーの描画 */

	//更新時間バーの描画
	float rate = static_cast<float>(m_updateTime) / 16666.6f;
	int width = static_cast<int>(Game::kWindowWidth * rate);
	DrawBox(40 + 2, Game::kWindowHeight - 32, 40 + 2 + width, Game::kWindowHeight - 16, 0x0000ff, true);

	//描画時間バーの描画
	rate = static_cast<float>(m_drawTime) / 16666.6f;
	width = static_cast<int>(Game::kWindowWidth * rate);
	DrawBox(40 + 2, Game::kWindowHeight - 16, 40 + 2 + width, Game::kWindowHeight, 0xff0000, true);

	//テキストの描画
	printf("更新処理時間:%lld\n",m_updateTime);
	printf("描画処理時間:%lld\n", m_drawTime);
#endif	//DISP_PROCESS
#endif	//_DEBUG
}

/// <summary>
/// 現在のシーンが完全に終了したかどうか
/// </summary>
bool SceneBase::IsSceneEnd()
{
	// そもそも終わると言っていない
	if (!m_isThisSceneEnd)	return false;
	// まだフェードアウト終わってない
	if (m_fadeAlpha < kBrightMax)	return false;


	//次のシーンに遷移するわけではなく、上に描画するならやっぱり終わらないってする
	if (m_isPushNextScene)
	{
		m_isThisSceneEnd = false;
	}
	else
	{
		////staticクラスのデータのリセット
		//ModelManager::GetInstance().Clear();
		//SoundManager::GetInstance().Clear();
		//EffectManager::GetInstance().Clear();
		//ImageManager::GetInstance().Clear();

		//ShaderManager::GetInstance().Clear();
		//MapManager::GetInstance().DeleteModel();
		//TrapManager::GetInstance().Clear();
	}



	//ここまで来たらシーンを完全に終了する
	return true;
}

/// <summary>
/// フェードの更新
/// </summary>
void SceneBase::UpdateFade()
{
	// リソースのロードが完了していない
	if (m_fadeSpeed < 0 && (!IsLoaded()))
	{
		return;	// ロード完了まではフェードイン処理を行わない
	}

	m_fadeAlpha += m_fadeSpeed;
	if (m_fadeAlpha >= kBrightMax)
	{
		m_fadeAlpha = kBrightMax;
		if (m_fadeSpeed > 0)
		{
			m_fadeSpeed = 0;
		}
	}
	if (m_fadeAlpha <= kBrightMin)
	{
		m_fadeAlpha = kBrightMin;
		if (m_fadeSpeed < 0)
		{
			m_fadeSpeed = 0;
		}
	}
}

/// <summary>
/// フェードの描画
/// </summary>
void SceneBase::DrawFade() const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, m_fadeColor, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

/// <summary>
/// ロード中描画
/// </summary>
void SceneBase::DrawLoading()
{
	if (!IsLoaded())
	{
		//ゲームシーンを開始しようとしていたら
		if (m_sceneName == "SCENE_GAME")
		{
			m_isDrawOperation = true;
		}

		//描画座標
		int x = Game::kWindowWidth - 128;
		int y = Game::kWindowHeight - 32;
		//for文の回数
		int num = 0;

		//文字列文まわす
		for (auto& text : kLoadingText)
		{
			//ロード中の経過フレームで高さを変える
			float height = sinf(static_cast<float>(m_loadingFrame - num * 2) / 6);
			//文字の描画
			FontManager::GetInstance().DrawCenteredText(x + num * 12, y - static_cast<int>(height * 4), text, 0xffffff,16, 0x000000);

			//文字数を更新
			num++;
		}
	}
}

/// <summary>
/// フェードイン開始
/// </summary>
void SceneBase::StartFadeIn()
{
	m_fadeSpeed = -kFadeSpeed;
	m_isPushNextScene = false;
}

/// <summary>
/// フェードアウト開始
/// </summary>
void SceneBase::StartFadeOut()
{
	m_fadeSpeed = kFadeSpeed;

}

/// <summary>
/// フェードインをスキップする
/// </summary>
void SceneBase::SkipFadeIn()
{
	m_fadeAlpha = kBrightMax;
	m_fadeSpeed = kFadeSpeed;
}

/// <summary>
/// フェードアウトをスキップする
/// </summary>
void SceneBase::SkipFadeOut()
{
	m_fadeAlpha = kBrightMin;
	m_fadeSpeed = -kFadeSpeed;
}
