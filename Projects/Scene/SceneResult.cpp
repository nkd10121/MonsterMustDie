#include "SceneResult.h"

#include "SceneGame.h"
//#include "SceneStrengthen.h"
//#include "SceneRanking.h"
#include "SceneStageSelect.h"

#include "ScoreManager.h"
#include "ResourceManager.h"
#include "FontManager.h"
#include "SoundManager.h"

#include "Game.h"
namespace
{
	//#ifdef _DEBUG	//デバッグ描画
		/*テキスト描画関係*/
	constexpr int kTextX = 64;			//テキスト描画X座標
	constexpr int kTextY = 32;			//テキスト描画Y座標
	constexpr int kTextYInterval = 16;	//テキスト描画Y座標の空白
	//#endif

	std::vector<std::string> kItemText =
	{
		"次のステージへ",
		"スコア詳細へ",
		"セレクトに戻る",
		"もう一度",
	};
}

/// <summary>
/// コンストラクタ
/// </summary>
SceneResult::SceneResult() :
	SceneBase("SCENE_RESULT"),
	m_isClear(false),
	m_score(0),
	m_drawScore(0),
	m_count(0),
	m_alpha(0),
	m_windowDrawPos(Game::kWindowWidth / 2, -350),
	m_resultTextAngle(0.0f),
	m_textAlpha(0),
	m_textAngle(0.0f),
	m_isChangeNextScene(false)
{
	m_updateFunc = &SceneResult::UpdateNormal;
	m_drawFunc = &SceneResult::DrawNormal;
}

