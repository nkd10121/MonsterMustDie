#pragma once
#include "Vec3.h"
#include "MyLib.h"

#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <map>
#include <string>

namespace MyLib {

class Collidable;
class ColliderBase;
class Rigidbody;

namespace ColInfo
{
	//最大当たり判定ポリゴン数
	constexpr int kMaxColHitPolyNum = 2000;
	//壁押し出し処理の最大試行回数
	constexpr int kMaxColHitTryNum = 32;
	//壁押し出し時にスライドさせる距離
	constexpr float kColHitSlideLength = 1.0f;
}

/// <summary>
/// 物理・衝突判定するオブジェクトを登録し、物理移動・衝突を通知する
/// </summary>
class Physics final
{
private:
	//当たり判定の種類
	enum class eOnCollideInfoKind
	{
		CollideEnter,		//押し出し処理をするオブジェクトが当たった瞬間
		CollideStay,		//押し出し処理をするオブジェクトが当たっている間
		CollideExit,		//押し出し処理をするオブジェクトが離れた瞬間
		TriggerEnter,		//押し出し処理をしないオブジェクトが当たった瞬間
		TriggerStay,		//押し出し処理をしないオブジェクトが当たっている間
		TriggerExit			//押し出し処理をしないオブジェクトが離れた瞬間
	};

	//なにかと当たったオブジェクトの情報
	struct OnCollideInfoData
	{
		std::weak_ptr<Collidable> own;	//自分自身
		std::weak_ptr<Collidable> send;	//当たった相手

		std::shared_ptr<MyLib::Collidable::Collide> ownCol;
		std::shared_ptr<MyLib::Collidable::Collide> sendCol;
		eOnCollideInfoKind kind;			//種類
	};

	struct SendInfo
	{
		std::weak_ptr<Collidable> own;
		std::weak_ptr<Collidable> send;

		int ownColIndex;
		int sendColIndex;

		std::shared_ptr<MyLib::Collidable::Collide> ownCol;
		std::shared_ptr<MyLib::Collidable::Collide> sendCol;
	};

	//using SendCollideInfo = std::unordered_map<std::shared_ptr<Collidable>, std::list<std::shared_ptr<Collidable>>>;
	using SendCollideInfo = std::list<SendInfo>;

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Physics(/*int normalStageCollisionHandle, int enemyStageCollisionHandle*/);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Physics();

	static Physics* m_instance;	//インスタンス
public:
	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;
	Physics(Physics&&) = delete;
	Physics& operator= (const Physics&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// PhysicsはGetInstance()を通した参照からしか利用できない
	/// </summary>
	/// <returns>インスタンス</returns>
	static Physics& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new Physics;
		}

		return *m_instance;
	}

	/// <summary>
	/// 削除
	/// これをし忘れると普通にメモリリーク
	/// </summary>
	static void Destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

public:
	void SetStageCollisionModel(std::string id, std::string id2);

	/// <summary>
	/// 衝突物の登録
	/// </summary>
	/// <param name="collidable">追加する当たり判定</param>
	void Entry(std::shared_ptr<Collidable> collidable);
	/// <summary>
	/// 衝突物の登録削除
	/// </summary>
	/// <param name="collidable">削除する当たり判定</param>
	void Exit(std::shared_ptr<Collidable> collidable);

	/// <summary>
	/// 更新（登録オブジェクトの物理移動、衝突通知）
	/// </summary>
	void Update();

	/// <summary>
	/// データのクリア
	/// </summary>
	void Clear();

private:
	std::vector<std::shared_ptr<Collidable>> GetCollisionList() const;

