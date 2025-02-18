#include "EnemyFast.h"
#include "EnemyStateWalk.h"
#include "EnemyStateDeath.h"
#include "EnemyStateDamaged.h"

#include "Player.h"
#include "Shot.h"
#include "TrapBase.h"

#include "SoundManager.h"
#include "EffectManager.h"
#include "LoadCSV.h"
#include "ResourceManager.h"

namespace
{
	constexpr float kCollisionCapsuleSize = 3.0f;	//カプセルの大きさ
	constexpr float kCollisionCapsuleRadius = 2.0f;	//カプセルの半径
	constexpr int kCollisionCapsuleDivNum = 10;		//カプセルの分割数
#ifdef _DEBUG	//デバッグ描画
#endif

	/*モデル関係*/
	constexpr float kModelScale = 0.00055f;		//モデルサイズ

	/*当たり判定関係*/
	constexpr float kSearchCollisionRadius = kCollisionCapsuleRadius * 10;

	const char* kAttachFrameName = "Bip001 Head";
}

/// <summary>
/// コンストラクタ
/// </summary>
EnemyFast::EnemyFast() :
	EnemyBase()
{
	//キャラクター名を設定
	m_characterName = "EnemyFast";
	m_isOffensive = false;

}

/// <summary>
/// デストラクタ
/// </summary>
EnemyFast::~EnemyFast()
{
}

/// <summary>
/// 初期化
/// </summary>
void EnemyFast::Init()
{
	//スタートウェイポイントの座標に設定する
	SetPos(m_route[0].pos);

	//当たり判定の初期化
	OnEntryPhysics();

	m_collisionRadius = kCollisionCapsuleRadius;
	m_collisionSize = kCollisionCapsuleSize;

	//ステートパターンの初期化
	m_pState = std::make_shared<EnemyStateWalk>(std::dynamic_pointer_cast<EnemyFast>(shared_from_this()));
	m_pState->SetNextKind(StateBase::StateKind::Walk);
	m_pState->Init("");

	//物理データの初期化
	rigidbody->Init(true);
	rigidbody->SetPos(m_drawPos);
	rigidbody->SetNextPos(rigidbody->GetPos());

	//モデルハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle("M_ENEMYNORMAL");
	//モデルのサイズを変更
	MV1SetScale(m_modelHandle, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetPosition(m_modelHandle, m_drawPos.ToVECTOR());

	//待機アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, LoadCSV::GetInstance().GetAnimIdx(m_characterName, "WALK_FORWARD"));
	m_preAnimIdx = 0;
	m_nowAnimIdx = 0;

	//ステータス取得
	m_status = LoadCSV::GetInstance().LoadStatus(m_characterName.c_str());
	//最大HPを設定しておく
	m_hpMax = m_status.hp;

	//当たり判定をとらないオブジェクトタグを設定
	AddThroughTag(GameObjectTag::Portion);		//ポーション
	AddThroughTag(GameObjectTag::SwarmEnemy);	//敵群れ

	{
		//通常の当たり判定の作成
		auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Cupsule, false);	//追加
		auto capsuleCol = dynamic_cast<MyLib::ColliderCupsule*>(collider.get());			//キャスト
		capsuleCol->m_radius = kCollisionCapsuleRadius;		//カプセルの半径
		capsuleCol->m_size = kCollisionCapsuleSize;			//カプセルの大きさ

	}

	{
		//ヘッドショット判定の作成
		auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Head);	//追加
		auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());			//キャスト
		sphereCol->m_radius = 2.0f;		//カプセルの半径

		auto attachFrameNum = MV1SearchFrame(m_modelHandle, kAttachFrameName);
		auto mat = MV1GetFrameLocalWorldMatrix(m_modelHandle, attachFrameNum);
		auto pos = Vec3(mat.m[3][0], mat.m[3][1], mat.m[3][2]);

		auto modelCenterPos = rigidbody->GetPos();

		auto vec = pos - modelCenterPos;
		vec.y *= 0.55f;

		sphereCol->SetOffsetPos(vec);
	}



	//存在フラグをtrueにする
	m_isExist = true;
}

/// <summary>
/// 終了
/// </summary>
void EnemyFast::Finalize()
{
	if (m_isExist)
	{
		//当たり判定の削除
		Collidable::OnExistPhysics();
	}
}

