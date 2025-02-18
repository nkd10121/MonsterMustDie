#include "TrapManager.h"
#include "MathHelp.h"

#include "SpikeTrap.h"
#include "ArrowWallTrap.h"
#include "FlameTrap.h"
#include "CutterTrap.h"
#include "IronSnareTrap.h"
#include "IronImpactTrap.h"

#include "Input.h"
#include "ResourceManager.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "FontManager.h"
#include "DrawUI.h"
#include "LoadCSV.h"
#include "Setting.h"

#include "Vec2.h"
#include <unordered_map>
#include <algorithm>

TrapManager* TrapManager::m_instance = nullptr;

namespace
{
	const std::string kStageDataPathFront = "data/stageData/";
	const std::string kStageDataPathBack = ".tLoc";

	//テキストを揺らすフレーム数
	constexpr int kTextShakeFrame = 30;

	//罠ポイントの背景の描画座標
	const Vec2 kTrapPointBgDrawPos = Vec2(80, 685);
	const int kTrapPointIconOffsetX = 46;
	const Vec2 kTrapPointOffsetPos = Vec2(60, 13);


	// 装備スロットの描画位置とスケール
	constexpr int kSlotBgX = 362;
	constexpr int kSlotBgY = 655;
	constexpr int kSlotBgOffset = 85;
	constexpr float kSlotBgScale = 0.08f;
	constexpr float kSlotIconScale = 0.5f;
	constexpr int kSlotBoxSize = 35;
}

TrapManager::TrapManager() :
	m_angle(0.0f),
	m_transparency(0.0f),
	m_slotIdx(-1),
	m_cameraPos(),
	m_cameraDir(),
	m_trapPoint(0),
	m_bgHandle(-1),
	m_iconHandle(-1),
	m_isTextShake(false),
	m_textShakeFrame(0),
	m_isPrePhase(false),
	m_trapRotationAngle(0.0f),
	m_attackEffectCreateCount(0),
	m_createEffectName("E_TRAPATTACKAREA")
{
}

TrapManager::~TrapManager()
{
	Clear();
}

const bool TrapManager::CheckNeighbor(std::list<std::weak_ptr<Trap>> check) const
{
	for (auto& t : check)
	{
		if (t.lock()->isPlaced)	return false;
	}
	return true;
}

