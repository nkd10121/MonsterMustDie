#include "LoadCSV.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>

LoadCSV* LoadCSV::m_instance = nullptr;

namespace
{
	/// <summary>
	/// csvデータの,で文字列を分割する
	/// </summary>
	/// <param name="input">取得した情報文字列</param>.
	/// 
	/// <param name="delimiter">区切る文字</param>
	/// <returns>区切る文字で取得した情報文字列を区切った文字列配列</returns>
	std::vector<std::string> Split(std::string& input, char delimiter)
	{
		std::istringstream stream(input);
		std::string field;
		std::vector<std::string> result;
		while (getline(stream, field, delimiter)) {
			result.push_back(field);
		}
		return result;
	}

	namespace LoadData
	{
		//ステータス情報の並び列挙型
		enum eStatusOrder : int
		{
			name,	//キャラクター名
			hp,		//体力
			atk,	//攻撃力
			def,	//防御力
			speed,	//移動速度
			point	//ドロップする罠ポイント
		};
	}

	//ステータス情報が入ったcsvのパス
	const char* kStatuCSVPath = "data/csv/Status.csv";
	//シーンごとに必要なリソース情報をまとめたcsvのパス
	const char* kLoadResourseCSVPath = "data/csv/LoadPath.csv";
	//アニメーションの名前と番号の対応表csvのパス
	const char* kAnimCSVPath = "data/csv/AnimIdx.csv";
	//クロスボウのロケーション情報csvのパス
	const char* kCrossbowLocationCSVPath = "data/csv/CrossbowLocationInfo.csv";
}

/// <summary>
/// ステータス情報を取得
/// </summary>
CharacterBase::Status LoadCSV::LoadStatus(const char* characterName)
{
	//返す情報
	CharacterBase::Status retStatus;

	// 一時保存用string
	std::string strBuf;
	// カンマ分け一時保存用string
	std::vector<std::string> strConmaBuf;

	// ファイル読み込み
	std::ifstream ifs(kStatuCSVPath);
	if (!ifs)
	{
		assert(0 && "ファイルにアクセスできませんでした。");
		return CharacterBase::Status();
	}

	//情報を取得できたかどうかのフラグ
	bool isGet = false;

	//最初は対応表情報が入っているだけなので無視する
	std::getline(ifs, strBuf);

	while (getline(ifs, strBuf))
	{
		//取得した文字列をカンマ区切りの配列(情報群)にする
		strConmaBuf = Split(strBuf, ',');

		//[0]:キャラクター名
		//[1]:体力
		//[2]:攻撃力
		//[3]:防御力
		//[4]:移動速度
		//[5]:死亡時のポイント

		//指定したキャラクター名と一致するデータがあれば情報を取得する
		if (strConmaBuf[LoadData::eStatusOrder::name] == characterName)
		{
			isGet = true;
			retStatus.hp = std::stoi(strConmaBuf[LoadData::eStatusOrder::hp]);
			retStatus.atk = std::stoi(strConmaBuf[LoadData::eStatusOrder::atk]);
			retStatus.def = std::stoi(strConmaBuf[LoadData::eStatusOrder::def]);
			retStatus.speed = std::stof(strConmaBuf[LoadData::eStatusOrder::speed]);
			retStatus.point = std::stoi(strConmaBuf[LoadData::eStatusOrder::point]);
		}

		if (isGet)
		{
			break;
		}
	}

#ifdef _DEBUG	//デバッグ描画
	//情報を取得できなかった時、エラーを吐くようにする
	//取得できなかった理由想定
	//①キャラクター名がスペルミスや大文字小文字の違いで異なる
	//②csvに情報を追加し忘れている
	if (!isGet)
	{
		assert(0 && "指定したキャラクター名の情報を取得できませんでした");
	}
#endif
	return retStatus;
}

