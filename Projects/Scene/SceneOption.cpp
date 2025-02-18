#include "SceneOption.h"

#include "Setting.h"
#include "SoundManager.h"
#include "FontManager.h"
#include "ResourceManager.h"
#include "Game.h"

#include <sstream>
#include <iomanip> 

namespace
{
	constexpr int kKeyRepeatInitFrame = 20;
}

/// <summary>
/// コンストラクタ
/// </summary>
SceneOption::SceneOption():
	SceneBase("SCENE_OPTION"),
	m_pushCount(0),
	m_keyRepeatFrame(kKeyRepeatInitFrame),
	m_angle(0.0f)
{
	m_updateFunc = &SceneOption::UpdateMasterVolume;
	m_drawFunc = &SceneOption::DrawSound;
	m_nowItem = MasterVolume;

	m_isUpdateSound = true;
}

/// <summary>
/// デストラクタ
/// </summary>
SceneOption::~SceneOption()
{
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void SceneOption::StartLoad()
{
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool SceneOption::IsLoaded() const
{
	return true;
}

/// <summary>
/// 初期化
/// </summary>
void SceneOption::Init()
{
	//フェードアウトをスキップする
	SkipFadeOut();

	m_uiArrowHandle = ResourceManager::GetInstance().GetHandle("I_UIARROW");
}

/// <summary>
/// 終了
/// </summary>
void SceneOption::End()
{
}

/// <summary>
/// 更新
/// </summary>
void SceneOption::Update()
{
	if (!Input::GetInstance().IsPushed("RIGHT") && !Input::GetInstance().IsPushed("LEFT"))
	{
		m_pushCount = 0;
		m_keyRepeatFrame = kKeyRepeatInitFrame;
	}


	if (Input::GetInstance().IsTriggered("RB"))
	{
		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");

		if (!m_isUpdateSound)
		{
			m_isUpdateSound = true;

			m_updateFunc = &SceneOption::UpdateMasterVolume;
			m_drawFunc = &SceneOption::DrawSound;
		}
		else
		{
			m_isUpdateSound = false;

			m_updateFunc = &SceneOption::UpdateSensitivity;
			m_drawFunc = &SceneOption::DrawOther;
		}
	}

	if (Input::GetInstance().IsTriggered("LB"))
	{
		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");

		if (m_isUpdateSound)
		{
			m_isUpdateSound = false;

			m_updateFunc = &SceneOption::UpdateSensitivity;
			m_drawFunc = &SceneOption::DrawOther;
		}
		else
		{
			m_isUpdateSound = true;

			m_updateFunc = &SceneOption::UpdateMasterVolume;
			m_drawFunc = &SceneOption::DrawSound;
		}
	}

	m_angle += 0.12f;

	//状態の更新
	(this->*m_updateFunc)();
}

/// <summary>
/// 描画
/// </summary>
void SceneOption::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 250);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#ifdef _DEBUG	//デバッグ描画
	DrawFormatString(0, 32, 0xffffff, "%f", GetNowSceneName());
#endif

	DrawRotaGraph(36, Game::kWindowHeight - 36, 0.5f, 0.0f, ResourceManager::GetInstance().GetHandle("I_B"), true);
	FontManager::GetInstance().DrawLeftText(36 + 24, Game::kWindowHeight - 36 - 6, ":戻る", 0xffffff, 32, 0xffffff);

	std::vector<unsigned int> textColor;
	for (int i = 0; i <= Item::DrawOperation; i++)
	{
		if (m_nowItem == i)
		{
			textColor.push_back(0xffffff);
		}
		else
		{
			textColor.push_back(0x999999);
		}
	}



	//状態の更新
	(this->*m_drawFunc)(textColor);


	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
	DrawBox(0, 50 - 24, Game::kWindowWidth, 50 + 36, 0xcccccc, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	auto tc = 0x999999;
	if (m_nowItem < 3)	tc = 0xffffff;
	FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 3, 50, "サウンド設定", tc, 40, 0x000000);
	tc = 0x999999;
	if (m_nowItem >= 3)	tc = 0xffffff;
	FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 3 * 2, 50, "その他", tc, 40, 0x000000);


	auto offset = sinf(m_angle) * 4;
	if (m_nowItem < 3)
	{
		DrawRotaGraph(Game::kWindowWidth / 3 + 180 + offset, 58,1.0f,0.0f,m_uiArrowHandle,true);
		DrawRotaGraph(Game::kWindowWidth / 3 + 180 + offset - 10, 58, 0.1f, 0.0f, ResourceManager::GetInstance().GetHandle("I_UIRB"), true);
		DrawRotaGraph(Game::kWindowWidth / 3 - 180 - offset, 58,1.0f,0.0f,m_uiArrowHandle,true,true);
		DrawRotaGraph(Game::kWindowWidth / 3 - 180 - offset + 10, 58, 0.1f, 0.0f, ResourceManager::GetInstance().GetHandle("I_UILB"), true);


	}
	else
	{
		DrawRotaGraph(Game::kWindowWidth / 3 * 2 + 120 + offset, 58, 1.0f, 0.0f, m_uiArrowHandle, true);
		DrawRotaGraph(Game::kWindowWidth / 3 * 2 - 120 - offset, 58, 1.0f, 0.0f, m_uiArrowHandle, true,true);
		DrawRotaGraph(Game::kWindowWidth / 3 * 2 + 120 + offset - 10, 58, 0.1f, 0.0f, ResourceManager::GetInstance().GetHandle("I_UIRB"), true);
		DrawRotaGraph(Game::kWindowWidth / 3 * 2 - 120 - offset + 10, 58, 0.1f, 0.0f, ResourceManager::GetInstance().GetHandle("I_UILB"), true);
	}
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void SceneOption::SelectNextSceneUpdate()
{
	//Bボタンを押したら元のシーンに戻る
	if (Input::GetInstance().IsTriggered("CANCEL"))
	{
		EndThisScene();
		SkipFadeIn();
		return;
	}
}