	/// <summary>
	/// 当たり判定チェック
	/// </summary>
	void CheckColide();
	/// <summary>
	/// /二つのオブジェクトが接触しているかどうか
	/// </summary>
	/// <param name="rigidA">オブジェクトAの物理データ</param>
	/// <param name="rigidB">オブジェクトBの物理データ</param>
	/// <param name="colliderA">オブジェクトAの当たり判定データ</param>
	/// <param name="colliderB">オブジェクトBの当たり判定データ</param>
	/// <returns>二つのオブジェクトが当たっているかどうか　false : 当たっていない, true : 当たっている</returns>
	bool IsCollide(std::shared_ptr<Rigidbody> rigidA, std::shared_ptr<Rigidbody> rigidB, ColliderBase* colliderA, ColliderBase* colliderB) const;
	/// <summary>
	/// 当たったオブジェクトのペアを登録する
	/// </summary>
	/// <param name="objA">オブジェクトA</param>
	/// <param name="objB">オブジェクトB</param>
	/// <param name="info">登録する配列</param>
	void AddNewCollideInfo(const std::weak_ptr<Collidable>& objA, const std::weak_ptr<Collidable>& objB, int colIndexA, int colIndexB, SendCollideInfo& info);
	/// <summary>
	/// 移動予定の座標を修正する
	/// </summary>
	/// <param name="primaryRigid">優先度高オブジェクトの物理データ</param>
	/// <param name="secondaryRigid">優先度低オブジェクトの物理データ</param>
	/// <param name="primaryCollider">優先度高オブジェクトの当たり判定データ</param>
	/// <param name="secondaryCollider">優先度低オブジェクトの当たり判定データ</param>
	void FixNextPosition(std::shared_ptr<Rigidbody> primaryRigid, std::shared_ptr<Rigidbody> secondaryRigid, ColliderBase* primaryCollider, ColliderBase* secondaryCollider) const;
	/// <summary>
	/// 種類ごとに衝突通知を飛ばす配列に追加する
	/// </summary>
	/// <param name="preSendInfo">衝突したオブジェクトのリストのログ</param>
	/// <param name="newSendInfo">衝突したオブジェクトのリスト</param>
	/// <param name="isTrigger">無視するかどうか</param>
	void CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger);
	/// <summary>
	/// 衝突通知を飛ばす配列に追加する
	/// </summary>
	/// <param name="own">自身</param>
	/// <param name="send">衝突した相手</param>
	/// <param name="kind">当たり判定の種類</param>
	void AddOnCollideInfo(const SendInfo& info, eOnCollideInfoKind kind);
	/// <summary>
	/// 最終的な位置を決定する
	/// </summary>
	void FixPosition();

private:
	/// <summary>
	/// チェックしたポリゴンが壁ポリゴンか床ポリゴンかを判断し保存する
	/// </summary>
	/// <param name="col">チェックするオブジェクト</param>
	void CheckWallAndFloor(std::shared_ptr<Collidable>& col);
	/// <summary>
	/// 壁ポリゴンとの当たり判定をチェックし、移動させる
	/// </summary>
	/// <param name="col">チェックするオブジェクト</param>
	void FixPositionWithWall(std::shared_ptr<Collidable>& col);
	/// <summary>
	/// 壁の中から押し出す
	/// </summary>
	/// <param name="col">チェックするオブジェクト</param>
	void FixPositionWithWallInternal(std::shared_ptr<Collidable>& col);
	/// <summary>
	/// 床ポリゴンとの当たり判定をチェックし、移動させる
	/// </summary>
	/// <param name="col">チェックするオブジェクト</param>
	void FixNowPositionWithFloor(std::shared_ptr<Collidable>& col);

private:
	std::vector<std::shared_ptr<Collidable>> m_collidables;	//登録されたCollidableのリスト
	std::list<OnCollideInfoData> m_onCollideInfo;			//衝突したオブジェクトのリスト

	SendCollideInfo m_newCollideInfo;	//衝突したオブジェクトのリスト
	SendCollideInfo m_preCollideInfo;	//衝突したオブジェクトのリストのログ
	SendCollideInfo m_newTirrigerInfo;	//衝突した押し出ししないオブジェクトのリスト
	SendCollideInfo m_preTirrigerInfo;	//衝突した押し出ししないオブジェクトのリストのログ

	/*ステージハンドル*/
	int m_stageCollisionHandle;	//通常のマップ当たり判定のハンドル
	int m_enemyCollisionHandle;	//敵のマップ当たり判定のハンドル

private:
	bool m_isMoveFlag;	//移動したかどうか
	bool m_isHitFlag;	//ポリゴンに当たったかどうか
	int m_wallNum;		//壁ポリゴンと判断されたポリゴン数
	int m_floorNum;		//床ポリゴンと判断されたポリゴン数

	
	MV1_COLL_RESULT_POLY_DIM m_hitDim{};
	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pWallPoly[ColInfo::kMaxColHitPolyNum]{};
	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pFloorPoly[ColInfo::kMaxColHitPolyNum]{};
	// ポリゴンの構造体にアクセスするために使用するポインタ
	MV1_COLL_RESULT_POLY* m_pPoly = nullptr;
	// 線分とポリゴンとの当たり判定の結果を代入する構造体
	HITRESULT_LINE m_lineRes{};

	/*デバッグ用*/
	int m_checkCount = 0;
};

}
