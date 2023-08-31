#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "MathUtilityForText.h"
#include <time.h>

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete spriteBG_;
	delete modelStage_;
	delete modelPlayer_;
	delete modelBeam_;
	delete modelEnemy2_;
	delete modelEnemy_;
	delete spriteTitle_;
	delete spriteEnter_;
	delete spriteGameOver_;
	delete spriteGameClear_;
	delete spriteSousa_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandleBG_ = TextureManager::Load("bg.png");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	viewProjection_.Initialize();

	textureHandleStage_ = TextureManager::Load("stage2.jpg");
	modelStage_ = Model::Create();
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].Initialize();
	}

	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].translation_ = {0, -1.5f,2.0f * i - 5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};
		// ステージ
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		worldTransformStage_[i].TransferMatrix();
	}

	// プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	// ビーム
	for (int i = 0; i < 10; i++) {
		textureHandleBeam_ = TextureManager::Load("beam.png");
		modelBeam_ = Model::Create();
		worldTransformBeam_[i].scale_ = {0.3f, 0.3f, 0.3f};
		worldTransformBeam_[i].Initialize();
	}

	// エネミー
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5};
		worldTransformEnemy_[i].Initialize();
	}
	srand((unsigned int)time(NULL));

	// エネミー2
	textureHandleEnemy2_ = TextureManager::Load("EnemyBeam.png");
	modelEnemy2_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy2_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy2_[i].Initialize();
	}

	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();

	//タイトル(2Dスプライト)
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});

	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 520});

	textureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(textureHandleGameOver_, {0, 0});

	textureHandleGameClear_ = TextureManager::Load("GameClear.png");
	spriteGameClear_ = Sprite::Create(textureHandleGameClear_, {0, 0});

	//サウンドデータ
	soundDataHandleTitleBGM_    = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleEnemyHitSE_  =  audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ =   audio_->LoadWave("Audio/tada.wav");

	//タイトルBGMを再生
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);

	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i] = Sprite::Create(textureHandleNumber_, {250.0f + i * 26, 20});
	}

	textureHandleSCORE_ = TextureManager::Load("score.png");
	spriteScore_ = Sprite::Create(textureHandleSCORE_, {110.0f, 20});

	textureHandleSousa_ = TextureManager::Load("Sousa.png");
	spriteSousa_ = Sprite::Create(textureHandleSousa_, {105.0f, 60});

	for (int i = 0; i < 3; i++) 
	{
		spriteLife_[i] = Sprite::Create(textureHandlePlayer_, {800.0f + i * 60, 30});
		spriteLife_[i]->SetSize({40, 40});
	}
}

void GameScene::Update() {
	switch (sceneMode_) 
	{
	case 0:
		GamePlayUpdate();
		break;
	case 1:
		TitleUpdate();
		break;
	}
}

void GameScene::GamePlayUpdate() { 
	PlayerUpdate();
	EnemyUpdate();
	Enemy2Update();

	BeamUpdate();
	
	Collision();
	StageUpdate();
    
    if (playerLife_ <= 0) 
	{
		sceneMode_ = 2;
		audio_->StopWave(voiceHandleBGM_);
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGameOverBGM_, true);
	}

	if (gameScore_ >= 1000) 
	{
		sceneMode_ = 3;
		for (int i = 0; i < 10; i++) 
		{
			beamFlag_[i] = 0;
		}
	}

}

void GameScene::GamePlayDraw3D() 
{
	for (int i = 0; i < 20; i++) 
	{
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandleStage_);
	}

	if (playerTimer_ % 4 < 2) 
	{
		modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	}

	for (int i = 0; i < 10; i++) 
	{
		if (beamFlag_[i] == 1) 
		{
			modelBeam_->Draw(worldTransformBeam_[i], viewProjection_, textureHandleBeam_);
		}
	}

	for (int i = 0; i < 10; i++) 
	{
		if (enemyFlag_[i] == 1 && gameScore_ <= 200) 
		{
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}
		if (enemyFlag_[i] == 2 && gameScore_ <= 200) 
		{
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}
	}

	for (int i = 0; i < 10; i++) 
	{
		if (Enemy2Flag_[i] == 1) 
		{
			modelEnemy2_->Draw(worldTransformEnemy2_[i], viewProjection_, textureHandleEnemy2_);
		}
	}
}

