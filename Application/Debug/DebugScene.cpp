#include "DebugScene.h"
#include"TextureManager/TextureManager.h"
#include"ImGuiManager/ImGuiManager.h"
#include"LightManager/LightManager.h"
#include"PostEffect/PostEffectManager/PostEffectManager.h"

#include"GvariGroup/GvariGroup.h"

DebugScnene::DebugScnene()
{

	input_ = Input::GetInstance();

	camera_ = Camera::GetInstance();

	object = std::make_unique<GameObject>();
	terrain = new GameObject();

	//ball = TextureManager::LoadTex("resources/Texture/CG/rostock_laage_airport_4k.dds").texNum;

	skybox_ = new SkyBoxModel("resources/Texture/CG/rostock_laage_airport_4k.dds");

	//MapLoader::GetInstance()->LoadLevelEditor("untitled",".json");
	//MapLoader::GetInstance()->CreateModel(0);


	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize(TextureManager::LoadTex("resources/Texture/CG/circle.png").texNum);



}

DebugScnene::~DebugScnene() { 
	delete terrain;
	delete skybox_; 
}

void DebugScnene::Initialize()
{
	//object->Initialize("AnimeCube");
	//object->Initialize("sphere");
	//object->Initialize("walk");
	object->Initialize("human");
	//object->Initialize("Boss");

	terrain->Initialize("terrain");

	camera_->Initialize();
	camera_->SetTarget(&object->GetWorld());

	skybox_->Initialize();
	skybox_->world_.scale_ = { 500,500,500 };

	pointLight_ = PointLight();

	particleManager_->SetOnlyImpact(true);

	std::unique_ptr<GVariGroup>gvg = std::make_unique<GVariGroup>("DebugScene");
	gvg->SetTreeData(object->model_->SetDebugParam());
	gvg->SetValue("オブジェクト回転",&object->world_.rotate_);
}

void DebugScnene::Update()
{
	Debug();

	particleManager_->Debug("particle");

	Vector3 move = input_->GetAllArrowKey();
	move.y = input_->GetWASD().z;

	pointLight_.position += move;

	object->Update();
	terrain->Update();
	skybox_->Update();
	camera_->Update();



	LightManager::GetInstance()->SetPLight(pointLight_);
	LightManager::GetInstance()->SetDLight(dLight_);
	
	particleManager_->Update();
	

	if (input_->PushKey(DIK_SPACE)) {
		particleManager_->SpawnE({0,0,0});
	} 

}



void DebugScnene::Draw()
{
	//skybox_->Draw();	

	//terrain->Draw();

	object->Draw();

	//MapLoader::GetInstance()->DrawLevelData();

	InstancingModelManager::GetInstance()->DrawAllModel();

	particleManager_->Draw();
	
	PostEffectManager::GetInstance()->PostEffectDraw(PostEffectManager::kBloom, true);
}

void DebugScnene::Debug()
{

	PostEffectManager::GetInstance()->Debug();
	//PostEffectManager::GetInstance()->Debug(PostEffectManager::kVinetting);

	camera_->DrawDebugWindow("camera");

	ImGui::Begin("Light");

	ImGui::DragInt("ball", &ball);
	ImGui::Text("PointLight");
	ImGui::DragFloat("p light intencity", &pointLight_.intensity, 0.01f);
	ImGui::DragFloat3("p light pos", &pointLight_.position.x, 0.1f);
	ImGui::ColorEdit4("light color", &pointLight_.color.x);
	ImGui::DragFloat("p light radius", &pointLight_.radius, 0.1f);
	ImGui::DragFloat("p light decay", &pointLight_.decay, 0.1f);

	ImGui::Text("DirectionalLight");
	ImGui::DragFloat("d light intencity", &dLight_.intensity, 0.01f);
	ImGui::ColorEdit4("light color", &dLight_.color.x);
	ImGui::DragFloat3("d direction", &dLight_.direction.x, 0.1f);

	ImGui::End();

	dLight_.direction.SetNormalize();
}
