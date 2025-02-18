#include "EffectManager.h"
#include "EffekseerForDXLib.h"

#include "ResourceManager.h"

#include <cassert>

EffectManager* EffectManager::m_instance = nullptr;

/// <summary>
/// デストラクタ
/// </summary>
EffectManager::~EffectManager()
{
	Clear();
}

/// <summary>
/// エフェクトをロード
/// </summary>
void EffectManager::Load(std::string name, std::string path, int endFrame, bool isEternal)
{
	//まずすでに同じパスのエフェクトがロードされていないか確認する
	for (auto& effect : m_effect)
	{
		if (effect.second->effectPath == path)
		{
			return;
		}
	}

	//ここまで来たらエフェクトをロードする
	std::shared_ptr<EffectEmitter> add = std::make_shared<EffectEmitter>();
	add->emitterHandle = LoadEffekseerEffect(path.c_str(), 1.0f);
	assert(add->emitterHandle != -1 && "エフェクトロード失敗");
	add->endFrame = endFrame;
	add->isEternal = isEternal;

	m_effect[name] = add;

}

/// <summary>
/// 更新
/// </summary>
void EffectManager::Update()
{
	UpdateEffekseer3D();

	//エフェクトの更新
	for (auto& effects : m_effect)
	{
		for (auto& ef : effects.second->effects)
		{
			if (ef.frame > effects.second->endFrame)
			{
				StopEffekseer3DEffect(ef.handle);
				ef.isExist = false;
			}

			ef.frame++;
		}

		//isExistがfalseのオブジェクトを削除
		auto it = std::remove_if(effects.second->effects.begin(), effects.second->effects.end(), [](auto& v)
			{
				return v.isExist == false;
			});
		effects.second->effects.erase(it, effects.second->effects.end());
	}
}

/// <summary>
/// 描画
/// </summary>
void EffectManager::Draw()
{
	DrawEffekseer3D();
}

void EffectManager::Clear()
{
	for (auto& emitter : m_effect)
	{
		for (auto& ef : emitter.second->effects)
		{
			StopEffekseer3DEffect(ef.handle);
		}
		//DeleteEffekseerEffect(emitter.second->emitterHandle);
		emitter.second->effects.clear();
	}

	m_effect.clear();
}

/// <summary>
/// エフェクトを生成
/// </summary>
void EffectManager::CreateEffect(std::string name, Vec3 pos, Vec3 rot)
{
	int effectFrame = 300;
	if (name == "E_ARROW")
	{
		effectFrame = 40;
	}

	int handle = -1;
	for (auto& effects : m_effect)
	{
		if (effects.first == name)
		{
			handle = effects.second->emitterHandle;
		}
	}
	if (handle == -1)
	{
		handle = ResourceManager::GetInstance().GetHandle(name);
		{
			//ここまで来たらエフェクトをロードする
			std::shared_ptr<EffectEmitter> add = std::make_shared<EffectEmitter>();
			add->emitterHandle = handle;
			assert(add->emitterHandle != -1 && "エフェクトロード失敗");
			add->endFrame = effectFrame;
			add->isEternal = false;

			m_effect[name] = add;
		}
	}

	//もし追加しようとしているエフェクトの名前のハンドルがなければ終わる
	if (handle == -1)
	{
#ifdef _DEBUG	//デバッグ描画
		assert(0 && "指定したIDのエフェクトは存在しません");
#endif
		return;
	}

	Effect add;
	add.handle = PlayEffekseer3DEffect(handle);
	add.frame = 0;
	add.x = pos.x;
	add.y = pos.y;
	add.z = pos.z;
	add.isExist = true;

	SetPosPlayingEffekseer3DEffect(add.handle, add.x, add.y, add.z);
	SetRotationPlayingEffekseer3DEffect(add.handle, rot.x, rot.y, rot.z);

	m_effect[name]->effects.emplace_back(add);

	return;
}

void EffectManager::StopEffect(std::string name)
{
	for (auto& emitter : m_effect)
	{
		if (emitter.first == name)
		{
			for (auto& ef : emitter.second->effects)
			{
				StopEffekseer3DEffect(ef.handle);
			}
			emitter.second->effects.clear();
		}
	}
}

void EffectManager::AllStopEffect()
{
	for (auto& emitter : m_effect)
	{
		for (auto& ef : emitter.second->effects)
		{
			StopEffekseer3DEffect(ef.handle);
		}

		emitter.second->effects.clear();
	}
}

/// <summary>
/// 指定した名前のすべてのエフェクトの座標を移動させる
/// </summary>
void EffectManager::SetPos(std::string name, Vec3 pos)
{
	std::shared_ptr<EffectEmitter> emit;

	for (auto& effects : m_effect)
	{
		if (effects.first == name)
		{
			for (auto& ef : effects.second->effects)
			{
				ef.x = pos.x;
				ef.y = pos.y;
				ef.z = pos.z;

				SetPosPlayingEffekseer3DEffect(ef.handle, ef.x, ef.y, ef.z);

			}
		}
	}

	return;
}

void EffectManager::SetPos(int handle, Vec3 pos)
{
	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y, pos.z);
}

/// <summary>
/// 指定した名前のすべてのエフェクトを回転させる
/// </summary>
void EffectManager::SetRotation(std::string name, Vec3 rot)
{
	for (auto& effects : m_effect)
	{
		if (effects.first == name)
		{
			for (auto& ef : effects.second->effects)
			{
				SetRotationPlayingEffekseer3DEffect(ef.handle, rot.x, rot.y, rot.z);
			}
		}
	}
}

std::list<int> EffectManager::GetIdHandles(std::string id)
{
	std::list<int> ret;

	for (auto& effects : m_effect)
	{
		if (effects.first == id)
		{
			for (auto& ef : effects.second->effects)
			{
				ret.push_back(ef.handle);
			}
		}
	}

	return ret;
}
