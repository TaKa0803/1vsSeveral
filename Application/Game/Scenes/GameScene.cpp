#include"GameScene.h"

#include<imgui.h>

#include"InstancingModelManager/InstancingModelManager.h"
#include"TextureManager/TextureManager.h"
#include"AudioManager/AudioManager.h"
#include"RandomNum/RandomNum.h"
#include"PostEffect/PostEffectManager/PostEffectManager.h"
//#include"MapLoader/MapLoader.h"
#include"PostEffect/PEs/PEHSVFilter.h"
#include"PostEffect/PEs/PEVignetting.h"
#include"ColliderOBB/OBBCollider.h"

GameScene::GameScene() {
	input_ = Input::GetInstance();

	camera_ = Camera::GetInstance();
	player_ = std::make_unique<ALPlayer>();

	plane_ = std::make_unique<Plane>();

	MapLoader::GetInstance()->Initialize();

	MapLoader::GetInstance()->LoadLevelEditor("untitled", ".json");
	//MapLoader::GetInstance()->CreateModel(0);
	/*std::vector<std::unique_ptr<OBBCollider>>& datas = MapLoader::GetInstance()->GetColliderData();

	for (auto& data : datas) {
		if (data->colliderTag_ == "plane c") {
			data->isActive_ = false;
			continue;
		}
	}*/

	enemyPopManager_ = std::make_unique<EnemyPopManager>();
	enemyPopManager_->LoadMapItem("EnemySpawn", MapLoader::GetInstance()->GetLevelData());



	//インスタンシングの複数モデルが同じ画像利用の時の対応
	InstancingModelManager* IMM = InstancingModelManager::GetInstance();
	std::string eModelparts[5] = {
		"Head",
		"LArm",
		"RArm",
		"LFoot",
		"RFoot"
	};
	int texture = TextureManager::LoadTex("resources/Models/Object/enemy.png").texNum;
	for (auto& tag : eModelparts) {
		IMM->SetTexture(tag, texture);
	}


	texture = TextureManager::LoadTex("resources/Texture/AL/skill.png").texNum;
	skillSp_.reset(Sprite::Create(texture, { 90,90 }, { 90,90 }, { 90,90 }, { 1000,600 }));



	texture = TextureManager::LoadTex("resources/Texture/AL/B.png").texNum;
	BButton_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 64,64 }, { 1045, 650 }));


	texture = TextureManager::LoadTex("resources/Texture/AL/punch.png").texNum;
	punchSp_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 90,90 }, { 1070,650 }));

	texture = TextureManager::LoadTex("resources/Texture/AL/kick.png").texNum;
	kickSp_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 90,90 }, { 1070,650 }));


	texture = TextureManager::LoadTex("resources/Texture/AL/ult.png").texNum;
	ultSp_.reset(Sprite::Create(texture, { 180,90 }, { 90,90 }, { 90,90 }, { 1070,650 }));


	texture = TextureManager::LoadTex("resources/Texture/AL/number64x90.png").texNum;
	num1_.reset(Sprite::Create(texture, { 640,90 }, { 64,90 }, { 90,90 }, { 680,90 }));
	num10_.reset(Sprite::Create(texture, { 640,90 }, { 64,90 }, { 90,90 }, { 610,90 }));
	num100_.reset(Sprite::Create(texture, { 640,90 }, { 64,90 }, { 90,90 }, { 540,90 }));


	texture = TextureManager::LoadTex("resources/Texture/AL/black.png").texNum;
	backScreen_.reset(Sprite::Create(texture, { 64,64 }, { 64,64 }, { 1280,720 }));

	texture = TextureManager::LoadTex("resources/Texture/AL/result.png").texNum;
	resultText_.reset(Sprite::Create(texture, { 320,90 }, { 320,90 }, { 830,290 }));


	brokenBody_ = BrokenBody::GetInstance();

	texture = TextureManager::LoadTex(white).texNum;
	sceneC_.reset(Sprite::Create(texture, { 1,1 }, { 1,1 }, { 1280,720 }));
	sceneC_->SetMaterialDataColor({ 0,0,0,1 });

	bgmGame_ = AudioManager::LoadSoundNum("game");

	bgmClear_ = AudioManager::LoadSoundNum("clear");



	peM_ = std::make_unique<ParticleManager>();

}

GameScene::~GameScene() {
	enemies_.clear();
	brokenBody_->Finalize();
}

