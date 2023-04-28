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
	delete modelEnemy_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	viewProjection_.Initialize();

	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();

	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};

	//ステージ
	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	    worldTransformStage_.scale_, worldTransformStage_.rotation_,
	    worldTransformStage_.translation_);
	worldTransformStage_.TransferMatrix();

	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	//ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransformBeam_.Initialize();

	//エネミー
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5};
	worldTransformEnemy_.Initialize();
	srand((unsigned int)time(NULL));
}

void GameScene::Update() { 
	PlayerUpdate();
	BeamUpdate();
	EnemyUpdate();
}

void GameScene::PlayerUpdate() {

	if (input_->PushKey(DIK_RIGHT)) 
	{
		worldTransformPlayer_.translation_.x += 0.1f;
	}
	if (worldTransformPlayer_.translation_.x >= 4) 
	{
		worldTransformPlayer_.translation_.x = 4;
	}

	if (input_->PushKey(DIK_LEFT))
	{
		worldTransformPlayer_.translation_.x -= 0.1f;
	}
	if (worldTransformPlayer_.translation_.x < -4) 
	{
		worldTransformPlayer_.translation_.x = -4;
	}

	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);
	worldTransformPlayer_.TransferMatrix();
}

void GameScene::BeamUpdate() {
	BeamBorn();
	BeamMove();
	worldTransformBeam_.matWorld_ = MakeAffineMatrix(
	    worldTransformBeam_.scale_,
		worldTransformBeam_.rotation_,
	    worldTransformBeam_.translation_);
	worldTransformBeam_.TransferMatrix();
}

void GameScene::BeamBorn() {
	if (input_->PushKey(DIK_SPACE)) {
		beamFlag_ = 1;
		worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
		worldTransformBeam_.translation_.y = worldTransformPlayer_.translation_.y;
		worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;
	}
}

void GameScene::BeamMove() { 
	if (beamFlag_ == 1) {
		worldTransformBeam_.translation_.z += 0.1f;
		worldTransformBeam_.rotation_.x += 0.1f;
	}
	if (worldTransformBeam_.translation_.z >= 40) {
		beamFlag_ = 0;
	}
}

void GameScene::EnemyUpdate() { 
	EnemyBorn();
	EnemyMove();
	worldTransformEnemy_.matWorld_ = MakeAffineMatrix(
	    worldTransformEnemy_.scale_, 
		worldTransformEnemy_.rotation_,
	    worldTransformEnemy_.translation_);
	worldTransformEnemy_.TransferMatrix();
}

void GameScene::EnemyBorn() {
	if (enemyFlag_ == 0) {
		enemyFlag_ = 1;
		int x = rand() % 80;
		float x2 = (float)x / 10 - 4;
		worldTransformEnemy_.translation_.x = x2;
		worldTransformEnemy_.translation_.z = 40;
	}
}

void GameScene::EnemyMove() { 
	if (enemyFlag_ == 1) {
		worldTransformEnemy_.translation_.z -= 0.1f;
		worldTransformEnemy_.rotation_.x -= 0.1f;
	}
	if (worldTransformEnemy_.translation_.z < -5) {
		enemyFlag_ = 0;
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
	spriteBG_->Draw();

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
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	if (beamFlag_ == 1) {
		modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	}
	if (enemyFlag_ == 1) {
		modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
	}
	/// 
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
