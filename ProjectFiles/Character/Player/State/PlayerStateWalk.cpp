#include "PlayerStateWalk.h"
#include "Input.h"
#include "Player.h"
#include "CharacterBase.h"

#include "LoadCSV.h"

namespace
{
	/*アナログスティックによる移動関連*/
	constexpr float kAnalogRangeMin = 0.1f;		//アナログスティックの入力判定最小範囲
	constexpr float kAnalogRangeMax = 0.8f;		//アナログスティックの入力判定最大範囲
	constexpr float kAnalogInputMax = 1000.0f;	//アナログスティックから入力されるベクトルの最大

	constexpr float kWalkAnimSpeed = 0.35f;
}

/// <summary>
/// コンストラクタ
/// </summary>
PlayerStateWalk::PlayerStateWalk(std::shared_ptr<CharacterBase> own) :
	StateBase(own),
	m_dir(),
	m_noInputFrame(0)
{
	//現在のステートを歩き状態にする
	m_nowState = StateKind::Walk;

	//このシーンに遷移した瞬間の左スティックの入力角度を取得しておく
	auto input = Input::GetInstance().GetInputStick(false);
	m_dir = GetDirection(input.first, -input.second);
	auto animName = GetWalkAnimName(m_dir);
	m_pOwn.lock()->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), animName), kWalkAnimSpeed);
}

/// <summary>
/// デストラクタ
/// </summary>
PlayerStateWalk::~PlayerStateWalk()
{
}

/// <summary>
/// 初期化
/// </summary>
void PlayerStateWalk::Init(std::string id)
{
	m_stageColId = id;
}

/// <summary>
/// 更新
/// </summary>
void PlayerStateWalk::Update()
{
	//持ち主がプレイヤーかどうかをチェックする
	if (!CheckPlayer())	return;

	//ダウンキャスト
	auto own = std::dynamic_pointer_cast<Player>(m_pOwn.lock());

	//左スティックが入力されていなかったらStateをIdleにする
	if (Input::GetInstance().GetInputStick(false).first == 0.0f &&
		Input::GetInstance().GetInputStick(false).second == 0.0f)
	{
		if (m_noInputFrame == 2)
		{
			ChangeState(StateKind::Idle);
			return;
		}

		m_noInputFrame++;
	}
	else
	{
		m_noInputFrame = 0;
	}

	//ジャンプボタンが押されていたらstateをJumpにする
	if (Input::GetInstance().IsTriggered("INPUT_JUMP"))
	{
		ChangeState(StateKind::Jump);
		return;
	}

	//ダッシュボタンが押されていたらstateをDashにする
	if (Input::GetInstance().IsTriggered("INPUT_DASH") && m_dir == eDir::Forward)
	{
		ChangeState(StateKind::Dash);
		return;
	}

	//コントローラーの左スティックの入力を取得
	auto input = Input::GetInstance().GetInputStick(false);
	auto dirLog = m_dir;
	m_dir = GetDirection(input.first, -input.second);

	//直前の入力方向と異なるとき
	if (dirLog != m_dir)
	{
		//アニメーションを変更する
		auto animName = GetWalkAnimName(m_dir);
		own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx("Player", animName));
	}

	//移動方向を設定する
	auto moveDir = Vec3(input.first,0.0f,-input.second);
	//移動ベクトルの長さを取得する
	float len = moveDir.Length();

	//ベクトルの長さを0.0～1.0の割合に変換する
	float rate = len / kAnalogInputMax;

	//アナログスティック無効な範囲を除外する(デッドゾーン)
	rate = (rate - kAnalogRangeMin) / (kAnalogRangeMax - kAnalogRangeMin);
	rate = min(rate, 1.0f);
	rate = max(rate, 0.0f);

	//速度が決定できるので移動ベクトルに反映する
	moveDir = moveDir.Normalize();
	float speed = own->GetMoveSpeed() * rate;

	//方向ベクトルと移動力をかけて移動ベクトルを生成する
	auto moveVec = moveDir * speed;

	//プレイヤーの正面方向を計算して正面方向を基準に移動する
	//カメラの正面方向ベクトル
	Vec3 front(own->GetCameraDirecton().x, 0.0f, own->GetCameraDirecton().z);
	//向きベクトル*移動量
	front = front * moveVec.z;
	//カメラの右方向ベクトル
	Vec3 right(-own->GetCameraDirecton().z, 0.0f, own->GetCameraDirecton().x);
	//向きベクトル*移動量
	right = right * (-moveVec.x);

	//移動ベクトルの生成
	moveVec = front + right;
	moveVec = moveVec.Normalize() * speed;
	
	//直前のY方向の移動速度と入力された移動速度を合わせて移動速度を決定する
	Vec3 prevVelocity = own->GetRigidbody()->GetVelocity();
	Vec3 newVelocity = Vec3(moveVec.x, prevVelocity.y, moveVec.z);
	own->GetRigidbody()->SetVelocity(newVelocity);
}

/// <summary>
/// 入力方向から歩きアニメーション名を取得する
/// </summary>
std::string PlayerStateWalk::GetWalkAnimName(eDir dir)
{
	if (dir == eDir::Forward)
	{
		return std::string("WALK_FORWARD");
	}
	else if (dir == eDir::ForwardRight)
	{
		return std::string("WALK_FORWARDRIGHT");
	}
	else if (dir == eDir::Right)
	{
		return std::string("WALK_RIGHT");
	}
	else if (dir == eDir::BackRight)
	{
		return std::string("WALK_BACKWARDRIGHT");
	}
	else if (dir == eDir::Back)
	{
		return std::string("WALK_BACKWARD");
	}
	else if (dir == eDir::BackLeft)
	{
		return std::string("WALK_BACKWARDLEFT");
	}
	else if (dir == eDir::Left)
	{
		return std::string("WALK_LEFT");
	}
	else if (dir == eDir::ForwardLeft)
	{
		return std::string("WALK_FORWARDLEFT");
	}

	return std::string();
}
