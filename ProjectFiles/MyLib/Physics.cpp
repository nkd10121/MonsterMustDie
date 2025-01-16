#include "Physics.h"
#include <cassert>
#include "MyLib.h"
#include "DxLib.h"
#include "MathHelp.h"
#include "CharacterBase.h"

//おそらく一時的な実装
#include "ResourceManager.h"
#include "TrapManager.h"

//軽量化できてるか比較用
//いろいろ試してみたがこれをtrueにした方が処理時間が増えていた…
#define weight_reduction false

MyLib::Physics* MyLib::Physics::m_instance = nullptr;

namespace
{
	//壁ポリゴンか床ポリゴンかを判断するための変数
	constexpr float kWallPolyBorder = 0.4f;
	//壁ポリゴンと判断するための高さ変数
	constexpr float kWallPolyHeight = 0.2f;

	//重力
	constexpr float kGravity = -0.04f;
	//最大重力加速度
	constexpr float kMaxGravityAccel = -1.50f;

	//補正前情報色
	const int kBeforeFixInfoColor = 0x0000ff;
	//補正前予定情報色
	const int kAimInfoColor = 0x00ff00;
	//補正後情報色
	const int kAfterFixInfoColor = 0xff00ff;
}

/// <summary>
/// コンストラクタ
/// </summary>
MyLib::Physics::Physics(/*int normalStageCollisionHandle,int enemyStageCollisionHandle*/)
{

}

/// <summary>
/// デストラクタ
/// </summary>
MyLib::Physics::~Physics()
{

}

void MyLib::Physics::SetStageCollisionModel(std::string id,std::string id2)
{
	m_stageCollisionHandle = ResourceManager::GetInstance().GetHandle(id);
	MV1SetScale(m_stageCollisionHandle, VGet(0.01f, 0.01f, 0.01f));
	MV1SetRotationXYZ(m_stageCollisionHandle, VGet(0.0f, DX_PI_F, 0.0f));

	m_enemyCollisionHandle = ResourceManager::GetInstance().GetHandle(id2);
	MV1SetScale(m_enemyCollisionHandle, VGet(0.01f, 0.01f, 0.01f));
	MV1SetRotationXYZ(m_enemyCollisionHandle, VGet(0.0f, DX_PI_F, 0.0f));
}

/// <summary>
/// 衝突物の登録
/// </summary>
/// <param name="collidable">追加する当たり判定</param>
void MyLib::Physics::Entry(std::shared_ptr<Collidable> collidable)
{
	// 登録
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), collidable) != m_collidables.end());
	if (!found)
	{
		m_collidables.emplace_back(collidable);
		//collidables.push_back(collidable);
	}
	// 既に登録されてたらエラー
	else
	{
		assert(0 && "指定のcollidableは登録済です。");
	}
}

/// <summary>
/// 衝突物の登録削除
/// </summary>
/// <param name="collidable">削除する当たり判定</param>
void MyLib::Physics::Exit(std::shared_ptr<Collidable> collidable)
{
	auto it = std::find(m_collidables.begin(), m_collidables.end(), collidable);
	// 登録済みなら削除
	if (it != m_collidables.end())
	{
		m_collidables.erase(it);
	}
	// 登録されてなかったらエラー
	else
	{
		assert(0 && "指定のcollidableが登録されていません。");
	}
}

/// <summary>
/// 更新（登録オブジェクトの物理移動、衝突通知）
/// </summary>
void MyLib::Physics::Update()
{
	//当たり判定のログを残す
	m_preCollideInfo = m_newCollideInfo;
	m_newCollideInfo.clear();
	m_preTirrigerInfo = m_newTirrigerInfo;
	m_newTirrigerInfo.clear();
	m_onCollideInfo.clear();

	for (auto& item : m_collidables)
	{
		item->DeleteCollider();
	}

	// 移動
	for (auto& item : m_collidables)
	{
		// ポジションに移動力を足す
		auto pos = item->rigidbody->GetPos();
		auto velocity = item->rigidbody->GetVelocity();

		// 重力を利用する設定なら、重力を追加する
		if (item->rigidbody->GetUseGravity())
		{
			velocity = velocity + Vec3(0.0f, kGravity, 0.0f);

			// 最大重力加速度より大きかったらクランプ
			if (velocity.y < kMaxGravityAccel)
			{
				velocity = Vec3(velocity.x, kMaxGravityAccel, velocity.z);
			}
		}
		auto nextPos = pos + velocity;

		item->rigidbody->SetVelocity(velocity);

		// 予定ポジション設定
		item->rigidbody->SetNextPos(nextPos);

		// もともとの情報、予定情報をデバッグ表示
		for (const auto& collider : item->m_colliders)
		{
			auto colPos = collider->collide->GetWorldPos();
			if (!collider->collide->GetIsStatic())
			{
				collider->collide->SetCenterPos(nextPos);
			}
			auto colNextPos = collider->collide->GetWorldPos();

			auto kind = collider->collide->GetKind();
			if (kind == ColliderBase::Kind::Sphere)
			{
				auto sphereData = dynamic_cast<MyLib::ColliderSphere*> (collider->collide.get());
				auto radius = sphereData->m_radius;
#if _DEBUG
				MyLib::DebugDraw::AddDrawSphere(colPos, radius, kBeforeFixInfoColor);
				MyLib::DebugDraw::AddDrawSphere(colNextPos, radius, kAimInfoColor);
#endif
			}
			else if (kind == ColliderBase::Kind::Cupsule)
			{
				auto cupsuleData = dynamic_cast<MyLib::ColliderCupsule*> (collider->collide.get());
				auto size = cupsuleData->m_size;
				auto radius = cupsuleData->m_radius;
#if _DEBUG
				MyLib::DebugDraw::AddDrawCupsule(colPos, size, radius, kBeforeFixInfoColor);
				MyLib::DebugDraw::AddDrawCupsule(colNextPos, size, radius, kAimInfoColor);
#endif
			}
		}
	}

	// 当たり判定チェック（nextPos指定）
	CheckColide();

	for (auto& item : m_collidables)
	{
		if (item->GetTag() == GameObjectTag::Player || item->GetTag() == GameObjectTag::Enemy)
		{
			float rad = 0;
			float size = 0;
			int modelHandle = -1;

			for (auto& col : item->m_colliders)
			{
				if (col->collide->GetKind() == ColliderBase::Kind::Cupsule)
				{
					rad = dynamic_cast<MyLib::ColliderCupsule*> (col->collide.get())->m_radius;
					size = dynamic_cast<MyLib::ColliderCupsule*> (col->collide.get())->m_size;

					modelHandle = m_stageCollisionHandle;
				}
				else
				{
					continue;
				}
				if (item->GetTag() == GameObjectTag::Player)
				{
					modelHandle = m_stageCollisionHandle;
				}
				else
				{
					modelHandle = m_enemyCollisionHandle;
				}
			}

			//カプセルの中心座標
			auto capsuleCenterPos = item->rigidbody->GetNextPosVECTOR();
			auto capsulePos1 = VGet(capsuleCenterPos.x, capsuleCenterPos.y + size, capsuleCenterPos.z);
			auto capsulePos2 = VGet(capsuleCenterPos.x, capsuleCenterPos.y - size, capsuleCenterPos.z);

			//カプセルの上下の座標と半径から、第一引数に設定したモデルとの当たり判定を検出してくれる
			m_hitDim = MV1CollCheck_Capsule(modelHandle, -1, capsulePos1, capsulePos2, rad);
		}
		else
		{
			continue;
		}

		//壁と床の当たり判定を行う
		CheckWallAndFloor(item);
		//壁との当たり判定処理
		FixPositionWithWall(item);
		//床との当たり判定処理
		FixNowPositionWithFloor(item);

		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(m_hitDim);
	}

	CheckSendOnCollideInfo(m_preCollideInfo, m_newCollideInfo, false);
	CheckSendOnCollideInfo(m_preTirrigerInfo, m_newTirrigerInfo, true);

	// 位置確定
	FixPosition();

	for (const auto& info : m_onCollideInfo)
	{
		if (!info.own.expired() && !info.send.expired())
		{
			if (info.own.lock()->m_colliders.size() != 0)
			{
				if (info.kind == eOnCollideInfoKind::CollideEnter)
				{
					info.own.lock()->OnCollideEnter(info.ownCol,info.send.lock(), info.sendCol);
				}
				else if (info.kind == eOnCollideInfoKind::CollideStay)
				{
					info.own.lock()->OnCollideStay(info.ownCol, info.send.lock(), info.sendCol);
				}
				else if (info.kind == eOnCollideInfoKind::CollideExit)
				{
					info.own.lock()->OnCollideExit(info.ownCol, info.send.lock(), info.sendCol);
				}
				else if (info.kind == eOnCollideInfoKind::TriggerEnter)
				{
					info.own.lock()->OnTriggerEnter(info.ownCol, info.send.lock(), info.sendCol);
				}
				else if (info.kind == eOnCollideInfoKind::TriggerStay)
				{
					info.own.lock()->OnTriggerStay(info.ownCol, info.send.lock(), info.sendCol);
				}
				else if (info.kind == eOnCollideInfoKind::TriggerExit)
				{
					info.own.lock()->OnTriggerExit(info.ownCol, info.send.lock(), info.sendCol);
				}
			}
		}
	}
}