/// <summary>
/// デストラクタ
/// </summary>
SceneResult::~SceneResult()
{
	////staticクラスのデータのリセット
	//ResourceManager::GetInstance().Clear(GetNowSceneName());
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void SceneResult::StartLoad()
{
	//// 非同期読み込みを開始する
	//SetUseASyncLoadFlag(true);

	////リソースデータ群をみてリソースのロードを開始する
	//ResourceManager::GetInstance().Load(GetNowSceneName());

	//// デフォルトに戻す
	//SetUseASyncLoadFlag(false);

	//ほんとはここでリザルトシーンで必要なリソースをロードしたかったが、
	//ロードすると画面が一瞬ちらついてしまうためゲームシーンで一緒にロードしてしまっている
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool SceneResult::IsLoaded() const
{
	////TODO:ここでリソースがロード中かどうかを判断する
	//if (!ResourceManager::GetInstance().IsLoaded())	return false;

	return true;
}

/// <summary>
/// 初期化
/// </summary>
void SceneResult::Init()
{
	//フェードアウトをスキップする
	SkipFadeOut();
	//最初は一番上の項目を選んでいる状態にする
	m_destinationScene = static_cast<eDestination>(static_cast<int>(eDestination::Start) + 1);

	m_windowHandle = ResourceManager::GetInstance().GetHandle("I_BIGWINDOW2");

	if (!m_isClear)
	{
		m_resultTextAngle = -80;
	}
}

/// <summary>
/// 終了
/// </summary>
void SceneResult::End()
{
	ScoreManager::GetInstance().Clear();
}

/// <summary>
/// 更新
/// </summary>
void SceneResult::Update()
{

	//最初の30フレームで画面を暗くする
	//次の30フレームで上からウィンドウを落としてくる
	//Clear!の文字を描画する
	//スコアを加算して数字をアニメーションをさせながら
	//遷移先の項目をフェードインさせる

	//状態の更新
	(this->*m_updateFunc)();

	
}

/// <summary>
/// 描画
/// </summary>
void SceneResult::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawRotaGraph(m_windowDrawPos.x, m_windowDrawPos.y, 1.0f, 0.0f, m_windowHandle, true);

	//状態の更新
	(this->*m_drawFunc)();

#ifdef _DEBUG	//デバッグ描画	
	DrawFormatString(0, 0, 0xffffff, "%s", GetNowSceneName());

	DrawString(kTextX - 24, kTextY + kTextYInterval * (m_destinationScene - 1), "→", 0xff0000);
	DrawFormatString(240, 240, 0xffffff, "Score:%d", m_score);

	DrawString(kTextX, kTextY, "次のステージへ", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval, "スコア詳細へ", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval * 2, "ステージセレクトに戻る", 0xffffff);
#endif

}

const void SceneResult::SetStageName(std::string stageName)
{
	m_score = ScoreManager::GetInstance().GetCalculationScore();
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void SceneResult::SelectNextSceneUpdate()
{
	if (m_count < 200)	return;

	//上を入力したら
	if (Input::GetInstance().IsTriggered("UP"))
	{
		//現在選択している項目から一個上にずらす
		m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) - 1);

		//もし一番上の項目を選択している状態になっていたら
		if (m_destinationScene == eDestination::Start)
		{
			//一個下にずらす
			m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) + 1);
		}
		else
		{
			//SEを流す
			SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
		}
	}

	//下を入力したら
	if (Input::GetInstance().IsTriggered("DOWN"))
	{
		//現在選択している項目から一個下にずらす
		m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) + 1);

		//もし一番下の項目を選択している状態になっていたら
		if (m_destinationScene == eDestination::Last)
		{
			//一個上にずらす
			m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) - 1);
		}
		else
		{
			//SEを流す
			SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
		}
	}

	//決定ボタンを押したら現在選択しているシーンに遷移する
	if (Input::GetInstance().IsTriggered("OK"))
	{
		if (!m_isChangeNextScene)
		{
			m_isChangeNextScene = true;

			//ゲームシーンに遷移する
			if (m_destinationScene == eDestination::InGame)
			{
				if (m_isClear)
				{
					SceneManager::GetInstance().SetNextScene(std::make_shared<SceneGame>());
					SceneManager::GetInstance().SetStageIdx(min(SceneManager::GetInstance().GetStageIdx() + 1, LoadCSV::GetInstance().GetAllStageName().size() - 1));
					EndThisScene();
					return;
				}
				else
				{
					SceneManager::GetInstance().SetNextScene(std::make_shared<SceneGame>());
					SceneManager::GetInstance().SetStageIdx(SceneManager::GetInstance().GetStageIdx());
					EndThisScene();
					return;
				}
			}
			//スコア詳細が出てくる
			else if (m_destinationScene == eDestination::ScoreDetail)
			{
				m_updateFunc = &SceneResult::UpdateScoreDetail;
				m_drawFunc = &SceneResult::DrawScoreDetail;

				return;
			}
			////強化シーンに遷移する
			//else if (m_destinationScene == eDestination::Strengthen)
			//{
			//	SceneManager::GetInstance().SetNextScene(std::make_shared<SceneStrengthen>());
			//	EndThisScene(true);
			//	return;
			//}
			////ランキングシーンに遷移する
			//else if (m_destinationScene == eDestination::Ranking)
			//{
			//	SceneManager::GetInstance().SetNextScene(std::make_shared<SceneRanking>());
			//	EndThisScene(true);
			//	return;
			//}
			//セレクトシーンに遷移する
			else if (m_destinationScene == eDestination::Select)
			{
				SceneManager::GetInstance().SetNextScene(std::make_shared<SceneStageSelect>());
				EndThisScene();
				return;
			}
		}
	}
}

void SceneResult::UpdateNormal()
{
	if (m_count < 30)
	{
		m_alpha = min(m_alpha + 8, 200);
	}
	else if (m_count < 70)
	{
		m_windowDrawPos.y = min(m_windowDrawPos.y + 40, Game::kWindowHeight / 2);
	}
	else if (m_count < 120)
	{
		if (m_isClear)
		{
			m_resultTextAngle = min(m_resultTextAngle + 0.1f, 2.0f);
		}
		else
		{
			m_resultTextAngle = min(m_resultTextAngle + 5.0f, Game::kWindowHeight / 4);
		}
	}
	else if (m_count < 200)
	{
		// 現在値を更新
		if (m_drawScore != m_score)
		{
			//差が50以上あったら
			if (abs(m_drawScore - m_score) > 50)
			{
				//差に応じて増減アニメーションを早める
				auto difference = abs(m_score - m_drawScore);
				difference = difference / 5;

				m_drawScore += (m_score > m_drawScore) ? difference : -difference; // 1フレームごとに追従
			}
			m_drawScore += (m_score > m_drawScore) ? 1 : -1; // 1フレームごとに追従
		}

		m_textAlpha = min(m_textAlpha + 4, 255);
	}


	m_textAngle += 0.05f;
	m_count++;
}