void GameScene::GamePlayDraw2DBack() { 
	spriteBG_->Draw(); 
}

void GameScene::GamePlayDraw2DNear() {

	for (int i = 0; i < playerLife_; i++) 
	{
		spriteLife_[i]->Draw();
	}
	spriteScore_->Draw();
	spriteSousa_->Draw();
	DrawScore();
}

void GameScene::PlayerUpdate() {
		if (input_->PushKey(DIK_RIGHT)) {
			worldTransformPlayer_.translation_.x += 0.1f;
		    /*worldTransformPlayer_.rotation_.y += -0.1f;*/
		}
		if (worldTransformPlayer_.translation_.x >= 4) {
			worldTransformPlayer_.translation_.x = 4;
		}

		if (input_->PushKey(DIK_LEFT)) {
			worldTransformPlayer_.translation_.x -= 0.1f;
		  /*  worldTransformPlayer_.rotation_.y += -0.1f;*/
		}
	    if (worldTransformPlayer_.translation_.x < -4) {
		    worldTransformPlayer_.translation_.x = -4;
	    }
	    worldTransformPlayer_.rotation_.x += 0.1f; 
		worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
		    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
		    worldTransformPlayer_.translation_);
		worldTransformPlayer_.TransferMatrix();

		if (playerTimer_ > 0) 
		{
		    playerTimer_ -= 1;
	    }
}

void GameScene::BeamUpdate() {
	BeamBorn();
	BeamMove();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].matWorld_ = MakeAffineMatrix(
		worldTransformBeam_[i].scale_, worldTransformBeam_[i].rotation_,
		worldTransformBeam_[i].translation_);
		worldTransformBeam_[i].TransferMatrix();
	}
}

void GameScene::BeamBorn() 
{
	if (beamtimer_ == 0) 
	{
		if (input_->PushKey(DIK_SPACE)) {
			for (int i = 0; i < 10; i++) {
				if (beamFlag_[i] == 0) {
					beamFlag_[i] = 1;
					worldTransformBeam_[i].translation_.x = worldTransformPlayer_.translation_.x;
					worldTransformBeam_[i].translation_.y = worldTransformPlayer_.translation_.y;
					worldTransformBeam_[i].translation_.z = worldTransformPlayer_.translation_.z;
					beamtimer_ = 1;
					break;
				}
			}
		}
	}
	else 
	{
		beamtimer_++;
		if (beamtimer_ > 10) 
		{
			beamtimer_ = 0;
		}
	}
}

void GameScene::BeamMove() { 
	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i] == 1) 
		{
		  worldTransformBeam_[i].translation_.z += 0.1f;
		  worldTransformBeam_[i].rotation_.x += 0.1f;
		}
		if (worldTransformBeam_[i].translation_.z >= 40) 
		{
		  beamFlag_[i] = 0;
		}
	}
}

void GameScene::EnemyUpdate() { 
	EnemyBorn();
	EnemyMove();
	EnemyJump();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformEnemy_[i].scale_, worldTransformEnemy_[i].rotation_,
		    worldTransformEnemy_[i].translation_);
		    worldTransformEnemy_[i].TransferMatrix();
	}
}

void GameScene::EnemyBorn() 
{
	if (rand() % 10 == 0)
	{
		    for (int i = 0; i < 10; i++) 
			{
		        if (enemyFlag_[i] == 0 && gameScore_ <= 200)
				{
					enemyFlag_[i] = 1;
					int x = rand() % 100;
					float x2 = (float)x / 10 - 4;
					worldTransformEnemy_[i].translation_.x = x2;
					worldTransformEnemy_[i].translation_.z = 40;
				    if (rand() % 2 == 0) 
					{
					enemySpeed_[i] = 0.1f;
				    } 
					else 
					{
					enemySpeed_[i] = -0.1f;
				    }
					break;
				}
		    }
	}
}