void TrapManager::Update()
{
	//現在設置しているトラップの更新
	for (auto& trap : m_traps)
	{
		trap->Update();
	}

	// 現在値を更新
	if (m_trapPoint != m_targetTrapPoint)
	{
		//差が50以上あったら
		if (abs(m_targetTrapPoint - m_trapPoint) > 50)
		{
			//差に応じて増減アニメーションを早める
			auto difference = abs(m_targetTrapPoint - m_trapPoint);
			difference = difference / 5;

			m_trapPoint += (m_targetTrapPoint > m_trapPoint) ? difference : -difference; // 1フレームごとに追従
		}
		m_trapPoint += (m_targetTrapPoint > m_trapPoint) ? 1 : -1; // 1フレームごとに追従
	}

	//テキストが揺れているなら
	if (m_isTextShake)
	{
		//揺れるフレーム数を0に近づける
		if (m_textShakeFrame > 0)
		{
			m_textShakeFrame--;
		}
		else
		{
			m_isTextShake = false;
		}
	}

	//現在プレイヤーが選択しているスロット番号、カメラの座標と向きベクトルを事前にもらっておく

	//スロット番号が0(クロスボウなら何もしない)
	if (m_slotIdx == 0) return;

	if (m_attackEffectCreateCount % 120 == 0)
	{
		auto attackPos = m_previewTraps[m_slotIdx - 1]->GetAttackPos();

		for (int i = 0;i < 3;i++)
		{
			auto idx = i;
			if (idx > attackPos.size() - 1)
			{
				idx = attackPos.size() - 1;
			}

			EffectManager::GetInstance().CreateEffect(m_createEffectName, attackPos[idx]);
		}
	}

	m_attackEffectCreateCount++;

	m_angle += 0.04f;
	m_transparency = abs(sinf(m_angle) / 2.5f) + 0.1f;

	//それ以外なら罠を設置しようとしている

	auto start = m_cameraPos;					//カメラ座標を開始座標
	auto end = m_cameraPos + m_cameraDir * 70;	//カメラからカメラの向いている方向の座標を終点座標にする

	auto rad = (end - start).Length() * 0.5f;

	//毎フレーム全部のトラップ座標とレイキャストしていたら処理時間が大幅に増えてしまう問題の対処
	//レイの終点座標を中心とした円との判定をとってその円の中にあるトラップ座標とレイキャストをする
	std::list <std::shared_ptr<Trap>> hit;

	while (1)
	{
		for (auto& p : m_trapPoss)
		{
			//現在のスロット番号の罠の種類と法線ベクトルを見て計算するかしないかを決める
			if (m_previewTraps[m_slotIdx - 1]->GetTrapKind() == 0)
			{
				if (abs(p->norm.y - 1.0f) > 0.1f)
				{
					continue;
				}
			}
			else if (m_previewTraps[m_slotIdx - 1]->GetTrapKind() == 1)
			{
				if (abs(p->norm.y) > 0.1f)
				{
					continue;
				}
			}

			//周囲に8個の候補地がなければ次へ
			if (p->neighborTraps.size() != 8)	continue;


			//ここまで来たならその座標と大きな円の判定をとる

			//円の中心座標と候補地の座標との距離を計算する
			auto dis = (end - p->pos).Length();

			//円の半径よりも短かったらレイキャストする対象に追加する
			if (rad > dis)
			{
				hit.push_back(p);
			}
		}

		//もしレイキャストする対象が見つかっていたらwhile文を抜ける
		if (hit.size() != 0)
		{
			break;
		}

		//ここまで来たならレイキャストする対象が見つかっていない場合ということ
		//半径を大きくしてもう一回繰り返す
		rad += 5.0f;
	}


#ifdef _DEBUG
	DrawSphere3D(end.ToVECTOR(), rad, 12, 0xff0000, 0xff0000, false);
#endif

	//上の二つをつなぐ線分と罠の座標の距離を格納する変数
	float defaultLength = 100.0f;

	auto preDebugTrap = debugTrap;

#ifdef _DEBUG
	printf("レイキャストを行うポイント数:%d\n", static_cast<int>(hit.size()));
#endif

	//円と当たったトラップ座標分回す
	for (auto& trapPos : hit)
	{
		//トラップが置かれていないかつ、周囲に8個のトラップがおかれていない候補地があるとき
		//if (!trapPos->isPlaced && trapPos->neighborTraps.size() == 8 /*&& CheckNeighbor(trapPos->neighborTraps)*/)
		{
			//線分と座標の距離を計算する
			float length = Segment_Point_MinLength(start.ToVECTOR(), end.ToVECTOR(), trapPos->pos.ToVECTOR());

			//もし、上で計算した距離が今までの距離より短かったらその距離と候補地を保存する
			if (defaultLength > length)
			{
				defaultLength = length;
				debugTrap = trapPos;
			}
		}
	}

	//もしライトトリガーボタン(RT)が押されたら
	if (Input::GetInstance().GetIsTriggeredTriggerButton(true))
	{
		if (!debugTrap->isPlaced)
		{
			//周囲の8個全てが空いていなかったら次へ
			if (CheckNeighbor(debugTrap->neighborTraps))
			{
				auto add = std::make_shared<TrapBase>();
				if (m_slotIdx == 1)			add = std::make_shared<SpikeTrap>();
				else if (m_slotIdx == 2)	add = std::make_shared<ArrowWallTrap>();
				else if (m_slotIdx == 3)	add = std::make_shared<FlameTrap>();
				else if (m_slotIdx == 4)	add = std::make_shared<CutterTrap>();
				else if (m_slotIdx == 5)	add = std::make_shared<IronSnareTrap>();
				else if (m_slotIdx == 6)	add = std::make_shared<IronImpactTrap>();

				//もし設置しようとしていたトラップのコストよりも現在持っているポイントが少なかったら設置できない
				if (m_trapPoint < add->GetCost())
				{
					m_isTextShake = true;
					m_textShakeFrame = kTextShakeFrame;
					//何もしない
					return;
				}

				//エフェクトの生成
				EffectManager::GetInstance().CreateEffect("E_TRAPCREATE", debugTrap->pos);
				SoundManager::GetInstance().PlaySE("S_TRAPCREATE");

				//所持トラップポイントをコスト分減らす
				AddTrapPoint(-add->GetCost());

				auto vector = debugTrap->norm;
				if (m_previewTraps[m_slotIdx - 1]->GetTrapKind() == 0)
				{
					vector = Vec3(0.0f, m_trapRotationAngle * DX_PI_F / 180.0f, 0.0f);
				}

				//初期化
				add->Init(debugTrap->pos, vector);

				//追加
				m_traps.emplace_back(add);

				//トラップを設置済みにする
				debugTrap->isPlaced = true;
				for (auto& trap : debugTrap->neighborTraps)
				{
					trap.lock()->isPlaced = true;
				}
			}
		}
	}

	//

	if (!CheckNeighbor(debugTrap->neighborTraps) || m_previewTraps[m_slotIdx - 1]->GetCost() > m_trapPoint)
	{
		if (m_createEffectName == "E_TRAPATTACKAREA")
		{
			m_attackEffectCreateCount = 0;
			EffectManager::GetInstance().StopEffect(m_createEffectName);
		}
		m_createEffectName = "E_TRAPATTACKAREARED";
	}
	else
	{
		if (m_createEffectName == "E_TRAPATTACKAREARED")
		{
			m_attackEffectCreateCount = 0;
			EffectManager::GetInstance().StopEffect(m_createEffectName);
		}
		m_createEffectName = "E_TRAPATTACKAREA";

	}

	if (Input::GetInstance().IsTriggered("X"))
	{
		m_trapRotationAngle += 90.0f;

		if (m_trapRotationAngle >= 360.0f)
		{
			m_trapRotationAngle = 0.0f;
		}
		m_previewTraps[m_slotIdx - 1]->SetPos(debugTrap->pos.ToVECTOR());
		m_previewTraps[m_slotIdx - 1]->SetRot(Vec3(0.0f, m_trapRotationAngle* DX_PI_F / 180.0f, 0.0f));

		auto effectHandles = EffectManager::GetInstance().GetIdHandles(m_createEffectName);
		auto attackPos = m_previewTraps[m_slotIdx - 1]->GetAttackPos();
		int i = 0;

		for (auto& handle : effectHandles)
		{
			if (i > attackPos.size() - 1)
			{
				i -= attackPos.size();
			}

			EffectManager::GetInstance().SetPos(handle, attackPos[i]);

			i++;
		}
	}


	if (m_isPrePhase)
	{
		bool isRemove = false;
		if (Input::GetInstance().GetIsTriggeredTriggerButton(false))
		{
			for (auto& t : m_traps)
			{
				if (isRemove)	break;

				if (abs((t->GetPos() - debugTrap->pos).Length()) < 1.0f)
				{
					//撤去したフラグをオンにする
					isRemove = true;

					//使用したコスト分追加する
					AddTrapPoint(t->GetCost());

					//削除する
					t->Finalize();
					t.reset();
					t = nullptr;

					//設置していた座標を設置可能状態に戻す
					debugTrap->isPlaced = false;
					for (auto& pos : debugTrap->neighborTraps)
					{
						pos.lock()->isPlaced = false;
					}
				}
			}
		}

		//不要になった罠をここで削除処理する
		auto it = remove_if(m_traps.begin(), m_traps.end(), [](auto& v) {
			return v == nullptr;
			});
		m_traps.erase(it, m_traps.end());
	}

	if (preDebugTrap != debugTrap || m_slotIdx != m_preSlotIdx)
	{
		m_previewTraps[m_slotIdx - 1]->SetPos(debugTrap->pos.ToVECTOR());
		if (m_previewTraps[m_slotIdx - 1]->GetTrapKind() == 0)
		{
			m_previewTraps[m_slotIdx - 1]->SetRot(Vec3(0.0f, m_trapRotationAngle* DX_PI_F / 180.0f, 0.0f));
		}
		else if(m_previewTraps[m_slotIdx - 1]->GetTrapKind() == 1)
		{
			m_previewTraps[m_slotIdx - 1]->SetRot(debugTrap->norm.ToVECTOR());
		}

		auto effectHandles = EffectManager::GetInstance().GetIdHandles(m_createEffectName);
		auto attackPos = m_previewTraps[m_slotIdx - 1]->GetAttackPos();
		int i = 0;

		for (auto& handle : effectHandles)
		{
			if (i > attackPos.size() - 1)
			{
				i -= attackPos.size();
			}

			EffectManager::GetInstance().SetPos(handle, attackPos[i]);

			i++;
		}
	}

	m_preSlotIdx = m_slotIdx;
}

