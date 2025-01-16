#pragma once

// 当たったものの判別を行うためのタグ
enum class GameObjectTag
{
	Crystal,		//クリスタル
	Player,			//プレイヤー
	Enemy,			//敵
	SwarmEnemy,		//敵の群れ
	Portion,		//回復ポーション
	PlayerShot,		//プレイヤーの遠距離攻撃の弾

	Trap,		//トラップ
};