void MyLib::Physics::Clear()
{
	MV1DeleteModel(m_stageCollisionHandle);
	MV1DeleteModel(m_enemyCollisionHandle);

	m_collidables.clear();
	m_onCollideInfo.clear();

	m_newCollideInfo.clear();
	m_preCollideInfo.clear();
	m_newTirrigerInfo.clear();
	m_preTirrigerInfo.clear();
}

std::vector<std::shared_ptr<MyLib::Collidable>> MyLib::Physics::GetCollisionList() const
{
	std::vector<std::shared_ptr<Collidable>> ret;

	for (int i = 0; i < m_collidables.size(); i++)
	{
		for (int j = i + 1; j < m_collidables.size(); j++)
		{
			auto& obj1 = m_collidables[i];
			auto& obj2 = m_collidables[j];

			//どちらかのオブジェクトが相手のオブジェクトタグを無視するようになっていたら追加せずに次へ
			if (obj1->IsThroughTarget(obj2) || obj2->IsThroughTarget(obj1))
			{
				continue;
			}

			// 移動しないオブジェクト同士なら判定しない
			if (obj1->GetPriority() == Collidable::Priority::Static && obj2->GetPriority() == Collidable::Priority::Static) continue;

			bool isAdd = false;
			for (auto& col1 : obj1->m_colliders)
			{
				float lengthA = 0.0f;
				if (col1->collide->GetKind() == ColliderBase::Kind::Cupsule)
				{
					auto colA = dynamic_cast<MyLib::ColliderCupsule*>(col1->collide.get());
					lengthA = colA->m_radius + colA->m_size;
				}
				else if (col1->collide->GetKind() == ColliderBase::Kind::Sphere)
				{
					auto colA = dynamic_cast<MyLib::ColliderSphere*>(col1->collide.get());
					lengthA = colA->m_radius;
				}

				for (auto& col2 : obj2->m_colliders)
				{
					float lengthB = 0.0f;
					if (col2->collide->GetKind() == ColliderBase::Kind::Cupsule)
					{
						auto colB = dynamic_cast<MyLib::ColliderCupsule*>(col2->collide.get());
						lengthB = colB->m_radius + colB->m_size;
					}
					else if (col2->collide->GetKind() == ColliderBase::Kind::Sphere)
					{
						auto colB = dynamic_cast<MyLib::ColliderSphere*>(col2->collide.get());
						lengthB = colB->m_radius;
					}

					const auto& pos1 = col1->collide->GetWorldPos();
					const auto& pos2 = col2->collide->GetWorldPos();

					float maxDistance = 40.0f * 40.0f;

					//もし規定距離より比較する二つの大きさの合計が大きければ問答無用で追加する
					if (maxDistance < (lengthA + lengthB)* (lengthA + lengthB))
					{
						// 判定リストに追加
						ret.push_back(obj1);
						ret.push_back(obj2);
						isAdd = true;
						break;
					}

					if ((pos1 - pos2).SqLength() < maxDistance)
					{
						// 判定リストに追加
						ret.push_back(obj1);
						ret.push_back(obj2);
						isAdd = true;
						break;
					}
				}
				if (isAdd) break;
			}
		}
	}


	return ret;
}