void GameScene::EnemyMove() { 
	for (int i = 0; i < 10; i++) 
	{
			if (enemyFlag_[i] == 1 && gameScore_ <= 200) 
			{
				worldTransformEnemy_[i].translation_.z -= 0.2f;
			    worldTransformEnemy_[i].translation_.z -= gameTimer_ / 1000.0f;
			    worldTransformEnemy_[i].translation_.x += enemySpeed_[i];
			    if (worldTransformEnemy_[i].translation_.x >= 4) 
				{
				    enemySpeed_[i] = -0.1f;
			    }
			    if (worldTransformEnemy_[i].translation_.x <= -4) 
				{
				    enemySpeed_[i] = 0.1f;
			    }
			}
			if (worldTransformEnemy_[i].translation_.z < -5) 
			{
				enemyFlag_[i] = 0;
			}
	}
}

void GameScene::EnemyJump() {
	for (int  i = 0; i < 10; i++) {

		    if (enemyFlag_[i] == 2) {

			    worldTransformEnemy_[i].translation_.y += enemyJumpSpeed_[i];

				enemyJumpSpeed_[i] -= 0.1f;

				worldTransformEnemy_[i].translation_.x += enemySpeed_[i] * 4;

				if (worldTransformEnemy_[i].translation_.y < -3) 
				{
				    enemyFlag_[i] = 0;
				    worldTransformEnemy_[i].translation_.y = 0;
			    }
		    }
	}
}


void GameScene::Enemy2Update() {
	Enemy2Born();
	Enemy2Move();
	for (int i = 0; i < 10; i++) {
		    worldTransformEnemy2_[i].matWorld_ = MakeAffineMatrix(
		        worldTransformEnemy2_[i].scale_, worldTransformEnemy2_[i].rotation_,
		        worldTransformEnemy2_[i].translation_);
		    worldTransformEnemy2_[i].TransferMatrix();
	}
}

void GameScene::Enemy2Born() {
	if (rand() % 10 == 0) {
		    for (int i = 0; i < 10; i++) {
			    if (Enemy2Flag_[i] == 0 && gameScore_ >= 200) {
				    Enemy2Flag_[i] = 1;
				    int x = rand() % 100;
				    float x2 = (float)x / 10 - 3;
				    worldTransformEnemy2_[i].translation_.x = x2;
				    worldTransformEnemy2_[i].translation_.z = 40;
				    if (rand() % 2 == 0) 
					{
					enemySpeed_[i] = 0.1f;
				    } 
					else 
					{
					enemySpeed_[i] = -0.1f;
				    }
				    break;
			    }
		    }
	}
}

void GameScene::Enemy2Move() {
	for (int i = 0; i < 10; i++) {
		    if (Enemy2Flag_[i] == 1) 
			{
			    worldTransformEnemy2_[i].translation_.z -= 0.2f;
			    worldTransformEnemy2_[i].rotation_.x += 0.1f;
			    worldTransformEnemy2_[i].translation_.z -= gameTimer_ / 1000.0f;
			    worldTransformEnemy2_[i].translation_.x += enemy2Speed_[i];
		    }
		    if (worldTransformEnemy2_[i].translation_.x >= 4) 
			{
			    enemy2Speed_[i] = -0.1f;
		    }
		    if (worldTransformEnemy2_[i].translation_.x <= -4) 
			{
			    enemy2Speed_[i] = 0.1f;
		    }

		    if (worldTransformEnemy2_[i].translation_.z < 0) 
			{
			    Enemy2Flag_[i] = 0;
		    }
	}
}