/// <summary>
/// そのシーンでロードするリソース情報を取得
/// </summary>
std::list<std::vector<std::string>> LoadCSV::GetLoadResourcePath(std::string stageId)
{
	//戻り値
	std::list<std::vector<std::string>> ret;

	// 一時保存用string
	std::string strBuf;
	// カンマ分け一時保存用string
	std::vector<std::string> strConmaBuf;

	// ファイル読み込み
	std::ifstream ifs(kLoadResourseCSVPath);
	if (!ifs)
	{
		assert(0 && "ファイルにアクセスできませんでした。");
		return ret;
	}

	//最初は対応表情報が入っているだけなので無視する
	std::getline(ifs, strBuf);

	while (getline(ifs, strBuf))
	{
		//取得した文字列をカンマ区切りの配列(情報群)にする
		strConmaBuf = Split(strBuf, ',');

		//[0] ステージID
		//[1] 識別ID
		//[2] パス
		//[3] 拡張子
		//[4] 常駐フラグ
		//[5] BGMフラグ(soundデータの場合のみ)

		//指定したステージIDと一致していたら
		if (stageId == strConmaBuf[0])
		{
			std::vector<std::string> add;

			for (auto& buf : strConmaBuf)
			{
				add.push_back(buf);
			}

			ret.emplace_back(add);
		}
	}

	return ret;
}

int LoadCSV::GetAnimIdx(std::string characterID, std::string animID)
{
	int ret = -1;

	// 一時保存用string
	std::string strBuf;
	// カンマ分け一時保存用string
	std::vector<std::string> strConmaBuf;

	// ファイル読み込み
	std::ifstream ifs(kAnimCSVPath);
	if (!ifs)
	{
		assert(0 && "ファイルにアクセスできませんでした。");
		return ret;
	}

	//最初は対応表情報が入っているだけなので無視する
	std::getline(ifs, strBuf);
	auto buf = Split(strBuf, ',');

	int index = -1;

	for (int i = 1;i < buf.size();i++)
	{
		//もしすでにアニメーション番号を見つけていたらfor文を抜ける
		if (index != -1) break;

		if (buf[i] == animID)
		{
			index = i;
		}
	}

	if (index == -1)
	{
		assert(index == -1 && "アニメーションIDが見つかりませんでした");
		return ret;
	}


	while (getline(ifs, strBuf))
	{
		//取得した文字列をカンマ区切りの配列(情報群)にする
		strConmaBuf = Split(strBuf, ',');

		//指定したステージIDと一致していたら
		if (characterID == strConmaBuf[0])
		{
			ret = stoi(strConmaBuf[index]);
		}
	}

	return ret;
}

void LoadCSV::GetCrossbowLocationData(int idx, Vec3& pos, Vec3& rot)
{
	// 一時保存用string
	std::string strBuf;
	// カンマ分け一時保存用string
	std::vector<std::string> strConmaBuf;

	// ファイル読み込み
	std::ifstream ifs(kCrossbowLocationCSVPath);
	if (!ifs)
	{
		assert(0 && "ファイルにアクセスできませんでした。");
		return;
	}

	//最初は対応表情報が入っているだけなので無視する
	std::getline(ifs, strBuf);
	int index = 0;


	while (getline(ifs, strBuf))
	{
		//取得した文字列をカンマ区切りの配列(情報群)にする
		strConmaBuf = Split(strBuf, ',');


		if (idx == index)
		{
			pos.x = stof(strConmaBuf[1]);
			pos.y = stof(strConmaBuf[2]);
			pos.z = stof(strConmaBuf[3]);

			rot.x = stof(strConmaBuf[4]);
			rot.y = stof(strConmaBuf[5]);
			rot.z = stof(strConmaBuf[6]);
			return;
		}

		index++;
	}

#ifdef _DEBUG	//デバッグ描画
	assert(0 && "クロスボウのロケーション情報の取得に失敗しました。");
#endif

	return;
}