/// <summary>
/// 当たり判定チェック
/// </summary>
void MyLib::Physics::CheckColide()
{
#if false
	int checkNum = 0;

	// 衝突通知、ポジション補正
	bool	doCheck = true;
	int		checkCount = 0;	// チェック回数
	while (doCheck)
	{
		doCheck = false;
		++checkCount;

#if weight_reduction
		//すでに当たり判定のチェックを試したペアじゃないか確認するための変数
		std::map<std::shared_ptr<MyLib::ColliderBase>, std::list<std::shared_ptr<MyLib::ColliderBase>>> checkedPair;
		//std::list<std::pair<std::shared_ptr<MyLib::Collidable>, std::shared_ptr<MyLib::Collidable>>> checkedPair;
#endif

		// 2重ループで全オブジェクト当たり判定
		// 中で同一オブジェクトじゃないか、すでにチェックしたペアと同じ組み合わせの当たり判定チェックをしていないか、比較するオブジェクト間の距離が一定距離以上じゃないか　判別したうえで計算している
		for (const auto& objA : m_collidables)
		{
			for (const auto& objB : m_collidables)
			{
				//同一オブジェクトなら早期リターン
				if (objA == objB)
					continue;

				for (int i = 0; i < objA->m_colliders.size(); i++)
				{
					for (int j = 0; j < objB->m_colliders.size(); j++)
					{
						auto& colA = objA->m_colliders.at(i);
						auto& colB = objB->m_colliders.at(j);

						// 優先度に合わせて変数を変更
						auto primary = objA;
						auto secondary = objB;
						auto primaryCollider = colA;
						auto secondaryCollider = colB;
						auto index = i;
						// Aの方が優先度低い場合
						if (objA->GetPriority() < objB->GetPriority())
						{
							primary = objB;
							secondary = objA;
							primaryCollider = colB;
							secondaryCollider = colA;
							index = j;
						}
						// 優先度が変わらない場合
						else if (objA->GetPriority() == objB->GetPriority())
						{
							// 速度の速い方を優先度が高いことにする
							if (objA->rigidbody->GetVelocity().SqLength() < objB->rigidbody->GetVelocity().SqLength())
							{
								primary = objB;
								secondary = objA;
								primaryCollider = colB;
								secondaryCollider = colA;
								index = j;
							}
						}



#if weight_reduction
						//最初は新しいペアという認識でいく
						bool isNewPair = true;
						//当たり判定チェック済みペアの中に同じ組み合わせのペアが存在しないかチェック
						for (auto& chacked : checkedPair)
						{
							//もし同じ組み合わせのペアが見つかったらこのペアは新しいペアじゃない
							if (chacked.first == colB)
							{
								for (auto& col : chacked.second)
								{
									if (!isNewPair)break;

									if (col == colA)
									{
										isNewPair = false;
									}
								}

							}
						}
						//新しいペアじゃないなら早期リターン
						if (!isNewPair)
						{
							continue;
						}
						//ここに来たということは新しいペア
						//今回のペアをチェック済みペアとして登録する
						checkedPair[colA].emplace_back(colB);
#endif

						checkNum++;

						if (!IsCollide(objA->rigidbody, objB->rigidbody, objA->m_colliders[i].collide.get(), objB->m_colliders[j].collide.get())) continue;

						bool isTrigger = objA->m_colliders[i]->collide->IsTrigger() || objB->m_colliders[j]->collide->IsTrigger();

						if (isTrigger)
						{
							AddNewCollideInfo(objA, objB, i, j, m_newTirrigerInfo);
							continue;
						}
						else
						{
							AddNewCollideInfo(objA, objB, i, j, m_newCollideInfo);
						}

						//auto primary = objA;
						//auto secondary = objB;

						//if (primary == secondary)
						//{
						//	break;
						//}

						//auto primaryCollider = objA->m_colliders[i];
						//auto secondaryCollider = objB->m_colliders[j];
						//if (objA->priority < objB->priority)
						//{
						//	primary = objB;
						//	secondary = objA;
						//	primaryCollider = objB->m_colliders[j];
						//	secondaryCollider = objA->m_colliders[i];
						//}

						FixNextPosition(primary->rigidbody, secondary->rigidbody, primaryCollider.collide.get(), secondaryCollider.collide.get());
						// 位置補正をしたらもう一度初めから行う
						doCheck = true;
						break;
					}
					if (doCheck) break;
				}
				if (doCheck) break;
			}
			if (doCheck) break;
		}
		if (doCheck && checkCount > 800)
		{
#if _DEBUG
			//printf("規定回数を超えました");
#endif
			break;
		}
	}

	printf("チェック回数:%d\n", checkNum);

#else 
	// 判定リスト取得
	const auto& checkColliders = GetCollisionList();
	// 判定リストはペアになっているので半分の数だけ繰り返す
	int colNum = static_cast<int>(checkColliders.size() * 0.5f);

	// 衝突通知、ポジション補正
	int		checkCount = 0;	// チェック回数
	while (true)
	{


		bool isNoHit = true;

		for (int i = 0; i < colNum; ++i)
		{
			auto objA = checkColliders[i * 2];
			auto objB = checkColliders[i * 2 + 1];

			for (int i = 0; i < objA->m_colliders.size(); i++)
			{
				for (int j = 0; j < objB->m_colliders.size(); j++)
				{
					auto& colA = objA->m_colliders.at(i);
					auto& colB = objB->m_colliders.at(j);

					//敵同士かつ、どちらかが通常の当たり判定じゃないときは次に行く
					//MEMO:判定回数が多くなるのの対策でこの処理を入れたが処理速度はあまり変わらない可能性あり。今後敵を大量に出したときにまた試してみて。
					bool isContinue = false;
					if (objA->GetTag() == GameObjectTag::Enemy && objB->GetTag() == GameObjectTag::Enemy)
					{
						if (colA->collideTag != MyLib::ColliderBase::CollisionTag::Normal || colB->collideTag != MyLib::ColliderBase::CollisionTag::Normal) isContinue = true;
					}
					if (isContinue)
					{
						continue;
					}

					// 判定回数増加
					checkCount++;



					// 優先度に合わせて変数を変更
					auto primary = objA;
					auto secondary = objB;
					auto primaryCollider = colA;
					auto secondaryCollider = colB;
					auto index = i;
					// Aの方が優先度低い場合
					if (objA->GetPriority() < objB->GetPriority())
					{
						primary = objB;
						secondary = objA;
						primaryCollider = colB;
						secondaryCollider = colA;
						index = j;
					}
					// 優先度が変わらない場合
					else if (objA->GetPriority() == objB->GetPriority())
					{
						// 速度の速い方を優先度が高いことにする
						if (objA->rigidbody->GetVelocity().SqLength() < objB->rigidbody->GetVelocity().SqLength())
						{
							primary = objB;
							secondary = objA;
							primaryCollider = colB;
							secondaryCollider = colA;
							index = j;
						}
					}

					if (!IsCollide(objA->rigidbody, objB->rigidbody, objA->m_colliders[i]->collide.get(), objB->m_colliders[j]->collide.get())) continue;

					bool isTrigger = objA->m_colliders[i]->collide->IsTrigger() || objB->m_colliders[j]->collide->IsTrigger();

					if (isTrigger)
					{
						AddNewCollideInfo(objA, objB, i, j, m_newTirrigerInfo);
						continue;
					}
					else
					{
						AddNewCollideInfo(objA, objB, i, j, m_newCollideInfo);
					}

					FixNextPosition(primary->rigidbody, secondary->rigidbody, primaryCollider->collide.get(), secondaryCollider->collide.get());
					// 位置補正をしたらもう一度初めから行う
					isNoHit = false;
					break;
				}
			}
		}

		// 当たっていなければ終了
		if (isNoHit) break;
		// 最大回数確認したら修正がうまく出来ていなくても終了する
		if (checkCount > 800) break;
	}

#ifdef _DEBUG
	printf("試行回数:%d\n", checkCount);
#endif


#endif
}

