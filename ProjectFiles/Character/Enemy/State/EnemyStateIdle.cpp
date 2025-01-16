#include "EnemyStateIdle.h"
#include "EnemyStateWalk.h"

#include "CharacterBase.h"
#include "EnemyBase.h"

#include "LoadCSV.h"

/// <summary>
/// コンストラクタ
/// </summary>
EnemyStateIdle::EnemyStateIdle(std::shared_ptr<CharacterBase> own) :
	StateBase(own)
{
	//現在のステートを待機状態にする
	m_nowState = StateKind::Idle;
	//アニメーションを待機にする
	own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), "IDLE"));
}

/// <summary>
/// 初期化
/// </summary>
void EnemyStateIdle::Init(std::string id)
{
}

/// <summary>
/// 更新
/// </summary>
void EnemyStateIdle::Update()
{
	//持ち主が敵かどうかをチェックする
	if (!CheakEnemy())	return;

	auto own = std::dynamic_pointer_cast<EnemyBase>(m_pOwn.lock());

	//索敵範囲内にプレイヤーが存在していたら歩き状態に遷移する
	if (own->GetIsSearchInPlayer())
	{
		ChangeState(StateKind::Walk);
		return;
	}

	//プレイヤーの速度を0にする(重力の影響を受けながら)
	auto prevVel = own->GetRigidbody()->GetVelocity();
	own->GetRigidbody()->SetVelocity(Vec3(0.0f, prevVel.y, 0.0f));
}
