#pragma once
#include<d3d12.h>
#include<stdint.h>

#include<string>
#include<vector>
#include<map>
#include<dxcapi.h>

#include"struct.h"
#include"Quaternion.h"


Vector3 CalculateValue(const std::vector<KayframeVector3>& keyframes, float time);

Quaternion CalculateValue(const std::vector<KayframeQuaternion>& keyframes, float time);


std::map<std::string,Animation> LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

ModelAllData LoadModelFile(const std::string& directoryPath, const std::string& modelName);

//MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
//ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

//void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

void Update(Skeleton& skeleton);

void Update(SkinCluster& skinCluster, const Skeleton& skeleton);

Skeleton CreateSkeleton(const Node& node);

SkinCluster CreateSkinCluster(ID3D12Device& device, const Skeleton& skeleton, const ModelData& modelData);

ID3D12Resource* CreateUAVBufferResource(ID3D12Device* device, size_t sizeInBytes);

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

