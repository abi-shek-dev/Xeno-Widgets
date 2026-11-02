#include "D2DRenderer.hpp"
#include "../core/Logger.hpp"

namespace xe {

    static uint32_t PackColor(const D2D1_COLOR_F& color) {
        auto r = static_cast<uint8_t>(color.r * 255.0f);
        auto g = static_cast<uint8_t>(color.g * 255.0f);
        auto b = static_cast<uint8_t>(color.b * 255.0f);
        auto a = static_cast<uint8_t>(color.a * 255.0f);
        return (static_cast<uint32_t>(a) << 24) |
               (static_cast<uint32_t>(r) << 16) |
               (static_cast<uint32_t>(g) << 8)  |
               (static_cast<uint32_t>(b));
    }

    D2DRenderer::D2DRenderer() = default;

    D2DRenderer::~D2DRenderer() {
        Shutdown();
    }

    bool D2DRenderer::Initialize() {
        if (m_initialized) return true;

        D2D1_FACTORY_OPTIONS options{};
        #if defined(_DEBUG)
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
        #endif

        HRESULT hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory),
            &options,
            reinterpret_cast<void**>(m_d2dFactory.GetAddressOf())
        );

        if (FAILED(hr)) {
            XE_LOG_ERROR("Failed to initialize Direct2D Factory");
            return false;
        }

        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf())
        );

        if (FAILED(hr)) {
            XE_LOG_ERROR("Failed to initialize DirectWrite Factory");
            return false;
        }

        m_initialized = true;
        XE_LOG_INFO("Direct2D and DirectWrite Factories initialized successfully");
        return true;
    }

    void D2DRenderer::Shutdown() {
        DiscardDeviceResources();
        m_dwriteFactory.Reset();
        m_d2dFactory.Reset();
        m_initialized = false;
    }

    bool D2DRenderer::CreateDeviceResources(HWND hwnd) {
        m_hwnd = hwnd;
        if (!m_d2dFactory) return false;
        if (m_renderTarget) return true;

        RECT rc{};
        GetClientRect(hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

        // Alpha mode premultiplied enables transparent desktop widgets in subsequent phases
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );

        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
            hwnd,
            size,
            D2D1_PRESENT_OPTIONS_IMMEDIATELY
        );

        HRESULT hr = m_d2dFactory->CreateHwndRenderTarget(
            rtProps,
            hwndProps,
            m_renderTarget.GetAddressOf()
        );

        if (FAILED(hr)) {
            // Fallback to IGNORE alpha mode if PREMULTIPLIED is unsupported on specific HWND
            rtProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
            hr = m_d2dFactory->CreateHwndRenderTarget(
                rtProps,
                hwndProps,
                m_renderTarget.GetAddressOf()
            );
        }

        if (FAILED(hr)) {
            XE_LOG_ERROR("Failed to create Direct2D HwndRenderTarget");
            return false;
        }

        UINT dpi = GetDpiForWindow(hwnd);
        UpdateDpi(dpi);

        XE_LOG_INFO("Direct2D HwndRenderTarget created successfully");
        return true;
    }

    void D2DRenderer::DiscardDeviceResources() {
        m_brushCache.clear();
        m_renderTarget.Reset();
    }

    void D2DRenderer::Resize(UINT width, UINT height) {
        if (m_renderTarget) {
            D2D1_SIZE_U size = D2D1::SizeU(width, height);
            m_renderTarget->Resize(size);
        }
    }

    void D2DRenderer::UpdateDpi(UINT dpi) {
        m_dpiScale = static_cast<float>(dpi) / 96.0f;
        if (m_renderTarget) {
            m_renderTarget->SetDpi(static_cast<float>(dpi), static_cast<float>(dpi));
        }
    }

    void D2DRenderer::BeginDraw() {
        if (m_renderTarget) {
            m_renderTarget->BeginDraw();
        }
    }

    HRESULT D2DRenderer::EndDraw() {
        if (!m_renderTarget) return S_OK;

        HRESULT hr = m_renderTarget->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET) {
            XE_LOG_WARN("Direct2D device lost. Discarding device resources...");
            DiscardDeviceResources();
            if (m_hwnd) {
                CreateDeviceResources(m_hwnd);
            }
        }
        return hr;
    }

    ComPtr<ID2D1SolidColorBrush> D2DRenderer::GetOrCreateBrush(const D2D1_COLOR_F& color) {
        if (!m_renderTarget) return nullptr;

        uint32_t key = PackColor(color);
        auto it = m_brushCache.find(key);
        if (it != m_brushCache.end()) {
            return it->second;
        }

        ComPtr<ID2D1SolidColorBrush> brush;
        HRESULT hr = m_renderTarget->CreateSolidColorBrush(color, brush.GetAddressOf());
        if (SUCCEEDED(hr)) {
            m_brushCache[key] = brush;
            return brush;
        }
        return nullptr;
    }

    void D2DRenderer::Clear(const D2D1_COLOR_F& color) {
        if (m_renderTarget) {
            m_renderTarget->Clear(color);
        }
    }

    void D2DRenderer::DrawLine(D2D1_POINT_2F p0, D2D1_POINT_2F p1, const D2D1_COLOR_F& color, float strokeWidth) {
        auto brush = GetOrCreateBrush(color);
        if (m_renderTarget && brush) {
            m_renderTarget->DrawLine(p0, p1, brush.Get(), strokeWidth);
        }
    }

    void D2DRenderer::DrawRectangle(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, float strokeWidth) {
        auto brush = GetOrCreateBrush(color);
        if (m_renderTarget && brush) {
            m_renderTarget->DrawRectangle(rect, brush.Get(), strokeWidth);
        }
    }

    void D2DRenderer::FillRectangle(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color) {
        auto brush = GetOrCreateBrush(color);
        if (m_renderTarget && brush) {
            m_renderTarget->FillRectangle(rect, brush.Get());
        }
    }

    void D2DRenderer::DrawRoundedRectangle(const D2D1_ROUNDED_RECT& roundedRect, const D2D1_COLOR_F& color, float strokeWidth) {
        auto brush = GetOrCreateBrush(color);
        if (m_renderTarget && brush) {
            m_renderTarget->DrawRoundedRectangle(roundedRect, brush.Get(), strokeWidth);
        }
    }

    void D2DRenderer::FillRoundedRectangle(const D2D1_ROUNDED_RECT& roundedRect, const D2D1_COLOR_F& color) {
        auto brush = GetOrCreateBrush(color);
        if (m_renderTarget && brush) {
            m_renderTarget->FillRoundedRectangle(roundedRect, brush.Get());
        }
    }

    void D2DRenderer::DrawText(
        const std::wstring& text,
        const D2D1_RECT_F& layoutRect,
        const D2D1_COLOR_F& color,
        IDWriteTextFormat* textFormat
    ) {
        auto brush = GetOrCreateBrush(color);
        if (m_renderTarget && brush && textFormat) {
            m_renderTarget->DrawText(
                text.c_str(),
                static_cast<UINT32>(text.length()),
                textFormat,
                layoutRect,
                brush.Get()
            );
        }
    }

    ComPtr<IDWriteTextFormat> D2DRenderer::CreateTextFormat(
        const std::wstring& fontFamilyName,
        float fontSize,
        DWRITE_FONT_WEIGHT fontWeight,
        DWRITE_FONT_STYLE fontStyle,
        DWRITE_TEXT_ALIGNMENT textAlignment,
        DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment
    ) {
        if (!m_dwriteFactory) return nullptr;

        ComPtr<IDWriteTextFormat> format;
        HRESULT hr = m_dwriteFactory->CreateTextFormat(
            fontFamilyName.c_str(),
            nullptr,
            fontWeight,
            fontStyle,
            DWRITE_FONT_STRETCH_NORMAL,
            fontSize,
            L"en-us",
            format.GetAddressOf()
        );

        if (FAILED(hr)) {
            // Fallback font chain: Geist -> Segoe UI -> Consolas
            hr = m_dwriteFactory->CreateTextFormat(
                L"Segoe UI",
                nullptr,
                fontWeight,
                fontStyle,
                DWRITE_FONT_STRETCH_NORMAL,
                fontSize,
                L"en-us",
                format.GetAddressOf()
            );
        }

        if (SUCCEEDED(hr) && format) {
            format->SetTextAlignment(textAlignment);
            format->SetParagraphAlignment(paragraphAlignment);
        }

        return format;
    }

}
