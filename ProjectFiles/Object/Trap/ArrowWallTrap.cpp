#include "ArrowWallTrap.h"

#include "ResourceManager.h"
#include "LoadCSV.h"

namespace
{
	//当たり判定の円の半径
	constexpr float kCollisionRadius = 11.0f;
	//モデルサイズ
	constexpr float kModelScale = 1.8f;

	//ボーン（フレーム）の名前を指定
	const char* kTargetFrameName = "Obstacle_16_1";

}

ArrowWallTrap::ArrowWallTrap() :
	m_attackCount(0),
	m_coolTimeCount(0),
	m_frameIdx(0),
	m_arrowPos(),
	m_arrowPosInit(),
	m_norm()
{
	m_trapName = "ArrowWall";
	//罠のステータスを取得
	m_status = LoadCSV::GetInstance().LoadTrapStatus(m_trapName.c_str());
}

ArrowWallTrap::~ArrowWallTrap()
{
}

void ArrowWallTrap::Init(Vec3 pos,Vec3 norm)
{
	//当たり判定を取るようにする
	OnEntryPhysics();

	//物理挙動の初期化
	rigidbody->Init();

	//座標の更新
	rigidbody->SetPos(pos);
	rigidbody->SetNextPos(pos);

	//設置場所に座標を移動させる
	m_arrowPos = pos;
	m_arrowPosInit = m_arrowPos;	//初期座標を保存

	//法線ベクトルの設定
	m_norm = norm;

	//モデルのハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle("M_ARROWWALL");
	MV1SetPosition(m_modelHandle, pos.ToVECTOR());
	//回転させる
	//atan2を使用して向いている角度を取得
	auto angle = atan2(norm.x, norm.z);
	auto rotation = VGet(0.0f, angle + DX_PI_F, 0.0f);
	MV1SetRotationXYZ(m_modelHandle, rotation);

	//索敵判定の作成(3つ作成)
	for (int i = 1; i < 4; i++)
	{
		auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Search);
		auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
		sphereCol->m_radius = kCollisionRadius;


		auto searchPos = norm * static_cast<float>(i * 20.0f);
		sphereCol->SetOffsetPos(searchPos);

		//索敵判定は動かすつもりがないため、先に中心座標を設定して動かないようにする
		sphereCol->SetCenterPos(pos);
		sphereCol->UseIsStatic();
	}

	//3Dモデルからスパイク部分のフレーム番号を取得
	m_frameIdx = MV1SearchFrame(m_modelHandle, kTargetFrameName);


	//存在フラグをtrueにする
	m_isExist = true;
}

void ArrowWallTrap::Update()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//攻撃フラグがオンになったら
	if (m_isAttack)
	{
		//攻撃用当たり判定を生成する
		if (m_attackCount == 0)
		{
			//当たり判定の生成
			auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Attack);
			auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
			sphereCol->m_radius = kCollisionRadius;
		}

		auto mat = MV1GetFrameLocalWorldMatrix(m_modelHandle, m_frameIdx);
		mat.m[3][0] += (m_norm).x * 2.0f;
		mat.m[3][1] += (m_norm).y * 2.0f;
		mat.m[3][2] += (m_norm).z * 2.0f;

		MV1SetFrameUserLocalWorldMatrix(m_modelHandle, m_frameIdx, mat);

		Vec3 prevVelocity = rigidbody->GetVelocity();
		Vec3 newVelocity = Vec3(mat.m[3][0], mat.m[3][1], mat.m[3][2]);
		rigidbody->SetVelocity(m_norm * 2.0f);

		if (m_attackCount >= 60)
		{
			m_isAttack = false;

			auto col = GetCollider(MyLib::ColliderBase::CollisionTag::Attack);
			if (col != nullptr)
			{
				Collidable::DeleteRequestCollider(col);
			}

			//矢の部分の座標を初期化する
			auto mat = MV1GetFrameLocalWorldMatrix(m_modelHandle, m_frameIdx);
			mat.m[3][0] = m_arrowPosInit.x - m_norm.x * 6.0f;
			mat.m[3][1] = m_arrowPosInit.y - m_norm.y * 6.0f;
			mat.m[3][2] = m_arrowPosInit.z - m_norm.z * 6.0f;
			MV1SetFrameUserLocalWorldMatrix(m_modelHandle, m_frameIdx, mat);
		}

		//攻撃カウントを更新
		m_attackCount++;
	}
	else
	{
		//攻撃をした後だったらクールタイムのカウントを更新する
		if (m_attackCount != 0)
		{
			m_coolTimeCount++;
		}

		//クールタイムカウントが設定されたクールタイムの値以上になったら攻撃可能状態に戻す
		if (m_coolTimeCount > m_status.coolTime)
		{
			//矢の部分の座標を初期化する
			auto mat = MV1GetFrameLocalWorldMatrix(m_modelHandle, m_frameIdx);
			mat.m[3][0] = m_arrowPosInit.x;
			mat.m[3][1] = m_arrowPosInit.y;
			mat.m[3][2] = m_arrowPosInit.z;
			MV1SetFrameUserLocalWorldMatrix(m_modelHandle, m_frameIdx, mat);

			m_arrowPos = m_arrowPosInit;

			//rigidbodyの初期化
			rigidbody->SetPos(m_arrowPosInit);
			rigidbody->SetNextPos(m_arrowPosInit);
			rigidbody->SetVelocity(Vec3(0.0f, 0.0f, 0.0f));

			//カウントを初期化する
			m_coolTimeCount = 0;
			m_attackCount = 0;
		}
	}


}

void ArrowWallTrap::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//モデルの描画
	MV1DrawModel(m_modelHandle);
}
