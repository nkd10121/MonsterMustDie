#include "EnemyStateDeath.h"
#include "CharacterBase.h"
#include "EnemyBase.h"

#include "LoadCSV.h"
#include "DrawUI.h"
#include "FontManager.h"
#include "ResourceManager.h"

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

	m_frame = 0;
	m_trapPointGraphHandle = ResourceManager::GetInstance().GetHandle("I_TRAPICON");
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
	//持ち主が敵かどうかをチェックする
	if (!CheakEnemy())	return;

	auto own = std::dynamic_pointer_cast<EnemyBase>(m_pOwn.lock());

	auto pos = own->GetPos();
	auto drawPos = ConvWorldPosToScreenPos(pos.ToVECTOR());

	auto point = own->GetDropPoint();

	//ドロップする罠ポイントの描画
	DrawUI::GetInstance().RegisterDrawRequest([=]()
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - 6 * m_frame);
		DrawRotaGraph(static_cast<int>(drawPos.x) - 60, static_cast<int>(drawPos.y) - 80 - m_frame / 4, 0.66f, 0.0f, m_trapPointGraphHandle, true);
		FontManager::GetInstance().DrawCenteredText(static_cast<int>(drawPos.x), static_cast<int>(drawPos.y) - 80 - m_frame / 4, std::to_string(point), 0x91cdd9, 32, 0x395f62);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}, 2);

	m_frame++;
}