#pragma once
#include<string>
#include<vector>

#include"Math/Vector3.h"
#include"Game/Enemy/Enemy.h"
#include"MapLoader/MapLoader.h"

struct PopData {
	std::string areaName;//出現エリアの名前
	Vector3 areaPosition;//出現エリアの場所
	float popAreaSize;//出現エリアのサイズ
	float PopStartingArea;//出現処理をする範囲
	float PopInterval;//出現間隔
	float maxAreaPop;//エリアでの最大出現数

	float PopIntervalCount = 0;
	float maxAreaPopCount = 0;
};




class EnemyPopManager {

public:

	EnemyPopManager();
	~EnemyPopManager();

	/// <summary>
	/// jsonファイル読み込み
	/// </summary>
	void LoadPopdata();

	void LoadMapItem(const std::string& tag,Leveldata* data);

	void Initialzie();

	void Update();

	void Draw();

	std::unique_ptr<ALEnemy> PopEnemy();

	//プレイヤーの座標取得
	void SetPlayerWorld(const EulerWorldTransform* world) { playerWorld_ = world; }

private:

#pragma region 読み込み用各種変数
	//ファイルのフルパス
	std::string filePath = "resources/jsonfile/enemyPopData.json";

	//最大エリア数
	static const int areaNum = 8;

	//エリアの名前
	std::string popAreaName[areaNum] = {
		"PopArea1",
		"PopArea2",
		"PopArea3",
		"PopArea4",
		"PopArea5",
		"PopArea6",
		"PopArea7",
		"PopArea8"

	};

	//各アイテムのEnumClass
	enum class INE {
		POSITION,
		POPAREASIZE,
		RANGEPOPSTART,
		POPINTERVAL,
		MAXAREAPOP
	};

	//各アイテム名のstring配列
	std::string itemNames[5] = {
		"position",
		"PopAreaSize",
		"PangeWithPopStarting",
		"PopInterval",
		"maxAreaPopCount"
	};
#pragma endregion

	//出現データまとめ
	std::vector<PopData>popDatas_;

	//プレイヤーの座標
	const  EulerWorldTransform* playerWorld_ = nullptr;

	float spawnHeight = 0.0f;

	//モデルにアクセスするタグ
	std::string flag_ = "Flag";

	std::string ring_ = "ring";

	std::vector<EulerWorldTransform>flagWorlds_;

	std::vector<std::unique_ptr<GameObject>>flagModel_;

	std::vector<std::unique_ptr<GameObject>>poleModel_;

};