void GameScene::Initialize() {

	scene_ = Game;
	limitMinute = maxmilitMinute;

	player_->Initialize();
	plane_->Initialize();

	//初期化
	camera_->Initialize();
	//各種設定
	camera_->SetTarget(&player_->GetWorld());
	camera_->SetCameraDirection(-40.0f);
	player_->SetCamera(camera_);

	enemyPopManager_->Initialzie();
	enemyPopManager_->SetPlayerWorld(&player_->GetWorld());

	enemies_.clear();

	backScreen_->SetColorAlpha(0);
	resultText_->SetColorAlpha(0);

	alphaNum_ = 0;
	nowFrameCountEnemy_ = 0;

	num1_->SetPosition({ 680,90 });
	num1_->SetScale({ 90,90 });

	num10_->SetPosition({ 610,90 });
	num10_->SetScale({ 90,90 });

	num100_->SetPosition({ 540,90 });
	num100_->SetScale({ 90,90 });

	serchComplete_ = false;


	brokenBody_->Initialize();

	sceneXhangeCount_ = maxSceneChangeCount_;
	isSceneChange_ = false;
	preSceneChange_ = false;
	sceneC_->SetColorAlpha(1);

	AudioManager::GetInstance()->StopAllSounds();
	AudioManager::PlaySoundData(bgmGame_, 0.08f);

	PEHSVFilter::materialData_->effective = 1.0f;
	PEHSVFilter::materialData_->saturation = 0.3f;

	PEVignetting::materialData_->darkness = 0.3f;

	peM_->Initialize(TextureManager::LoadTex("resources/Texture/CG/circle.png").texNum);
	peM_->SetOnlyImpact(true);
	EmiterSphere*emit = peM_->GetEmiterData();

	emit->color =  {0,0,1,1} ;
}



void GameScene::Update() {

	PostEffectManager::GetInstance()->Debug();

	peM_->Update();
	switch (scene_) {
	case GameScene::Game:

#pragma region ゲームシーン
		//デバッグウィンドウ表示
		DebugWindows();

		//プレイヤー更新
		player_->Update();

		//カメラ更新
		Vector2 stick;
		if (input_->IsControllerActive()) {
			stick = input_->GetjoyStickR();
		}
		else {
			Vector3 sti = input_->GetAllArrowKey();
			stick = { sti.x,sti.z };
		}
		stick.Normalize();
		stick.x *= xrotateNum;
		stick.y *= yrotatenum * -1.0f;
		camera_->AddCameraR_X(stick.y);
		camera_->AddCameraR_Y(stick.x);

		CameraShake();



		enemyPopManager_->Update();
		//敵の生成処理
		if (std::unique_ptr<ALEnemy>newEnemy = enemyPopManager_->PopEnemy()) {
			enemies_.push_back(std::move(newEnemy));
		}

		for (auto& enemy : enemies_) {
			if (!enemy->GetDead()) {
				enemy->Update();
			}
		}

		//MapLoader::GetInstance()->UpdateLevelData();

		Collision();

		LimitUI();

		brokenBody_->Update();

#pragma endregion

		break;
	case GameScene::Clear:
		num1_->DrawDebugImGui("num1");
		num10_->DrawDebugImGui("num10");
		num100_->DrawDebugImGui("num100");
		resultText_->DrawDebugImGui("taosita");

		break;
	default:
		break;
	}
	camera_->Update();

	SceneChange();
}

void GameScene::Draw() {

	//地面
	//MapLoader::GetInstance()->DrawLevelData();

	//敵の旗
	enemyPopManager_->Draw();

	//敵
	for (auto& enemy : enemies_) {
		if (!enemy->GetDead()) {
			enemy->Draw();
		}
	}

	//プレイヤー
	player_->Draw();

	brokenBody_->Draw();

	//インスタンシングのモデルを全描画
	InstancingModelManager::GetInstance()->DrawAllModel();

	peM_->Draw();
	player_->DrawParticle();

	//PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kVinetting, true);



	int Count = 0;
	switch (scene_) {
	case GameScene::Game:
		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kLightOutline, true);

		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kVinetting, true);

		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kHSV, true);


		for (auto& enemy : enemies_) {
			if (enemy->GetDead()) {
				Count++;
			}
		}
		if (Count >= 200) {
			PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kRadialBlur, true);
		}

		UIUpdateDraw();
		break;
	case GameScene::Clear:
		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kDepthBasedOutline, true);

		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kGaussianFilter, true);


		PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kGrayScale, true);


		ClearUIUpdate();


		break;
	default:
		break;
	}

	sceneC_->Draw();
}

