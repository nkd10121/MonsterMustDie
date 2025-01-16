#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include "Util/Game.h"
#include "Camera.h"
#include "Input.h"
#include "SceneManager.h"
#include "FontManager.h"
#include "ResourceManager.h"
#include "LoadCSV.h"
#include <memory>

#ifdef _DEBUG
#include "SceneDebug.h"
#endif
#include "SceneTitle.h"

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// windowモード設定
	ChangeWindowMode(true);
	//ウィンドウサイズの変更
	SetGraphMode(Game::kWindowWidth, Game::kWindowHeight, 32);
	// ウインドウ名設定
	SetMainWindowText(Game::kWindowName);
	//フルスクリーン切り替え時にリソースハンドルをリセットしないように設定する
	SetChangeScreenModeGraphicsSystemResetFlag(false);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

#ifdef _DEBUG	//デバッグ描画
	AllocConsole();                                        // コンソール
	FILE* out = 0; freopen_s(&out, "CON", "w", stdout); // stdout
	FILE* in = 0; freopen_s(&in, "CON", "r", stdin);   // stdin

	SetForegroundWindow(GetMainWindowHandle());
#endif

	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetUseBackCulling(true);

	Effekseer_Init(800);
	Effekseer_InitDistortion();
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);

	FontManager::GetInstance().LoadFont();

#ifdef _DEBUG
	//最初にタイトルシーンに遷移する
	SceneManager::GetInstance().SetNextScene(std::make_shared<SceneDebug>());
#else
	//最初にタイトルシーンに遷移する
	SceneManager::GetInstance().SetNextScene(std::make_shared<SceneTitle>());
#endif
	//タイトルシーンに必要なリソースをロードする
	SceneManager::GetInstance().StartLoadThisScene();

	while (ProcessMessage() == 0)
	{
		LONGLONG  time = GetNowHiPerformanceCount();
		// 画面のクリア
		ClearDrawScreen();

		Effekseer_Sync3DSetting();

		//シーンの更新
		if (SceneManager::GetInstance().Update())	break;

		//シーンの描画
		SceneManager::GetInstance().Draw();

		//裏画面を表画面を入れ替える
		ScreenFlip();

		// escキーを押したら終了する
		if (CheckHitKey(KEY_INPUT_ESCAPE))	break;

		// fpsを60に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}
	}

	//staticクラスの削除
	SceneManager::GetInstance().Destroy();
	Input::GetInstance().Destroy();
	LoadCSV::GetInstance().Destroy();
	MyLib::Physics::GetInstance().Destroy();
	FontManager::GetInstance().Destroy();
	ResourceManager::GetInstance().AllClear();
	ResourceManager::GetInstance().Destroy();



	Effkseer_End();
	DxLib_End();				// ＤＸライブラリ使用の終了処理

#ifdef _DEBUG	//デバッグ描画
	// コンソール解放
	fclose(out); fclose(in); FreeConsole();
#endif

	return 0;				// ソフトの終了 
}