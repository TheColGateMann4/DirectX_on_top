#pragma once

#include "hlsl.g.h"

namespace winrt::DirectXontop::implementation
{
    struct hlsl : hlslT<hlsl>
    {
        hlsl();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::DirectXontop::factory_implementation
{
    struct hlsl : hlslT<hlsl, implementation::hlsl>
    {
    };
}