void GameScene::DebugWindows() {

#ifdef _DEBUG

	//カメラのデバッグ表示
	camera_->DrawDebugWindow("camera");

	//プレイヤーデバッグ表示
	player_->DebugWindow("player");

	plane_->DebagWindow();

	skillSp_->DrawDebugImGui("skill");
	BButton_->DrawDebugImGui("BButton");
	punchSp_->DrawDebugImGui("punch");

#endif // _DEBUG

}

void GameScene::Collision() {



	int e1Num = 0;
	for (auto& enemy : enemies_) {
		if (!enemy->GetDead()) {

			//プレイヤーの攻撃ヒット処理
			if (player_->IsPlayerATK()) {
				if (enemy->Collision(player_->GetCollider())) {
					ShakeStart(3);
					peM_->SpawnE(enemy->GetWorld().GetWorldTranslate());
				}
			}

			enemy->OshiDashi(player_->GetCollider());

			//敵同士の当たり判定
			int e2Num = 0;
			for (auto& enemy2 : enemies_) {
				if (!enemy2->GetDead() && !enemy2->isHit() && e1Num != e2Num) {
					Vector3 backV = enemy->OshiDashi(enemy2->GetCollider());
					enemy2->AddTranslate(-backV);
				}
				e2Num++;
			}
		}

		//マップの押し出し
		//Vector3 backV;
		//if (MapLoader::GetInstance()->IsCollisionMap(enemy->GetCollider(), backV)) {
		//	enemy->PushBack(backV);
		//}

		e1Num++;
	}

	//あたらなくなるまで処理

	//Vector3 backV;
	//if (MapLoader::GetInstance()->IsCollisionMap(player_->GetCollider(), backV)) {

	//	player_->OnCollisionBack(backV);

	//}




}

void GameScene::SceneChange() {

	switch (scene_) {
	case GameScene::Game:
		if (limitMinute-- <= 0) {
			scene_ = Clear;
			AudioManager::GetInstance()->StopAllSounds();
			AudioManager::PlaySoundData(bgmClear_, 0.08f);

		}


		//デバッグ用シーンチェンジ
#ifdef _DEBUG
		if (input_->TriggerKey(DIK_P)) {
			scene_ = Clear;


		}
#endif // _DEBUG


		break;
	case GameScene::Clear:
		if (serchComplete_) {
			if (input_->TriggerKey(DIK_SPACE) || input_->IsTriggerButton(kButtonB)) {
				isSceneChange_ = true;
			}
		}
		break;
	default:
		break;
	}

	if (input_->TriggerKey(DIK_ESCAPE)) {
		leaveGame = true;
	}

	if (isSceneChange_) {

		float alpha = float(sceneXhangeCount_ / maxSceneChangeCount_);

		sceneC_->SetColorAlpha(alpha);

		if (sceneXhangeCount_++ >= maxSceneChangeCount_) {
			sceneC_->SetColorAlpha(1);
			sceneNo = TITLE;

		}
	}
	else {

		if (!preSceneChange_) {

			float alpha = float(sceneXhangeCount_ / maxSceneChangeCount_);

			sceneC_->SetColorAlpha(alpha);

			if (sceneXhangeCount_-- <= 0) {
				sceneXhangeCount_ = 0;
				sceneC_->SetColorAlpha(0);
				preSceneChange_ = true;
			}

		}
	}
}

void GameScene::UIUpdateDraw() {

	if (player_->GetConboCount() == 0) {
		BState_ = Wait;
	}
	if (player_->GetConboCount() == 1) {
		BState_ = Punch;
	}if (player_->GetConboCount() == 2) {
		BState_ = Kick;
	}
	if (player_->GetConboCount() == 3) {
		BState_ = Ult;
	}

	switch (BState_) {
	case GameScene::Wait:
		skillSp_->SetPosition(ATKpos);
		skillSp_->SetScale(ATKscale);

		BButton_->SetPosition({ 1155,635 });

		punchSp_->SetTVTranslate({ 0.5f,0 });
		punchSp_->SetPosition(waitATKpos);
		punchSp_->SetScale(waitATKscale);


		skillSp_->Draw();

		punchSp_->Draw();


		break;
	case GameScene::Punch:

		punchSp_->SetTVTranslate({ 0.0f,0.0f });
		punchSp_->SetPosition(ATKpos);
		punchSp_->SetScale(ATKscale);

		kickSp_->SetTVTranslate({ 0.5f,0.0f });
		kickSp_->SetPosition(waitATKpos);
		kickSp_->SetScale(waitATKscale);

		punchSp_->Draw();

		kickSp_->Draw();

		break;
	case GameScene::Kick:
		kickSp_->SetTVTranslate({ 0.0f,0.0f });
		kickSp_->SetPosition(ATKpos);
		kickSp_->SetScale(ATKscale);

		ultSp_->SetTVTranslate({ 0.5f,0.0f });
		ultSp_->SetPosition(waitATKpos);
		ultSp_->SetScale(waitATKscale);

		kickSp_->Draw();

		ultSp_->Draw();


		break;
	case GameScene::Ult:
		ultSp_->SetTVTranslate({ 0.0f,0.0f });
		ultSp_->SetPosition(ATKpos);
		ultSp_->SetScale(ATKscale);


		ultSp_->Draw();



		break;
	default:
		break;
	}

	BButton_->Draw();



	num1_->Draw();
	num10_->Draw();

}

