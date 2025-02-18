#include "StateBase.h"
#include "CharacterBase.h"
#include <string>
#include <cassert>

#include "PlayerStateIdle.h"
#include "PlayerStateWalk.h"
#include "PlayerStateDash.h"
#include "PlayerStateJump.h"

#include "EnemyStateIdle.h"
#include "EnemyStateWalk.h"
#include "EnemyStateAttack.h"
#include "EnemyStateDamaged.h"

#include "LoadCSV.h"
#include "Input.h"

/// <summary>
/// コンストラクタ
/// </summary>
StateBase::StateBase(std::shared_ptr<CharacterBase> own) :
	m_pOwn(own),
	m_nowState(StateKind::Null)
{
}

/// <summary>
/// デストラクタ
/// </summary>
StateBase::~StateBase()
{
}

#ifdef _DEBUG	//デバッグ描画
/// <summary>
/// 現在のステートのデバッグ描画
/// </summary>
void StateBase::DebugDrawState(int x, int y)
{
	std::string own = "null";

	switch (m_pOwn.lock()->GetTag())
	{
	case GameObjectTag::Player:
		own = "Player";
		break;
	case GameObjectTag::Enemy:
		own = "Enemy";
		break;
	default:
		own = "null";
		break;
	}

	std::string state = "null";

	switch (m_nowState)
	{
	case StateBase::StateKind::Idle:
		state = "Idle";
		break;
	case StateBase::StateKind::Walk:
		state = "Walk";
		break;
	case StateBase::StateKind::Dash:
		state = "Dash";
		break;
	case StateBase::StateKind::Jump:
		state = "Jump";
		break;
	case StateBase::StateKind::Attack:
		state = "Attack";
		break;
	case StateBase::StateKind::Damaged:
		state = "Damaged";
		break;
	case StateBase::StateKind::Death:
		state = "Death";
		break;
	default:
		state = "null";
		break;
	}

	DrawFormatString(x, y, 0xffffff, "%s:%s", own.c_str(), state.c_str());
}
#endif

void StateBase::ChangeState(StateKind kind)
{
	//タグを取得
	auto tag = m_pOwn.lock()->GetTag();

	//タグから持ち主がプレイヤーか敵かを判断して処理を分ける
	if (tag == GameObjectTag::Player)
	{
		//持ち主の名前を取得(アニメーション番号を取得するために今後必要)
		auto name = m_pOwn.lock()->GetCharacterName();
		if (kind == StateKind::Idle)
		{
			auto pNext = std::make_shared<PlayerStateIdle>(m_pOwn.lock());
			pNext->Init(m_stageColId);

			m_pOwn.lock()->ChangeState(pNext);
			return;
		}
		else if (kind == StateKind::Walk)
		{
			auto pNext = std::make_shared<PlayerStateWalk>(m_pOwn.lock());
			pNext->Init(m_stageColId);

			m_pOwn.lock()->ChangeState(pNext);
			return;
		}
		else if (kind == StateKind::Dash)
		{
			auto pNext = std::make_shared<PlayerStateDash>(m_pOwn.lock());
			pNext->Init(m_stageColId);

			m_pOwn.lock()->ChangeState(pNext);
			return;
		}
		else if (kind == StateKind::Jump)
		{
			auto pNext = std::make_shared<PlayerStateJump>(m_pOwn.lock());
			pNext->Init(m_stageColId);

			m_pOwn.lock()->ChangeState(pNext);
			return;
		}
	}
	else
	{
		//持ち主の名前を取得(アニメーション番号を取得するために今後必要)
		auto name = m_pOwn.lock()->GetCharacterName();

		if (kind == StateKind::Idle)
		{
			auto pNext = std::make_shared<EnemyStateIdle>(m_pOwn.lock());
			pNext->Init(m_stageColId);

			m_pOwn.lock()->ChangeState(pNext);
			return;
		}
		else if (kind == StateKind::Walk)
		{
			auto pNext = std::make_shared<EnemyStateWalk>(m_pOwn.lock());
			pNext->Init(m_stageColId);

			m_pOwn.lock()->ChangeState(pNext);
			return;
		}
		else if (kind == StateKind::Attack)
		{
			auto pNext = std::make_shared<EnemyStateAttack>(m_pOwn.lock());
			pNext->Init(m_stageColId);

			m_pOwn.lock()->ChangeState(pNext);
			return;
		}
		else if(kind == StateKind::Damaged)
		{
			auto pNext = std::make_shared<EnemyStateDamaged>(m_pOwn.lock());
			pNext->Init(m_stageColId);

			m_pOwn.lock()->ChangeState(pNext);
			return;
		}
	}
}

/// <summary>
/// 持ち主がプレイヤーかどうかを判断する
/// </summary>
bool StateBase::CheckPlayer()
{
	if (m_pOwn.lock()->GetTag() != GameObjectTag::Player)
	{
		//#ifdef _DEBUG	//デバッグ描画
		//		assert(0 && "持ち主がプレイヤーではありません");
		//#endif
		return false;
	}

	return true;
}

bool StateBase::CheakEnemy()
{
	if (m_pOwn.lock()->GetTag() != GameObjectTag::Enemy)
	{
		//#ifdef _DEBUG	//デバッグ描画
		//		assert(0 && "持ち主が敵ではありません");
		//#endif
		return false;
	}

	return true;
}

const StateBase::eDir StateBase::GetDirection(float x, float y) const
{
	auto angle = atan2(y, x);
	if (angle < 0) {
		angle = angle + 2 * DX_PI_F;
	}
	angle = floor(angle * 360 / (2 * DX_PI_F));

	if (23 <= angle && angle <= 67) {
		return eDir::ForwardRight;
	}
	else if (68 <= angle && angle <= 112) {
		return eDir::Forward;
	}
	else if (113 <= angle && angle <= 157) {
		return eDir::ForwardLeft;
	}
	else if (158 <= angle && angle <= 202) {
		return eDir::Left;
	}
	else if (203 <= angle && angle <= 247) {
		return eDir::BackLeft;
	}
	else if (248 <= angle && angle <= 292) {
		return eDir::Back;
	}
	else if (293 <= angle && angle <= 337) {
		return eDir::BackRight;
	}
	else {
		return eDir::Right;
	}
}