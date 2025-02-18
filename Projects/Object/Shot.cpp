#include "Shot.h"

#include "ResourceManager.h"
#include "MapManager.h"
#include "EffectManager.h"

namespace
{
	//当たり判定の半径
	constexpr float kCollisionRadius = 1.0f;

	//パス
	const std::string kPath = "data/model/arrow.mv1";
}

/// <summary>
/// コンストラクタ
/// </summary>
Shot::Shot(GameObjectTag tag):
	ObjectBase(Collidable::Priority::Low, tag),
	m_moveDir(),
	m_frameCount(0),
	m_mapHandle(0),
	m_atk(0)
{
	//当たり判定の生成
	auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
	sphereCol->m_radius = kCollisionRadius;
}

/// <summary>
/// デストラクタ
/// </summary>
Shot::~Shot()
{
	MV1DeleteModel(m_mapHandle);
}

/// <summary>
/// 初期化
/// </summary>
void Shot::Init()
{
	//当たり判定の初期化
	OnEntryPhysics();
	//物理クラスの初期化
	rigidbody->Init();

	//モデルハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle("M_ARROW");
	MV1SetScale(m_modelHandle, VECTOR(8.0f, 8.0f, 8.0f));

	//マップの当たり判定モデルのハンドルを取得
	//m_mapHandle = MapManager::GetInstance().GetStageCollisionHandle();

	//存在フラグをtrueにする
	m_isExist = true;
}

/// <summary>
/// 生成座標と向きと攻撃力を設定
/// </summary>
void Shot::Set(std::string stageId, const Vec3& pos, const Vec3& m_dir, const int& atk)
{
	m_mapHandle = ResourceManager::GetInstance().GetHandle(stageId);
	MV1SetScale(m_mapHandle, VGet(0.01f, 0.01f, 0.01f));		//サイズの変更
	MV1SetRotationXYZ(m_mapHandle, VGet(0.0f, DX_PI_F, 0.0f));	//回転


	rigidbody->SetPos(pos + m_dir);
	m_moveDir = m_dir;
	m_atk = atk;

	//回転させる方向を計算する
	auto angle = -atan2f(m_moveDir.z, m_moveDir.x) - DX_PI_F / 2;
	auto rot = Vec3(0.0f, angle, 0.0f);

	MV1SetRotationXYZ(m_modelHandle, rot.ToVECTOR());


}

/// <summary>
/// 更新
/// </summary>
void Shot::Update()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//移動方向を計算して設定
	auto dirNorm = m_moveDir.Normalize();
	rigidbody->SetVelocity(dirNorm * 4.0f);

	/*地形との当たり判定を検出し、地形と当たっていたら自身を削除する*/
	auto hitDim = MV1CollCheck_Sphere(m_mapHandle, -1, rigidbody->GetPosVECTOR(), kCollisionRadius);

	// 検出した周囲のポリゴン情報を開放する
	MV1CollResultPolyDimTerminate(hitDim);

	//もし地形と当たっていたら
	if (hitDim.HitNum != 0)
	{
		//自身を削除する
		m_isExist = false;
	}

	if (m_frameCount % 10 == 0)
	{
		auto angle = atan2(dirNorm.x, dirNorm.z);
		EffectManager::GetInstance().CreateEffect("E_ARROW", rigidbody->GetPos(), Vec3(0.0f, angle,0.0f));
	}

	//何も当たらずに飛んで行ったとき、8秒後に自身を削除する
	if (m_frameCount > 60 * 8)
	{
		m_isExist = false;
	}


	//フレームカウントを増やす
	m_frameCount++;
}

/// <summary>
/// 描画
/// </summary>
void Shot::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//座標の設定
	rigidbody->SetPos(rigidbody->GetNextPos());
	auto pos = rigidbody->GetPos();
	MV1SetPosition(m_modelHandle, pos.ToVECTOR());

	//モデルの描画
	MV1DrawModel(m_modelHandle);
}

/// <summary>
/// 終了通知
/// 当たったオブジェクトから呼ばれる
/// </summary>
void Shot::End()
{
	m_isExist = false;
}