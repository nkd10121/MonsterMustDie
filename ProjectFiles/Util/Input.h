#pragma once
#include "DxLib.h"
#include<unordered_map>
#include<map>
#include<string>

enum class InputType
{
	keyboard,	//キーボード
	pad			//パッド
};

//エイリアス宣言
using InputTable_t = std::unordered_map<std::string, int>;

/// <summary>
/// 入力情報管理クラス
/// </summary>
class Input
{
private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Input();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Input() {};

	static Input* m_instance;	//インスタンス

public:
	//コピーコンストラクタを禁止する
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator= (const Input&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static Input& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new Input;
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
	/// 入力情報を更新する
	/// </summary>
	void Update();

	/// <summary>
	/// 指定のコマンドが押された瞬間なのか
	/// </summary>
	/// <param name="command">コマンド文字列</param>
	/// <returns> true : 押された瞬間, false : 押されていないか、押しっぱ</returns>
	bool IsTriggered(const char* command)const;

	/// <summary>
	/// 指定のコマンドが押し続けられているかどうか
	/// </summary>
	/// <param name="command">コマンド文字列</param>
	/// <returns>true : 押し続けられている, false : 押されていないか、押された瞬間</returns>
	bool IsPushed(const char* command)const;

	/// <summary>
	/// スティックの入力情報を取得
	/// </summary>
	/// <param name="isRight">右スティックかどうか　false : 左スティック, true : 右スティック</param>
	/// <returns></returns>
	std::pair<float, float> GetInputStick(bool isRight)const;

	/// <summary>
	/// ZR,ZLボタンの入力情報を取得
	/// </summary>
	/// <param name="isRight">true:ZR,false:ZL</param>
	/// <returns>true:押されている,false:押されていない</returns>
	bool GetIsPushedTriggerButton(bool isRight)const;

private:
	InputTable_t m_commandTable;	//コマンド配列

	//コマンドの入力を覚えておくための配列
	std::map < std::string, bool> m_inputData;		//現在の入力
	std::map < std::string, bool> m_lastInputData;	//直前の入力

	//コントローラーの入力情報
	DINPUT_JOYSTATE m_padState;
	XINPUT_STATE* XInputState;

};

