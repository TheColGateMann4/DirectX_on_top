#include "pch.h"
#include "hlsl.h"
#if __has_include("hlsl.g.cpp")
#include "hlsl.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::DirectXontop::implementation
{
    hlsl::hlsl()
    {
        InitializeComponent();
    }

    int32_t hlsl::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void hlsl::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void hlsl::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
