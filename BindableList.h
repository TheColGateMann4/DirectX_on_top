#pragma once
#include "Includes.h"
#include "Bindable.h"
#include <memory>
#include <type_traits>
#include <unordered_map>
#include "ErrorMacros.h"

class BindableList
{
public:
	template <class T, class ...Params>
	static std::shared_ptr<T> GetBindable(GFX& gfx, Params&& ...params)
	{
		static_assert(std::is_base_of<Bindable, T>::value, "passed bindable must be delivered from Bindable class");

		return m_Get().m_GetBindable<T>(gfx, std::forward<Params>(params)...);
	}

	template <class T, class ...Params>
	static std::shared_ptr<T> GetBindableWithoutCreation(GFX& gfx, Params&& ...params)
	{
		static_assert(std::is_base_of<Bindable, T>::value, "passed bindable must be delivered from Bindable class");

		return m_Get().m_GetBindableWithoutCreation<T>(gfx, std::forward<Params>(params)...);
	}

	template <class T>
	static void PushBindable(std::shared_ptr<T> bind, const char* uid)
	{
		static_assert(std::is_base_of<Bindable, T>::value, "passed bindable must be delivered from Bindable class");
		return m_Get().m_PushBindable(bind, uid);
	}

private:
	template <class T, class ...Params>
	std::shared_ptr<T> m_GetBindable(GFX &gfx, Params&& ...params)
	{
		const auto uid = typeid(T).name() + std::string("@") + T::GetStaticUID(std::forward<Params>(params)...);
		const auto pBind = m_bindables.find(uid);

		if (pBind == m_bindables.end())
		{
			std::shared_ptr<T> bind = std::make_shared<T>(gfx, std::forward<Params>(params)...);
			m_bindables[uid] = bind;
			return bind;
		}
			
		return std::static_pointer_cast<T, Bindable>(pBind->second);
	}

	template <class T, class ...Params>
	std::shared_ptr<T> m_GetBindableWithoutCreation(GFX &gfx, Params&& ...params)
	{
		const auto uid = typeid(T).name() + std::string("@") + T::GetStaticUID(std::forward<Params>(params)...);
		const auto pBind = m_bindables.find(uid);

		if (pBind == m_bindables.end())
			THROW_INTERNAL_ERROR("Bindable could not be found. Used no creation option.");
			
		return std::static_pointer_cast<T, Bindable>(pBind->second);
	}

	template <class T>
	void m_PushBindable(std::shared_ptr<T> bind, const char* uid)
	{
		std::string finalUid = typeid(T).name() + std::string("@") + uid;
		m_bindables[finalUid] = bind;
	}

	static BindableList& m_Get()
	{
		static BindableList list;
		return list;
	}

private:
	std::unordered_map<std::string, std::shared_ptr<Bindable>> m_bindables;
};