void SceneResult::UpdateScoreDetail()
{
	if (Input::GetInstance().IsTriggered("CANCEL"))
	{
		m_updateFunc = &SceneResult::UpdateNormal;
		m_drawFunc = &SceneResult::DrawNormal;
	}
}

void SceneResult::DrawNormal()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_textAlpha);

	if (m_isClear)
	{
		DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 3, 1.35f, static_cast<float>(m_count) / 90.0f, ResourceManager::GetInstance().GetHandle("I_CLEAREFFECT"), true);
		FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 2, Game::kWindowHeight / 3, std::to_string(m_drawScore), 0xffffff, 80, 0xff0000);
	}

	auto addSize = sinf(m_textAngle) / 16;
	for (int i = 0; i < kItemText.size() - 1; i++)
	{
		float rate = 1.0f;
		if (m_destinationScene - 1 == i)
		{
			rate += addSize;
		}

		int idx = i;
		if (i == 0 && !m_isClear)
		{
			idx = 3;
		}

		FontManager::GetInstance().DrawCenteredExtendText(Game::kWindowWidth / 2, Game::kWindowHeight / 7 * (i + 4), kItemText[idx], 0xffffff, 48, 0x000000, rate);
	}


	//FontManager::GetInstance().DrawCenteredExtendText(Game::kWindowWidth / 2, Game::kWindowHeight / 7 * 4, "次のステージへ", 0xffffff, 48, 0x000000,1.0f);
	//FontManager::GetInstance().DrawCenteredExtendText(Game::kWindowWidth / 2, Game::kWindowHeight / 7 * 5, "スコア詳細へ", 0xffffff, 48, 0x000000, 1.0f);
	//FontManager::GetInstance().DrawCenteredExtendText(Game::kWindowWidth / 2, Game::kWindowHeight / 7 * 6, "セレクトに戻る", 0xffffff, 48, 0x000000, 1.0f);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	if (m_isClear)
	{
		//FontManager::GetInstance().DrawCenteredExtendText(Game::kWindowWidth / 2, Game::kWindowHeight / 6, "Clear!", 0xffff00, 80, 0xff0000, sin(m_resultTextAngle));
		DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 6, 2 * sin(m_resultTextAngle), 0.0f, ResourceManager::GetInstance().GetHandle("I_CLEARTEXT"), true);
	}
	else
	{
		DrawRotaGraph(Game::kWindowWidth / 2, m_resultTextAngle, 1.6f, 0.0f, ResourceManager::GetInstance().GetHandle("I_GAMEOVERTEXT"), true);
	}
}

void SceneResult::DrawScoreDetail()
{
	FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 2, Game::kWindowHeight / 9, "スコア:" + std::to_string(ScoreManager::GetInstance().GetCalculationScore()), 0xffffff, 40, 0x000000);
	FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 2, Game::kWindowHeight / 9 * 3, "タイム:" + std::to_string(ScoreManager::GetInstance().GetTimeScore()), 0xffffff, 40, 0x000000);
	FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 2, Game::kWindowHeight / 9 * 4, "プレイヤーキル:" + std::to_string(ScoreManager::GetInstance().GetPlayerKillScore()), 0xffffff, 40, 0x000000);
	FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 2, Game::kWindowHeight / 9 * 5, "トラップキル:" + std::to_string(ScoreManager::GetInstance().GetTrapKillScore()), 0xffffff, 40, 0x000000);
	FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 2, Game::kWindowHeight / 9 * 6, "クリスタル:" + std::to_string(ScoreManager::GetInstance().GetCrystalScore()), 0xffffff, 40, 0x000000);
	FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 2, Game::kWindowHeight / 9 * 7, "連続キル:" + std::to_string(ScoreManager::GetInstance().GetComboScore()), 0xffffff, 40, 0x000000);

	DrawRotaGraph(36, Game::kWindowHeight - 36, 0.5f, 0.0f, ResourceManager::GetInstance().GetHandle("I_B"), true);
	FontManager::GetInstance().DrawLeftText(36 + 24, Game::kWindowHeight - 36 - 6, ":戻る", 0xffffff, 32, 0xffffff);

}
