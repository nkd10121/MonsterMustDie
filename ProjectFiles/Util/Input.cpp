#include "Input.h"
#include <cassert>

Input* Input::m_instance = nullptr;

namespace
{
	enum PadCheckMask : int
	{
		A = 0x00000010,	//Aボタン
		B = 0x00000020,	//Bボタン
		X = 0x00000040,	//Xボタン
		Y = 0x00000080,	//Yボタン

		L = 0x00000100,	//Lボタン
		R = 0x00000200,	//Rボタン

		RStickPush = 0x00002000,	//Rスティック押し込み
		test = 0x00008000,	//Rスティック押し込み

		P = 0x00000800,	//ホームボタン(?)

		UP = 0x00000008,	//上ボタン
		RIGHT = 0x00000004,	//右ボタン
		DOWN = 0x00000001,	//下ボタン
		LEFT = 0x00000002,	//左ボタン
	};

	//トリガーボタンのデッドゾーン
	constexpr int kTriggerDeadZone = 60;
}

/// <summary>
/// コンストラクタ
/// </summary>
Input::Input():
	m_padState(),
	XInputState(new XINPUT_STATE)
{
	//ここでコマンドテーブルにコマンドを追加する

	m_commandTable["INPUT_JUMP"] = { PadCheckMask::A };
	m_commandTable["INPUT_DASH"] = { PadCheckMask::B };

	m_commandTable["OK"] = { PadCheckMask::A };
	m_commandTable["CANCEL"] = { PadCheckMask::B };

	m_commandTable["X"] = { PadCheckMask::X };
	m_commandTable["Y"] = { PadCheckMask::Y };

	m_commandTable["RB"] = { PadCheckMask::R };
	m_commandTable["LB"] = { PadCheckMask::L };

	m_commandTable["PAUSE"] = { PadCheckMask::P };

	m_commandTable["UP"] = { PadCheckMask::UP };
	m_commandTable["DOWN"] = { PadCheckMask::DOWN };
	m_commandTable["LEFT"] = { PadCheckMask::LEFT };
	m_commandTable["RIGHT"] = { PadCheckMask::RIGHT };

}

/// <summary>
/// 入力情報を更新する
/// </summary>
void Input::Update()
{
	m_lastInputData = m_inputData;	//直前入力をコピーしておく(押した瞬間を取得する用)

	int padstate = GetJoypadInputState(DX_INPUT_PAD1);//パッド情報の取得

	//登録された情報とハードの情報を照らし合わせながら
	//inputData_の内容を更新していく
	for (const auto& cmd : m_commandTable)
	{
		auto& input = m_inputData[cmd.first];//コマンドの名前から入力データを作る

		//コントローラーのチェック
		input = false;
		if (padstate & cmd.second)
		{
			input = true;
		}
	}

	//ボタンの入力を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_padState);
	//ZR,ZLの入力を取得
	GetJoypadXInputState(DX_INPUT_PAD1, XInputState);
}

/// <summary>
/// 指定のコマンドが押された瞬間なのか
/// </summary>
bool Input::IsTriggered(const char* command) const
{
	auto it = m_inputData.find(command);
	if (it == m_inputData.end())	//情報がなければfalseを返す
	{
		assert(0 && "指定されたコマンドは登録されていません");
		return false;
	}
	return (m_inputData.at(command) && !m_lastInputData.at(command));
}

/// <summary>
/// 指定のコマンドが押し続けられているかどうか
/// </summary>
bool Input::IsPushed(const char* command) const
{
	auto it = m_inputData.find(command);
	if (it == m_inputData.end())	//情報がなければfalseを返す
	{
		assert(0 && "指定されたコマンドは登録されていません");
		return false;
	}
	return (m_inputData.at(command) && m_lastInputData.at(command));
}

/// <summary>
/// スティックの入力情報を取得
/// </summary>
std::pair<float, float> Input::GetInputStick(bool isRight) const
{
	if (isRight)
	{
		//右スティックの入力情報を返す
		return std::make_pair(static_cast<float>(m_padState.Rx), static_cast<float>(m_padState.Ry));
	}
	else
	{
		//左スティックの入力情報を返す
		return std::make_pair(static_cast<float>(m_padState.X), static_cast<float>(m_padState.Y));
	}
}

/// <summary>
/// ZR,ZLボタンの入力情報を取得
/// </summary>
bool Input::GetIsPushedTriggerButton(bool isRight)const
{
	if (isRight)
	{
		//ZRの入力情報を返す
		if (XInputState->RightTrigger > kTriggerDeadZone)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		//ZRの入力情報を返す
		if (XInputState->LeftTrigger > kTriggerDeadZone)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}