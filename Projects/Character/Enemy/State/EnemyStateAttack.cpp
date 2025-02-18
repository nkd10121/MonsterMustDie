#include "EnemyStateAttack.h"

#include "CharacterBase.h"
#include "EnemyBase.h"

#include "LoadCSV.h"

namespace
{
	/// <summary>
	/// 攻撃アニメーションが1回終わったかどうか
	/// </summary>
	/// <param name="num">アニメーションの再生フレーム</param>
	/// <returns></returns>
	const bool IsAttackAnimEnd(float num)
	{
		while (true)
		{
			if (num < 40.0f) break;
			num -= 40.0f;
		}

		return num == 0.0f;
	}

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
	m_waitCount(0),
	m_attackVec()
{
	//現在のステートを歩き状態にする
	m_nowState = StateKind::Attack;
	own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), "ATTACK"));

	auto enemy = std::dynamic_pointer_cast<EnemyBase>(m_pOwn.lock());
	auto playerPos = enemy->GetPlayerPos();
	m_attackVec = playerPos - own->GetRigidbody()->GetPos();
}

/// <summary>
/// 初期化
/// </summary>
void EnemyStateAttack::Init(std::string id)
{
}

/// <summary>
/// 更新
/// </summary>
void EnemyStateAttack::Update()
{
	auto own = std::dynamic_pointer_cast<EnemyBase>(m_pOwn.lock());

	//プレイヤーの速度を0にする(重力の影響を受けながら)
	auto prevVel = own->GetRigidbody()->GetVelocity();
	own->GetRigidbody()->SetVelocity(Vec3(0.0f, prevVel.y, 0.0f));

	if (m_waitCount == kCreateAttackCollisionFrame)
	{
		own->CreateAttackCollision(m_attackVec);
	}
	else if (m_waitCount == kDeleteAttackCollisionFrame)
	{
		own->DeleteAttackCollision();
	}
	m_waitCount++;

	auto frame = m_pOwn.lock()->GetAnimNowFrame();

	//アニメーション上で攻撃が一回終了した時
	if (IsAttackAnimEnd(frame) && frame > 0)
	{
		//索敵範囲内にプレイヤーがいて
		if (own->GetIsSearchInPlayer())
		{
			auto playerPos = own->GetPlayerPos();
			auto moveVec = playerPos - own->GetRigidbody()->GetPos();

			//プレイヤーとの距離が一定距離以下の時は攻撃
			if (moveVec.Length() <= kStartAttackLength)
			{
				//atan2を使用して向いている角度を取得
				auto angle = atan2(moveVec.x, moveVec.z);
				auto rotation = VGet(0.0f, angle + DX_PI_F, 0.0f);
				//移動方向に体を回転させる
				own->SetModelRotation(rotation);
				own->SetHeadCollisionFrontVec(playerPos);

				m_waitCount = 0;
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
		//索敵範囲内にプレイヤーがいなかったら待機にする
		else
		{
			ChangeState(StateKind::Walk);
			return;
		}
	}
}
