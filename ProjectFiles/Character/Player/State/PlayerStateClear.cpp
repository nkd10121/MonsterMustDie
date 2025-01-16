#include "PlayerStateClear.h"
#include "CharacterBase.h"
#include "Player.h"

#include "LoadCSV.h"

PlayerStateClear::PlayerStateClear(std::shared_ptr<CharacterBase> own):
	StateBase(own)
{
	//現在のステートをダッシュ状態にする
	m_nowState = StateKind::Clear;
	own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), "CLEAR"));
}

PlayerStateClear::~PlayerStateClear()
{
}

void PlayerStateClear::Init(std::string id)
{
	m_stageColId = id;
}

void PlayerStateClear::Update()
{
	//持ち主がプレイヤーかどうかをチェックする
	if (!CheckPlayer())	return;

	//持っているキャラクターベースクラスをプレイヤークラスにキャストする(ダウンキャスト)
	auto own = std::dynamic_pointer_cast<Player>(m_pOwn.lock());

	//プレイヤーの速度を0にする(重力の影響を受けながら)
	auto prevVel = own->GetRigidbody()->GetVelocity();
	own->GetRigidbody()->SetVelocity(Vec3(0.0f, prevVel.y, 0.0f));
}
