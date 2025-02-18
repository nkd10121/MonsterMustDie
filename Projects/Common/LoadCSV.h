#pragma once
#include "CharacterBase.h"
#include "TrapBase.h"

class LoadCSV
{
//public:
//	/// <summary>
//	/// リソースのデータ
//	/// </summary>
//	struct ResourceData
//	{
//		std::string id;			//リソースID
//		std::string path;		//パス
//		std::string extension;	//拡張子
//		bool isEternal;			//常駐フラグ
//		bool isBGM;				//サウンドデータの場合格納するデータ
//	};

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	LoadCSV() {};
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~LoadCSV() {};

	static LoadCSV* m_instance;	//インスタンス
public:
	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	LoadCSV(const LoadCSV&) = delete;
	LoadCSV& operator=(const LoadCSV&) = delete;
	LoadCSV(LoadCSV&&) = delete;
	LoadCSV& operator= (const LoadCSV&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns></returns>
	static LoadCSV& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new LoadCSV;
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
	/// <summary>
	/// ステータス情報を取得
	/// </summary>
	/// <param name="characterName">ステータスを取得したいキャラクター名</param>
	CharacterBase::Status LoadStatus(const char* characterName);

	/// <summary>
	/// そのシーンでロードするリソース情報を取得
	/// </summary>
	/// <param name="stageId">ステージID</param>
	/// <returns>そのシーンでロードするリソース情報</returns>
	std::list<std::vector<std::string>>GetLoadResourcePath(std::string stageId);

	/// <summary>
	/// 指定したキャラクターIDの指定したアニメーションの名前を取得する
	/// </summary>
	/// <param name="characterID">キャラクターID</param>
	/// <param name="animID">アニメーションID</param>
	/// <returns>アニメーション番号</returns>
	int GetAnimIdx(std::string characterID,std::string animID);

	/// <summary>
	/// クロスボウのロケーション情報を取得
	/// </summary>
	/// <param name="idx"></param>
	/// <param name="pos"></param>
	/// <param name="rot"></param>
	void GetCrossbowLocationData(int idx, Vec3& pos, Vec3& rot);

	/// <summary>
	/// トラップのステータス情報を取得
	/// </summary>
	/// <param name="trapName">取得したいトラップ名</param>
	/// <returns></returns>
	TrapBase::TrapInfo LoadTrapStatus(const char* trapName);

	std::vector<std::string> GetAllTrapName();

	std::vector<std::string> LoadStageInfo(int stageIdx);
	std::vector<std::string> GetAllStageName();
	std::vector<std::string> GetAllEnemyName();

	std::vector<std::vector<std::string>> LoadEnemyCreateData(std::string stageName);
};