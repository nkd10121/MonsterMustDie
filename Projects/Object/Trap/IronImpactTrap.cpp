#include "IronImpactTrap.h"

#include "ResourceManager.h"
#include "LoadCSV.h"

namespace
{
	//当たり判定の円の半径
	constexpr float kCollisionRadius = 11.0f;
	//索敵判定の間隔
	constexpr float kSearchCollisionInterval = 10.0f;
}

IronImpactTrap::IronImpactTrap() :
	m_attackCount(0),
	m_waitCount(0)
{
	m_trapName = "IronImpact";
	//罠のステータスを取得
	m_status = LoadCSV::GetInstance().LoadTrapStatus(m_trapName.c_str());
	//モデルのハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle(m_status.modelId);
	//モデルのスケールを設定
	MV1SetScale(m_modelHandle, VGet(m_status.modelSize, m_status.modelSize, m_status.modelSize));
}

IronImpactTrap::~IronImpactTrap()
{
}

void IronImpactTrap::Init(Vec3 pos, Vec3 norm)
{
	//当たり判定を取るようにする
	OnEntryPhysics();

	//物理挙動の初期化
	rigidbody->Init();

	pos.y -= 8.0f;

	//座標の更新
	rigidbody->SetPos(pos);
	rigidbody->SetNextPos(pos);

	//法線ベクトルの設定
	m_norm = norm;

	MV1SetPosition(m_modelHandle, pos.ToVECTOR());
	//回転させる
	SetRot(norm);

	auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Search);
	auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
	sphereCol->m_radius = m_status.searchRange;

	auto searchPos = norm * kSearchCollisionInterval;
	searchPos.y += 8.0f;
	sphereCol->SetOffsetPos(searchPos);

	//索敵判定は動かすつもりがないため、先に中心座標を設定して動かないようにする
	sphereCol->SetCenterPos(pos);
	sphereCol->UseIsStatic();

	m_currentAnimNo = MV1AttachAnim(m_modelHandle, 0);
	m_nowAnimIdx = 0;
	m_animSpeed = 1.0f;

	m_animEndFrame = MV1GetAttachAnimTotalTime(m_modelHandle, m_currentAnimNo);

	//存在フラグをtrueにする
	m_isExist = true;
}

void IronImpactTrap::Update()
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

			if (m_attackCount == 14)
			{
				auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Attack);
				auto attackCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
				attackCol->m_radius = m_status.atkRange;

				auto attackPos = m_norm * kSearchCollisionInterval;
				attackPos.y += 8.0f;
				attackCol->SetOffsetPos(attackPos);
			}
			else if (m_attackCount == 48)
			{
				auto col = GetCollider(MyLib::ColliderBase::CollisionTag::Attack);
				if (col != nullptr)
				{
					Collidable::DeleteRequestCollider(col);
				}
			}

			m_attackCount++;
		}
	}

	if (m_waitCount > 0)
	{
		if (m_waitCount > m_status.coolTime)
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

void IronImpactTrap::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//モデルの描画
	MV1DrawModel(m_modelHandle);
}

void IronImpactTrap::SetRot(Vec3 vec)
{
	//atan2を使用して向いている角度を取得
	auto angle = atan2(vec.x, vec.z);
	auto rotation = VGet(0.0f, angle + DX_PI_F / 2, 0.0f);
	MV1SetRotationXYZ(m_modelHandle, rotation);

	//法線ベクトルの設定
	m_norm = vec;
}

void IronImpactTrap::SetPos(Vec3 vec)
{
	vec.y -= 8.0f;

	rigidbody->SetPos(vec);
	MV1SetPosition(m_modelHandle, vec.ToVECTOR());
}

const Vec3 IronImpactTrap::GetPos() const
{
	auto ret = rigidbody->GetPos();
	ret.y += 8.0f;

	return ret;
}

std::vector<Vec3> IronImpactTrap::GetAttackPos()
{
	std::vector<Vec3> ret;

	auto pos = rigidbody->GetPos() + m_norm * kSearchCollisionInterval;
	//pos.y -= 8.0f;
	ret.push_back(pos);

	return ret;
}

bool IronImpactTrap::UpdateAnim(int attachNo, float startTime)
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
