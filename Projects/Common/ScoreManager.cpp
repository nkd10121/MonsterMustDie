#include "ScoreManager.h"
#include "DxLib.h"

#include "LoadCSV.h"

#include <fstream>
#include <sstream>
#include <cassert>

ScoreManager* ScoreManager::m_instance = nullptr;

namespace
{
	const std::string kSaveDataFilename = "data/save/score.dat";
}

ScoreManager::ScoreManager():
	m_clearTime(0),
	m_playerKillNum(0),
	m_trapKillNum(0),
	m_crystalHp(0),
	m_maxComboNum(0),
	m_calculationScore(0)
{
}

ScoreManager::~ScoreManager()
{
}

void ScoreManager::Clear()
{
	m_clearTime = 0;
	m_playerKillNum = 0;
	m_trapKillNum = 0;
	m_crystalHp = 0;
	m_maxComboNum = 0;
	m_calculationScore = 0;
}

int ScoreManager::GetTimeScore()
{
	return (m_targetClearTime - m_clearTime) / 6;
}

int ScoreManager::GetPlayerKillScore()
{
	return m_playerKillNum * 50;
}

int ScoreManager::GetTrapKillScore()
{
	return  m_trapKillNum * 100;
}

int ScoreManager::GetCrystalScore()
{
	return m_crystalHp * 100;
}

int ScoreManager::GetComboScore()
{
	return m_maxComboNum * 100;
}

const void ScoreManager::CalculationScore(std::string stageName)
{
	//タイムの計算
	m_calculationScore = 0;
	m_calculationScore += (m_targetClearTime - m_clearTime) / 6;
	m_calculationScore += m_playerKillNum * 50;
	m_calculationScore += m_trapKillNum * 100;
	m_calculationScore += m_crystalHp * 100;
	m_calculationScore += m_maxComboNum * 100;

	auto it = m_score.find(stageName);
	//もし、ステージ名のスコアが保存されていたら
	if (it != m_score.end())
	{
		//もともとあったスコアと比較して大きかったら上書きする
		m_score[stageName] = max(m_score[stageName], m_calculationScore);
	}
	else
	{
		m_score[stageName] = m_calculationScore;
	}
}

void ScoreManager::Save() const
{
	std::ofstream ofs(kSaveDataFilename, std::ios::binary);
	if (!ofs)
	{
		assert(0 && "ファイルを開けませんでした:%d",kSaveDataFilename);
		return;
	}

	// スコアの数を保存
	size_t size = m_score.size();
	ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));

	// スコアの内容を保存
	for (const auto& pair : m_score)
	{
		size_t keySize = pair.first.size();
		ofs.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
		ofs.write(pair.first.data(), keySize);
		ofs.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
	}
}

void ScoreManager::Load()
{
	std::ifstream ifs(kSaveDataFilename, std::ios::binary);
	if (!ifs)
	{
		// ファイル読み込み失敗 セーブデータを作る
		CreateNewData();
		return;
	}

	// スコアの数を読み込む
	size_t size;
	ifs.read(reinterpret_cast<char*>(&size), sizeof(size));

	// スコアの内容を読み込む
	ClearData();
	for (size_t i = 0; i < size; ++i)
	{
		size_t keySize;
		ifs.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));

		std::string key(keySize, '\0');
		ifs.read(&key[0], keySize);

		int value;
		ifs.read(reinterpret_cast<char*>(&value), sizeof(value));

		m_score[key] = value;
	}
}

void ScoreManager::CreateNewData()
{
	ClearData();

	auto stageNames = LoadCSV::GetInstance().GetAllStageName();

	for (int i = 0; i < stageNames.size(); i++)
	{
		m_score[stageNames[i]] = 0;
	}

	Save();
}

void ScoreManager::ClearData()
{
	m_score.clear();
}