/// <summary>
/// 更新
/// </summary>
void EnemyFast::Update()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//攻撃してきたオブジェクト名配列のサイズが0以外なら
	if (m_attackerName.size() != 0)
	{
		//条件にあっていたら
		if (m_attackerNameClearCount > m_attackerNameClearLimit)
		{
			//配列を削除する
			m_attackerName.clear();

			//カウントを初期化する
			m_attackerNameClearCount = 0;
		}
		else
		{
			//カウントを更新する
			m_attackerNameClearCount++;
		}
	}

	//ステートの更新
	m_pState->Update();

	//アニメーションの更新
	m_isAnimationFinish = UpdateAnim(m_currentAnimNo);
	//アニメーションブレンド
	AnimationBlend();

	//HPが0になったら自身を削除する
	if (m_status.hp <= 0 && !m_isStartDeathAnimation)
	{
		//当たり判定を削除
		Finalize();

		//死亡アニメーションを開始する
		m_isStartDeathAnimation = true;

		//現在のステートを強制的に死亡にする
		m_pState = std::make_shared<EnemyStateDeath>(std::dynamic_pointer_cast<EnemyBase>(shared_from_this()));
		m_pState->SetNextKind(StateBase::StateKind::Death);
		m_pState->Init("");
	}

	//死亡アニメーションが始まっていて
	if (m_isStartDeathAnimation)
	{
		//死亡アニメーションが終了したら
		if (GetAnimEnd())
		{
			//完全に死亡したものとする
			m_isExist = false;

			EffectManager::GetInstance().CreateEffect("E_ENEMYDEAD", rigidbody->GetPos());
		}
	}

	m_moveDebuff = 1.0f;


#ifdef _DEBUG
	//何の当たり判定を持っているかをデバッグ描画
	for (auto& col : m_colliders)
	{
		switch (col->collideTag)
		{
		case MyLib::ColliderBase::CollisionTag::Normal:
			printf("通常の当たり判定\n");
			break;

		case MyLib::ColliderBase::CollisionTag::Attack:
			printf("攻撃の当たり判定\n");
			break;

		case MyLib::ColliderBase::CollisionTag::Search:
			printf("索敵の当たり判定\n");
			break;

		case MyLib::ColliderBase::CollisionTag::Head:
			printf("頭の当たり判定\n");
			break;
		default:
			break;
		}
	}
#endif

}

/// <summary>
/// 描画
/// </summary>
void EnemyFast::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//モデルを描画
	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG	//デバッグ描画
	//ステートパターンの確認
	m_pState->DebugDrawState(0, 280);
#endif
}

/// <summary>
/// 索敵判定を生成する
/// </summary>
void EnemyFast::CreateSearchCollision()
{
	//当たり判定の作成
	auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Search);
	auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
	sphereCol->m_radius = kSearchCollisionRadius;
}

/// <summary>
/// 索敵判定を削除する
/// </summary>
void EnemyFast::DeleteSearchCollision()
{
	Collidable::DeleteRequestCollider(Collidable::GetCollider(MyLib::ColliderBase::CollisionTag::Search));
}

/// <summary>
/// 3Dモデルの座標更新
/// </summary>
void EnemyFast::UpdateModelPos()
{
	rigidbody->SetPos(rigidbody->GetNextPos());
	m_drawPos = rigidbody->GetPos();
	m_drawPos.y -= kCollisionCapsuleRadius + kCollisionCapsuleSize;
	MV1SetPosition(m_modelHandle, m_drawPos.ToVECTOR());
}

/// <summary>
/// 索敵判定の半径を取得
/// </summary>
const float EnemyFast::GetSearchCollisionRadius() const
{
	return  kSearchCollisionRadius;
}