/// <summary>
/// /二つのオブジェクトが接触しているかどうか
/// </summary>
bool MyLib::Physics::IsCollide(std::shared_ptr<Rigidbody> rigidA, std::shared_ptr<Rigidbody> rigidB, ColliderBase* colliderA, ColliderBase* colliderB) const
{
	bool isCollide = false;

	auto kindA = colliderA->GetKind();
	auto kindB = colliderB->GetKind();

	if (kindA == MyLib::ColliderBase::Kind::Sphere && kindB == MyLib::ColliderBase::Kind::Sphere)
	{
		auto colA = dynamic_cast<MyLib::ColliderSphere*>(colliderA);
		auto colB = dynamic_cast<MyLib::ColliderSphere*>(colliderB);

		auto atob = colA->GetWorldPos() - colB->GetWorldPos();
		auto atobLength = atob.Length();

		// お互いの距離が、それぞれの半径を足したものより小さければ当たる
		isCollide = (atobLength < colA->m_radius + colB->m_radius);
	}

	//カプセルとカプセルの当たり判定
	if (kindA == MyLib::ColliderBase::Kind::Cupsule && kindB == MyLib::ColliderBase::Kind::Cupsule)
	{
		auto colA = dynamic_cast<MyLib::ColliderCupsule*>(colliderA);
		auto colB = dynamic_cast<MyLib::ColliderCupsule*>(colliderB);

		//二つのカプセルの直線部分同士の最近点間の距離が二つの判定を足した距離と比較する
		auto colACenter = colA->GetWorldPos();
		auto colAPos1 = VGet(colACenter.x, colACenter.y + colA->m_size, colACenter.z);
		auto colAPos2 = VGet(colACenter.x, colACenter.y - colA->m_size, colACenter.z);

		auto colBCenter = colB->GetWorldPos();
		auto colBPos1 = VGet(colBCenter.x, colBCenter.y + colB->m_size, colBCenter.z);
		auto colBPos2 = VGet(colBCenter.x, colBCenter.y - colB->m_size, colBCenter.z);

		auto minLength = Segment_Segment_MinLength(colAPos1, colAPos2, colBPos1, colBPos2);

		isCollide = (minLength < colA->m_radius + colB->m_radius);
	}

	//カプセルと球の当たり判定
	if (kindA == MyLib::ColliderBase::Kind::Cupsule && kindB == MyLib::ColliderBase::Kind::Sphere)
	{
		auto colA = dynamic_cast<MyLib::ColliderCupsule*>(colliderA);
		auto colB = dynamic_cast<MyLib::ColliderSphere*>(colliderB);

		//カプセルの情報を取得
		auto cupsuleCenterPos = colA->GetWorldPos();
		auto cupsuleSize = colA->m_size;

		auto cupsulePos1 = VGet(cupsuleCenterPos.x, cupsuleCenterPos.y + cupsuleSize, cupsuleCenterPos.z);
		auto cupsulePos2 = VGet(cupsuleCenterPos.x, cupsuleCenterPos.y - cupsuleSize, cupsuleCenterPos.z);

		//球の情報を取得
		auto sphereCenterPos = colB->GetWorldPos();
		//カプセルの線分と球の中心座標の距離がカプセルの半径と球の半径を足した値より長いか短いかで判断する
		auto length = Segment_Point_MinLength(cupsulePos1, cupsulePos2, sphereCenterPos.ToVECTOR());

		isCollide = length < colA->m_radius + colB->m_radius;
	}
	else if (kindA == MyLib::ColliderBase::Kind::Sphere && kindB == MyLib::ColliderBase::Kind::Cupsule)
	{
		auto sphere = dynamic_cast<MyLib::ColliderSphere*>(colliderA);
		auto capsule = dynamic_cast<MyLib::ColliderCupsule*>(colliderB);

		//カプセルの情報を取得
		auto cupsuleCenterPos = capsule->GetWorldPos();
		auto cupsuleSize = capsule->m_size;

		auto cupsulePos1 = VGet(cupsuleCenterPos.x, cupsuleCenterPos.y + cupsuleSize, cupsuleCenterPos.z);
		auto cupsulePos2 = VGet(cupsuleCenterPos.x, cupsuleCenterPos.y - cupsuleSize, cupsuleCenterPos.z);

		//球の情報を取得
		auto sphereCenterPos = sphere->GetWorldPos();
		//カプセルの線分と球の中心座標の距離がカプセルの半径と球の半径を足した値より長いか短いかで判断する
		auto length = Segment_Point_MinLength(cupsulePos1, cupsulePos2, sphereCenterPos.ToVECTOR());

		isCollide = length < sphere->m_radius + capsule->m_radius;
	}

	return isCollide;
}

/// <summary>
/// 当たったオブジェクトのペアを登録する
/// </summary>
void MyLib::Physics::AddNewCollideInfo(const std::weak_ptr<Collidable>& objA, const std::weak_ptr<Collidable>& objB, int colIndexA, int colIndexB, SendCollideInfo& info)
{
	auto colA = objA.lock()->m_colliders[colIndexA];
	auto colB = objB.lock()->m_colliders[colIndexB];


	// 既に追加されている通知リストにあれば追加しない
	for (auto& inf : info)
	{
		if (inf.own.lock() == objA.lock() && inf.send.lock() == objB.lock() && inf.ownColIndex == colIndexA && inf.sendColIndex == colIndexB && inf.ownCol == colA && inf.sendCol == colB) return;
		if (inf.own.lock() == objB.lock() && inf.send.lock() == objA.lock() && inf.ownColIndex == colIndexB && inf.sendColIndex == colIndexA && inf.ownCol == colB && inf.sendCol == colA) return;
	}

	SendInfo add;
	add.own = objA;
	add.send = objB;
	add.ownColIndex = colIndexA;
	add.sendColIndex = colIndexB;
	add.ownCol = colA;
	add.sendCol = colB;
	
	info.emplace_back(add);
}