void SceneOption::UpdateMasterVolume()
{
	m_nowItem = MasterVolume;
	if (Input::GetInstance().IsPushed("RIGHT"))
	{
		if (m_pushCount == 0 || m_pushCount > m_keyRepeatFrame)
		{
			if (m_pushCount > m_keyRepeatFrame)
			{
				m_pushCount = 0;
				m_keyRepeatFrame = max(m_keyRepeatFrame * 0.6f, 0);
			}
			Setting::GetInstance().SetMasterVolume(min(Setting::GetInstance().GetMasterVolume() + 0.01f, 1.0f));
			SoundManager::GetInstance().BGMChangeVolume();
		}
		m_pushCount++;
	}
	if (Input::GetInstance().IsPushed("LEFT"))
	{
		if (m_pushCount == 0 || m_pushCount > m_keyRepeatFrame)
		{
			if (m_pushCount > m_keyRepeatFrame)
			{
				m_pushCount = 0;
				m_keyRepeatFrame = max(m_keyRepeatFrame * 0.6f, 0);
			}
			Setting::GetInstance().SetMasterVolume(max(Setting::GetInstance().GetMasterVolume() - 0.01f, 0.0f));
			SoundManager::GetInstance().BGMChangeVolume();
		}
		m_pushCount++;
	}

	if (Input::GetInstance().IsTriggered("DOWN"))
	{
		m_updateFunc = &SceneOption::UpdateBGMVolume;

		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
	}
}

void SceneOption::UpdateBGMVolume()
{
	m_nowItem = BGMVolume;
	if (Input::GetInstance().IsPushed("RIGHT"))
	{
		if (m_pushCount == 0 || m_pushCount > m_keyRepeatFrame)
		{
			if (m_pushCount > m_keyRepeatFrame)
			{
				m_pushCount = 0;
				m_keyRepeatFrame = max(m_keyRepeatFrame * 0.6f, 0);
			}
			Setting::GetInstance().SetBGMVolume(min(Setting::GetInstance().GetBGMVolume() + 0.01f, 1.0f));
			SoundManager::GetInstance().BGMChangeVolume();
		}
		m_pushCount++;
	}
	if (Input::GetInstance().IsPushed("LEFT"))
	{
		if (m_pushCount == 0 || m_pushCount > m_keyRepeatFrame)
		{
			if (m_pushCount > m_keyRepeatFrame)
			{
				m_pushCount = 0;
				m_keyRepeatFrame = max(m_keyRepeatFrame * 0.6f, 0);
			}
			Setting::GetInstance().SetBGMVolume(max(Setting::GetInstance().GetBGMVolume() - 0.01f, 0.0f));
			SoundManager::GetInstance().BGMChangeVolume();
		}
		m_pushCount++;
	}

	if (Input::GetInstance().IsTriggered("UP"))
	{
		m_updateFunc = &SceneOption::UpdateMasterVolume;

		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
	}
	if (Input::GetInstance().IsTriggered("DOWN"))
	{
		m_updateFunc = &SceneOption::UpdateSEVolume;

		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
	}
}

