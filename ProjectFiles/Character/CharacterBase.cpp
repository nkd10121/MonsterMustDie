#include "CharacterBase.h"

namespace
{
	/*アニメーション関係*/
	constexpr float kAnimChangeFrame = 10.0f;							//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;		//1フレーム当たりのアニメーション切り替えが進む速さ
	constexpr float kAnimBlendRateMax = 1.0f;							//アニメーションブレンド率の最大

}

/// <summary>
/// コンストラクタ
/// </summary>
CharacterBase::CharacterBase(Priority priority, GameObjectTag tag) :
	Collidable(priority, tag),
	m_modelHandle(-1),
	m_characterName(),
	m_status(),
	m_hpMax(0),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(1.0f),
	m_nowAnimIdx(-1),
	m_preAnimIdx(-1),
	m_animSpeed(0.5f),
	m_isAnimationFinish(false)
{
}

/// <summary>
/// デストラクタ
/// </summary>
CharacterBase::~CharacterBase()
{
	//基底クラスで削除しておく
	MV1DeleteModel(m_modelHandle);

}

/// <summary>
/// アニメーションの切り替え
/// </summary>
void CharacterBase::AnimationBlend()
{
	if (m_prevAnimNo != -1)
	{
		//フレームでアニメーションを切り替える
		m_animBlendRate += kAnimChangeRateSpeed;
		if (m_animBlendRate >= kAnimBlendRateMax)
		{
			m_animBlendRate = kAnimBlendRateMax;
		}

		//アニメーションのブレンド率を設定する
		MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, kAnimBlendRateMax - m_animBlendRate);
		MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
	}
}

/// <summary>
/// アニメーションの更新
/// </summary>
bool CharacterBase::UpdateAnim(int attachNo, float startTime)
{
	//アニメーションが設定されていなかったら早期リターン
	if (attachNo == -1)	return false;

	//アニメーションを進行させる
	float nowFrame = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//現在の再生カウントを取得
	nowFrame += m_animSpeed;

	//現在再生中のアニメーションの総カウントを取得する
	float totalAnimframe = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;

	//NOTE:もしかしたら総フレーム分引いても総フレームより大きいかもしれないからwhileで大きい間引き続ける
	while (totalAnimframe <= nowFrame)
	{
		//NOTE:nowFrameを0にリセットするとアニメーションフレームの飛びがでるから総フレーム分引く
		nowFrame -= totalAnimframe;
		nowFrame += startTime;
		isLoop = true;
	}

	//進めた時間に設定
	MV1SetAttachAnimTime(m_modelHandle, attachNo, nowFrame);

	return isLoop;
}

/// <summary>
///	アニメーションの変更
/// </summary>
void CharacterBase::ChangeAnim(int animIndex, float animSpeed)
{
	//さらに古いアニメーションがアタッチされている場合はこの時点で消しておく
	if (m_prevAnimNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_prevAnimNo);
	}

	m_preAnimIdx = m_nowAnimIdx;
	m_nowAnimIdx = animIndex;

	//現在再生中の待機アニメーションは変更目のアニメーションの扱いにする
	m_prevAnimNo = m_currentAnimNo;

	//変更後のアニメーションとして攻撃アニメーションを改めて設定する
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, animIndex);

	//切り替えの瞬間は変更前のアニメーションが再生される状態にする
	m_animBlendRate = 0.0f;

	m_animSpeed = animSpeed;

	//変更前のアニメーション100%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//変更後のアニメーション0%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
}

bool CharacterBase::IsInAir()
{
	auto prePos = rigidbody->GetPos();
	auto newPos = rigidbody->GetNextPos();

	if (abs(prePos.y - newPos.y) > 0.5f)
	{
		return false;
	}

	return true;
}

void CharacterBase::ChangeState(std::shared_ptr<StateBase> nextState)
{
	m_pState = nextState;
}