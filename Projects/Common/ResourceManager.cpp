#include "ResourceManager.h"
#include "DxLib.h"
#include "EffekseerForDXLib.h"

#include "LoadCSV.h"

#include <cassert>

ResourceManager* ResourceManager::m_instance = nullptr;

namespace
{
	enum Order : int
	{
		StageId,
		Id,
		Path,
		Extension,
		EternalFlag,
		BgmFlag
	};
}


ResourceManager::~ResourceManager()
{
	for (auto& resource : m_resources)
	{
		if (resource->kind == Kind::Image)
		{
			DeleteGraph(resource->handle);
		}
		else if (resource->kind == Kind::Sound)
		{
			DeleteSoundMem(resource->handle);
		}
		else if (resource->kind == Kind::Model)
		{
			MV1DeleteModel(resource->handle);
		}
		else if (resource->kind == Kind::Effect)
		{
			//エフェクトはDelete処理なし
		}
		else if (resource->kind == Kind::Shader)
		{
			//シェーダーもDelete処理なし
		}
	}

	m_resources.clear();
}

void ResourceManager::Load(std::string sceneName)
{
	auto loadData = LoadCSV::GetInstance().GetLoadResourcePath(sceneName);

	//[0]:ステージID
	//[1]:識別ID
	//[2]:パス
	//[3]:拡張子
	//[4]:常駐フラグ(0:F,1:T)
	//[5]:BGMかどうか

	for (auto& data : loadData)
	{
		//すでにロードされていたら何もしない
		for (auto& resource : m_resources)
		{
			if (resource->id == data[Order::Id])
			{
#ifdef _DEBUG	//デバッグ描画
				//assert(0 && "このIDはすでにロードされています");
#endif
				continue;
			}
		}

		//この処理に来た
		//	→ロードする
		LoadResource(data);

	}
	return;
}

int ResourceManager::GetHandle(std::string id)
{
	//ロードされていたらハンドルを返す
	for (auto& resource : m_resources)
	{
		if (resource->id == id)
		{
			if (resource->kind == Kind::Model)
			{
				return MV1DuplicateModel(resource->handle);
			}

			return resource->handle;
		}
	}

	//ここまで来たということはロードされていなかった
#ifdef _DEBUG
	//念のためassertを仕込んでおく
	assert(0 && "指定したIDはロードされていません");
#endif
	return -1;
}

void ResourceManager::Clear(std::string sceneName)
{
	for (auto& resource : m_resources)
	{
		if (resource->sceneName != sceneName)	continue;

		if (!resource->isEternal)
		{
			if (resource->kind == Kind::Image)
			{
				DeleteGraph(resource->handle);
			}
			else if (resource->kind == Kind::Sound)
			{
				DeleteSoundMem(resource->handle);
			}
			else if (resource->kind == Kind::Model)
			{
				MV1DeleteModel(resource->handle);
			}
			else if (resource->kind == Kind::Effect)
			{
				//エフェクトはDelete処理なし
			}
			else if (resource->kind == Kind::Shader)
			{
				//シェーダーもDelete処理なし
			}
		}
	}

	//不要になったハンドルをここで削除処理する
	auto it = remove_if(m_resources.begin(), m_resources.end(), [](auto& v) {
		return v->isEternal == false;
		});
	m_resources.erase(it, m_resources.end());
}

void ResourceManager::AllClear()
{
	for (auto& resource : m_resources)
	{
		if (resource->kind == Kind::Image)
		{
			DeleteGraph(resource->handle);
		}
		else if (resource->kind == Kind::Sound)
		{
			DeleteSoundMem(resource->handle);
		}
		else if (resource->kind == Kind::Model)
		{
			MV1DeleteModel(resource->handle);
		}
		else if (resource->kind == Kind::Effect)
		{
			//エフェクトはDelete処理なし
		}
		else if (resource->kind == Kind::Shader)
		{
			//シェーダーもDelete処理なし
		}
	}

	m_resources.clear();
}

bool ResourceManager::IsLoaded()
{
	for (auto& resource : m_resources)
	{
		if (resource->kind == Kind::Effect) continue;

		if (CheckHandleASyncLoad(resource->handle))	return false;
	}

	return true;
}

const void ResourceManager::LoadResource(const std::vector<std::string>& data)
{
	std::shared_ptr<Resource> add;

	//画像
	if (data[Order::Extension] == ".png")
	{
		add = std::make_shared<Resource>();
		add->kind = Kind::Image;
		add->handle = LoadGraph((data[Order::Path] + data[Order::Extension]).c_str());
	}
	//音
	else if (data[Order::Extension] == ".mp3")
	{
		auto temp = std::make_shared<Sound>();
		temp->kind = Kind::Sound;
		temp->handle = LoadSoundMem((data[Order::Path] + data[Order::Extension]).c_str());
		temp->isBgm = stoi(data[Order::BgmFlag]);
		add = temp;
	}
	//モデル
	else if (data[Order::Extension] == ".mv1")
	{
		add = std::make_shared<Resource>();
		add->kind = Kind::Model;
		add->handle = MV1LoadModel((data[Order::Path] + data[Order::Extension]).c_str());
	}
	//エフェクト
	else if (data[Order::Extension] == ".efk")
	{
		// デフォルトに戻す
		SetUseASyncLoadFlag(false);

		add = std::make_shared<Resource>();
		add->kind = Kind::Effect;
		add->handle = LoadEffekseerEffect((data[Order::Path] + data[Order::Extension]).c_str());

		// 非同期読み込みを開始する
		SetUseASyncLoadFlag(true);
	}
	//シェーダー
	else if (data[Order::Extension] == ".pso" || data[Order::Extension] == ".vso")
	{
		add = std::make_shared<Resource>();
		add->kind = Kind::Shader;

		if (data[Order::Extension] == ".vso")
		{
			add->handle = LoadVertexShader((data[Order::Path] + data[Order::Extension]).c_str());
		}
		else
		{
			add->handle = LoadPixelShader((data[Order::Path] + data[Order::Extension]).c_str());
		}
	}

	add->sceneName = data[Order::StageId];
	add->id = data[Order::Id];
	add->isEternal = stoi(data[Order::EternalFlag]);

	m_resources.push_back(add);

	if (add->handle == -1)
	{
#ifdef _DEBUG
		//ロードに失敗していたらエラーを吐くようにする
		assert(0 && "のロードに失敗しました");
#endif
		return;
	}
	else
	{
		return;
	}
}