void SceneOption::UpdateSEVolume()
{
	m_nowItem = SEVolume;
	if (Input::GetInstance().IsPushed("RIGHT"))
	{
		if (m_pushCount == 0 || m_pushCount > m_keyRepeatFrame)
		{
			if (m_pushCount > m_keyRepeatFrame)
			{
				m_pushCount = 0;
				m_keyRepeatFrame = max(m_keyRepeatFrame * 0.6f, 0);
			}
			Setting::GetInstance().SetSEVolume(min(Setting::GetInstance().GetSEVolume() + 0.01f, 1.0f));
		}
		m_pushCount++;
	}
	if (Input::GetInstance().IsPushed("LEFT"))
	{
		if (m_pushCount == 0 || m_pushCount > m_keyRepeatFrame)
		{
			if (m_pushCount > m_keyRepeatFrame)
			{
				m_pushCount = 0;
				m_keyRepeatFrame = max(m_keyRepeatFrame * 0.6f, 0);
			}
			Setting::GetInstance().SetSEVolume(max(Setting::GetInstance().GetSEVolume() - 0.01f, 0.0f));
		}
		m_pushCount++;
	}

	if (Input::GetInstance().IsTriggered("UP"))
	{
		m_updateFunc = &SceneOption::UpdateBGMVolume;

		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
	}
	//if (Input::GetInstance().IsTriggered("DOWN"))
	//{
	//	m_updateFunc = &SceneOption::SensitivityUpdate;
	//}
}

void SceneOption::UpdateSensitivity()
{
	m_nowItem = Sensitivity;
	if (Input::GetInstance().IsPushed("RIGHT"))
	{
		if (m_pushCount == 0 || m_pushCount > m_keyRepeatFrame)
		{
			if (m_pushCount > m_keyRepeatFrame)
			{
				m_pushCount = 0;
				m_keyRepeatFrame = max(m_keyRepeatFrame * 0.6f, 0);
			}
			Setting::GetInstance().SetSensitivity(min(Setting::GetInstance().GetSensitivity() + 0.01f, 1.0f));
		}
		m_pushCount++;
	}
	if (Input::GetInstance().IsPushed("LEFT"))
	{
		if (m_pushCount == 0 || m_pushCount > m_keyRepeatFrame)
		{
			if (m_pushCount > m_keyRepeatFrame)
			{
				m_pushCount = 0;
				m_keyRepeatFrame = max(m_keyRepeatFrame * 0.6f, 0);
			}
			Setting::GetInstance().SetSensitivity(max(Setting::GetInstance().GetSensitivity() - 0.01f, 0.0f));
		}
		m_pushCount++;
	}

	//if (Input::GetInstance().IsTriggered("UP"))
	//{
	//	m_updateFunc = &SceneOption::SeUpdate;
	//}
	if (Input::GetInstance().IsTriggered("DOWN"))
	{
		m_updateFunc = &SceneOption::UpdateFullScreen;

		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
	}
}

void SceneOption::UpdateFullScreen()
{
	m_nowItem = FullScreen;
	if (Input::GetInstance().IsTriggered("RIGHT"))
	{
		Setting::GetInstance().SetIsFullScreen(!Setting::GetInstance().GetIsFullScreen());
		ChangeWindowMode(!Setting::GetInstance().GetIsFullScreen());
	}
	else if(Input::GetInstance().IsTriggered("LEFT"))
	{
		Setting::GetInstance().SetIsFullScreen(!Setting::GetInstance().GetIsFullScreen());
		ChangeWindowMode(!Setting::GetInstance().GetIsFullScreen());
	}

	if (Input::GetInstance().IsTriggered("UP"))
	{
		m_updateFunc = &SceneOption::UpdateSensitivity;

		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
	}
	if (Input::GetInstance().IsTriggered("DOWN"))
	{
		m_updateFunc = &SceneOption::UpdateDrawOperation;

		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
	}
}

void SceneOption::UpdateDrawOperation()
{
	m_nowItem = DrawOperation;
	if (Input::GetInstance().IsTriggered("RIGHT"))
	{
		Setting::GetInstance().SetIsDrawOperation(!Setting::GetInstance().GetIsDrawOperation());
	}
	else if (Input::GetInstance().IsTriggered("LEFT"))
	{
		Setting::GetInstance().SetIsDrawOperation(!Setting::GetInstance().GetIsDrawOperation());
	}

	if (Input::GetInstance().IsTriggered("UP"))
	{
		m_updateFunc = &SceneOption::UpdateFullScreen;

		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
	}
}