TrapBase::Status LoadCSV::LoadTrapStatus(const char* trapName)
{
	TrapBase::Status ret;

	// 一時保存用string
	std::string strBuf;
	// カンマ分け一時保存用string
	std::vector<std::string> strConmaBuf;

	// ファイル読み込み
	std::ifstream ifs("data/csv/trapStatus.csv");
	if (!ifs)
	{
		assert(false);
		return ret;
	}

	//情報を取得できたかどうかのフラグ
	bool isGet = false;

	//最初は対応表情報が入っているだけなので無視する
	std::getline(ifs, strBuf);

	while (getline(ifs, strBuf))
	{
		//取得した文字列をカンマ区切りの配列(情報群)にする
		strConmaBuf = Split(strBuf, ',');

		//[0]:キャラクター名
		//[1]:攻撃力
		//[2]:索敵範囲
		//[3]:攻撃範囲
		//[4]:クールタイム
		//[5]:設置コスト

		//指定したキャラクター名と一致するデータがあれば情報を取得する
		if (strConmaBuf[0] == trapName)
		{
			isGet = true;

			ret.atk = std::stoi(strConmaBuf[1]);
			ret.searchRange = std::stof(strConmaBuf[2]);
			ret.atkRange = std::stof(strConmaBuf[3]);
			ret.coolTime = std::stoi(strConmaBuf[4]);
			ret.cost = std::stoi(strConmaBuf[5]);
			ret.kind = static_cast<TrapBase::Kind>(std::stoi(strConmaBuf[6]));
		}
	}

#ifdef _DEBUG
	//情報を取得できなかった時、エラーを吐くようにする
	if (!isGet)
	{
		assert(0 && "指定した罠のステータス情報を取得できませんでした");
	}
#endif

	return ret;
}

std::vector<std::string> LoadCSV::LoadStageInfo(int stageIdx)
{
	std::vector<std::string> ret;

	// 一時保存用string
	std::string strBuf;
	// カンマ分け一時保存用string
	std::vector<std::string> strConmaBuf;

	// ファイル読み込み
	std::ifstream ifs("data/csv/StageData.csv");
	if (!ifs)
	{
		assert(false);
		return ret;
	}

	//情報を取得できたかどうかのフラグ
	bool isGet = false;

	//最初は対応表情報が入っているだけなので無視する
	std::getline(ifs, strBuf);

	while (getline(ifs, strBuf))
	{
		//もしすでに情報を取得していたら終わる
		if (isGet)
		{
			break;
		}

		//取得した文字列をカンマ区切りの配列(情報群)にする
		strConmaBuf = Split(strBuf, ',');

		//指定したキャラクター名と一致するデータがあれば情報を取得する
		if (std::stoi(strConmaBuf[0]) == stageIdx)
		{
			isGet = true;
			for (int i = 1; i < strConmaBuf.size(); i++)
			{
				ret.push_back(strConmaBuf[i]);
			}
		}
	}

#ifdef _DEBUG
	//情報を取得できなかった時、エラーを吐くようにする
	if (!isGet)
	{
		assert(0 && "指定した情報を取得できませんでした");
	}
#endif

	return ret;
}

std::vector<std::string> LoadCSV::GetAllStageName()
{
	std::vector<std::string> ret;

	// 一時保存用string
	std::string strBuf;
	// カンマ分け一時保存用string
	std::vector<std::string> strConmaBuf;

	// ファイル読み込み
	std::ifstream ifs("data/csv/StageData.csv");
	if (!ifs)
	{
		assert(false);
		return ret;
	}

	//最初は対応表情報が入っているだけなので無視する
	std::getline(ifs, strBuf);

	while (getline(ifs, strBuf))
	{
		//取得した文字列をカンマ区切りの配列(情報群)にする
		strConmaBuf = Split(strBuf, ',');

		//ステージ名がはいっているインデックスは1
		ret.push_back(strConmaBuf[1]);

	}

	return ret;
}

std::vector<std::vector<std::string>> LoadCSV::LoadEnemyCreateData(std::string stageName)
{
	std::vector<std::vector<std::string>> ret;

	// 一時保存用string
	std::string strBuf;
	// カンマ分け一時保存用string
	std::vector<std::string> strConmaBuf;

	// ファイル読み込み
	std::ifstream ifs("data/csv/" + stageName + ".csv");
	if (!ifs)
	{
		assert(false);
		return ret;
	}

	//最初は対応表情報が入っているだけなので無視する
	std::getline(ifs, strBuf);

	while (getline(ifs, strBuf))
	{
		//取得した文字列をカンマ区切りの配列(情報群)にする
		strConmaBuf = Split(strBuf, ',');

		//[0]:出現フェーズ
		//[1]:キャラクター名

		std::vector<std::string> add;
		for (auto& buf : strConmaBuf)
		{
			add.push_back(buf);
		}

		ret.push_back(add);
	}

	return ret;
}