void TrapManager::Draw()
{
	for (auto& trap : m_traps)
	{
		trap->Draw();
	}

	//#ifdef _DEBUG	//デバッグ描画
		//for (auto& pos : m_trapPoss)
		//{
		//	if (pos->isPlaced)
		//	{
		//		DrawSphere3D(pos->pos.ToVECTOR(), 2, 4, 0xffffff, 0xffffff, false);
		//	}
		//	else
		//	{
		//		DrawSphere3D(pos->pos.ToVECTOR(), 2, 4, 0x0000ff, 0x0000ff, false);
		//	}
		//}
	
		//if (debugTrap != nullptr)
		//{
		//	if (!debugTrap->isPlaced && debugTrap->neighborTraps.size() == 8 && CheckNeighbor(debugTrap->neighborTraps))
		//	{
		//		DrawSphere3D(debugTrap->pos.ToVECTOR(), 4, 4, 0x00ff00, 0x00ff00, false);
		//	}
		//	else
		//	{
		//		DrawSphere3D(debugTrap->pos.ToVECTOR(), 4, 4, 0xff0000, 0xff0000, false);
		//	}
		//}
	//#endif


	if (Setting::GetInstance().GetIsDrawOperation())
	{
		if (debugTrap && m_slotIdx != 0)
		{
			bool isDrawrRelease = false;

			if (debugTrap->isPlaced && m_isPrePhase)
			{
				//罠ポイントの描画
				DrawUI::GetInstance().RegisterDrawRequest([=]()
				{
						FontManager::GetInstance().DrawCenteredText(1034, 440, "罠解除", 0x91cdd9, 24, 0x395f62);
						DrawRotaGraph(1210, 450, 0.5f, 0.0f, ResourceManager::GetInstance().GetHandle("I_LT"), true);
				}, 2);

				isDrawrRelease = true;
			}

			if (m_previewTraps[m_slotIdx - 1]->GetTrapKind() == 0)
			{
				if (isDrawrRelease)
				{
					//罠ポイントの描画
					DrawUI::GetInstance().RegisterDrawRequest([=]()
					{
							FontManager::GetInstance().DrawCenteredText(1046, 400, "罠を回転", 0x91cdd9, 24, 0x395f62);
							DrawRotaGraph(1210, 400, 0.5f, 0.0f, ResourceManager::GetInstance().GetHandle("I_X"), true);
					}, 2);
				}
				else
				{
					//罠ポイントの描画
					DrawUI::GetInstance().RegisterDrawRequest([=]()
					{
							FontManager::GetInstance().DrawCenteredText(1046, 440, "罠を回転", 0x91cdd9, 24, 0x395f62);
							DrawRotaGraph(1210, 440, 0.5f, 0.0f, ResourceManager::GetInstance().GetHandle("I_X"), true);
					}, 2);
				}

			}
		}
	}

	// 装備スロットの描画
	DrawUI::GetInstance().RegisterDrawRequest([=]()
	{
		for (int i = 1; i <= m_trapNames.size(); i++)
		{
			int x = kSlotBgX + i * kSlotBgOffset;
			int y = kSlotBgY;
			DrawRotaGraph(x, y, kSlotBgScale, 0.0f, m_slotBgHandle, true);
			DrawRotaGraph(x, y, kSlotIconScale, 0.0f, ResourceManager::GetInstance().GetHandle(m_previewTraps[i - 1]->GetImageId()), true);

			FontManager::GetInstance().DrawCenteredText(x, y + 45, std::to_string(m_previewTraps[i - 1]->GetCost()), 0x91cdd9, 24, 0x395f62);
		}
	}, 0);

	DrawUI::GetInstance().RegisterDrawRequest([=]()
	{
		// 現在選択しているスロット枠の描画
		DrawRotaGraph(kSlotBgX + m_slotIdx * kSlotBgOffset, kSlotBgY, 1.0f, 0.0f, ResourceManager::GetInstance().GetHandle("I_SLOTSELECT"), true);

	}, 1);

	//罠ポイントの背景画像の描画
	DrawUI::GetInstance().RegisterDrawRequest([=]()
	{
		DrawRotaGraph(static_cast<int>(kTrapPointBgDrawPos.x), static_cast<int>(kTrapPointBgDrawPos.y), 0.72f, 0.0f, m_bgHandle, true);
		DrawRotaGraph(static_cast<int>(kTrapPointBgDrawPos.x) - kTrapPointIconOffsetX, static_cast<int>(kTrapPointBgDrawPos.y), 0.66f, 0.0f, m_iconHandle, true);
	}, 0);

	//背景画像の座標 + 相対座標で罠ポイントの描画座標を計算
	auto drawPos = kTrapPointBgDrawPos + kTrapPointOffsetPos;
	//罠ポイントの描画
	DrawUI::GetInstance().RegisterDrawRequest([=]()
	{
		FontManager::GetInstance().DrawBottomRightAndQuakeText(static_cast<int>(drawPos.x), static_cast<int>(drawPos.y), std::to_string(m_trapPoint), 0x91cdd9, 32, 0x395f62, m_isTextShake, m_textShakeFrame);
	}, 2);
}

