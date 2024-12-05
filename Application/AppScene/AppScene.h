#pragma once
#include"SceneManager/SceneManager.h"
#include"SingleGameObjects/Model.h"
class AppScene {

public:

	AppScene();
	~AppScene();

	//初期化
	void Initialize();

	//ゲーム更新
	void Update();

	//描画
	void Draw();

private:
	//シーンマネージャ
	SceneManager* sceneManager = nullptr;
	
};