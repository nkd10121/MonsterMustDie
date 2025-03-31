#include "EnemyStateDeath.h"
#include "CharacterBase.h"
#include "EnemyBase.h"

#include "LoadCSV.h"
#include "DrawUI.h"
#include "FontManager.h"
#include "ResourceManager.h"

namespace
{
	//α値の最大
	constexpr int kAlphaMax = 255;
	//α値の倍率
	constexpr int kAlphaMag = 6;

	constexpr int kDrawXOffset = 60;
	constexpr int kDrawYOffset = 80;
	// フレーム分割数
	constexpr int kFrameDivisor = 4;
	// アイコンのスケール
	constexpr float kIconScale = 0.66f;
	// フォントサイズ
	constexpr int kFontSize = 32;
	// フォントカラー
	constexpr unsigned int kFontColor = 0x91cdd9;
	// フォントの影のカラー
	constexpr unsigned int kFontShadowColor = 0x395f62;
	// 描画優先度
	constexpr int kDrawPriority = 2;
}

/// <summary>
/// コンストラクタ
/// </summary>
EnemyStateDeath::EnemyStateDeath(std::shared_ptr<CharacterBase> own) :
	StateBase(own)
{
	//現在のステートを待機状態にする
	m_nowState = StateKind::Death;

	//アニメーションを変える
	own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), "DEATH_A"));

	m_deathFrame = 0;
	m_trapPointGraphHandle = ResourceManager::GetInstance().GetHandle("I_TRAPICON");
}

/// <summary>
/// 更新
/// </summary>
void EnemyStateDeath::Update()
{
	//持ち主が敵かどうかをチェックする
	if (!CheakEnemy())	return;

	auto own = std::dynamic_pointer_cast<EnemyBase>(m_pOwn.lock());

	//自身の3次元座標を取得
	auto pos = own->GetPos();
	//スクリーン上の2次元座標に変換
	auto drawPos = ConvWorldPosToScreenPos(pos.ToVECTOR());
	//描画座標を計算
	int x = static_cast<int>(drawPos.x);
	int y = static_cast<int>(drawPos.y) - kDrawYOffset - m_deathFrame / kFrameDivisor;

	//ドロップする罠ポイントを取得
	auto point = own->GetDropPoint();

	//ドロップする罠ポイントの描画
	DrawUI::GetInstance().RegisterDrawRequest([=]()
	{
		//α値の設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, kAlphaMax - kAlphaMag * m_deathFrame);
		//罠ポイントアイコンを描画
		DrawRotaGraph(x - kDrawXOffset/2, y, kIconScale, 0.0f, m_trapPointGraphHandle, true);
		//罠ポイントを描画
		FontManager::GetInstance().DrawCenteredText(x + kDrawXOffset/2, y, std::to_string(point), kFontColor, kFontSize, kFontShadowColor);
		//α値の設定を解除
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}, kDrawPriority);

	m_deathFrame++;
}