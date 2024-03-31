#pragma once
#include "Includes.h"
#include "RenderPassInput.h"
#include "RenderPassOutput.h"

class RenderPass
{
	friend class RenderGraph;
public:
	RenderPass(const char* name);
	virtual ~RenderPass() = default;

public:
	const char* GetName() const;

	RenderPassInput* GetInput(const char* inputName);
	RenderPassOutput* GetOutput(const char* outputName);

	std::vector<std::unique_ptr<RenderPassOutput>>& GetOutputs();

public:
	void RegisterInput(std::unique_ptr<RenderPassInput> renderInput);
	void RegisterOutput(std::unique_ptr<RenderPassOutput> renderOutput);

	void LinkOutput(const char* outputName, const std::string& linkedResource);

public:
	virtual void Render(class GFX& gfx) const noexcept(!_DEBUG) = 0;
	virtual void CheckPassIntegrity() const;

	virtual void Reset() {};

private:
	std::vector<std::unique_ptr<RenderPassInput>> m_inputs;
	std::vector<std::unique_ptr<RenderPassOutput>> m_outputs;
	const char* m_name;
};

