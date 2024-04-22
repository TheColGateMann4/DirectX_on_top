#pragma once
#include "Includes.h"
#include "RenderPassOuput.h"
#include "RenderPassInput.h"

class Scene;

class RenderPass
{
	friend class RenderGraph;
public:
	RenderPass(const char* name);
	virtual ~RenderPass() = default;

public:
	const char* GetName() const;

	RenderPassOutput* GetOutput(const char* outputName);
	RenderPassInput* GetInput(const char* inputName);

	std::vector<std::unique_ptr<RenderPassInput>>& GetInputs();

public:
	void RegisterOutput(std::unique_ptr<RenderPassOutput> renderOutput);
	void RegisterInput(std::unique_ptr<RenderPassInput> renderInput);

	void LinkInput(const char* inputName, const std::string& linkedResource);

public:
	virtual void Render(class GFX& gfx) const noexcept(!IS_DEBUG) = 0;
	virtual void CheckPassIntegrity() const;

	virtual void Reset() {};

	void SetScenePtr(Scene* scene);

private:
	std::vector<std::unique_ptr<RenderPassOutput>> m_outputs;
	std::vector<std::unique_ptr<RenderPassInput>> m_inputs;
	const char* m_name;

protected:
	Scene* m_scene;
};

