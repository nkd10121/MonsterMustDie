#include "Crystal.h"
#include <string>
#include "ResourceManager.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "DrawUI.h"
#include "FontManager.h"
#include "MathHelp.h"

#include "EnemyBase.h"

namespace
{
	constexpr float kModelSize = 0.6f;
	//当たり判定の半径
	constexpr float kCollisionRadius = kModelSize * 30;
	constexpr float kCollisionSize = kModelSize * 20;
	
	//パス
	const std::string kCrystalPath = "data/model/stage/crystal.mv1";
	const std::string kCrystalStandPath = "data/model/stage/crystalStand.mv1";

	// クリスタルHPの描画位置とフォントサイズ
	constexpr int kCrystalHpX = 450;
	constexpr int kCrystalHpY = 36;
	constexpr int kCrystalHpFontSize = 24;
	constexpr float kBGScale = 0.65f;
}

/// <summary>
/// コンストラクタ
/// </summary>
Crystal::Crystal(int hp):
	ObjectBase(Collidable::Priority::Low,GameObjectTag::Crystal),
	m_pos(),
	m_hp(hp),
	m_isBreak(false),
	m_crystalStandHandle(-1),
	m_angle(0.0f),
	m_effectCreateCount(0),
	m_bgHandle(-1),
	m_textMagPower(1.0f),
	m_isDamaged(false)
{
	//当たり判定の生成
	auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Cupsule, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderCupsule*>(collider.get());
	sphereCol->m_radius = kCollisionRadius;
	sphereCol->m_size = kCollisionSize;


}

/// <summary>
/// デストラクタ
/// </summary>
Crystal::~Crystal()
{
	MV1DeleteModel(m_modelHandle);
	MV1DeleteModel(m_crystalStandHandle);

	//DeleteGraph(m_noizeTexHandle);
}

/// <summary>
/// 初期化
/// </summary>
void Crystal::Init()
{
	//当たり判定の初期化
	OnEntryPhysics();
	//物理クラスの初期化
	rigidbody->Init();

	//モデルハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle("M_CRYSTAL");
	m_crystalStandHandle = ResourceManager::GetInstance().GetHandle("M_CRYSTALSTAND");

	MV1SetScale(m_modelHandle, VGet(kModelSize, kModelSize, kModelSize));
	MV1SetScale(m_crystalStandHandle, VGet(kModelSize, kModelSize, kModelSize));

	m_psHandle = ResourceManager::GetInstance().GetHandle("P_SHADER");
	m_vsHandle = ResourceManager::GetInstance().GetHandle("V_SHADER");

	m_noizeTexHandle = ResourceManager::GetInstance().GetHandle("I_NOIZETEX");

	cBufferHandle = CreateShaderConstantBuffer(sizeof(UserData));
	pUserData = static_cast<UserData*>(GetBufferShaderConstantBuffer(cBufferHandle));
	pUserData->time = 0.0f;
	pUserData->isNormalDraw = true;

	// Zバッファに書き込まないようにする
	//MV1SetWriteZBuffer(m_modelHandle, false);

	m_preHp = m_hp;
}

/// <summary>
/// 更新
/// </summary>
void Crystal::Update()
{
	if (m_isBreak)	return;

	if (m_hp <= 0)
	{
		m_isBreak = true;
	}

	if (m_effectCreateCount % 250 == 0)
	{
		//エフェクトを出す
		EffectManager::GetInstance().CreateEffect("E_CRYSTALDEFAULT", rigidbody->GetPos());
		EffectManager::GetInstance().CreateEffect("E_CRYSTALLIGHTNING", rigidbody->GetPos());
	}

	m_angle += 0.025f;
	auto posY = sinf(m_angle);

	MV1SetPosition(m_modelHandle, VECTOR(m_pos.x, m_pos.y + posY, m_pos.z));
	MV1SetRotationXYZ(m_modelHandle, VECTOR(0.0f, m_angle / 4, 0.0f));

	pUserData->time += 0.01f;  // 時間を進める
	m_effectCreateCount++;

	//if (m_isDamaged)
	//{
	//	auto sin = sinf(m_textMagPower);


	//	m_textMagPower += 1.0f;
	//}
}

