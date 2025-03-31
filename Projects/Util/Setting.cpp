#include "Setting.h"
#include <cassert>
#include <fstream>
#include <string>
#include <filesystem>

Setting* Setting::m_instance = nullptr;

namespace
{
	struct Header
	{
		char id[4] = "inf";		//最後に\nが入っているためこれで4文字だと思っていい(4バイト)
		float version = 1.0f;	//4バイト
		size_t dataCount = 0;	//4バイト
		//空白の4バイトが入っている(パディング)
	};

	// セーブデータファイル名
	const char* const kSaveDataFilename = "data/save/config.dat";

	//デフォルト値
	constexpr float kDefaultValue = 0.5f;
}

/// <summary>
/// オプションデータを読み込む
/// </summary>
void Setting::Load()
{
	// データのクリア
	ResetData();

	std::ifstream ifs;
	//セーブデータのファイルを開く
	ifs.open(kSaveDataFilename, std::ios_base::binary);
	//開くのに失敗したら
	if (ifs.fail())
	{
		//セーブデータを作る
		CreateNewData();
		return;
	}
	else
	{
		//開くのに成功していたらデータを読み込んで取得する
		Data data;
		ifs.read((char*)&data, sizeof(Data));
		ifs.close();

		// 読み込んだデータを実際に使用するデータにコピー
		m_data = data;
	}
}

/// <summary>
/// オプションデータを保存する
/// </summary>
void Setting::Save()
{
	// バイナリモードでファイルを開く
	std::ofstream ofs(kSaveDataFilename, std::ios_base::binary);
	// データを書き込む
	ofs.write((char*)&m_data, sizeof(Data));

	// ファイルを閉じる
	ofs.close();
}

/// <summary>
/// オプションデータをリセットする
/// </summary>
void Setting::ResetData()
{
	//全部初期設定にする
	m_data.masterVolume = kDefaultValue;
	m_data.bgmVolume = kDefaultValue;
	m_data.seVolume = kDefaultValue;
	m_data.sensitivity = kDefaultValue;
	m_data.isFullScreen = true;
	m_data.isDrawOperation = true;
}

/// <summary>
/// 新しいオプションデータを生成する
/// </summary>
void Setting::CreateNewData()
{
	//念のためデータをリセットしておく
	ResetData();

	// セーブデータ保存用のフォルダがない場合生成する
	if (!std::filesystem::is_directory("data/save"))
	{
		std::filesystem::create_directory("data/save");
	}

	// バイナリモードでファイルを開く
	std::ofstream ofs(kSaveDataFilename, std::ios_base::binary);
	// データを書き込む
	ofs.write((char*)&m_data, sizeof(Data));
}
