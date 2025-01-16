#include "PlayerStateIdle.h"
#include "Input.h"
#include "Player.h"

#include "LoadCSV.h"

/// <summary>
/// コンストラクタ
/// </summary>
PlayerStateIdle::PlayerStateIdle(std::shared_ptr<CharacterBase> own):
	StateBase(own)
{
	//現在のステートを待機状態にする
	m_nowState = StateKind::Idle;
	own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), "IDLE"));
}

/// <summary>
/// デストラクタ
/// </summary>
PlayerStateIdle::~PlayerStateIdle()
{
}

/// <summary>
/// 初期化
/// </summary>
void PlayerStateIdle::Init(std::string id)
{
	m_stageColId = id;
}

/// <summary>
/// 更新
/// </summary>
void PlayerStateIdle::Update()
{
	//持ち主がプレイヤーかどうかをチェックする
	if (!CheckPlayer())	return;

	//左スティックが入力されていたらStateをWalkにする
	if (Input::GetInstance().GetInputStick(false).first != 0.0f ||
		Input::GetInstance().GetInputStick(false).second != 0.0f)
	{
		ChangeState(StateKind::Walk);
		return;
	}

	//ジャンプボタンが押されていたらstateをJumpにする
	if (Input::GetInstance().IsTriggered("INPUT_JUMP"))
	{
		ChangeState(StateKind::Jump);
		return;
	}

	//持っているキャラクターベースクラスをプレイヤークラスにキャストする(ダウンキャスト)
	auto own = std::dynamic_pointer_cast<Player>(m_pOwn.lock());

	//プレイヤーの速度を0にする(重力の影響を受けながら)
	auto prevVel = own->GetRigidbody()->GetVelocity();
	own->GetRigidbody()->SetVelocity(Vec3(0.0f, prevVel.y, 0.0f));
}