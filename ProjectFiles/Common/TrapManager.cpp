#include "TrapManager.h"
#include "MathHelp.h"

#include "SpikeTrap.h"
#include "ArrowWallTrap.h"

#include "Input.h"
#include "ResourceManager.h"
#include "FontManager.h"
#include "LoadCSV.h"

#include "Vec2.h"

TrapManager* TrapManager::m_instance = nullptr;

namespace
{
	const std::string kStageDataPathFront = "data/stageData/";
	const std::string kStageDataPathBack = ".tLoc";

	//テキストを揺らすフレーム数
	constexpr int kTextShakeFrame = 30;

	//罠ポイントの背景の描画座標
	const Vec2 kTrapPointBgDrawPos = Vec2(80, 660);
	const int kTrapPointIconOffsetX = 46;
	const Vec2 kTrapPointOffsetPos = Vec2(60, 13);
}

TrapManager::TrapManager() :
	m_previewTrapModelHandle(-1),
	m_angle(0.0f),
	m_transparency(0.0f),
	m_slotIdx(-1),
	m_cameraPos(),
	m_cameraDir(),
	m_trapPoint(0),
	m_rightTriggerPushCount(0),
	m_bgHandle(-1),
	m_iconHandle(-1),
	m_isTextShake(false),
	m_textShakeFrame(0),
	m_isPrePhase(false)
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