void TrapManager::PreviewDraw()
{
	//仮設置描画をする
	//現在のスロット番号から対応する罠のハンドルを取得して半透明描画する
	//	罠のサイズがそれぞれ異なるため、どうにかして設定するためにスケール値を取得する必要がある

	if (m_slotIdx == 0) return;
	if (!debugTrap)	return;

	m_previewTraps[m_slotIdx - 1]->SetPos(debugTrap->pos.ToVECTOR());
	if (m_previewTraps[m_slotIdx - 1]->GetTrapKind() == 0)
	{
		m_previewTraps[m_slotIdx - 1]->SetRot(Vec3(0.0f, m_trapRotationAngle * DX_PI_F / 180.0f, 0.0f));

	}
	else if (m_previewTraps[m_slotIdx - 1]->GetTrapKind() == 1)
	{
		m_previewTraps[m_slotIdx - 1]->SetRot(debugTrap->norm.ToVECTOR());
	}
	m_previewTraps[m_slotIdx - 1]->PreviewDraw();
}

void TrapManager::Load(const char* stageName)
{
	//開くファイルのハンドルを取得
	int handle = FileRead_open((kStageDataPathFront + stageName + kStageDataPathBack).c_str());

	//読み込むオブジェクト数が何個あるか取得
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	//読み込むオブジェクト数分の配列に変更する
	m_trapPoss.resize(dataCnt);

	for (auto& trap : m_trapPoss)
	{
		trap = std::make_shared<Trap>();
		//座標を取得する
		FileRead_read(&trap->pos, sizeof(Vec3), handle);
		FileRead_read(&trap->norm, sizeof(Vec3), handle);
		trap->isPlaced = false;
	}

	FileRead_close(handle);
}

