#pragma once
#include "Bindable.h"
#include "Vertex.h"
#include "ErrorMacros.h"
#include "BindableList.h"
#include "DynamicConstantBuffer.h"

class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer();

	ConstantBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, std::vector<TargetShaderBufferBinding> targetShaders);

	ConstantBuffer(GFX& gfx, DynamicConstantBuffer::BufferLayout& layout, std::vector<TargetShaderBufferBinding> targetShader);

protected:
	ConstantBuffer(std::vector<TargetShaderBufferBinding> targetShader);

public:
	void Update(GFX& gfx, const DynamicConstantBuffer::BufferData& bufferData);

	void Bind(GFX& gfx) noexcept override;
	
public:
	const ConstantBuffer& operator=(ConstantBuffer& toAssign);

public:
	std::vector<TargetShaderBufferBinding> GetShaders() const;

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	std::vector<TargetShaderBufferBinding> m_targetShaders;
};




class CachedBuffer : public ConstantBuffer
{
public:
	CachedBuffer();

	CachedBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, std::vector<TargetShaderBufferBinding> targetShaders);

	CachedBuffer(GFX& gfx, DynamicConstantBuffer::BufferLayout& layout, std::vector<TargetShaderBufferBinding> targetShaders);

public:
	DynamicConstantBuffer::BufferData GetBufferData();

	void Update(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData);

	using ConstantBuffer::Bind;

	void SetBufferID(const char* bufferID);

public:
	static std::shared_ptr<CachedBuffer> GetBindable(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, std::vector<TargetShaderBufferBinding> targetShaders);

	static std::shared_ptr<CachedBuffer> GetSharedBindable(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, std::vector<TargetShaderBufferBinding> targetShaders, const char* bufferID);

	static std::shared_ptr<CachedBuffer> GetBindableWithoutCreation(GFX& gfx, const char* bufferID);

	std::string GetLocalUID() const noexcept override;

	static std::string GetStaticUID(const char* bufferID) noexcept;

private:
	static std::string GenerateUID(const char* bufferID);

private:
	DynamicConstantBuffer::BufferData constBufferData;
	std::string m_bufferID = "";
};





class NonCachedBuffer : public ConstantBuffer
{
public:
	using ConstantBuffer::ConstantBuffer;

	NonCachedBuffer(GFX& gfx, UINT32 structureSize, UINT32 structuresInBufferNum, std::vector<TargetShaderBufferBinding> targetShader);

public:
	static std::shared_ptr<NonCachedBuffer> GetBindable(GFX& gfx, DynamicConstantBuffer::BufferLayout& bufferLayout, std::vector<TargetShaderBufferBinding> targetShaders);

	static std::shared_ptr<NonCachedBuffer> GetBindable(GFX& gfx, UINT32 structureSize, UINT32 structuresInBufferNum, std::vector<TargetShaderBufferBinding> targetShader);

	NonCachedBuffer& operator=(NonCachedBuffer toAssign);
};