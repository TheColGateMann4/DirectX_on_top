#pragma once
#include "Includes.h"
#include "Bindable.h"
#include <memory>
#include <type_traits>
#include <unordered_map>

class BindableList
{
public:
	template <class T, class ...Params>
	static std::shared_ptr<T> GetBindable(GFX& gfx, Params&& ...params)
	{
		static_assert(std::is_base_of<Bindable, T>::value, "passed bindable must be delivered from Bindable class");
		return m_Get().m_GetBindable<T>(gfx, std::forward<Params>(params)...);
	}

private:
	template <class T, class ...Params>
	std::shared_ptr<T> m_GetBindable(GFX &gfx, Params&& ...params)
	{
		const auto uid = typeid(T).name() + std::string("@") + T::GetUID(std::forward<Params>(params)...);
		const auto pBind = m_bindables.find(uid);

		if (pBind == m_bindables.end())
		{
			std::shared_ptr<T> bind = std::make_shared<T>(gfx, std::forward<Params>(params)...);
			m_bindables[uid] = bind;
			return bind;
		}
			
		return std::static_pointer_cast<T, Bindable>(pBind->second);
	}

	static BindableList& m_Get()
	{
		static BindableList list;
		return list;
	}

private:
	std::unordered_map<std::string, std::shared_ptr<Bindable>> m_bindables;
};