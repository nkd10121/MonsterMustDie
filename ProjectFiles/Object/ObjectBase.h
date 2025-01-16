#pragma once
#include "DxLib.h"
#include "MyLib.h"
#include "Vec3.h"
#include <memory>

/// <summary>
/// オブジェクトの基底クラス
/// </summary>
class ObjectBase : public MyLib::Collidable
{
public:
	/// <summary>
	///	コンストラクタ
	/// </summary>
	/// <param name="priority">優先度</param>
	/// <param name="tag">タグ</param>
	ObjectBase(Priority priority, GameObjectTag tag);
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ObjectBase();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() {};

	virtual void Finalize();
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() {};
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() {};

	/// <summary>
	/// 存在しているかどうかを取得
	/// </summary>
	/// <returns></returns>
	const bool GetIsExist()const { return m_isExist; }

	/// <summary>
	/// モデルのロードを申請する
	/// </summary>
	virtual void LoadModel() {};

	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="pos">設定座標</param>
	virtual void SetPosition(Vec3 pos);

protected:
	int m_modelHandle;	//モデルハンドル

	bool m_isExist;	//存在フラグ
};