/// <summary>
/// 移動予定の座標を修正する
/// </summary>
void MyLib::Physics::FixNextPosition(std::shared_ptr<Rigidbody> primaryRigid, std::shared_ptr<Rigidbody> secondaryRigid, ColliderBase* primaryCollider, ColliderBase* secondaryCollider) const
{
	// 当たり判定の種別ごとに補正方法を変える
	auto primaryKind = primaryCollider->GetKind();
	auto secondaryKind = secondaryCollider->GetKind();

	if (primaryKind == MyLib::ColliderBase::Kind::Sphere && secondaryKind == MyLib::ColliderBase::Kind::Sphere)
	{
		auto primaryToSecondary = secondaryRigid->GetNextPos() - primaryRigid->GetNextPos();
		auto primaryToSecondaryN = primaryToSecondary.Normalize();

		auto primaryColliderBase = dynamic_cast<MyLib::ColliderSphere*>(primaryCollider);
		auto secondaryColliderBase = dynamic_cast<MyLib::ColliderSphere*>(secondaryCollider);
		auto awayDist = primaryColliderBase->m_radius + secondaryColliderBase->m_radius + 0.0001f;	// そのままだとちょうど当たる位置になるので少し余分に離す
		auto primaryToNewSecondaryPos = primaryToSecondaryN * awayDist;
		auto fixedPos = primaryRigid->GetNextPos() + primaryToNewSecondaryPos;
		fixedPos.y = secondaryRigid->GetPos().y;
		secondaryRigid->SetNextPos(fixedPos);
		secondaryCollider->SetCenterPos(fixedPos);
	}

	//カプセルとカプセルの補正
	if (primaryKind == MyLib::ColliderBase::Kind::Cupsule && secondaryKind == MyLib::ColliderBase::Kind::Cupsule)
	{
		//TODO:カプセルの位置補正がめんどくさそうなので早めに考えろ

		auto colA = dynamic_cast<MyLib::ColliderCupsule*>(primaryCollider);
		auto colB = dynamic_cast<MyLib::ColliderCupsule*>(secondaryCollider);

		//二つのカプセルの直線部分同士の最近点間の距離が二つの半径を足した距離になるように修正する
		auto colACenter = primaryRigid->GetNextPos();
		auto colAPos1 = Vec3(colACenter.x, colACenter.y + colA->m_size, colACenter.z);
		auto colATopVec = colAPos1 - colACenter;

		auto colBCenter = secondaryRigid->GetNextPos();
		auto colBPos1 = Vec3(colBCenter.x, colBCenter.y + colB->m_size, colBCenter.z);
		auto colBTopVec = colBPos1 - colBCenter;

		//それぞれのカプセルの線分上の最近接点を計算
		//結果格納用変数
		Vec3 nearPosOnALine, nearPosOnBLine;
		GetNearestPtOnLine(colACenter, colATopVec, colBCenter, colBTopVec, nearPosOnALine, nearPosOnBLine);

		//カプセルAのカプセルBとの最近接点からカプセルBのカプセルAとの最近接点に向かうベクトルを取得
		auto nearPosToNearPos = nearPosOnBLine - nearPosOnALine;
		//正規化して方向ベクトルにする
		nearPosToNearPos = nearPosToNearPos.Normalize();
		//離す距離を計算(カプセルと級の半径を足した距離+余分)
		auto awayDist = colA->m_radius + colB->m_radius + 0.2f;
		//最近接点の修正座標を計算
		auto fixedNearPos = nearPosOnALine + nearPosToNearPos * awayDist;

		//カプセルBの最近接点からカプセルBの中心座標に向かうベクトルを計算
		auto nearPosToCenterB = colBCenter - nearPosOnBLine;

		auto fixedPos = fixedNearPos + nearPosToCenterB;
		fixedPos.y = secondaryRigid->GetPos().y;
		//修正座標を設定
		secondaryRigid->SetNextPos(fixedPos);
		secondaryCollider->SetCenterPos(fixedPos);
	}

	//カプセルと球の補正
	if (primaryKind == MyLib::ColliderBase::Kind::Cupsule && secondaryKind == MyLib::ColliderBase::Kind::Sphere)
	{
		auto pCupsule = dynamic_cast<MyLib::ColliderCupsule*>(primaryCollider);
		auto pSphere = dynamic_cast<MyLib::ColliderSphere*>(secondaryCollider);

		//カプセルの直線上の球との最近接点と球の中心座標の距離がそれぞれの半径を足した距離になるように修正する
		auto cupsuleCenter = primaryRigid->GetNextPos();
		auto cupsulePos1 = Vec3(cupsuleCenter.x, cupsuleCenter.y - pCupsule->m_size, cupsuleCenter.z);
		auto cupsulePos2 = Vec3(cupsuleCenter.x, cupsuleCenter.y + pCupsule->m_size, cupsuleCenter.z);

		auto sphereCenter = secondaryRigid->GetNextPos();

		//カプセルの直線上の球との最近接点の座標を出す必要がある
		auto nearresPosOnLine = GetNearestPtOnLine(cupsulePos1, cupsulePos2, sphereCenter);

		//カプセルの直線上の球との最近接点から球の中心座標に向かうベクトル
		auto nearPosToSphere = sphereCenter - nearresPosOnLine;
		//上のベクトルを正規化
		nearPosToSphere = nearPosToSphere.Normalize();
		//長さを計算
		auto awayDist = pCupsule->m_radius + pSphere->m_radius + 0.0001f;
		//球の修正後の座標を計算
		auto fixedPos = cupsuleCenter + nearPosToSphere * awayDist;
		fixedPos.y = secondaryRigid->GetPos().y;
		//修正座標を設定
		secondaryRigid->SetNextPos(fixedPos);
		secondaryCollider->SetCenterPos(fixedPos);
	}

	//球とカプセルの補正
	if (primaryKind == MyLib::ColliderBase::Kind::Sphere && secondaryKind == MyLib::ColliderBase::Kind::Cupsule)
	{

	}

}

/// <summary>
/// 種類ごとに衝突通知を飛ばす配列に追加する
/// </summary>
void MyLib::Physics::CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger)
{
#if false
	for (auto& parent : newSendInfo)
	{
		// 以前の情報に親として登録されているか
		bool isPreParent = preSendInfo.find(parent.first) != preSendInfo.end();
		bool isAllEnter = true;

		for (auto& child : parent.second)
		{
			bool isPreChild = false;
			if (isPreParent)
			{
				// 以前の情報に子として登録されているか
				auto& preParent = preSendInfo[parent.first];
				isPreChild = std::find(preParent.begin(), preParent.end(), child) != preParent.end();
			}

			// 今回入ってきた場合はEnterを呼ぶ(子として登録されていない)
			if (!isPreChild)
			{
				if (isTrigger)
				{
					AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::TriggerEnter);
					AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::TriggerEnter);
				}
				else
				{
					AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::CollideEnter);
					AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::CollideEnter);
				}
			}

			// Staryは毎度呼ぶ
			if (isTrigger)
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::TriggerStay);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::TriggerStay);
			}
			else
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::CollideStay);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::CollideStay);
			}

			// 登録されていた情報を削除
			if (isPreChild)
			{
				preSendInfo[parent.first].remove(child);
			}
			// 全て登録されていなかったとする
			else
			{
				isAllEnter = false;
			}
		}

		// 全て登録されていたら親の情報も消す
		if (isAllEnter)
		{
			preSendInfo.erase(parent.first);
		}
	}

	// 残っている前情報からExistを呼ぶ(登録が残っている＝今回抜けた)
	for (auto& parent : preSendInfo)
	{
		for (auto& child : parent.second)
		{
			if (isTrigger)
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::TriggerExit);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::TriggerExit);
			}
			else
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::CollideExit);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::CollideExit);
			}
		}
	}
