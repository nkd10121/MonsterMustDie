#include "EnemyStateDamaged.h"

#include "EnemyBase.h"

#include "LoadCSV.h"

namespace
{
	//攻撃を開始するプレイヤーとの距離
	constexpr float kStartAttackLength = 6.0f;
}

EnemyStateDamaged::EnemyStateDamaged(std::shared_ptr<CharacterBase> own) :
	StateBase(own),
	m_damagedCount(0)
{
	//現在のステートを歩き状態にする
	m_nowState = StateKind::Damaged;
	//歩きアニメーションに変える
	own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), "DANAGED"));
}

void EnemyStateDamaged::Init(std::string id)
{
}

void EnemyStateDamaged::Update()
{
	//持ち主が敵かどうかをチェックする
	if (!CheakEnemy())	return;

	//ダウンキャスト
	auto own = std::dynamic_pointer_cast<EnemyBase>(m_pOwn.lock());

	//プレイヤーの速度を0にする(重力の影響を受けながら)
	auto prevVel = own->GetRigidbody()->GetVelocity();
	own->GetRigidbody()->SetVelocity(Vec3(0.0f, prevVel.y, 0.0f));

	//もし被ダメージアニメーションが終わったら
	if (own->GetAnimEnd())
	{
		//索敵範囲内にプレイヤーがいて
		if (own->GetIsSearchInPlayer())
		{
			auto playerPos = own->GetPlayerPos();
			auto moveVec = playerPos - own->GetRigidbody()->GetPos();

			//プレイヤーとの距離が一定距離以下の時は攻撃
			if (moveVec.Length() <= kStartAttackLength)
			{
				ChangeState(StateBase::StateKind::Attack);
				return;
			}
			//離れていたら歩きに遷移する
			else
			{
				ChangeState(StateKind::Walk);
				return;
			}
		}
		else
		{
			ChangeState(StateKind::Walk);
			return;
		}
	}
}
