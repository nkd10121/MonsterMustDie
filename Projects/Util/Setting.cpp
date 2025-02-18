#include "Setting.h"
#include "DxLib.h"
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
}

Setting::~Setting()
{
}

void Setting::Load()
{
	// データのクリア
	ClearData();

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

void Setting::Save()
{
	std::ofstream ofs(kSaveDataFilename, std::ios_base::binary);
	ofs.write((char*)&m_data, sizeof(Data));
}

void Setting::ClearData()
{
	m_data.masterVolume = 0.5f;
	m_data.bgmVolume = 0.5f;
	m_data.seVolume = 0.5f;
	m_data.sensitivity = 0.5f;
	m_data.isFullScreen = true;
	m_data.isDrawOperation = true;
}

void Setting::CreateNewData()
{
	ClearData();

	// セーブデータ保存用のフォルダがない場合生成する
	if (!std::filesystem::is_directory("data/save"))
	{
		std::filesystem::create_directory("data/save");
	}

	std::ofstream ofs(kSaveDataFilename, std::ios_base::binary);
	ofs.write((char*)&m_data, sizeof(Data));
}