#else
	// 1つ前に通知リストが当たったか
	auto isPreExist = preSendInfo.size() != 0;

	for (auto& info : newSendInfo)
	{
		bool isEntry = false;

		// 1つ前に通知リストがあった場合
		if (isPreExist)
		{
			bool isEnter = true;
			// 1つ前の通知リストをすべて回す
			auto it = preSendInfo.begin();
			for (; it != preSendInfo.end(); ++it)
			{
				// 通知リストが存在した場合は当たった時の通知を呼ばないようにする
				if (it->own.lock() == info.own.lock() && it->send.lock() == info.send.lock() && it->ownColIndex == info.ownColIndex && it->sendColIndex == info.sendColIndex && it->ownCol == info.ownCol && it->sendCol == info.sendCol) isEnter = false;
				if (it->own.lock() == info.send.lock() && it->send.lock() == info.own.lock() && it->ownColIndex == info.sendColIndex && it->sendColIndex == info.ownColIndex && it->ownCol == info.sendCol && it->sendCol == info.ownCol) isEnter = false;
				if (!isEnter) break;
			}
			if (isEnter)
			{
				// 当たった時の通知を追加
				if (isTrigger) AddOnCollideInfo(info, eOnCollideInfoKind::TriggerEnter);
				else           AddOnCollideInfo(info, eOnCollideInfoKind::CollideEnter);
			}
			else
			{
				// 一つ前の通知リストから今回存在する通知のものを削除する
				preSendInfo.erase(it);
			}
		}
		// 1つ前に通知リストがなかった場合
		else
		{
			// 当たった時の通知を追加
			if (isTrigger) AddOnCollideInfo(info, eOnCollideInfoKind::TriggerEnter);
			else           AddOnCollideInfo(info, eOnCollideInfoKind::CollideEnter);
		}

		// 当たっている状態の通知を追加
		if (isTrigger) AddOnCollideInfo(info, eOnCollideInfoKind::TriggerStay);
		else           AddOnCollideInfo(info, eOnCollideInfoKind::CollideStay);
	}

	// 上で削除されずに残った1つ前の通知リストは今回抜けているため
	// 離れた時の通知を追加
	for (auto& info : preSendInfo)
	{
		if (isTrigger) AddOnCollideInfo(info, eOnCollideInfoKind::TriggerExit);
		else           AddOnCollideInfo(info, eOnCollideInfoKind::CollideExit);
	}
#endif
}

/// <summary>
/// 衝突通知を飛ばす配列に追加する
/// </summary>
void MyLib::Physics::AddOnCollideInfo(const SendInfo& info, eOnCollideInfoKind kind)
{
	m_onCollideInfo.emplace_back(OnCollideInfoData{ info.own,info.send,info.ownCol,info.sendCol, kind });
	m_onCollideInfo.emplace_back(OnCollideInfoData{ info.send,info.own,info.sendCol,info.ownCol, kind });
}

/// <summary>
/// 最終的な位置を決定する
/// </summary>
void MyLib::Physics::FixPosition()
{
	for (auto& item : m_collidables)
	{
#if _DEBUG
		//// 補正後の位置をデバッグ表示
		//DebugDraw::DrawLine(item->rigidbody->GetPos(), item->nextPos, AfterFixInfoColor);

		//if (item->ColliderBase->GetKind() == ColliderBase::Kind::Circle2D)
		//{
		//	auto itemCircleData = dynamic_cast<ColliderBaseCircle2D*>(item->ColliderBase);
		//	DebugDraw::DrawCircle(item->nextPos, itemCircleData->radius, AfterFixInfoColor);
		//}
#endif
		// Posを更新するので、velocityもそこに移動するvelocityに修正
		auto nextPos = item->rigidbody->GetNextPos();
		auto pos = item->rigidbody->GetPos();
		Vec3 toFixedPos = nextPos - pos;
		item->rigidbody->SetVelocity(toFixedPos);

		if (item->GetTag() == GameObjectTag::Player)
		{
			printf("補正前:%f,%f,%f\n", pos.x, pos.y, pos.z);
		}

		// 位置確定
		item->rigidbody->SetPos(item->rigidbody->GetNextPos());

		if (item->GetTag() == GameObjectTag::Player)
		{
			auto p = item->rigidbody->GetNextPos();
			printf("補正後:%f,%f,%f\n", p.x, p.y, p.z);
		}
	}
}

/// <summary>
/// チェックしたポリゴンが壁ポリゴンか床ポリゴンかを判断し保存する
/// </summary>
void MyLib::Physics::CheckWallAndFloor(std::shared_ptr<Collidable>& col)
{
	// 壁ポリゴンと床ポリゴンの数を初期化する
	m_wallNum = 0;
	m_floorNum = 0;
	for (auto& wall : m_pWallPoly)
	{
		wall = {};
	}
	for (auto& floor : m_pFloorPoly)
	{
		floor = {};
	}

	// 検出されたポリゴンの数だけ繰り返し
	for (int i = 0; i < m_hitDim.HitNum; i++)
	{
		// ポリゴンの法線のＹ成分が壁ポリゴンボーダーに達っしているかどうかで壁ポリゴンか床ポリゴンかを判断する
		if (m_hitDim.Dim[i].Normal.y < kWallPolyBorder && m_hitDim.Dim[i].Normal.y > -kWallPolyBorder)
		{
			float radius = 0.0f;
			float size = 0.0f;
			for (auto& col : col->m_colliders)
			{
				if (col->collideTag == MyLib::ColliderBase::CollisionTag::Normal)
				{
					radius = dynamic_cast<MyLib::ColliderCupsule*> (col->collide.get())->m_radius;
					size = dynamic_cast<MyLib::ColliderCupsule*> (col->collide.get())->m_size;
				}
			}

			// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標より高いポリゴンのみ当たり判定を行う
			if (m_hitDim.Dim[i].Position[0].y > col->rigidbody->GetPos().y - size - radius / 2 ||
				m_hitDim.Dim[i].Position[1].y > col->rigidbody->GetPos().y - size - radius / 2 ||
				m_hitDim.Dim[i].Position[2].y > col->rigidbody->GetPos().y - size - radius / 2)
			{
				// ポリゴンの数が限界数に達していなかったらポリゴンを配列に追加
				if (m_wallNum < ColInfo::kMaxColHitPolyNum)
				{
					// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
					m_pWallPoly[m_wallNum] = &m_hitDim.Dim[i];

					// 壁ポリゴンの数を加算する
					m_wallNum++;
				}
			}
		}
		else
		{
			// ポリゴンの数が限界数に達していなかったらポリゴンを配列に追加
			if (m_floorNum < ColInfo::kMaxColHitPolyNum)
			{
				// ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
				m_pFloorPoly[m_floorNum] = &m_hitDim.Dim[i];

				// 床ポリゴンの数を加算する
				m_floorNum++;
			}
		}
	}
}

