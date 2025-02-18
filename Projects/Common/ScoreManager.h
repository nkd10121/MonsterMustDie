#pragma once
#include <unordered_map>
#include <string>

class ScoreManager
{
private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ScoreManager();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ScoreManager();

	static ScoreManager* m_instance;	//インスタンス

public:
	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	ScoreManager(const ScoreManager&) = delete;
	ScoreManager& operator=(const ScoreManager&) = delete;
	ScoreManager(ScoreManager&&) = delete;
	ScoreManager& operator= (const ScoreManager&&) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static ScoreManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new ScoreManager;
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
	/// 持っているデータを初期化する
	/// </summary>
	void Clear();

	/// <summary>
	/// 目標クリアタイムを設定する
	/// </summary>
	/// <param name="time">目標クリアタイム</param>
	/// <returns></returns>
	const void SetTargetClearTime(int time) { m_targetClearTime = time; }
	/// <summary>
	/// クリアタイムを設定する
	/// </summary>
	/// <param name="time">クリアタイム</param>
	/// <returns></returns>
	const void SetClearTime(int time) { m_clearTime = time; }

	int GetTimeScore();

	/// <summary>
	/// 敵が何によって倒されたかの値データを設定する
	/// </summary>
	/// <param name="player">プレイヤーによって倒された数</param>
	/// <param name="trap">トラップによって倒された数</param>
	/// <returns></returns>
	const void SetKillData(int player, int trap) { m_playerKillNum = player; m_trapKillNum = trap; }

	int GetPlayerKillScore();
	int GetTrapKillScore();
	/// <summary>
	/// クリスタルの残りHPを設定する
	/// </summary>
	/// <param name="hp">残りHP</param>
	/// <returns></returns>
	const void SetCrystalHp(int hp) { m_crystalHp = hp; }

	int GetCrystalScore();

	/// <summary>
	/// 最大コンボ数を設定する
	/// </summary>
	/// <param name="num">最大コンボ数</param>
	/// <returns></returns>
	const void SetMaxComboNum(int num) { m_maxComboNum = num; }

	int GetComboScore();

	/// <summary>
	/// スコアを計算する
	/// </summary>
	/// <returns></returns>
	const void CalculationScore(std::string stageName);

	/// <summary>
	/// スコアを取得する
	/// </summary>
	/// <returns></returns>
	const int GetScore(std::string stageName)const { return m_score.at(stageName); }

	const int GetCalculationScore()const { return m_calculationScore; }

	/// <summary>
	/// スコアをファイルに保存する
	/// </summary>
	void Save() const;

	/// <summary>
	/// ファイルからスコアを読み込む
	/// </summary>
	void Load();

	void CreateNewData();
	void ClearData();

private:
	int m_targetClearTime;	//目標クリアタイム
	int m_clearTime;		//クリアタイム
	int m_playerKillNum;	//プレイヤーが倒した数
	int m_trapKillNum;		//トラップが倒した数
	int m_crystalHp;		//クリスタルの残りHP
	int m_maxComboNum;		//最大コンボ数

	int m_calculationScore;			//スコア

	std::unordered_map<std::string,int> m_score;
};