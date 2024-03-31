#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "ErrorMacros.h"
#include "RenderTarget.h"

class GraphicBuffer;

class RenderPassInput
{
public:
	RenderPassInput(const char* name)
		: m_name(name)
	{

	}

	static std::unique_ptr<RenderPassInput> GetUnique(const char* name)
	{
		return std::move(std::make_unique<RenderPassInput>(name));
	}

public:
	virtual ~RenderPassInput() = default;

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
		std::string errorString = "This object cannot be used to get bindable. Pass name is: \"";
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
class RenderPassBindableInput : public RenderPassInput
{
	static_assert(std::is_base_of_v<Bindable, T> || typeid(Bindable) == typeid(T));

public:
	RenderPassBindableInput(const char* name, std::shared_ptr<T>* bindable)
		: RenderPassInput(name), m_bindable(bindable), m_linked(false)
	{

	}

	static std::unique_ptr<RenderPassBindableInput> GetUnique(const char* name, std::shared_ptr<T>* bindable)
	{
		return std::move(std::make_unique<RenderPassBindableInput<T>>(name, bindable));
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
class RenderPassBufferInput : public RenderPassInput
{
public:
	RenderPassBufferInput(const char* name, std::shared_ptr<T>* buffer)
		: RenderPassInput(name),m_buffer(buffer), m_linked(false)
	{

	}

	static std::unique_ptr<RenderPassBufferInput> GetUnique(const char* name, std::shared_ptr<T>* buffer)
	{
		return std::move(std::make_unique<RenderPassBufferInput<T>>(name, buffer));
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
			std::string errorString = "This intput was not bound. Object name is: \"";
			errorString += this->GetName();
			errorString += "\".";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}
	}

private:
	std::shared_ptr<T>* m_buffer;
	bool m_linked;
};