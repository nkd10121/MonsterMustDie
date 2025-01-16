#include "EnemyStateDeath.h"
#include "CharacterBase.h"

#include "LoadCSV.h"

/// <summary>
/// コンストラクタ
/// </summary>
EnemyStateDeath::EnemyStateDeath(std::shared_ptr<CharacterBase> own):
	StateBase(own)
{
	//現在のステートを待機状態にする
	m_nowState = StateKind::Death;

	//アニメーションを変える
	own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), "DEATH_A"));
}

/// <summary>
/// 初期化
/// </summary>
void EnemyStateDeath::Init(std::string id)
{
}

/// <summary>
/// 更新
/// </summary>
void EnemyStateDeath::Update()
{
}
