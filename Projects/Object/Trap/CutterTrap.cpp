#include "CutterTrap.h"
#include "LoadCSV.h"
#include "ResourceManager.h"

namespace
{
	//当たり判定の円の半径
	constexpr float kCollisionRadius = 11.0f;

	//ボーン（フレーム）の名前を指定
	const char* kTargetFrameName = "Obstacle-7-Saw-Holder";
}

CutterTrap::CutterTrap() :
	TrapBase(),
	m_frameIdx(0),
	m_attackCount(0)
{
	m_trapName = "Cutter";
	//罠のステータスを取得
	m_status = LoadCSV::GetInstance().LoadTrapStatus(m_trapName.c_str());
	//モデルのハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle(m_status.modelId);
	MV1SetScale(m_modelHandle, VECTOR(m_status.modelSize, m_status.modelSize, m_status.modelSize));
}

CutterTrap::~CutterTrap()
{
}

void CutterTrap::Init(Vec3 pos, Vec3 norm)
{
	//当たり判定を取るようにする
	OnEntryPhysics();

	//物理挙動の初期化
	rigidbody->Init();

	//座標の更新
	rigidbody->SetPos(pos);
	rigidbody->SetNextPos(pos);

	//モデルのハンドルを取得
	MV1SetPosition(m_modelHandle, pos.ToVECTOR());

	//回転させる
	//MEMO:この罠は床にのみ設置できる罠にすると思うため、壁に設置する処理をなくす可能性あり
	SetRot(norm);

	//3Dモデルから回転させる部分のフレーム番号を取得
	m_frameIdx = MV1SearchFrame(m_modelHandle, kTargetFrameName);

	//存在フラグをtrueにする
	m_isExist = true;
}

void CutterTrap::Update()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//攻撃用当たり判定を生成する
	if (m_attackCount % 30 == 0)
	{
		//すでにある攻撃判定を削除する
		if (m_attackCount > 0)
		{
			auto col = GetCollider(MyLib::ColliderBase::CollisionTag::Attack);
			if (col != nullptr)
			{
				Collidable::DeleteRequestCollider(col);
			}
		}

		//当たり判定の生成
		auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Attack);
		auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
		sphereCol->m_radius = m_status.atkRange;
	}

	// Y軸回転行列を取得
	auto rotationMatrix = MGetRotY(DX_PI_F / 180.0f * 6); // 1度回転
	// 現在のフレーム行列を取得
	auto mat = MV1GetFrameLocalWorldMatrix(m_modelHandle, m_frameIdx);
	// 回転行列を適用
	mat = MMult(rotationMatrix, mat);
	// フレーム行列を設定
	MV1SetFrameUserLocalWorldMatrix(m_modelHandle, m_frameIdx, mat);

	m_attackCount++;
}

void CutterTrap::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//モデルの描画
	MV1DrawModel(m_modelHandle);
}

void CutterTrap::SetRot(Vec3 vec)
{
	MV1SetRotationXYZ(m_modelHandle, vec.ToVECTOR());
}

std::vector<Vec3> CutterTrap::GetAttackPos()
{
	std::vector<Vec3> ret;
	ret.push_back(rigidbody->GetPos());
	return ret;
}
