#include "HPBar.h"

#include "ResourceManager.h"

namespace
{
	const std::string kId[] =
	{
		"I_HPGAUGEBG",
		"I_HPGAUGEDECREASE",
		"I_HPGAUGE",
		"I_HPFRAME",
	};

	constexpr float kHpBarScale = 0.55f;

	//描画基準座標
	constexpr int kDrawPosX = 210;
	constexpr int kDrawPosY = 40;

	//黄色ゲージが1フレームあたりに短くなるスピード
	constexpr int kYellowGaugeDecreaseSpeed = 6;

	//揺らすフレーム数
	//大きくすればするほど揺れる時間と揺れ幅が大きくなる
	constexpr int kVibrationFrame = 16;

}

HPBar::HPBar():
	m_maxHP(0),
	m_drawRedGaugePos(),
	m_drawYellowGaugePos(Vec2(kDrawPosX, kDrawPosY)),
	m_drawPos(),
	m_offset(),
	m_hpBarWidth(0),
	m_hpBarHeight(0),
	m_gaugeWidth(0),
	m_gaugeYellowWidth(0),
	m_vibrationCount(0),
	m_handles()
{
	//画像ハンドルを取得
	for (auto& id : kId)
	{
		m_handles.push_back(ResourceManager::GetInstance().GetHandle(id));
	}
}

HPBar::~HPBar()
{
	//画像を削除
	for (auto& handle : m_handles)
	{
		DeleteGraph(handle);
	}
	m_handles.clear();
}

void HPBar::Init(int max)
{
	//最大HPを保存しておく(ゲージの割合を計算するために必要)
	m_maxHP = max;

	//描画座標を初期化する
	m_drawPos = Vec2(kDrawPosX, kDrawPosY);
	m_drawRedGaugePos = Vec2(kDrawPosX, kDrawPosY);
	m_drawYellowGaugePos = Vec2(kDrawPosX, kDrawPosY);

	//HPバーの画像サイズを取得する
	GetGraphSize(m_handles[2], &m_hpBarWidth, &m_hpBarHeight);
	m_gaugeWidth = m_hpBarWidth;
	m_gaugeYellowWidth = m_hpBarWidth;
}

void HPBar::Update(int hp)
{
	//HPバーの計算
	//プレイヤーのHP÷プレイヤーの最大HPで現在のHPの割合を出す
	float widthScale = static_cast<float>(hp) / static_cast<float>(m_maxHP);
	//HPの割合分の画像のX幅を出す
	auto width = static_cast<int>(m_hpBarWidth * widthScale);
	if (m_gaugeWidth != width)
	{
		//ゲージの幅が小さくなっていたらゲージを揺らすようにする
		if (m_gaugeWidth > width)
		{
			m_vibrationCount = kVibrationFrame;
		}
		m_gaugeWidth = width;
	}

	//何もしないと画像が中心によっていくため画像の中心座標をHPに応じてずらす
	m_drawRedGaugePos.x = kDrawPosX - static_cast<float>((m_hpBarWidth - m_gaugeWidth) * kHpBarScale / 2);
	m_drawRedGaugePos.y = 40.0f;

	//振動カウントが0以上なら揺らす
	if (m_vibrationCount > 0)
	{
		//揺らすランダム値を取得(時間経過で揺れ幅が小さくなるように)
		m_offset.x = static_cast<float>(GetRand(m_vibrationCount) - m_vibrationCount / 2);
		m_offset.y = static_cast<float>(GetRand(m_vibrationCount) - m_vibrationCount / 2);
	
		//揺らすフレーム数を減らす
		m_vibrationCount--;
	}
	//0以下(揺らさない)のとき
	else
	{
		//初期化
		m_offset = Vec2(0.0f, 0.0f);
		m_vibrationCount = 0;

		//揺れが終わったら黄色ゲージを減らす
		if (m_gaugeWidth < m_gaugeYellowWidth)
		{
			m_gaugeYellowWidth -= kYellowGaugeDecreaseSpeed;
			m_drawYellowGaugePos.x = kDrawPosX - static_cast<float>((m_hpBarWidth - m_gaugeYellowWidth) * kHpBarScale / 2);
		}
	}

}

void HPBar::Draw()
{
	//Hpバーの描画
	DrawRotaGraph(static_cast<int>(m_drawPos.x + m_offset.x), static_cast<int>(m_drawPos.y + m_offset.y), kHpBarScale, 0.0f, m_handles[0], true);
	DrawRectRotaGraph(static_cast<int>(m_drawYellowGaugePos.x + m_offset.x), static_cast<int>(m_drawYellowGaugePos.y + m_offset.y), 0, 0, m_gaugeYellowWidth, m_hpBarHeight, kHpBarScale, 0.0f, m_handles[1], true);
	DrawRectRotaGraph(static_cast<int>(m_drawRedGaugePos.x + m_offset.x), static_cast<int>(m_drawRedGaugePos.y + m_offset.y), 0, 0, m_gaugeWidth, m_hpBarHeight, kHpBarScale, 0.0f, m_handles[2], true);
	DrawRotaGraph(static_cast<int>(m_drawPos.x + m_offset.x), static_cast<int>(m_drawPos.y + m_offset.y), kHpBarScale, 0.0f, m_handles[3], true);
}
