#pragma once
#include "Vec3.h"
#include <memory>
#include <list>
#include <map>

#include "TrapBase.h"

class TrapManager
{
public:
	struct Trap
	{
		//std::shared_ptr<TrapBase> trap;
		Vec3 pos;
		Vec3 norm;			//法線ベクトル
		bool isPlaced;
		std::list<std::weak_ptr<Trap>> neighborTraps;
	};

	struct TrapInfo
	{
		int kind;				//設置種類
		std::string trapName;	//名前
		int modelHandle;		//モデルID
		int imageHandle;		//画像ID
		int cost;				//設置コスト
	};
private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TrapManager();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~TrapManager();

	static TrapManager* m_instance;	//インスタンス

public:
	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	TrapManager(const TrapManager&) = delete;
	TrapManager& operator=(const TrapManager&) = delete;
	TrapManager(TrapManager&&) = delete;
	TrapManager& operator= (const TrapManager&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static TrapManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new TrapManager;
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

private:
	const bool CheckNeighbor(std::list<std::weak_ptr<Trap>> check)const;
public:
	void Update();
	void Draw();

	void PreviewDraw();

	void Load(const char* stageName);

	void SetUp(int point);

	void Clear();

	/// <summary>
	/// トラップポイントを追加する
	/// </summary>
	/// <param name="addPoint"></param>
	void AddTrapPoint(int addPoint);


	const void SetCameraInfo(Vec3 cameraPos, Vec3 dirVec);
	const void SetSlotIdx(int idx);
	const void SetIsPrePhase(bool isPrePhase) { m_isPrePhase = isPrePhase; }
private:
	std::list<std::shared_ptr<Trap>> m_trapPoss;

	std::list<std::shared_ptr<TrapBase>> m_traps;
	std::vector<std::string> m_trapNames;

	std::vector<std::shared_ptr<TrapBase>> m_previewTraps;


	//罠を点滅させるためのアングル
	float m_angle;
	//罠の透明度
	float m_transparency;

	int m_slotIdx;		//プレイヤーのスロット番号
	int m_preSlotIdx;	//プレイヤーのスロット番号
	Vec3 m_cameraPos;	//カメラ座標
	Vec3 m_cameraDir;	//カメラの向いている方向

	int m_trapPoint;	//罠ポイント
	int m_targetTrapPoint;	//増減後罠ポイント

	int m_slotBgHandle;

	int m_bgHandle;
	int m_iconHandle;

	bool m_isTextShake;
	int m_textShakeFrame;

	bool m_isPrePhase;

	float m_trapRotationAngle;

	int m_attackEffectCreateCount;
	std::string m_createEffectName;

	//デバッグ用
	std::shared_ptr<Trap> debugTrap;
};
