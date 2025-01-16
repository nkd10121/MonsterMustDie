#include "SpikeTrap.h"

#include "ResourceManager.h"
#include "LoadCSV.h"

namespace
{
	//当たり判定の円の半径
	constexpr float kCollisionRadius = 11.0f;

	//モデルサイズ
	constexpr float kModelScale = 1.8f;

	constexpr float kSpikeMoveSpeed = 0.8f;

	//モデルのスパイク部分を地面に埋めておくためのオフセット
	constexpr float kSpikePosYOffset = 7.5f;

	//スパイクモデルが動く幅。大きくすればするほど大きく動く。
	constexpr float kSpikeModelMoveRange = 120.0f;
	//サインカーブの制限。(0.0f～0.1f)
	constexpr float kSinLimit = 0.075f;

	//ボーン（フレーム）の名前を指定
	const char* kTargetFrameName = "wooden_spikes.001"; 

}

SpikeTrap::SpikeTrap() :
	TrapBase(),
	m_attackCount(0),
	m_coolTimeCount(0),
	m_frameIdx(0),
	m_spikePos(),
	m_norm(),
	m_movedPos()
{
	//当たり判定の生成
	auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Search);
	auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
	sphereCol->m_radius = kCollisionRadius;

	m_trapName = "Spike";
	//罠のステータスを取得
	m_status = LoadCSV::GetInstance().LoadTrapStatus(m_trapName.c_str());
}


SpikeTrap::~SpikeTrap()
{
	//MEMO:モデルの削除はTrapBaseでしているためしなくて大丈夫
}


void SpikeTrap::Init(Vec3 pos, Vec3 norm)
{
	//当たり判定を取るようにする
	OnEntryPhysics();

	//物理挙動の初期化
	rigidbody->Init();

	//座標の更新
	rigidbody->SetPos(pos);
	rigidbody->SetNextPos(pos);

	//設置場所に座標を移動させる
	m_spikePos = pos;
	m_spikePosInit = m_spikePos;	//初期座標を保存

	//モデルのハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle("M_SPIKE");
	MV1SetScale(m_modelHandle, VECTOR(kModelScale, kModelScale, kModelScale));
	MV1SetPosition(m_modelHandle, m_spikePos.ToVECTOR());

	//第二引数の法線ベクトルに沿ってモデルの向きを回転させたい
	m_norm = norm;

	//回転させる
	//MEMO:この罠は床にのみ設置できる罠にすると思うため、壁に設置する処理をなくす可能性あり
	MV1SetRotationXYZ(m_modelHandle, VGet(norm.z * (DX_PI_F / 2), 0.0f, -norm.x * (DX_PI_F / 2)));

	//3Dモデルからスパイク部分のフレーム番号を取得
	m_frameIdx = MV1SearchFrame(m_modelHandle, kTargetFrameName);

	//そのままだとスパイク部分が飛び出たままだから地面に埋めるために下げる
	m_spikePosInit.y -= 7.5f;

	//スパイク部分の座標を下げる
	auto mat = MV1GetFrameLocalWorldMatrix(m_modelHandle, m_frameIdx);
	mat.m[3][0] = m_spikePosInit.x;
	mat.m[3][1] = m_spikePosInit.y;
	mat.m[3][2] = m_spikePosInit.z;

	//スパイク部分に行列をセットする
	MV1SetFrameUserLocalWorldMatrix(m_modelHandle, m_frameIdx, mat);

	//存在フラグをtrueにする
	m_isExist = true;
}


void SpikeTrap::Update()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

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

		//攻撃カウントを更新
		m_attackCount++;

		//現在の攻撃フレームと1フレーム前のsinカーブを計算する
		auto presin = sinf(DX_PI_F * (m_attackCount - 1) * 0.0125f) * 0.4f;
		auto sin = sinf(DX_PI_F * m_attackCount * 0.0125f) * 0.4f;

		//二つのsinカーブを比較して上昇か下降かを計算する
		auto move = (sin - presin) * kSpikeModelMoveRange;



		//制限より小さいときはモデルを動かす
		if (sin < kSinLimit)
		{
			auto mat = MV1GetFrameLocalWorldMatrix(m_modelHandle, m_frameIdx);
			mat.m[3][0] += (m_norm * move).x;
			mat.m[3][1] += (m_norm * move).y;
			mat.m[3][2] += (m_norm * move).z;

			MV1SetFrameUserLocalWorldMatrix(m_modelHandle, m_frameIdx, mat);
		}

		//サインカーブが0以下になった時に攻撃中から抜け出す
		if (sin < -0.2f)
		{
			m_isAttack = false;

			auto col = GetCollider(MyLib::ColliderBase::CollisionTag::Attack);
			if (col != nullptr)
			{
				Collidable::DeleteRequestCollider(col);
			}
		}
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
			//スパイク部分の座標を初期化する
			auto mat = MV1GetFrameLocalWorldMatrix(m_modelHandle, m_frameIdx);
			mat.m[3][0] = m_spikePosInit.x;
			mat.m[3][1] = m_spikePosInit.y;
			mat.m[3][2] = m_spikePosInit.z;
			MV1SetFrameUserLocalWorldMatrix(m_modelHandle, m_frameIdx, mat);

			m_spikePos = m_spikePosInit;
			m_movedPos = Vec3();

			//カウントを初期化する
			m_coolTimeCount = 0;
			m_attackCount = 0;
		}

	}


}


void SpikeTrap::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//モデルの描画
	MV1DrawModel(m_modelHandle);
}