#pragma once
#include "Includes.h"
#include "RenderPassInput.h"

class Bindable;
class RenderPassInput;

class RenderPassOutput
{
	friend class RenderPass;
protected:
	RenderPassOutput(const char* name);

public:
	virtual ~RenderPassOutput() = default;

public:
	void Link(std::string& objectName, std::string& passName);

	virtual void Bind(RenderPassInput& renderPassInput);

public:
	const char* GetOutputName() const; // output of output is input
	std::string GetLinkObjectName() const;
	std::string GetLinkObjectOwnerName() const;

	std::string GetLocalInfo() const;

public:
	virtual void CheckOutputIntegrity() const
	{

	}

private:
	const char* m_name;
	std::string m_linkObjectName;
	std::string m_linkPassName;
};

template<class T>
class RenderPassBindableOutput : public RenderPassOutput
{
	static_assert(std::is_base_of_v<Bindable, T> || typeid(Bindable) == typeid(T));

public:
	RenderPassBindableOutput(const char* name, T* bindable)
		: RenderPassOutput(name), m_bindable(bindable), m_linked(false)
	{

	}

	static std::unique_ptr<RenderPassBindableOutput<Bindable>> GetUnique(const char* name, T* bindable)
	{
		return std::move(std::make_unique<RenderPassBindableOutput<Bindable>>(name, bindable));
	}

public:
	virtual void Bind(RenderPassInput& renderPassInput) override
	{
		std::shared_ptr<Bindable> pRenderPassInputBindables = renderPassInput.GetBindable();

		if (pRenderPassInputBindables == nullptr)
		{
			std::string errorString = "Buffer of bindables passed to output object: \"";
			errorString += this->GetOutputName();
			errorString += "\". From input object with name: \"";
			errorString += renderPassInput.GetName();
			errorString += "\". was null";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}

		*m_bindable = *dynamic_cast<T*>(pRenderPassInputBindables.get());
		
		m_linked = true;
	}

public:
	virtual void CheckOutputIntegrity() const override
	{
		if (!m_linked)
		{
			std::string errorString = "This output was not bound. Object name is: \"";
			errorString += this->GetOutputName();
			errorString += "\".";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}
	}

private:
	T* m_bindable;
	bool m_linked;
};

// When we are pushing bindable output to an array, regular RenderPassBindableOutput wouldn't work.
// Since it takes pointer to bindable, which in that case would take nullptr

class RenderPassEmptyBindableOutput : public RenderPassOutput
{
public:
	RenderPassEmptyBindableOutput(const char* name, std::vector<std::shared_ptr<Bindable>>* bindables, size_t bindableIndex)
		: RenderPassOutput(name), m_bindables(bindables), m_bindableIndex(bindableIndex), m_linked(false)
	{

	}

	static std::unique_ptr<RenderPassEmptyBindableOutput> GetUnique(const char* name, std::vector<std::shared_ptr<Bindable>>* bindables, size_t bindableIndex)
	{
		return std::move(std::make_unique<RenderPassEmptyBindableOutput>(name, bindables, bindableIndex));
	}

public:
	virtual void Bind(RenderPassInput& renderPassInput) override
	{
		std::shared_ptr<Bindable> pRenderPassInputBindables = renderPassInput.GetBindable();

		if (pRenderPassInputBindables == nullptr)
		{
			std::string errorString = "Buffer of bindables passed to output object: \"";
			errorString += this->GetOutputName();
			errorString += "\". From input object with name: \"";
			errorString += renderPassInput.GetName();
			errorString += "\". was null";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}

		m_bindables->at(m_bindableIndex) = pRenderPassInputBindables;
		
		m_linked = true;
	}

public:
	virtual void CheckOutputIntegrity() const override
	{
		if (!m_linked)
		{
			std::string errorString = "This output was not bound. Object name is: \"";
			errorString += this->GetOutputName();
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
class RenderPassBufferOutput : public RenderPassOutput
{
	static_assert(std::is_base_of_v<GraphicBuffer, T> || typeid(GraphicBuffer) == typeid(T));

public:
	RenderPassBufferOutput(const char* name, std::shared_ptr<T>* buffer)
		: RenderPassOutput(name), m_buffer(buffer), m_linked(false)
	{
		
	}

	static std::unique_ptr<RenderPassBufferOutput> GetUnique(const char* name, std::shared_ptr<T>* buffer)
	{
		return std::move(std::make_unique<RenderPassBufferOutput<T>>(name, buffer));
	}

public:
	virtual void Bind(RenderPassInput& renderPassInput) override
	{
		std::shared_ptr<T> pBuffer = std::dynamic_pointer_cast<T>(renderPassInput.GetBuffer());

		if (pBuffer == nullptr)
		{
			std::string errorString = "Buffer wasn't passed to output.";
			errorString += GetLocalInfo();

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}

		m_linked = true;
		*m_buffer = pBuffer;
	}

public:
	virtual void CheckOutputIntegrity() const override
	{
		if (!m_linked)
		{
			std::string errorString = "This output was not bound.";
			errorString += GetLocalInfo();

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}
	}

private:
	std::shared_ptr<T>* m_buffer;
	bool m_linked;
};