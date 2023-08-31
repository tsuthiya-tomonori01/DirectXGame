#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugText.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

	ViewProjection viewProjection_;

	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_[20];

	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;

	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_[10];

	uint32_t textureHandleEnemy2_ = 0;
	Model* modelEnemy2_ = nullptr;
	WorldTransform worldTransformEnemy2_[10];


	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_[10];

	DebugText* debugText_ = nullptr;

	void PlayerUpdate();
	void BeamUpdate();
	void BeamMove();
	void BeamBorn();

	void Enemy2Update();
	void Enemy2Move();
	void Enemy2Born();

	int beamFlag_[10] = {};
	int beamtimer_ = 0;

	int Enemy2Flag_[10] = {};

	void EnemyUpdate();
	void EnemyMove();

	int enemyFlag_[10] = {};

	float enemySpeed_[10] = {};

	float enemy2Speed_[10] = {};

	void EnemyBorn();

	void EnemyJump();

	void Collision();
	void CollisionPlayerEnemy();
	void CollisionEnemy2Player();
	void CollisionBeamEnemy();
	void CollisionBeamEnemy2();

	int gameScore_ = 0;
	int playerLife_ = 3;

	void GamePlayUpdate();
	void GamePlayDraw3D(); 
	void GamePlayDraw2DBack();
	void GamePlayDraw2DNear();

	int sceneMode_ = 1;

	void TitleUpdate();
	void TitleDraw2DNear();
	
	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;

	int gameTimer_ = 0;

	uint32_t textureHandleGameOver_ = 0;
	Sprite* spriteGameOver_ = nullptr;

	void GameOverDraw2DNear();

	void GameClearDraw2DNear();

	void GamePlayStart();

	uint32_t soundDataHandleTitleBGM_ = 0;
	uint32_t soundDataHandleGamePlayBGM_ = 0;
	uint32_t soundDataHandleGameOverBGM_ = 0;
	uint32_t soundDataHandleEnemyHitSE_ = 0;
	uint32_t soundDataHandlePlayerHitSE_ = 0;
	uint32_t voiceHandleBGM_ = 0;

	void StageUpdate();

	float enemyJumpSpeed_[10] = {};

	uint32_t textureHandleNumber_ = 0;
	Sprite* spriteNumber_[5] = {};

	void DrawScore();

	uint32_t textureHandleSCORE_ = 0;
	Sprite* spriteScore_ = {};

	Sprite* spriteLife_[3] = {};

	int playerTimer_ = 0;

	uint32_t textureHandleGameClear_ = 0;
	Sprite* spriteGameClear_ = nullptr;

	uint32_t textureHandleSousa_ = 0;
	Sprite* spriteSousa_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
