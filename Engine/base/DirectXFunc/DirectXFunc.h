#pragma once
#include"WinApp/WinApp.h"

#include"Vector4.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<stdint.h>
#include<wrl.h>
#include<chrono>


class VignettingPSO;

class DirectXFunc {
public://シングルトンパターン
	static DirectXFunc* GetInstance();

private://シングルトンパターン

	DirectXFunc() = default;
	~DirectXFunc() = default;
	DirectXFunc(const DirectXFunc& o) = delete;
	const DirectXFunc& operator=(const DirectXFunc& o) = delete;

public:


	/// <summary>
	/// イニシャライズ
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WindowApp* winApp);


	void PreDraw();

	
	
	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// キックコマンド
	/// </summary>
	void KickCommand();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	ID3D12Resource* CreateRenderTextureResource( DXGI_FORMAT format, const Vector4& clearColor);


#pragma region ゲッター
	ID3D12Device* GetDevice()const { return device.Get(); }

	ID3D12GraphicsCommandList* GetCMDList()const { return commandList.Get(); }

	DXGI_SWAP_CHAIN_DESC1 GetswapChainDesc()const { return swapChainDesc; }

	D3D12_RENDER_TARGET_VIEW_DESC GetrtvDesc()const { return rtvDesc; }

	IDXGISwapChain4* GetSwapChain() { return swapChain.Get(); }
#pragma endregion

private://メンバ関数

	/// <summary>
	/// FPS固定初期化
	/// </summary>
	void FixFPSInitialize();

	/// <summary>
	/// FPS固定更新
	/// </summary>
	void UpdateFixFPS();

#pragma region クラス内関数
	void D3D12Devicenitialize();

	void CommandInitialize();

	void SwapChainInitialize();

	void RTVInitialize();

	void DSVInitialize();

	void FenceInitialize();

	void RenderTextureInitialize();
#pragma endregion

private://メンバ変数


	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
#endif // _DEBUG

	//ポインタ
	WindowApp* winApp_ = nullptr;



	ComPtr<IDXGIAdapter4> useAdapter = nullptr;

	//dxgiファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	//デバイス
	ComPtr <ID3D12Device> device = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain = nullptr;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//スワップチェーンリソース
	ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };


	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	//描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;


	//フェンス
	ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	//イベント
	HANDLE fenceEvent;




	//fps固定用
	std::chrono::steady_clock::time_point reference_;

	//開放チェックエラーで実行を止めるか	
	const bool isAssertForgetReleasing_ = true;


};