void GameScene::Collision() { 
	CollisionPlayerEnemy(); 
	CollisionEnemy2Player();
	CollisionBeamEnemy();
	CollisionBeamEnemy2();
}

void GameScene::CollisionPlayerEnemy() {
	for (int i = 0; i < 10; i++) {
		    if (enemyFlag_[i] == 1 || enemyFlag_[i] == 2) 
			{ 
				float dx =
			    abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
			    float dz =
			    abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);

			    if (dx < 1 && dz < 1) 
				{
				 enemyFlag_[i] = 0; 
				 playerLife_ -= 1;
				 playerTimer_ = 60;
				 audio_->PlayWave(soundDataHandlePlayerHitSE_);
			    }
		    }
	}
}

void GameScene::CollisionEnemy2Player() {
	for (int i = 0; i < 10; i++) {
		    if (Enemy2Flag_[i] == 1) 
			{ 
				float dx =
			    abs(worldTransformPlayer_.translation_.x - worldTransformEnemy2_[i].translation_.x);
			    float dz =
			    abs(worldTransformPlayer_.translation_.z - worldTransformEnemy2_[i].translation_.z);

			    if (dx < 1 && dz < 1) 
				{
				 Enemy2Flag_[i] = 0; 
				 playerLife_ -= 2;
				 playerTimer_ = 60;
				 audio_->PlayWave(soundDataHandlePlayerHitSE_);
			    }
		    }
	}

}

void GameScene::CollisionBeamEnemy() 
{
	for (int i = 0; i < 10; i++) 
	{
		    for (int b = 0; b < 10; b++)
			    if (enemyFlag_[i] == 1 && beamFlag_[b] == 1) 
				{
			float dx = abs(worldTransformBeam_[b].translation_.x - worldTransformEnemy_[i].translation_.x);
			float dz = abs(worldTransformBeam_[b].translation_.z - worldTransformEnemy_[i].translation_.z);

		if (dx < 1 && dz < 1 && gameScore_ <= 200) 
		{
			enemyFlag_[i] = 2;
			enemyJumpSpeed_[i] = 1;
			beamFlag_[b] = 0;
			gameScore_ += 10;
			audio_->PlayWave(soundDataHandleEnemyHitSE_);
		}
	}
	}
}

void GameScene::CollisionBeamEnemy2() {
	for (int i = 0; i < 10; i++) {
		    for (int b = 0; b < 10; b++)
	if (Enemy2Flag_[i] == 1 && beamFlag_[b] == 1) {
		float dx =
			abs(worldTransformBeam_[b].translation_.x - worldTransformEnemy2_[i].translation_.x);
		float dz =
			abs(worldTransformBeam_[b].translation_.z - worldTransformEnemy2_[i].translation_.z);

		if (dx < 1 && dz < 1 && gameScore_ >= 200) {
			Enemy2Flag_[i] = 0;
			beamFlag_[b] = 0;
			gameScore_ += 10;
			audio_->PlayWave(soundDataHandleEnemyHitSE_);
		}
	}
	}
}

void GameScene::StageUpdate() {
	for (int i = 0; i < 20; i++) 
	{
		    worldTransformStage_[i].translation_.z -= 0.1f;
		    if (worldTransformStage_[i].translation_.z < -5) 
			{
	            worldTransformStage_[i].translation_.z += 40;
		    }
		    worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_,
		    worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		    worldTransformStage_[i].TransferMatrix();
	}
}

void GameScene::DrawScore()
{ 
	int eachNumber[5] = {};
	int number = gameScore_;

	int keta = 10000;
	for (int i = 0; i < 5; i++) 
	{
	    eachNumber[i] = number / keta;
		number = number % keta;
		keta = keta / 10;
	}

	for (int i = 0; i < 5; i++) 
	{
		spriteNumber_[i]->SetSize({32, 64});
		spriteNumber_[i]->SetTextureRect({32.0f * eachNumber[i], 0}, {32, 64});
		spriteNumber_[i]->Draw();
	}
}

