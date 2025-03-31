#include "EnemyStateAttack.h"

#include "CharacterBase.h"
#include "EnemyBase.h"

#include "LoadCSV.h"

namespace
{
	//攻撃の当たり判定を生成するフレーム
	constexpr int kCreateAttackCollisionFrame = 20;
	//攻撃の当たり判定を削除するフレーム
	constexpr int kDeleteAttackCollisionFrame = 45;
	//攻撃を開始するプレイヤーとの距離
	constexpr float kStartAttackLength = 6.0f;
}

/// <summary>
/// コンストラクタ
/// </summary>
EnemyStateAttack::EnemyStateAttack(std::shared_ptr<CharacterBase> own):
	StateBase(own),
	m_attackCollisionCount(0),
	m_attackVec()
{
	//現在のステートを歩き状態にする
	m_nowState = StateKind::Attack;
	own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), "ATTACK"));

	//攻撃方向を計算
	auto enemy = std::dynamic_pointer_cast<EnemyBase>(m_pOwn.lock());
	auto playerPos = enemy->GetPlayerPos();
	m_attackVec = playerPos - own->GetRigidbody()->GetPos();
}

/// <summary>
/// 更新
/// </summary>
void EnemyStateAttack::Update()
{
	// EnemyBaseへのポインタを取得
	auto own = std::dynamic_pointer_cast<EnemyBase>(m_pOwn.lock());

	//プレイヤーの速度を0にする(重力の影響を受けながら)
	auto prevVel = own->GetRigidbody()->GetVelocity();
	own->GetRigidbody()->SetVelocity(Vec3(0.0f, prevVel.y, 0.0f));

	// 攻撃の当たり判定を生成するフレームになったら判定を生成する
	if (m_attackCollisionCount == kCreateAttackCollisionFrame)
	{
		own->CreateAttackCollision(m_attackVec);
	}
	// 攻撃の当たり判定を削除するフレームになったら判定を削除する
	else if (m_attackCollisionCount == kDeleteAttackCollisionFrame)
	{
		own->DeleteAttackCollision();
	}
	//カウントを更新する
	m_attackCollisionCount++;

	//アニメーションが一回終了した時
	if (m_pOwn.lock()->GetAnimEnd())
	{
		//索敵範囲内にプレイヤーがいて
		if (own->GetIsSearchInPlayer())
		{
			//プレイヤーと敵との距離を計算
			auto playerPos = own->GetPlayerPos();
			auto moveVec = playerPos - own->GetRigidbody()->GetPos();

			//プレイヤーとの距離が一定距離以下の時は攻撃状態に入りなおす
			if (moveVec.Length() <= kStartAttackLength)
			{
				//atan2を使用して向いている角度を取得
				auto angle = atan2(moveVec.x, moveVec.z);
				auto rotation = VGet(0.0f, angle + DX_PI_F, 0.0f);
				//移動方向に体を回転させる
				own->SetModelRotation(rotation);
				own->SetHeadCollisionFrontVec(playerPos);

				m_attackCollisionCount = 0;
				ChangeState(StateBase::StateKind::Attack);
				return;
			}
			//離れていたら歩き状態に遷移する
			else
			{
				ChangeState(StateKind::Walk);
				return;
			}
		}
		//索敵範囲内にプレイヤーがいなかったら歩き状態に遷移する
		else
		{
			ChangeState(StateKind::Walk);
			return;
		}
	}
}