/// <summary>
/// 押し出し処理を行わないオブジェクトと衝突したとき
/// </summary>
void EnemyFast::OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)
{
	//当たったオブジェクトのタグを取得する
	m_hitObjectTag = send->GetTag();

	bool isDamaged = false;

	if (ownCol->collideTag == MyLib::ColliderBase::CollisionTag::Normal)
	{
		//当たったオブジェクトがプレイヤーが撃った弾なら
		if (m_hitObjectTag == GameObjectTag::PlayerShot)
		{
			isDamaged = true;
			//弾の攻撃力分自身のHPを減らす(防御力と調整しながら)
			Shot* col = dynamic_cast<Shot*>(send.get());
			auto damage = col->GetAtk() - m_status.def;
			if (damage > 0)
			{
				m_status.hp -= damage;
			}
			//攻撃してきたタグを保存
			m_lastAttackTag = m_hitObjectTag;
			//敵ヒットSEを流す
			SoundManager::GetInstance().PlaySE("S_ENEMYHIT");
			//敵ヒットエフェクトを出す
			EffectManager::GetInstance().CreateEffect("E_ENEMYHIT", rigidbody->GetPos());
			//当たった弾の終了処理を呼ぶ
			col->End();

			AddAttackerName("Player");
		}
		//当たったオブジェクトがトラップなら
		else if (m_hitObjectTag == GameObjectTag::Trap)
		{
			if (sendCol->collideTag == MyLib::ColliderBase::CollisionTag::Attack)
			{
				isDamaged = true;
				//そのトラップの攻撃力分HPを減らす
				TrapBase* col = dynamic_cast<TrapBase*>(send.get());
				auto damage = col->GetAtk() - m_status.def;
				if (damage > 0)
				{
					m_status.hp -= damage;
				}

				//攻撃してきたタグを保存
				m_lastAttackTag = m_hitObjectTag;

				EffectManager::GetInstance().CreateEffect("E_TRAPATTACK", rigidbody->GetPos());

				AddAttackerName(col->GetTrapName());
			}
		}
	}
	else if (ownCol->collideTag == MyLib::ColliderBase::CollisionTag::Search)
	{
		//当たったオブジェクトがプレイヤーがなら
		if (m_hitObjectTag == GameObjectTag::Player)
		{
			m_isSearchInPlayer = true;
			if (m_pState->GetKind() != StateBase::StateKind::Attack)
			{
				m_pState->SetNextKind(StateBase::StateKind::Walk);
			}
		}
	}
	else if (ownCol->collideTag == MyLib::ColliderBase::CollisionTag::Head)
	{
		//当たったオブジェクトがプレイヤーが撃った弾なら
		if (m_hitObjectTag == GameObjectTag::PlayerShot)
		{
			{
				isDamaged = true;
				//弾の攻撃力分自身のHPを減らす(防御力と調整しながら)
				Shot* col = dynamic_cast<Shot*>(send.get());
				auto damage = col->GetAtk() - m_status.def;
				if (damage > 0)
				{
					//通常の3倍のダメージを与える
					m_status.hp -= damage * 3;
				}
				//攻撃してきたタグを保存
				m_lastAttackTag = m_hitObjectTag;
				//敵ヒットSEを流す
				SoundManager::GetInstance().PlaySE("S_ENEMYHIT");
				//敵ヒットエフェクトを出す
				EffectManager::GetInstance().CreateEffect("E_ENEMYCRITICALHIT", rigidbody->GetPos());
				//当たった弾の終了処理を呼ぶ
				col->End();

				AddAttackerName("Player");
#ifdef _DEBUG	//デバッグ描画

#endif
			}
		}
	}

	//ダメージを食らっていたら
	if (isDamaged)
	{
		//30%の確率で被ダメージ状態にする
		if (GetRand(99) + 1 <= 30)
		{
			m_pState = std::make_shared<EnemyStateDamaged>(std::dynamic_pointer_cast<EnemyBase>(shared_from_this()));
			m_pState->SetNextKind(StateBase::StateKind::Damaged);
			m_pState->Init("");
		}
	}
}

/// <summary>
/// 押し出し処理を行わないオブジェクトと衝突しなくなった時
/// </summary>
void EnemyFast::OnTriggerExit(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)
{
	//当たったオブジェクトのタグを取得する
	m_hitObjectTag = send->GetTag();

	//当たったオブジェクトがプレイヤーのとき
	if (m_hitObjectTag == GameObjectTag::Player)
	{
		//当たったコリジョンが索敵の時
		if (ownCol->collideTag == MyLib::ColliderBase::CollisionTag::Search)
		{
			m_isSearchInPlayer = false;
			if (m_pState->GetKind() != StateBase::StateKind::Attack)
			{
				m_pState->SetNextKind(StateBase::StateKind::Idle);
			}
		}
	}
}