//タイトル
//タイトル更新
void GameScene::TitleUpdate()
{
	if (input_->TriggerKey(DIK_RETURN)) 
	{

		sceneMode_ = 0;   
		GamePlayStart();
		GamePlayUpdate();

		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_);
	    voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGamePlayBGM_, true);
	}
}
//タイトル表示
void GameScene::TitleDraw2DNear()
{
	spriteTitle_->Draw();
	gameTimer_ += 1;

	if (gameTimer_ % 40 >= 20) 
	{
		spriteEnter_->Draw();
	}
}

void GameScene::GameOverDraw2DNear()
{
		spriteGameOver_->Draw();
	    gameTimer_ += 1;
		if (gameTimer_ % 40 >= 20) 
		{
		   spriteEnter_->Draw();
		}

		if (input_->TriggerKey(DIK_RETURN)) 
		{
		   GamePlayStart();
		}
}

void GameScene::GameClearDraw2DNear()
{
	    spriteGameClear_->Draw();
	    gameTimer_ += 1;
	    if (gameTimer_ % 40 >= 20) {
		   spriteEnter_->Draw();
	    }

	    if (input_->TriggerKey(DIK_RETURN)) 
		{
		   GamePlayStart();
	    }
}

void GameScene::GamePlayStart()
{ 
	if (sceneMode_ == 2) 
	{
		   sceneMode_ = 1;
		   gameScore_ = 0;
		   playerLife_ = 3;
		   gameTimer_ = 0;
		   playerTimer_ = 0;
		   for (int i = 0; i < 10; i++) 
		   {
	         worldTransformEnemy_[i].translation_.z = 40.0f;
		   }
		   for (int i = 0; i < 10; i++) 
		   {
			 worldTransformEnemy2_[i].translation_.z = 40.0f;
		   }
		   for (int i = 0; i < 10; i++) 
		   {
			 beamFlag_[i] = 0; 		   
		   }
		   for (int i = 0; i < 10; i++) 
		   {
			 Enemy2Flag_[i] = 0;
		   }
		   worldTransformPlayer_.translation_.x = 0.0f;
		   audio_->StopWave(voiceHandleBGM_);
		   voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
		   GamePlayUpdate();
		   PlayerUpdate();
	}

	if (sceneMode_ == 3) 
	{
		   sceneMode_ = 1;
		   gameScore_ = 0;
		   playerLife_ = 3;
		   gameTimer_ = 0;
		   playerTimer_ = 0;
		   for (int i = 0; i < 10; i++) {
			 worldTransformEnemy_[i].translation_.z = 40.0f;
		   }
		   for (int i = 0; i < 10; i++) {
			 worldTransformEnemy2_[i].translation_.z = 40.0f;
		   }
		   for (int i = 0; i < 10; i++) {
			 beamFlag_[i] = 0;
		   }
		   for (int i = 0; i < 10; i++) {
			 Enemy2Flag_[i] = 0;
		   }
		   worldTransformPlayer_.translation_.x = 0.0f;
		   audio_->StopWave(voiceHandleBGM_);
		   voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
		   GamePlayUpdate();
		   PlayerUpdate();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	/// 
	/// 
	switch (sceneMode_) 
	{
	case 0:
		GamePlayDraw2DBack();
		break;
	case 2:
		GamePlayDraw2DBack();
		break;
	case 3:
		GamePlayDraw2DBack();
		break;
	}

	debugText_->DrawAll();

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	/// 
	
	switch (sceneMode_) 
	{
	case 0:
		GamePlayDraw3D();
		break;
	case 2:
		GamePlayDraw3D();
		break;
	case 3:
		GamePlayDraw3D();
	}

	/// 
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DNear();
		break;
	case 1:
		TitleDraw2DNear();
		break;
	case 2:
		GameOverDraw2DNear();
		break;
	case 3:
		GameClearDraw2DNear();
		break;
	}

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
