#pragma once
#include "Includes.h"
#include "RenderPassOuput.h"

class Bindable;
class RenderPassOutput;

class RenderPassInput
{
	friend class RenderPass;
protected:
	RenderPassInput(const char* name);

public:
	virtual ~RenderPassInput() = default;

public:
	void Link(std::string& objectName, std::string& passName);

	virtual void Bind(RenderPassOutput& renderPassOutput);

public:
	const char* GetInputName() const;
	std::string GetLinkObjectName() const;
	std::string GetLinkObjectOwnerName() const;

	std::string GetLocalInfo() const;

public:
	virtual void CheckInputIntegrity() const
	{

	}

private:
	const char* m_name;
	std::string m_linkObjectName;
	std::string m_linkPassName;
};

template<class T>
class RenderPassBindableInput : public RenderPassInput
{
	static_assert(std::is_base_of_v<Bindable, T>);

public:
	RenderPassBindableInput(const char* name, T* bindable)
		: RenderPassInput(name), m_bindable(bindable), m_linked(false)
	{

	}

	static std::unique_ptr<RenderPassBindableInput<Bindable>> GetUnique(const char* name, T* bindable)
	{
		return std::move(std::make_unique<RenderPassBindableInput<Bindable>>(name, bindable));
	}

public:
	virtual void Bind(RenderPassOutput& renderPassInput) override
	{
		std::shared_ptr<Bindable> pRenderPassInputBindables = renderPassInput.GetBindable();

		if (pRenderPassInputBindables == nullptr)
		{
			std::string errorString = "Buffer of bindables passed to output object: \"";
			errorString += this->GetInputName();
			errorString += "\". From input object with name: \"";
			errorString += renderPassInput.GetName();
			errorString += "\". was null";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}

		*m_bindable = *dynamic_cast<T*>(pRenderPassInputBindables.get());
		
		m_linked = true;
	}

public:
	virtual void CheckInputIntegrity() const override
	{
		if (!m_linked)
		{
			std::string errorString = "This input was not bound. Object name is: \"";
			errorString += this->GetInputName();
			errorString += "\".";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}
	}

private:
	T* m_bindable;
	bool m_linked;
};

// When we are pushing bindable input to an array, regular RenderPassBindableInput wouldn't work.
// Since it takes pointer to bindable, which in that case would take nullptr

class RenderPassEmptyBindableInput : public RenderPassInput
{
public:
	RenderPassEmptyBindableInput(const char* name, std::vector<std::shared_ptr<Bindable>>* bindables, size_t bindableIndex)
		: RenderPassInput(name), m_bindables(bindables), m_bindableIndex(bindableIndex), m_linked(false)
	{

	}

	static std::unique_ptr<RenderPassEmptyBindableInput> GetUnique(const char* name, std::vector<std::shared_ptr<Bindable>>* bindables, size_t bindableIndex)
	{
		return std::move(std::make_unique<RenderPassEmptyBindableInput>(name, bindables, bindableIndex));
	}

public:
	virtual void Bind(RenderPassOutput& renderPassOutput) override
	{
		std::shared_ptr<Bindable> pRenderPassOutputBindables = renderPassOutput.GetBindable();

		if (pRenderPassOutputBindables == nullptr)
		{
			std::string errorString = "Buffer of bindables passed to input object: \"";
			errorString += this->GetInputName();
			errorString += "\". From output object with name: \"";
			errorString += renderPassOutput.GetName();
			errorString += "\". was null";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}

		m_bindables->at(m_bindableIndex) = pRenderPassOutputBindables;
		
		m_linked = true;
	}

public:
	virtual void CheckInputIntegrity() const override
	{
		if (!m_linked)
		{
			std::string errorString = "This input was not bound. Object name is: \"";
			errorString += this->GetInputName();
			errorString += "\".";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}
	}

private:
	std::vector<std::shared_ptr<Bindable>>* m_bindables;
	size_t m_bindableIndex;
	bool m_linked;
};


template<class T>
class RenderPassBufferInput : public RenderPassInput
{
	static_assert(std::is_base_of_v<GraphicBuffer, T>);

public:
	RenderPassBufferInput(const char* name, std::shared_ptr<T>* buffer)
		: RenderPassInput(name), m_buffer(buffer), m_linked(false)
	{
		
	}

	static std::unique_ptr<RenderPassBufferInput> GetUnique(const char* name, std::shared_ptr<T>* buffer)
	{
		return std::move(std::make_unique<RenderPassBufferInput<T>>(name, buffer));
	}

public:
	virtual void Bind(RenderPassOutput& renderPassInput) override
	{
		std::shared_ptr<T> pBuffer = std::dynamic_pointer_cast<T>(renderPassInput.GetBuffer());

		if (pBuffer == nullptr)
		{
			std::string errorString = "Buffer wasn't passed to input.";
			errorString += GetLocalInfo();

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}

		m_linked = true;
		*m_buffer = pBuffer;
	}

public:
	virtual void CheckInputIntegrity() const override
	{
		if (!m_linked)
		{
			std::string errorString = "This input was not bound.";
			errorString += GetLocalInfo();

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}
	}

private:
	std::shared_ptr<T>* m_buffer;
	bool m_linked;
};