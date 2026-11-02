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

    void D2DRenderer::SetActiveWindow(HWND hwnd) {
        m_activeHwnd = hwnd;
    }

    bool D2DRenderer::CreateDeviceResources(HWND hwnd) {
        if (!hwnd || !m_d2dFactory) return false;
        m_activeHwnd = hwnd;

        auto it = m_targets.find(hwnd);
        if (it != m_targets.end() && it->second.renderTarget) {
            return true;
        }

        RECT rc{};
        GetClientRect(hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(
            std::max<UINT>(1, static_cast<UINT>(rc.right - rc.left)),
            std::max<UINT>(1, static_cast<UINT>(rc.bottom - rc.top))
        );

        // Alpha mode premultiplied enables transparent desktop widgets
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );

        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
            hwnd,
            size,
            D2D1_PRESENT_OPTIONS_IMMEDIATELY
        );

        ComPtr<ID2D1HwndRenderTarget> renderTarget;
        HRESULT hr = m_d2dFactory->CreateHwndRenderTarget(
            rtProps,
            hwndProps,
            renderTarget.GetAddressOf()
        );

        if (FAILED(hr)) {
            // Fallback to IGNORE alpha mode if PREMULTIPLIED is unsupported
            rtProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
            hr = m_d2dFactory->CreateHwndRenderTarget(
                rtProps,
                hwndProps,
                renderTarget.GetAddressOf()
            );
        }

        if (FAILED(hr)) {
            XE_LOG_ERROR("Failed to create Direct2D HwndRenderTarget for HWND");
            return false;
        }

        TargetResources res{};
        res.hwnd = hwnd;
        res.renderTarget = renderTarget;
        UINT dpi = GetDpiForWindow(hwnd);
        res.dpiScale = static_cast<float>(dpi) / 96.0f;
        res.renderTarget->SetDpi(static_cast<float>(dpi), static_cast<float>(dpi));

        m_targets[hwnd] = std::move(res);
        XE_LOG_INFO("Direct2D HwndRenderTarget created successfully for HWND");
        return true;
    }

    void D2DRenderer::DiscardDeviceResources(HWND hwnd) {
        if (hwnd) {
            m_targets.erase(hwnd);
            if (m_activeHwnd == hwnd) {
                m_activeHwnd = nullptr;
            }
        } else {
            m_targets.clear();
            m_activeHwnd = nullptr;
        }
    }

    ID2D1HwndRenderTarget* D2DRenderer::GetRenderTarget() const {
        if (!m_activeHwnd) return nullptr;
        auto it = m_targets.find(m_activeHwnd);
        if (it != m_targets.end()) {
            return it->second.renderTarget.Get();
        }
        return nullptr;
    }

    float D2DRenderer::GetDpiScale() const {
        if (!m_activeHwnd) return 1.0f;
        auto it = m_targets.find(m_activeHwnd);
        if (it != m_targets.end()) {
            return it->second.dpiScale;
        }
        return 1.0f;
    }

    void D2DRenderer::Resize(UINT width, UINT height) {
        if (m_activeHwnd) {
            Resize(m_activeHwnd, width, height);
        }
    }

    void D2DRenderer::Resize(HWND hwnd, UINT width, UINT height) {
        auto it = m_targets.find(hwnd);
        if (it != m_targets.end() && it->second.renderTarget) {
            D2D1_SIZE_U size = D2D1::SizeU(std::max<UINT>(1, width), std::max<UINT>(1, height));
            it->second.renderTarget->Resize(size);
        }
    }

    void D2DRenderer::UpdateDpi(UINT dpi, HWND hwnd) {
        HWND targetHwnd = hwnd ? hwnd : m_activeHwnd;
        if (!targetHwnd) return;
        auto it = m_targets.find(targetHwnd);
        if (it != m_targets.end()) {
            it->second.dpiScale = static_cast<float>(dpi) / 96.0f;
            if (it->second.renderTarget) {
                it->second.renderTarget->SetDpi(static_cast<float>(dpi), static_cast<float>(dpi));
            }
        }
    }

    void D2DRenderer::BeginDraw(HWND hwnd) {
        if (hwnd) {
            m_activeHwnd = hwnd;
        }
        auto target = GetRenderTarget();
        if (target) {
            target->BeginDraw();
        }
    }

    HRESULT D2DRenderer::EndDraw() {
        auto target = GetRenderTarget();
        if (!target) return S_OK;

        HRESULT hr = target->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET) {
            XE_LOG_WARN("Direct2D device lost. Discarding device resources...");
            HWND lostHwnd = m_activeHwnd;
            DiscardDeviceResources(lostHwnd);
            if (lostHwnd) {
                CreateDeviceResources(lostHwnd);
            }
        }
        return hr;
    }

    ComPtr<ID2D1SolidColorBrush> D2DRenderer::GetOrCreateBrush(const D2D1_COLOR_F& color) {
        if (!m_activeHwnd) return nullptr;
        auto it = m_targets.find(m_activeHwnd);
        if (it == m_targets.end() || !it->second.renderTarget) return nullptr;

        uint32_t key = PackColor(color);
        auto brushIt = it->second.brushCache.find(key);
        if (brushIt != it->second.brushCache.end()) {
            return brushIt->second;
        }

        ComPtr<ID2D1SolidColorBrush> brush;
        HRESULT hr = it->second.renderTarget->CreateSolidColorBrush(color, brush.GetAddressOf());
        if (SUCCEEDED(hr)) {
            it->second.brushCache[key] = brush;
            return brush;
        }
        return nullptr;
    }

    void D2DRenderer::Clear(const D2D1_COLOR_F& color) {
        auto target = GetRenderTarget();
        if (target) {
            target->Clear(color);
        }
    }

    void D2DRenderer::DrawLine(D2D1_POINT_2F p0, D2D1_POINT_2F p1, const D2D1_COLOR_F& color, float strokeWidth) {
        auto target = GetRenderTarget();
        auto brush = GetOrCreateBrush(color);
        if (target && brush) {
            target->DrawLine(p0, p1, brush.Get(), strokeWidth);
        }
    }

    void D2DRenderer::DrawRectangle(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, float strokeWidth) {
        auto target = GetRenderTarget();
        auto brush = GetOrCreateBrush(color);
        if (target && brush) {
            target->DrawRectangle(rect, brush.Get(), strokeWidth);
        }
    }

    void D2DRenderer::FillRectangle(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color) {
        auto target = GetRenderTarget();
        auto brush = GetOrCreateBrush(color);
        if (target && brush) {
            target->FillRectangle(rect, brush.Get());
        }
    }

    void D2DRenderer::DrawRoundedRectangle(const D2D1_ROUNDED_RECT& roundedRect, const D2D1_COLOR_F& color, float strokeWidth) {
        auto target = GetRenderTarget();
        auto brush = GetOrCreateBrush(color);
        if (target && brush) {
            target->DrawRoundedRectangle(roundedRect, brush.Get(), strokeWidth);
        }
    }

    void D2DRenderer::FillRoundedRectangle(const D2D1_ROUNDED_RECT& roundedRect, const D2D1_COLOR_F& color) {
        auto target = GetRenderTarget();
        auto brush = GetOrCreateBrush(color);
        if (target && brush) {
            target->FillRoundedRectangle(roundedRect, brush.Get());
        }
    }

    void D2DRenderer::DrawEllipse(const D2D1_ELLIPSE& ellipse, const D2D1_COLOR_F& color, float strokeWidth) {
        auto target = GetRenderTarget();
        auto brush = GetOrCreateBrush(color);
        if (target && brush) {
            target->DrawEllipse(ellipse, brush.Get(), strokeWidth);
        }
    }

    void D2DRenderer::FillEllipse(const D2D1_ELLIPSE& ellipse, const D2D1_COLOR_F& color) {
        auto target = GetRenderTarget();
        auto brush = GetOrCreateBrush(color);
        if (target && brush) {
            target->FillEllipse(ellipse, brush.Get());
        }
    }

    void D2DRenderer::DrawGeometry(ID2D1Geometry* geometry, const D2D1_COLOR_F& color, float strokeWidth) {
        auto target = GetRenderTarget();
        auto brush = GetOrCreateBrush(color);
        if (target && brush && geometry) {
            target->DrawGeometry(geometry, brush.Get(), strokeWidth);
        }
    }

    void D2DRenderer::DrawText(
        const std::wstring& text,
        const D2D1_RECT_F& layoutRect,
        const D2D1_COLOR_F& color,
        IDWriteTextFormat* textFormat
    ) {
        auto target = GetRenderTarget();
        auto brush = GetOrCreateBrush(color);
        if (target && brush && textFormat) {
            target->DrawText(
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
