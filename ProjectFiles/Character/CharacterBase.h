#pragma once
#include "MyLib.h"
#include <map>
#include <memory>
#include <string>

#include "StateBase.h"

/// <summary>
/// ゲーム内のキャラクターの基底クラス
/// </summary>
class CharacterBase : public MyLib::Collidable
{
public:
	//ステータス構造体
	struct Status
	{
		int hp;			//体力
		int atk;		//攻撃力
		int def;		//防御力
		float speed;	//移動速度
		int point;		//死亡時のポイント
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="priority">優先度</param>
	/// <param name="tag">タグ</param>
	CharacterBase(Priority priority, GameObjectTag tag);
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~CharacterBase();

	/// <summary>
	/// ほかのオブジェクトと衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider">当たったオブジェクト</param>
	virtual void OnCollideEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override {};

	/// <summary>
	/// 自身の現在のHPを取得
	/// </summary>
	/// <returns>現在のHP</returns>
	const int GetHp()const { return m_status.hp; }
	///// <summary>
	///// 自身の最大HPを取得
	///// </summary>
	///// <returns>最大HP</returns>
	//const int GetMaxHp()const { return m_maxHp; }
	/// <summary>
	/// 自身の移動速度を取得
	/// </summary>
	/// <returns>自身の移動速度</returns>
	const float GetMoveSpeed()const { return m_status.speed; }
	/// <summary>
	/// 物理データを取得
	/// </summary>
	/// <returns>物理データ</returns>
	std::shared_ptr<MyLib::Rigidbody> GetRigidbody(){ return rigidbody; }
	/// <summary>
	/// 現在再生中のアニメーションの終了フレームを取得
	/// </summary>
	/// <returns></returns>
	const float GetNowAnimEndFrame()const { return MV1GetAttachAnimTotalTime(m_modelHandle, m_currentAnimNo); }
	/// <summary>
	/// 現在再生中のアニメーションの現在フレームを取得
	/// </summary>
	/// <returns></returns>
	const float GetAnimNowFrame()const { return MV1GetAttachAnimTime(m_modelHandle, m_currentAnimNo); }
	/// <summary>
	/// 現在再生中のアニメーションが終了したかどうか
	/// </summary>
	/// <returns></returns>
	const bool GetAnimEnd()const { return m_isAnimationFinish; }
	/// <summary>
	/// 自身のキャラクター名を取得
	/// </summary>
	/// <returns>キャラクター名</returns>
	const std::string GetCharacterName()const { return m_characterName; }
protected:		/*継承先のクラスで呼ぶ関数*/
	/// <summary>
	/// アニメーションの切り替え
	/// </summary>
	void AnimationBlend();
public:
	/// <summary>
	/// アニメーションの更新
	/// </summary>
	/// <param name="attachNo">進行させたいアニメーション番号</param>
	/// <returns>ループしたかどうか</returns>
	bool UpdateAnim(int attachNo, float startTime = 0.0f);
	/// <summary>
	///	アニメーションの変更
	/// </summary>
	/// <param name="animIndex">変更後のアニメーション番号</param>
	void ChangeAnim(int animIndex, float animSpeed = 0.5f);

	bool IsInAir();

	/// <summary>
	/// ステートを変更する
	/// 自身のステートクラスから呼ぶ専用
	/// </summary>
	/// <param name="nextState">変更先</param>
	void ChangeState(std::shared_ptr<StateBase> nextState);

protected:
	int m_modelHandle;	//モデルハンドル
	std::string m_characterName;	//キャラクター名 
									//データの取得や種類の判別に使う

	Status m_status;	//ステータス
	int m_hpMax;

	/*アニメーション関係*/
	std::map<std::string, int> m_animIdx;
	int m_currentAnimNo;		//現在のアニメーション
	int m_prevAnimNo;			//変更前のアニメーション
	float m_animBlendRate;		//アニメーションの合成割合
	//0.0f:prevが再生
	//1.0f:currntが再生

	int m_nowAnimIdx;
	int m_preAnimIdx;

	float m_animSpeed;			//アニメーション再生速度
	bool m_isAnimationFinish;	//アニメーションが終わったかどうか

	std::shared_ptr<StateBase> m_pState;	//Stateパターン

};

