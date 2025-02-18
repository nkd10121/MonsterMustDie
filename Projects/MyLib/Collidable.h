#pragma once
#include "GameObjectTag.h"
#include <List>
#include <memory>

namespace MyLib
{
	class Rigidbody;
	class Physics;
	class ColliderBase;

	/// <summary>
	/// 当たり判定クラス
	/// </summary>
	class Collidable : public std::enable_shared_from_this<Collidable>
	{
		friend Physics;
	public:
		struct Collide
		{
			std::shared_ptr<ColliderBase> collide;
			ColliderBase::CollisionTag collideTag;

			//削除する予定かどうか
			bool isDelete = false;

			bool operator==(const Collide& other) const {
				return this->collide == other.collide && this->collideTag == other.collideTag;
			}
		};

		// 位置補正の優先度の判別に使う
		enum class Priority : int
		{
			Low,		// 低
			Middle,		// 中
			High,		// 高
			Static,		// 動かない（最高）
		};

	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="priority">優先度</param>
		/// <param name="tag">タグ</param>
		Collidable(Priority priority, GameObjectTag tag);
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~Collidable();

		//当たり判定関数
		virtual void OnCollideEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send,const std::shared_ptr<Collide>& sendCol) {}
		virtual void OnCollideStay(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol) {}
		virtual void OnCollideExit(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol) {}
		virtual void OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol) {}
		virtual void OnTriggerStay(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol) {}
		virtual void OnTriggerExit(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol) {}

		/// <summary>
		/// タグを取得
		/// </summary>
		/// <returns>タグ</returns>
		GameObjectTag GetTag() const { return tag; }
		/// <summary>
		/// 優先度を取得
		/// </summary>
		/// <returns>優先度</returns>
		Priority GetPriority() const { return priority; }

		//当たり判定を無視（スルー）するタグの追加/削除
		void AddThroughTag(GameObjectTag tag);
		void RemoveThroughTag(GameObjectTag tag);

		//当たり判定を無視（スルー）する対象かどうか
		bool IsThroughTarget(const std::shared_ptr<Collidable> target) const;

	protected:
		/// <summary>
		/// Physicsに登録する
		/// </summary>
		void OnEntryPhysics();
		/// <summary>
		/// Physicsから削除する
		/// </summary>
		void OnExistPhysics();
		/// <summary>
		/// 当たり判定を追加
		/// </summary>
		/// <param name="kind">当たり判定の種類</param>
		/// <param name="isTrigger">押し出し判定をしないかどうか</param>
		/// <returns>自身</returns>
		std::shared_ptr<ColliderBase> AddCollider(const ColliderBase::Kind& kind, bool isTrigger,ColliderBase::CollisionTag collisionTag = ColliderBase::CollisionTag::Normal);

		/// <summary>
		/// 指定した当たり判定の削除申請を出す
		/// </summary>
		/// <param name="col"></param>
		void DeleteRequestCollider(std::shared_ptr<MyLib::Collidable::Collide> col);

		/// <summary>
		/// 削除申請が出されている当たり判定を削除する
		/// </summary>
		void DeleteCollider();

	public:
		const std::shared_ptr<Collide> GetCollider(ColliderBase::CollisionTag collisionTag)const;

	protected:
		// 物理データ
		std::shared_ptr<Rigidbody> rigidbody;
		//当たり判定情報
		std::vector<std::shared_ptr<Collide>> m_colliders;



	private:
		//タグ
		GameObjectTag tag;
		//優先度
		Priority priority;

		// 当たり判定を無視（スルー）するタグのリスト
		std::list<GameObjectTag> throughTags;
	};
}