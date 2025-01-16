#include "PlayerStateDeath.h"
#include "Player.h"

#include "LoadCSV.h"

PlayerStateDeath::PlayerStateDeath(std::shared_ptr<CharacterBase> own):
	StateBase(own)
{
	//現在のステートを待機状態にする
	m_nowState = StateKind::Death;

	//アニメーションを変える
	own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), "DEATH_A"));
}

PlayerStateDeath::~PlayerStateDeath()
{
}

void PlayerStateDeath::Init(std::string id)
{
	m_stageColId = id;
}

void PlayerStateDeath::Update()
{
	//持っているキャラクターベースクラスをプレイヤークラスにキャストする(ダウンキャスト)
	auto own = std::dynamic_pointer_cast<Player>(m_pOwn.lock());

	//プレイヤーの速度を0にする(重力の影響を受けながら)
	auto prevVel = own->GetRigidbody()->GetVelocity();
	own->GetRigidbody()->SetVelocity(Vec3(0.0f, prevVel.y, 0.0f));
}
