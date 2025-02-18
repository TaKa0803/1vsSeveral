#pragma once
#include"GVariDatas.h"

//すべてのGVariablesの管理  
class GlobalVariableManager {
	
public://シングルトン
	static GlobalVariableManager* GetInstance();

private:
	GlobalVariableManager() = default;
	~GlobalVariableManager() = default;
	GlobalVariableManager(const GlobalVariableManager& o) = delete;
	const GlobalVariableManager& operator=(const GlobalVariableManager& o) = delete;

public://パブリック関数

	//値の追加
	void SetGroup(const std::string& group,GroupData&data);

	//更新処理
	void Update();

	//セーブデータすべて読み込み
	void LoadAllSaveData();



	//全セーブデータのセット
	void SetLoadAllData();

	//グループセーブデータのセット
	void SetLoadGroupData(const std::string& groupName);
private:

	//グループデータデータ保存
	void SaveGroupItemData(const std::string& groupName);

	//ファイルよみこみ(１ファイル
	void LoadGroupData(const std::string& groupName);
private:
	std::string baseName_ = "Game";



	std::map<std::string, GroupData> datas_;

	std::map<std::string, SavedGroupData> saveDatas_;


	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";

	//ノード表記にまとめ始める量
	size_t nodeSize_ = 3;
};