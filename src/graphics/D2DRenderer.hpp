#pragma once

#include "Common.hpp"
#include <unordered_map>
#include <string>

namespace xe {

    class D2DRenderer {
    public:
        D2DRenderer();
        ~D2DRenderer();

        bool Initialize();
        void Shutdown();

        bool CreateDeviceResources(HWND hwnd);
        void DiscardDeviceResources();

        void Resize(UINT width, UINT height);

        void BeginDraw();
        HRESULT EndDraw();

        void Clear(const D2D1_COLOR_F& color);
        void DrawLine(D2D1_POINT_2F p0, D2D1_POINT_2F p1, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);
        void DrawRectangle(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);
        void FillRectangle(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color);
        void DrawRoundedRectangle(const D2D1_ROUNDED_RECT& roundedRect, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);
        void FillRoundedRectangle(const D2D1_ROUNDED_RECT& roundedRect, const D2D1_COLOR_F& color);

        void DrawText(
            const std::wstring& text,
            const D2D1_RECT_F& layoutRect,
            const D2D1_COLOR_F& color,
            IDWriteTextFormat* textFormat
        );

        ComPtr<IDWriteTextFormat> CreateTextFormat(
            const std::wstring& fontFamilyName,
            float fontSize,
            DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING,
            DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR
        );

        ID2D1HwndRenderTarget* GetRenderTarget() const { return m_renderTarget.Get(); }
        ID2D1Factory* GetD2DFactory() const { return m_d2dFactory.Get(); }
        IDWriteFactory* GetDWriteFactory() const { return m_dwriteFactory.Get(); }

        float GetDpiScale() const { return m_dpiScale; }
        void UpdateDpi(UINT dpi);

    private:
        ComPtr<ID2D1SolidColorBrush> GetOrCreateBrush(const D2D1_COLOR_F& color);

        ComPtr<ID2D1Factory> m_d2dFactory;
        ComPtr<IDWriteFactory> m_dwriteFactory;
        ComPtr<ID2D1HwndRenderTarget> m_renderTarget;

        // Cached brushes keyed by packed 32-bit color representation
        std::unordered_map<uint32_t, ComPtr<ID2D1SolidColorBrush>> m_brushCache;

        HWND m_hwnd = nullptr;
        float m_dpiScale = 1.0f;
        bool m_initialized = false;
    };

}