/// <summary>
/// 壁ポリゴンとの当たり判定をチェックし、移動させる
/// </summary>
void MyLib::Physics::FixPositionWithWall(std::shared_ptr<Collidable>& col)
{
	float radius = 0.0f;
	float size = 0.0f;
	for (auto& col : col->m_colliders)
	{
		if (col->collide->GetKind() == ColliderBase::Kind::Cupsule)
		{
			radius = dynamic_cast<MyLib::ColliderCupsule*> (col->collide.get())->m_radius;
			size = dynamic_cast<MyLib::ColliderCupsule*> (col->collide.get())->m_size;
		}
	}

	// 壁ポリゴンがない場合は何もしない
	if (m_wallNum == 0) return;

	printf("当たった壁の数:%d\n", m_wallNum);

	// 壁ポリゴンとの当たり判定処理
	// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
	m_isHitFlag = false;

	auto capsuleCenterPos = col->rigidbody->GetNextPosVECTOR();
	auto capsulePos1 = VGet(capsuleCenterPos.x, capsuleCenterPos.y + size, capsuleCenterPos.z);
	auto capsulePos2 = VGet(capsuleCenterPos.x, capsuleCenterPos.y - size, capsuleCenterPos.z);

	// 移動したかどうかで処理を分岐
	if (col->rigidbody->GetVelocity().Length() >= 0.05f)
	{
		// 壁ポリゴンの数だけ繰り返し
		for (int i = 0; i < m_wallNum; i++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			m_pPoly = m_pWallPoly[i];

			// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
			if (!HitCheck_Capsule_Triangle(capsulePos1, capsulePos2, radius,
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

			// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
			m_isHitFlag = true;

			//壁を考慮した移動を外積を使って算出
			Vec3 SlideVec;

			VECTOR ret;
			ret = VCross(col->rigidbody->GetVelocityVECTOR(), m_pPoly->Normal);
			// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
			SlideVec = Vec3(ret.x, ret.y, ret.z);

			// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
			// 元の移動成分から壁方向の移動成分を抜いたベクトル
			ret = VCross(m_pPoly->Normal, SlideVec.ToVECTOR());
			SlideVec = Vec3(ret.x, ret.y, ret.z);

			// それを移動前の座標に足したものを新たな座標とする
			col->rigidbody->SetNextPos(col->rigidbody->GetPos() + SlideVec);

			//変更したため新たに取得
			//tempNextPos = col->rigidbody->GetNextPos();
			//nextPos = tempNextPos.ToVECTOR();

			// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
			bool isHitWallPolygon = false;

			capsuleCenterPos = col->rigidbody->GetNextPosVECTOR();
			capsulePos1 = VGet(capsuleCenterPos.x, capsuleCenterPos.y + size, capsuleCenterPos.z);
			capsulePos2 = VGet(capsuleCenterPos.x, capsuleCenterPos.y - size, capsuleCenterPos.z);

			for (int j = 0; j < m_wallNum; j++)
			{
				// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				m_pPoly = m_pWallPoly[j];

				// 当たっていたらループから抜ける
				if (HitCheck_Capsule_Triangle(capsulePos1, capsulePos2, radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					//trueにする
					isHitWallPolygon = true;
					break;
				}
			}

			// 全てのポリゴンと当たっていなかったらここでループ終了
			if (!isHitWallPolygon)
			{
				//ヒットフラグを倒す
				m_isHitFlag = false;
				break;
			}
		}
	}
	else
	{
		// 移動していない場合の処理

		// 壁ポリゴンの数だけ繰り返し
		for (int i = 0; i < m_wallNum; i++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			m_pPoly = m_pWallPoly[i];

			// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
			if (HitCheck_Capsule_Triangle(capsulePos1, capsulePos2, radius,
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
			{
				m_isHitFlag = true;
				break;
			}
		}
	}

	// 壁に当たっていたら壁から押し出す処理を行う
	if (m_isHitFlag)
	{
		FixPositionWithWallInternal(col);
	}
}

/// <summary>
/// 壁の中から押し出す
/// </summary>
void MyLib::Physics::FixPositionWithWallInternal(std::shared_ptr<Collidable>& col)
{
	float radius = 0.0f;
	float size = 0.0f;
	for (auto& col : col->m_colliders)
	{
		if (col->collide->GetKind() == ColliderBase::Kind::Cupsule)
		{
			radius = dynamic_cast<MyLib::ColliderCupsule*> (col->collide.get())->m_radius;
			size = dynamic_cast<MyLib::ColliderCupsule*> (col->collide.get())->m_size;
		}
	}

	auto capsuleCenterPos = col->rigidbody->GetNextPosVECTOR();


#if TRUE

	auto nextPos = capsuleCenterPos;
	// 壁からの押し出し処理を試みる最大数だけ繰り返し
	for (int i = 0; i < ColInfo::kMaxColHitTryNum; i++)
	{
		// 当たる可能性のある壁ポリゴンを全て見る
		bool isHitWall = false;

		auto capsulePos1 = VGet(nextPos.x, nextPos.y + size, nextPos.z);
		auto capsulePos2 = VGet(nextPos.x, nextPos.y - size, nextPos.z);

		// 壁ポリゴンの数だけ繰り返し
		for (int j = 0; j < m_wallNum; j++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			m_pPoly = m_pWallPoly[j];

			// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
			if (!HitCheck_Capsule_Triangle(capsulePos1, capsulePos2, radius,
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

			auto ret = VAdd(nextPos, VScale(m_pPoly->Normal, col->rigidbody->GetVelocity().Length() * 0.035f));

			nextPos = VGet(ret.x, ret.y, ret.z);

			//// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
			//col->rigidbody->SetNextPos(set);

			//移動後の座標からカプセルの上下の座標を求める
			capsulePos1 = VGet(nextPos.x, nextPos.y + size, nextPos.z);
			capsulePos2 = VGet(nextPos.x, nextPos.y - size, nextPos.z);

			// 移動した上で壁ポリゴンと接触しているかどうかを判定
			for (int k = 0; k < m_wallNum; k++)
			{
				// 当たっていたらループを抜ける
				m_pPoly = m_pWallPoly[k];
				if (HitCheck_Capsule_Triangle(capsulePos1, capsulePos2, radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					isHitWall = true;
					printf("試行回数:%d\n", i);
					break;
				}
			}

			// 全てのポリゴンと当たっていなかったらここでループ終了
			if (!isHitWall)
			{
				printf("試行回数:%d\n", i);
				break;
			}
		}

		//ループ終了
		if (!isHitWall)
		{
			printf("試行回数:%d\n", i);
			break;
		}
	}
	col->rigidbody->SetNextPos(Vec3(nextPos));
#else
	Vec3 fixVec;

	// 壁ポリゴンの数だけ繰り返し
	for (int j = 0; j < m_wallNum; j++)
	{
		// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
		m_pPoly = m_pWallPoly[j];

		// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
		if (!HitCheck_Capsule_Triangle(capsulePos1, capsulePos2, radius,
			m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

		fixVec += m_pPoly->Normal;
	}

	if (fixVec.Length() == 0.0f)	return;

	auto pos = col->rigidbody->GetPos();
	auto nextPos = col->rigidbody->GetNextPos();
	auto dir = nextPos - pos;

	fixVec = fixVec.Normalize();

	printf("修正ベクトル:%f,%f,%f\n", fixVec.x, fixVec.y, fixVec.z);

	auto set = Vec3(VAdd(col->rigidbody->GetNextPosVECTOR(), VScale(fixVec.ToVECTOR(), dir.Length() + 0.00001f)));

	// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
	col->rigidbody->SetNextPos(set);
#endif
}

/// <summary>
/// 床ポリゴンとの当たり判定をチェックし、移動させる
/// </summary>
void MyLib::Physics::FixNowPositionWithFloor(std::shared_ptr<Collidable>& col)
{
#if false
	//床ポリゴンがない場合は何もしない
	if (m_floorNum == 0) return;

	float radius = 0.0f;
	float size = 0.0f;
	for (auto& col : col->m_colliders)
	{
		radius = dynamic_cast<MyLib::ColliderCupsule*> (col.get())->m_radius;
		size = dynamic_cast<MyLib::ColliderCupsule*> (col.get())->m_size;
	}

	// 床ポリゴンとの当たり判定処理
	//あたったかどうかのフラグ初期化
	bool IsHitFlag = false;

	// 床ポリゴンとの当たり判定
	//一番高い床ポリゴンにぶつける為の判定用変数を初期化
	float polyMaxPosY = 0.0f;

	// 床ポリゴンに当たったかどうかのフラグを倒しておく
	m_isHitFlag = false;

	auto capsuleCenterPos = col->rigidbody->GetNextPosVECTOR();
	auto capsuleUnderPos = VGet(capsuleCenterPos.x, capsuleCenterPos.y - size, capsuleCenterPos.z);

	// 床ポリゴンの数だけ繰り返し
	for (int i = 0; i < m_floorNum; i++)
	{
		// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
		m_pPoly = m_pFloorPoly[i];

		auto hit = MV1CollCheck_Sphere(m_stageCollisionHandle, -1, capsuleUnderPos, radius);

		// 当たっていなかったら何もしない
		if (hit.HitNum == 0) continue;

		// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
		if (m_isHitFlag && polyMaxPosY > hit.Dim->HitPosition.y) continue;

		polyMaxPosY = hit.Dim->HitPosition.y;

		// ポリゴンに当たったフラグを立てる
		m_isHitFlag = true;

		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hit);

	}
	if (m_isHitFlag)
	{
		// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
		auto set = col->rigidbody->GetNextPos();
		set.y = polyMaxPosY + size + radius;
		col->rigidbody->SetNextPos(set);
	}
#elif false
	//床ポリゴンがない場合は何もしない
	if (m_floorNum == 0) return;

	float radius = 0.0f;
	float size = 0.0f;
	for (auto& col : col->m_colliders)
	{
		radius = dynamic_cast<MyLib::ColliderCupsule*> (col.get())->m_radius;
		size = dynamic_cast<MyLib::ColliderCupsule*> (col.get())->m_size;
	}

	// 床ポリゴンとの当たり判定処理
	//あたったかどうかのフラグ初期化
	bool IsHitFlag = false;

	// 床ポリゴンとの当たり判定
	//一番高い床ポリゴンにぶつける為の判定用変数を初期化
	Vec3 polyMaxPosY;

	// 床ポリゴンに当たったかどうかのフラグを倒しておく
	m_isHitFlag = false;

	auto capsuleCenterPos = col->rigidbody->GetNextPosVECTOR();
	auto capsuleUnderPos = VGet(capsuleCenterPos.x, capsuleCenterPos.y - size, capsuleCenterPos.z);

	// 床ポリゴンの数だけ繰り返し
	for (int i = 0; i < m_floorNum; i++)
	{
		// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
		m_pPoly = m_pFloorPoly[i];

		auto hit = MV1CollCheck_Sphere(m_stageCollisionHandle, -1, capsuleUnderPos, radius);

		// 当たっていなかったら何もしない
		if (hit.HitNum == 0) continue;

		// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
		if (m_isHitFlag && polyMaxPosY.y > hit.Dim->HitPosition.y) continue;

		polyMaxPosY = hit.Dim->HitPosition;

		// ポリゴンに当たったフラグを立てる
		m_isHitFlag = true;

		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hit);

	}
	if (m_isHitFlag)
	{
		auto undest = Vec3(capsuleCenterPos.x, capsuleCenterPos.y - radius - size, capsuleCenterPos.z);
		auto centerToUndest = undest - capsuleCenterPos;
		auto centerToUndestLegth = centerToUndest.Length();

		auto most = Vec3(capsuleCenterPos.x, polyMaxPosY.y, capsuleCenterPos.z);
		auto centerToMost = most - capsuleCenterPos;
		auto centerToMostLength = centerToMost.Length();

		//if (centerToUndestLegth - centerToMostLength > 1.0f)
		//{
		//	polyMaxPosY.y += centerToUndestLegth - centerToMostLength;
		//}

		// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
		auto set = col->rigidbody->GetNextPos();
		set.y = polyMaxPosY.y + size + radius;
		col->rigidbody->SetNextPos(set);
}
#else
	//床ポリゴンがない場合は何もしない
	if (m_floorNum == 0) return;

	float radius = 0.0f;
	float size = 0.0f;
	for (auto& col : col->m_colliders)
	{
		if (col->collide->GetKind() == ColliderBase::Kind::Cupsule)
		{
			radius = dynamic_cast<MyLib::ColliderCupsule*> (col->collide.get())->m_radius;
			size = dynamic_cast<MyLib::ColliderCupsule*> (col->collide.get())->m_size;
		}
	}

	// 床ポリゴンとの当たり判定処理
	//あたったかどうかのフラグ初期化
	bool IsHitFlag = false;

	// 床ポリゴンとの当たり判定
	//一番高い床ポリゴンにぶつける為の判定用変数を初期化
	float polyMaxPosY = 0.0f;

	// 床ポリゴンに当たったかどうかのフラグを倒しておく
	m_isHitFlag = false;

	auto capsuleCenterPos = col->rigidbody->GetNextPosVECTOR();
	auto capsuleUnderPos = VGet(capsuleCenterPos.x, capsuleCenterPos.y - size - radius, capsuleCenterPos.z);

	// 床ポリゴンの数だけ繰り返し
	for (int i = 0; i < m_floorNum; i++)
	{
		// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
		m_pPoly = m_pFloorPoly[i];

		// 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
		m_lineRes = HitCheck_Line_Triangle(capsuleCenterPos, capsuleUnderPos,
			m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

		// 当たっていなかったら何もしない
		if (m_lineRes.HitFlag == 0)	continue;

		// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
		if (m_isHitFlag && polyMaxPosY > m_lineRes.Position.y) continue;

		polyMaxPosY = m_lineRes.Position.y;

		// ポリゴンに当たったフラグを立てる
		m_isHitFlag = true;

	}
	if (m_isHitFlag)
	{
		// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
		auto set = col->rigidbody->GetNextPos();
		set.y = polyMaxPosY + size + radius;
		col->rigidbody->SetNextPos(set);
	}
#endif
}