void GameScene::ClearUIUpdate() {


	//数字検索
#pragma region 数計算
	if (!serchComplete_) {

		num1_->SetPosition({ 368,285 });
		num1_->SetScale({ 160,160 });

		num10_->SetPosition({ 238,285 });
		num10_->SetScale({ 160,160 });

		num100_->SetPosition({ 114,285 });
		num100_->SetScale({ 160,160 });

		resultText_->SetPosition({ 830,290 });

		nowFrameCountEnemy_ += frameCountEnemy_;

		serchComplete_ = true;
		int Count = 0;
		for (auto& enemy : enemies_) {
			if (enemy->GetDead()) {
				Count++;

				if (Count >= nowFrameCountEnemy_) {
					serchComplete_ = false;
					break;
				}
			}
		}


		int num1 = Count % 10;


		//一桁目決定

		num1_->SetTVTranslate({ ((float)num1 / 10.0f) - 0.1f ,0 });

		int count2 = (int)(Count - num1) / 10;

		int num2 = count2 % 10;

		if (Count >= 10) {

			num10_->SetTVTranslate({ ((float)num2 / 10.0f) - 0.1f ,0 });

			if (Count >= 100) {
				int count3 = (int)(Count - (num1 + num2 * 10)) / 100;

				int num3 = count3 % 10;

				num100_->SetTVTranslate({ ((float)num3 / 10.0f) - 0.1f ,0 });
			}
			else {
				num100_->SetTVTranslate({ 0.9f, 0 });
			}
		}
		else {
			num100_->SetTVTranslate({ 0.9f, 0 });
			num10_->SetTVTranslate({ 0.9f, 0 });
		}


		if (serchComplete_ == true) {
			int num = 0;
		}
	}
#pragma endregion





	if (alphaNum_ < addAlphaNum_) {
		alphaNum_++;

		float screenAlpha = alphaNum_ / addAlphaNum_;
		if (screenAlpha > screenmaxAlphaNum_) {
			screenAlpha = screenmaxAlphaNum_;
		}
		backScreen_->SetColorAlpha(screenAlpha);


		float resultAlpha = alphaNum_ / addAlphaNum_;
		resultText_->SetColorAlpha(resultAlpha);

	}
	else {
		resultText_->SetColorAlpha(1.0f);

	}


	backScreen_->Draw();
	resultText_->Draw();

	num1_->Draw();
	num10_->Draw();
	num100_->Draw();

}

void GameScene::LimitUI() {

	//分に変換
	int minute = limitMinute / 60;

	int second = (int)minute % 10;

	num1_->SetTVTranslate({ ((float)second / 10.0f) - 0.1f, 0 });

	//二桁目のみの情報取得
	minute = minute - second;

	int minute2 = minute / 10;



	num10_->SetTVTranslate({ ((float)minute2 / 10.0f) - 0.1f,0 });
}

void GameScene::ShakeStart(int count)
{
	if (!isShake_) {
		tempP_ = { 0,0,0 };
		isShake_ = true;
	}

	cameraShakeCount_ += count;

}

void GameScene::CameraShake()
{
	if (isShake_) {

		Vector3 pos = {
		RandomNumber::Get(-0.5f,0.5f),
		RandomNumber::Get(-0.5f,0.5f),
		0
		};

		Vector3 newp = tempP_ + pos;

		camera_->SetMainCameraPos(newp);

		if (cameraShakeCount_-- <= 0) {
			isShake_ = false;
			cameraShakeCount_ = 0;
			camera_->SetMainCameraPos(tempP_);
		}

	}
}