void TrapManager::SetUp(int point)
{
	for (auto& trap : m_trapPoss)
	{
		for (auto& temp : m_trapPoss)
		{
			if (abs((trap->pos - temp->pos).Length()) > 0.0f && abs((trap->pos - temp->pos).Length()) < 12.0f)
			{
				if (Dot(trap->norm.Normalize(), temp->norm.Normalize()) >= 1.0f)
				{
					trap->neighborTraps.emplace_back(temp);
				}
			}
		}
	}

	m_trapNames = LoadCSV::GetInstance().GetAllTrapName();

	m_previewTraps.push_back(std::make_shared<SpikeTrap>());
	m_previewTraps.back()->SetIsPreview();
	m_previewTraps.push_back(std::make_shared<ArrowWallTrap>());
	m_previewTraps.back()->SetIsPreview();
	m_previewTraps.push_back(std::make_shared<FlameTrap>());
	m_previewTraps.back()->SetIsPreview();
	m_previewTraps.push_back(std::make_shared<CutterTrap>());
	m_previewTraps.back()->SetIsPreview();
	m_previewTraps.push_back(std::make_shared<IronSnareTrap>());
	m_previewTraps.back()->SetIsPreview();
	m_previewTraps.push_back(std::make_shared<IronImpactTrap>());
	m_previewTraps.back()->SetIsPreview();

	// 順序を保持するマップを作成
	std::unordered_map<std::string, int> orderMap;
	for (int i = 0; i < m_trapNames.size(); ++i)
	{
		orderMap[m_trapNames[i]] = i;
	}

	// 並び替え
	std::sort(m_previewTraps.begin(), m_previewTraps.end(), [&orderMap](const std::shared_ptr<TrapBase>& a, const std::shared_ptr<TrapBase>& b) {
		return orderMap[a->GetTrapName()] < orderMap[b->GetTrapName()];
	});

	m_slotBgHandle = ResourceManager::GetInstance().GetHandle("I_SLOTBG");
	m_bgHandle = ResourceManager::GetInstance().GetHandle("I_TRAPPOINTBG");
	m_iconHandle = ResourceManager::GetInstance().GetHandle("I_TRAPICON");

	m_trapPoint = point;
	m_targetTrapPoint = m_trapPoint;
}

void TrapManager::Clear()
{
	for (auto& trap : m_trapPoss)
	{
		trap->neighborTraps.clear();
	}
	m_trapPoss.clear();
	m_traps.clear();

	m_trapNames.clear();

	m_slotIdx = 0;
	m_cameraPos = Vec3();
	m_cameraDir = Vec3();
	m_trapPoint = 0;

	DeleteGraph(m_bgHandle);
	DeleteGraph(m_iconHandle);
	m_trapPoint = 0;

	m_previewTraps.clear();
}

void TrapManager::AddTrapPoint(int addPoint)
{
	m_targetTrapPoint += addPoint;
}

const void TrapManager::SetCameraInfo(Vec3 cameraPos, Vec3 dirVec)
{
	m_cameraPos = cameraPos;
	m_cameraDir = dirVec;
}

const void TrapManager::SetSlotIdx(int idx)
{
	m_slotIdx = idx;

	return void();
}
