#include "PlayerStateDash.h"
#include "Input.h"
#include "Player.h"
#include "CharacterBase.h"

#include "LoadCSV.h"

namespace
{
	/*アナログスティックによる移動関連*/
	constexpr float kMaxSpeed = 0.2f;			//プレイヤーの最大速度
	constexpr float kAnalogRangeMin = 0.1f;		//アナログスティックの入力判定最小範囲
	constexpr float kAnalogRangeMax = 0.8f;		//アナログスティックの入力判定最大範囲
	constexpr float kAnalogInputMax = 1000.0f;	//アナログスティックから入力されるベクトルの最大
}

/// <summary>
/// コンストラクタ
/// </summary>
PlayerStateDash::PlayerStateDash(std::shared_ptr<CharacterBase> own) :
	StateBase(own)
{
	//現在のステートをダッシュ状態にする
	m_nowState = StateKind::Dash;
	own->ChangeAnim(LoadCSV::GetInstance().GetAnimIdx(own->GetCharacterName(), "RUN_FORWARD"));
}

/// <summary>
/// デストラクタ
/// </summary>
PlayerStateDash::~PlayerStateDash()
{
}

/// <summary>
/// 初期化
/// </summary>
void PlayerStateDash::Init(std::string id)
{
	m_stageColId = id;
}

/// <summary>
/// 更新
/// </summary>
void PlayerStateDash::Update()
{
	//持ち主がプレイヤーかどうかをチェックする
	if (!CheckPlayer())	return;

	//持っているキャラクターベースクラスをプレイヤークラスにキャストする(ダウンキャスト)
	auto own = std::dynamic_pointer_cast<Player>(m_pOwn.lock());

	//コントローラーの左スティックの入力を取得
	auto input = Input::GetInstance().GetInputStick(false);
	auto dir = GetDirection(input.first, -input.second);

	//左スティックが入力されていなかったらStateをIdleにする
	if (Input::GetInstance().GetInputStick(false).first == 0.0f &&
		Input::GetInstance().GetInputStick(false).second == 0.0f)
	{
		ChangeState(StateKind::Idle);
		return;
	}

	//ジャンプボタンが押されていたらstateをJumpにする
	if (Input::GetInstance().IsTriggered("INPUT_JUMP"))
	{
		ChangeState(StateKind::Jump);
		return;
	}

	//ダッシュボタンが押されていたらstateをWalkにする
	if (Input::GetInstance().IsTriggered("INPUT_DASH") || dir != eDir::Forward)
	{
		ChangeState(StateKind::Walk);
		return;
	}

	//移動方向を設定する
	auto moveDir = Vec3(input.first, 0.0f, -input.second);
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
	float speed = own->GetMoveSpeed() * 2.0f * rate;

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
	moveVec = front;
	moveVec = moveVec.Normalize() * speed;

	//直前のY方向の移動速度と入力された移動速度を合わせて移動速度を決定する
	Vec3 prevVelocity = own->GetRigidbody()->GetVelocity();
	Vec3 newVelocity = Vec3(moveVec.x, prevVelocity.y, moveVec.z);
	own->GetRigidbody()->SetVelocity(newVelocity);
}
