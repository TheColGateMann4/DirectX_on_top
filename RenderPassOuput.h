#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "ErrorMacros.h"
#include "RenderTarget.h"

class GraphicBuffer;

class RenderPassOutput
{
public:
	RenderPassOutput(const char* name)
		: m_name(name)
	{

	}

	static std::unique_ptr<RenderPassOutput> GetUnique(const char* name)
	{
		return std::move(std::make_unique<RenderPassOutput>(name));
	}

public:
	virtual ~RenderPassOutput() = default;

public:
	virtual std::shared_ptr<Bindable> GetBindable()
	{
		std::string errorString = "This object cannot be used to get bindable. Pass name is: \"";
		errorString += m_name;
		errorString += "\".";

		THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
	}

	virtual std::shared_ptr<GraphicBuffer> GetBuffer()
	{
		std::string errorString = "This object cannot be used to get graphic buffer. Pass name is: \"";
		errorString += m_name;
		errorString += "\".";

		THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
	}

public:
	virtual void CheckInputIntegrity() const
	{

	}

public:
	const char* GetName() const
	{
		return m_name;
	}

private:
	const char* m_name;
};

template<class T>
class RenderPassBindableNewOuput : public RenderPassOutput
{
	static_assert(std::is_base_of_v<Bindable, T>);

public:
	RenderPassBindableNewOuput(const char* name, std::shared_ptr<T>* bindable)
		: RenderPassOutput(name), m_bindable(bindable), m_linked(false)
	{

	}

	static std::unique_ptr<RenderPassBindableNewOuput> GetUnique(const char* name, std::shared_ptr<T>* bindable)
	{
		return std::move(std::make_unique<RenderPassBindableNewOuput<T>>(name, bindable));
	}

public:
	virtual std::shared_ptr<Bindable> GetBindable() noexcept override
	{
		m_linked = true;

		return m_bindable;
	}

public:
	virtual void CheckInputIntegrity() const override
	{
		if (!m_linked)
		{
			std::string errorString = "This output was not bound. Object name is: \"";
			errorString += this->GetName();
			errorString += "\".";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}
	}

private:
	bool m_linked;
	std::shared_ptr<Bindable>* m_bindable;
};

template<class T>
class RenderPassBufferNewOutput : public RenderPassOutput
{
public:
	RenderPassBufferNewOutput(const char* name, std::shared_ptr<T>* buffer)
		: RenderPassOutput(name),m_buffer(buffer), m_linked(false)
	{

	}

	static std::unique_ptr<RenderPassBufferNewOutput> GetUnique(const char* name, std::shared_ptr<T>* buffer)
	{
		return std::move(std::make_unique<RenderPassBufferNewOutput<T>>(name, buffer));
	}

public:
	virtual std::shared_ptr<GraphicBuffer> GetBuffer() override
	{
		if (m_linked)
		{
			std::string errorString = "Buffer from this object was already bound. Object name is: \"";
			errorString += this->GetName();
			errorString += "\".";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}

		m_linked = true;

		return std::dynamic_pointer_cast<GraphicBuffer>(*m_buffer);
	}
	virtual std::shared_ptr<Bindable> GetBindable() noexcept override
	{
		m_linked = true;

		return std::dynamic_pointer_cast<RenderTargetWithTexture>(*m_buffer);
	}

public:
	virtual void CheckInputIntegrity() const override
	{
		if (!m_linked)
		{
			std::string errorString = "This output was not bound. Object name is: \"";
			errorString += this->GetName();
			errorString += "\".";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}
	}

private:
	std::shared_ptr<T>* m_buffer;
	bool m_linked;
};