/// <summary>
/// 描画
/// </summary>
void Crystal::Draw()
{
	MV1DrawModel(m_crystalStandHandle);


	MV1SetUseOrigShader(true);

	// シェーダーをセット
	SetUseVertexShader(m_vsHandle);
	SetUsePixelShader(m_psHandle);

	// コンスタントバッファを更新
	UpdateShaderConstantBuffer(cBufferHandle);
	SetShaderConstantBuffer(cBufferHandle, DX_SHADERTYPE_PIXEL, 4);  // ピクセルシェーダーに渡す

	SetUseTextureToShader(1, m_noizeTexHandle);

	MV1DrawModel(m_modelHandle);

	// シェーダーを解除
	SetUseVertexShader(-1);
	SetUsePixelShader(-1);

	//クリスタルスタンドは通常描画
	SetUseTextureToShader(1, -1);
	MV1SetUseOrigShader(false);

}

void Crystal::DrawHP()
{
	if (m_bgHandle == -1)
	{
		m_bgHandle = ResourceManager::GetInstance().GetHandle("I_CRYSTALBG");
	}

	if (m_preHp != m_hp)
	{
		m_textMagPower = max(m_textMagPower - 0.1f, 1.0f);
	}

	// クリスタルの残りHPの描画
	DrawUI::GetInstance().RegisterDrawRequest([=]()
	{
		DrawRotaGraph(kCrystalHpX, kCrystalHpY + 4, kBGScale, 0.0f, m_bgHandle, true);
		FontManager::GetInstance().DrawCenteredExtendText(kCrystalHpX, kCrystalHpY, std::to_string(m_hp), 0xffffff, kCrystalHpFontSize, 0x395f62, m_textMagPower);
	}, 2);
}

/// <summary>
/// 生成座標を設定
/// </summary>
void Crystal::Set(const Vec3& pos)
{
	m_pos = pos;

	m_pos.y -= kModelSize * 90;

	MV1SetPosition(m_modelHandle, m_pos.ToVECTOR());
	MV1SetPosition(m_crystalStandHandle, m_pos.ToVECTOR());

	rigidbody->SetPos(pos);
}

/// <summary>
/// ほかのオブジェクトと衝突したときに呼ばれる
/// </summary>
void Crystal::OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)
{
	//当たったオブジェクトが敵なら自身のHPを減らす
	if (send->GetTag() == GameObjectTag::Enemy)
	{
		if (sendCol->collideTag == MyLib::ColliderBase::CollisionTag::Normal)
		{
			EnemyBase* col = dynamic_cast<EnemyBase*>(send.get());
			col->Finalize();
			col->End();

			SoundManager::GetInstance().PlaySE("S_CRYSTALHIT");

			m_isDamaged = true;

			m_preHp = m_hp;
			m_textMagPower = 4.0f;

			m_hp -= col->GetCrystalDamage();

#ifdef _DEBUG	//デバッグ描画
			printf("クリスタルにダメージが入りました。\n");
#endif
		}
	}
}

const void Crystal::PlayerDead()
{
	m_preHp = m_hp;
	m_textMagPower = 4.0f;

	m_hp -= 5;
}

/// <summary>
/// カメラから注視点までのレイとクリスタルが当たっているかを確認
/// </summary>
void Crystal::CheckCameraRayHit(const Vec3 cameraPos, const Vec3 targetPos)
{

	auto center = Vec3(rigidbody->GetPos().x, (kCollisionSize / 2 + kCollisionRadius)/2,rigidbody->GetPos().z);
	auto nearPos = GetNearestPtOnLine(cameraPos, targetPos, center);

	if ((center - nearPos).Length() <= (kCollisionSize / 2 + kCollisionRadius)/2)
	{
		pUserData->isNormalDraw = false;
	}
	else
	{
		pUserData->isNormalDraw = true;
	}
}