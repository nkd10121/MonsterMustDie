#pragma once
#include <memory>
#include <string>
#include "MyLib.h"

class CharacterBase;

/// <summary>
/// ステートパターンの基底クラス
/// </summary>
class StateBase : public std::enable_shared_from_this<StateBase>
{
public:
	//ステート
	enum class StateKind
	{
		Null,
		Idle,		//待機
		Walk,		//移動
		Dash,		//ダッシュ
		Jump,		//ジャンプ
		Attack,		//攻撃
		Damaged,		//被ダメージ
		Death,		//死亡
		Clear,		//クリア
	};

protected:
	//左スティックの入力方向
	const enum eDir : int
	{
		Forward,
		ForwardRight,
		Right,
		BackRight,
		Back,
		BackLeft,
		Left,
		ForwardLeft,
		None
	};
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="own">所有者</param>
	StateBase(std::shared_ptr<CharacterBase> own);
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~StateBase();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init(std::string id)abstract;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() abstract;

	/// <summary>
	/// 現在のStateを返す
	/// </summary>
	/// <returns>現在のState</returns>
	StateKind GetKind() { return m_nowState; }
	/// <summary>
	/// 遷移予定のStateを返す
	/// </summary>
	/// <returns>遷移予定のState</returns>
	const StateKind GetNextKind()const { return m_nextState; }

	/// <summary>
	/// 遷移予定のStateを設定する
	/// </summary>
	/// <param name="state">遷移予定のState</param>
	void SetNextKind(StateKind state) { m_nextState = state; }

	///// <summary>
	///// ダメージを受けた時の処理を行う
	///// </summary>
	///// <param name="collider"></param>
	///// <returns>ダメージ</returns>
	//virtual int OnDamage(std::shared_ptr<MyLib::Collidable> collider) abstract;


#ifdef _DEBUG	//デバッグ描画
	/// <summary>
	/// 現在のステートのデバッグ描画
	/// </summary>
	/// <param name="x">x座標</param>
	/// <param name="y">y座標</param>
	void DebugDrawState(int x, int y);
#endif

protected:
	/// <summary>
	/// ステートを遷移する
	/// </summary>
	/// <param name="kind"></param>
	void ChangeState(StateKind kind);

	/// <summary>
	/// 持ち主がプレイヤーかどうかを判断する
	/// </summary>
	bool CheckPlayer();

	/// <summary>
	/// 持ち主が敵かどうかを判断する
	/// </summary>
	/// <returns></returns>
	bool CheakEnemy();

	/// <summary>
	/// 引数の2つの値から角度を計算する
	/// </summary>
	/// <param name="x">X</param>
	/// <param name="y">Y</param>
	/// <returns>入力方向</returns>
	const eDir GetDirection(float x, float y)const;

protected:
	std::weak_ptr<CharacterBase> m_pOwn;	//所有者のポインタ

	std::string m_stageColId;

	StateKind m_nowState;	//現在のステート
	StateKind m_nextState;	//現在のステート

};