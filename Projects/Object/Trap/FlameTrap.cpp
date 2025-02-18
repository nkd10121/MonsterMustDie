#include "FlameTrap.h"

#include "LoadCSV.h"
#include "ResourceManager.h"
#include "EffectManager.h"
#include "SoundManager.h"

FlameTrap::FlameTrap() :
	TrapBase(),
	m_waitCount(0),
	m_attackCount(0)
{
	m_trapName = "Flame";
	//罠のステータスを取得
	m_status = LoadCSV::GetInstance().LoadTrapStatus(m_trapName.c_str());
	//モデルのハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle("M_FLAME");
	MV1SetScale(m_modelHandle, Vec3(m_status.modelSize, m_status.modelSize, m_status.modelSize).ToVECTOR());
}

FlameTrap::~FlameTrap()
{
}

void FlameTrap::Init(Vec3 pos, Vec3 direction)
{
	//当たり判定を取るようにする
	OnEntryPhysics();

	//物理挙動の初期化
	rigidbody->Init();

	//座標の更新
	rigidbody->SetPos(pos);
	rigidbody->SetNextPos(pos);

	MV1SetPosition(m_modelHandle, pos.ToVECTOR());
	//回転させる
	SetRot(direction);

	m_angle = direction.y;
	//向きを計算して置く
	auto radian = -direction.y + DX_PI_F / 2 * 3;
	m_direction = Vec3(cos(radian), 0.0f, sin(radian));
	m_direction = m_direction.Normalize();

	//索敵判定の作成(3つ作成)
	for (int i = 1; i < 4; i++)
	{
		auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Search);
		auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
		sphereCol->m_radius = m_status.searchRange;

		//索敵判定を出す座標を計算
		auto searchPos = pos;
		searchPos.y += 6.0f;
		searchPos += m_direction * 14.0f * i;

		//索敵判定は動かすつもりがないため、先に中心座標を設定して動かないようにする
		sphereCol->SetCenterPos(searchPos);
		sphereCol->UseIsStatic();
	}

	m_currentAnimNo = MV1AttachAnim(m_modelHandle, 0);
	m_nowAnimIdx = 0;
	m_animSpeed = 0.5f;

	m_animEndFrame = MV1GetAttachAnimTotalTime(m_modelHandle, m_currentAnimNo);

	//存在フラグをtrueにする
	m_isExist = true;
}

void FlameTrap::Update()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	if (m_isAttack)
	{
		bool isAnimEnd = false;
		if (m_waitCount == 0)
		{
			isAnimEnd = UpdateAnim(m_currentAnimNo);
			if (isAnimEnd)
			{
				m_waitCount++;
			}

			//攻撃開始から一定フレーム経ったら
			if (m_attackCount == 20)
			{
				//攻撃SEを流す
				SoundManager::GetInstance().PlaySE("S_FLAMETRAPATTACK");

				//攻撃の炎エフェクトを描画する
				auto effectPos = rigidbody->GetPos();
				effectPos.y += 6.0f;
				effectPos += m_direction * 4.0f;
				EffectManager::GetInstance().CreateEffect("E_FLAMETRAP", effectPos, Vec3(0.0f, m_angle, 0.0f));
			}

			m_attackCount++;
			if (m_attackCount % 20 == 0 && m_attackCount <= m_animEndFrame / 5 * 2)
			{
				//攻撃判定を生成する前にすべての攻撃判定を一回削除
				while (1)
				{
					auto col = GetCollider(MyLib::ColliderBase::CollisionTag::Attack);
					if (col != nullptr)
					{
						Collidable::DeleteRequestCollider(col);
					}
					else
					{
						break;
					}
				}

				//攻撃判定の作成(3つ作成)
				for (int i = 1; i < 4; i++)
				{
					auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Attack);
					auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
					sphereCol->m_radius = m_status.atkRange;

					//索敵判定を出す座標を計算
					auto pos = rigidbody->GetPos();
					pos.y += 6.0f;
					pos += m_direction * 14.0f * i;

					//索敵判定は動かすつもりがないため、先に中心座標を設定して動かないようにする
					sphereCol->SetCenterPos(pos);
					sphereCol->UseIsStatic();
				}
			}

			//最後に出した攻撃判定がだしっぱのままになってしまうためそれを削除
			if (m_attackCount % 20 == 0 && m_attackCount >= m_animEndFrame / 5 * 2)
			{
				//攻撃判定を生成する前にすべての攻撃判定を一回削除
				//TODO:攻撃判定がないのにここが呼ばれてしまうのが気になる
				while (1)
				{
					auto col = GetCollider(MyLib::ColliderBase::CollisionTag::Attack);
					if (col != nullptr)
					{
						Collidable::DeleteRequestCollider(col);
					}
					else
					{
						break;
					}
				}
			}
		}
	}


	if (m_waitCount != 0)
	{
		if (m_waitCount > 120)
		{
			m_isAttack = false;
			m_waitCount = 0;
			m_attackCount = 0;
		}
		else
		{
			m_waitCount++;
		}
	}
}

void FlameTrap::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//モデルの描画
	MV1DrawModel(m_modelHandle);
}

void FlameTrap::SetRot(Vec3 vec)
{
	//向きを計算して置く
	auto radian = -vec.y + DX_PI_F / 2 * 3;
	m_direction = Vec3(cos(radian), 0.0f, sin(radian));
	m_direction = m_direction.Normalize();

	MV1SetRotationXYZ(m_modelHandle, vec.ToVECTOR());
}

std::vector<Vec3> FlameTrap::GetAttackPos()
{
	std::vector<Vec3> ret;

	//攻撃判定の作成(3つ作成)
	for (int i = 1; i < 4; i++)
	{
		//索敵判定を出す座標を計算
		auto pos = rigidbody->GetPos();
		pos += m_direction * 14.0f * i;

		ret.push_back(pos);
	}

	return ret;
}

bool FlameTrap::UpdateAnim(int attachNo, float startTime)
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

void FlameTrap::ChangeAnim(int animIndex, float animSpeed)
{
	////さらに古いアニメーションがアタッチされている場合はこの時点で消しておく
	//if (m_prevAnimNo != -1)
	//{
	//	MV1DetachAnim(m_modelHandle, m_prevAnimNo);
	//}

	//m_preAnimIdx = m_nowAnimIdx;
	//m_nowAnimIdx = animIndex;

	////現在再生中の待機アニメーションは変更目のアニメーションの扱いにする
	//m_prevAnimNo = m_currentAnimNo;

	////変更後のアニメーションとして攻撃アニメーションを改めて設定する
	//m_currentAnimNo = MV1AttachAnim(m_modelHandle, animIndex);

	////切り替えの瞬間は変更前のアニメーションが再生される状態にする
	//m_animBlendRate = 0.0f;

	//m_animSpeed = animSpeed;

	////変更前のアニメーション100%
	//MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	////変更後のアニメーション0%
	//MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
}