void TrapManager::AddTrapPos(Vec3 pos)
{
	//std::shared_ptr<Trap> add = std::make_shared<Trap>();
	//add->isPlaced = false;
	//add->pos = pos;
	//add->neighborTraps.clear();
	//m_trapPoss.emplace_back(add);
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
		if (m_targetTrapPoint - m_trapPoint > 100)
		{
			auto difference = m_targetTrapPoint - m_trapPoint;
			difference = difference / 10;

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
			if (m_trapKind[m_slotIdx - 1] == 0)
			{
				if (abs(p->norm.y - 1.0f) > 0.1f)
				{
					continue;
				}
			}
			else if (m_trapKind[m_slotIdx - 1] == 1)
			{
				if (abs(p->norm.y) > 0.1f)
				{
					continue;
				}
			}

			//周囲に8個の候補地がなければ次へ
			if (p->neighborTraps.size() != 8)
			{
				continue;
			}

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

	//円と当たったトラップ座標分回す
	for (auto& trapPos : hit)
	{
		//トラップが置かれていないかつ、周囲に8個のトラップがおかれていない候補地があるとき
		//if (!trapPos->isPlaced &&/* trapPos->neighborTraps.size() == 8 && */CheckNeighbor(trapPos->neighborTraps))
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
	if (Input::GetInstance().GetIsPushedTriggerButton(true))
	{
		//トリガーボタンを押した瞬間なら
		if (m_rightTriggerPushCount == 0)
		{
			if (!debugTrap->isPlaced)
			{
				//トリガーボタンを押したカウントを更新する
				m_rightTriggerPushCount++;

				switch (m_slotIdx)
				{
				case 1:
				{
					auto add = std::make_shared<SpikeTrap>();
					//もし設置しようとしていたトラップのコストよりも現在持っているポイントが少なかったら設置できない
					if (m_trapPoint < add->GetCost())
					{
						m_isTextShake = true;
						m_textShakeFrame = kTextShakeFrame;
						//何もしない
						return;
					}

					//所持トラップポイントをコスト分減らす
					m_trapPoint -= add->GetCost();
					if (m_trapPoint == m_targetTrapPoint)
					{
						m_targetTrapPoint = m_trapPoint;
					}
					else
					{
						m_targetTrapPoint -= add->GetCost();
					}

					//初期化
					add->Init(debugTrap->pos, debugTrap->norm);

					//追加
					m_traps.emplace_back(add);

					//トラップを設置済みにする
					debugTrap->isPlaced = true;
					for (auto& trap : debugTrap->neighborTraps)
					{
						trap.lock()->isPlaced = true;
					}
				}
				break;
				case 2:
				{
					auto add = std::make_shared<ArrowWallTrap>();
					//もし設置しようとしていたトラップのコストよりも現在持っているポイントが少なかったら設置できない
					if (m_trapPoint < add->GetCost())
					{
						m_isTextShake = true;
						m_textShakeFrame = kTextShakeFrame;
						//何もしない
						return;
					}

					//所持トラップポイントをコスト分減らす
					m_trapPoint -= add->GetCost();
					if (m_trapPoint == m_targetTrapPoint)
					{
						m_targetTrapPoint = m_trapPoint;
					}
					else
					{
						m_targetTrapPoint -= add->GetCost();
					}

					//初期化
					add->Init(debugTrap->pos, debugTrap->norm);

					//追加
					m_traps.emplace_back(add);

					//トラップを設置済みにする
					debugTrap->isPlaced = true;
					for (auto& trap : debugTrap->neighborTraps)
					{
						trap.lock()->isPlaced = true;
					}
				}
				break;
				default:
					break;
				}
			}
		}
	}
	else
	{
		m_rightTriggerPushCount = 0;
	}


	if (m_isPrePhase)
	{
		bool isRemove = false;
		if (Input::GetInstance().IsTriggered("X"))
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
}

void TrapManager::Draw()
{
	for (auto& trap : m_traps)
	{
		trap->Draw();
	}

	DrawFormatString(435, 700, 0xffffff, "350");
	DrawFormatString(520, 700, 0xffffff, "600");

	//#ifdef _DEBUG	//デバッグ描画
	//	for (auto& pos : m_trapPoss)
	//	{
	//		if (pos->isPlaced)
	//		{
	//			DrawSphere3D(pos->pos.ToVECTOR(), 3, 4, 0xffffff, 0xffffff, false);
	//		}
	//		else
	//		{
	//			DrawSphere3D(pos->pos.ToVECTOR(), 3, 4, 0xffff00, 0xffff00, false);
	//		}
	//	}
	//
	//	if (debugTrap != nullptr)
	//	{
	//		if (!debugTrap->isPlaced && debugTrap->neighborTraps.size() == 8 && CheckNeighbor(debugTrap->neighborTraps))
	//		{
	//			DrawSphere3D(debugTrap->pos.ToVECTOR(), 4, 4, 0x00ff00, 0x00ff00, false);
	//		}
	//		else
	//		{
	//			DrawSphere3D(debugTrap->pos.ToVECTOR(), 4, 4, 0xff0000, 0xff0000, false);
	//		}
	//	}
	//#endif

	DrawRotaGraph(static_cast<int>(kTrapPointBgDrawPos.x), static_cast<int>(kTrapPointBgDrawPos.y), 0.72f, 0.0f, m_bgHandle, true);
	DrawRotaGraph(static_cast<int>(kTrapPointBgDrawPos.x) - kTrapPointIconOffsetX, static_cast<int>(kTrapPointBgDrawPos.y), 0.66f, 0.0f, m_iconHandle, true);

	auto drawPos = kTrapPointBgDrawPos + kTrapPointOffsetPos;
	FontManager::GetInstance().DrawBottomRightAndQuakeText(static_cast<int>(drawPos.x), static_cast<int>(drawPos.y), std::to_string(m_trapPoint), 0x9effff, 32, m_isTextShake, m_textShakeFrame);
	//DrawFormatString(76, 720 - 16 * 4, 0xffffff, "%d", m_trapPoint);
}

void TrapManager::PreviewDraw()
{
	//仮設置描画をする
	//現在のスロット番号から対応する罠のハンドルを取得して半透明描画する
	//	罠のサイズがそれぞれ異なるため、どうにかして設定するためにスケール値を取得する必要がある

	if (m_slotIdx == 0) return;
	if (!debugTrap)	return;

	m_previewTrapModelHandle = m_trapModelHandles[m_slotIdx - 1].first;
	auto scale = m_trapModelHandles[m_slotIdx - 1].second;
	MV1SetScale(m_previewTrapModelHandle, VGet(scale, scale, scale));
	MV1SetPosition(m_previewTrapModelHandle, debugTrap->pos.ToVECTOR());
	//回転させる
	auto angle = atan2(debugTrap->norm.x, debugTrap->norm.z);
	auto rotation = VGet(0.0f, angle + DX_PI_F, 0.0f);
	MV1SetRotationXYZ(m_previewTrapModelHandle, rotation);

	//モデルの半透明設定
	MV1SetOpacityRate(m_previewTrapModelHandle, m_transparency);

	MV1DrawModel(m_previewTrapModelHandle);

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

	m_trapModelHandles.push_back(std::make_pair(ResourceManager::GetInstance().GetHandle("M_SPIKE"), 1.8f));
	m_trapModelHandles.push_back(std::make_pair(ResourceManager::GetInstance().GetHandle("M_ARROWWALL"), 1.0f));
	m_trapKind.push_back(LoadCSV::GetInstance().LoadTrapStatus("Spike").kind);
	m_trapKind.push_back(LoadCSV::GetInstance().LoadTrapStatus("ArrowWall").kind);

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

	m_slotIdx = 0;
	m_cameraPos = Vec3();
	m_cameraDir = Vec3();
	m_trapPoint = 0;

	DeleteGraph(m_bgHandle);
	DeleteGraph(m_iconHandle);
	m_trapPoint = 0;

	for (auto& h : m_trapModelHandles)
	{
		MV1DeleteModel(h.first);
	}
	m_trapModelHandles.clear();

	m_trapKind.clear();

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