void SceneOption::DrawSound(std::vector<unsigned int> color)
{
	std::string text = "マスターボリューム:";
	FontManager::GetInstance().DrawLeftText(200, 150, text, color[0], 32, 0x000000);

	FontManager::GetInstance().DrawCenteredText(240, 220, std::to_string(static_cast<int>(Setting::GetInstance().GetMasterVolume() * 100)), color[0], 48, 0x000000);
	DrawBox(320, 220, 900, 240, color[0], false);
	auto x = (900 - 320) * Setting::GetInstance().GetMasterVolume();
	DrawBox(320, 220, 320 + x, 240, color[0], true);

	text = "BGMボリューム:";
	FontManager::GetInstance().DrawLeftText(200, 150 + 180, text, color[1], 32, 0x000000);

	FontManager::GetInstance().DrawCenteredText(240, 220 + 180, std::to_string(static_cast<int>(Setting::GetInstance().GetBGMVolume() * 100)), color[1], 48, 0x000000);
	DrawBox(320, 220 + 180, 900, 240 + 180, color[1], false);
	x = (900 - 320) * Setting::GetInstance().GetBGMVolume();
	DrawBox(320, 220 + 180, 320 + x, 240 + 180, color[1], true);

	text = "SEボリューム:";
	FontManager::GetInstance().DrawLeftText(200, 150 + 180 * 2, text, color[2], 32, 0x000000);
	FontManager::GetInstance().DrawCenteredText(240, 220 + 180 * 2, std::to_string(static_cast<int>(Setting::GetInstance().GetSEVolume() * 100)), color[2], 48, 0x000000);

	DrawBox(320, 220 + 180 * 2, 900, 240 + 180 * 2, color[2], false);
	x = (900 - 320) * Setting::GetInstance().GetSEVolume();
	DrawBox(320, 220 + 180 * 2, 320 + x, 240 + 180 * 2, color[2], true);
}

void SceneOption::DrawOther(std::vector<unsigned int> color)
{
	std::string text = "カメラ感度:";
	FontManager::GetInstance().DrawLeftText(200, 150, text, color[3], 32, 0x000000);

	FontManager::GetInstance().DrawCenteredText(240, 220, std::to_string(static_cast<int>(Setting::GetInstance().GetSensitivity() * 100)), color[3], 48, 0x000000);
	DrawBox(320, 220, 900, 240, color[3], false);
	auto x = (900 - 320) * Setting::GetInstance().GetSensitivity();
	DrawBox(320, 220, 320 + x, 240, color[3], true);

	text = "ディスプレイモード:";
	FontManager::GetInstance().DrawLeftText(200, 150 + 180, text, color[4], 32, 0x000000);

	if (Setting::GetInstance().GetIsFullScreen())
	{
		text = "フルスクリーン";
	}
	else
	{
		text = "ウィンドウ";
	}

	FontManager::GetInstance().DrawCenteredText(600, 220 + 180, text, color[4], 32, 0x000000);

	if (m_nowItem == Item::FullScreen)
	{
		auto offset = sinf(-m_angle) * 2;

		DrawRotaGraph(600 + 180 + offset, 220 + 180 + 5, 1.0f, 0.0f, m_uiArrowHandle, true);
		DrawRotaGraph(600 - 180 - offset, 220 + 180 + 5, 1.0f, 0.0f, m_uiArrowHandle, true, true);
	}

	text = "操作説明UIを描画:";
	FontManager::GetInstance().DrawLeftText(200, 150 + 180 * 2, text, color[5], 32, 0x000000);


	if (Setting::GetInstance().GetIsDrawOperation())
	{
		text = "する";
	}
	else
	{
		text = "しない";
	}

	FontManager::GetInstance().DrawCenteredText(600, 220 + 180 * 2, text, color[5], 32, 0x000000);
	if (m_nowItem == Item::DrawOperation)
	{
		auto offset = sinf(-m_angle) * 2;

		DrawRotaGraph(600 + 110 + offset, 220 + 180 * 2 + 5, 1.0f, 0.0f, m_uiArrowHandle, true);
		DrawRotaGraph(600 - 110 - offset, 220 + 180 * 2 + 5, 1.0f, 0.0f, m_uiArrowHandle, true, true);
	}
}
