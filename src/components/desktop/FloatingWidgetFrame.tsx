import { useState, useEffect } from "react";
import { Pin, X, LayoutDashboard } from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";
import {
  closeWidgetWindow,
  setWidgetAlwaysOnTop,
  showMainWindow,
  getWidgetConfigs,
} from "../../services/desktopWidgetService";
import ClockWidget from "../widgets/ClockWidget";
import SystemMonitorWidget from "../widgets/SystemMonitorWidget";
import NotesWidget from "../widgets/NotesWidget";
import MusicWidget from "../widgets/MusicWidget";
import WeatherWidget from "../widgets/WeatherWidget";
import CalendarWidget from "../widgets/CalendarWidget";

interface FloatingWidgetFrameProps {
  widgetId: string;
}

const WIDGET_TITLES: Record<string, string> = {
  clock: "XE Precision Clock",
  monitor: "XE System Telemetry",
  notes: "XE Quick Scratchpad",
  music: "XE Ambient Sound & Media",
  weather: "XE Atmosphere & Forecast",
  calendar: "XE Event Horizon Calendar",
};

export default function FloatingWidgetFrame({ widgetId }: FloatingWidgetFrameProps) {
  const { settings } = useWidgets();
  const [isAlwaysOnTop, setIsAlwaysOnTop] = useState(() => {
    if (typeof window !== "undefined") {
      const injected = (window as unknown as { __XE_INITIAL_ALWAYS_ON_TOP__?: boolean })
        .__XE_INITIAL_ALWAYS_ON_TOP__;
      if (typeof injected === "boolean") return injected;
    }
    return false;
  });

  // Sync theme to document element
  useEffect(() => {
    document.documentElement.setAttribute("data-theme", settings.theme);
  }, [settings.theme]);

  // Read initial alwaysOnTop state for this widget from store
  useEffect(() => {
    async function loadConfig() {
      try {
        const configs = await getWidgetConfigs();
        if (configs[widgetId]) {
          setIsAlwaysOnTop(Boolean(configs[widgetId].alwaysOnTop));
        }
      } catch (e) {
        console.error("Failed to load widget config:", e);
      }
    }
    loadConfig();
  }, [widgetId]);

  const handleTogglePin = async () => {
    const nextState = !isAlwaysOnTop;
    setIsAlwaysOnTop(nextState);
    try {
      await setWidgetAlwaysOnTop(widgetId, nextState);
    } catch (e) {
      console.error("Failed to toggle always-on-top:", e);
    }
  };

  const handleClose = async () => {
    try {
      await closeWidgetWindow(widgetId);
    } catch (e) {
      console.error("Failed to close widget window:", e);
    }
  };

  const handleOpenDashboard = async () => {
    try {
      await showMainWindow();
    } catch (e) {
      console.error("Failed to open main dashboard:", e);
    }
  };

  const renderWidget = () => {
    switch (widgetId) {
      case "clock":
        return <ClockWidget />;
      case "monitor":
        return <SystemMonitorWidget />;
      case "notes":
        return <NotesWidget />;
      case "music":
        return <MusicWidget />;
      case "weather":
        return <WeatherWidget />;
      case "calendar":
        return <CalendarWidget />;
      default:
        return (
          <div
            className="flex h-full items-center justify-center text-xs"
            style={{ color: "var(--xe-muted)", fontFamily: "var(--xe-font-mono)" }}
          >
            Widget not found: {widgetId}
          </div>
        );
    }
  };

  const title = WIDGET_TITLES[widgetId] || `XE Widget [${widgetId}]`;

  return (
    <div
      className="flex h-screen w-screen flex-col overflow-hidden select-none"
      style={{
        background: "var(--xe-bg)",
        color: "var(--xe-fg)",
        border: "1px solid var(--xe-border)",
        fontFamily: "var(--xe-font-body)",
      }}
    >
      {/* ── NATIVE DRAG TITLEBAR ────────────────────────────── */}
      <div
        data-tauri-drag-region
        className="flex shrink-0 items-center justify-between"
        style={{
          height: 28,
          padding: "0 8px 0 10px",
          background: "var(--xe-surface)",
          borderBottom: "1px solid var(--xe-border)",
          cursor: "grab",
        }}
      >
        {/* Left: Indicator & Title */}
        <div
          data-tauri-drag-region
          className="flex items-center gap-2 overflow-hidden"
          style={{ maxWidth: "calc(100% - 85px)" }}
        >
          <div className="xe-status-dot active" style={{ width: 5, height: 5 }} />
          <span
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 10,
              fontWeight: 500,
              color: "var(--xe-fg)",
              letterSpacing: "0.02em",
              textTransform: "uppercase",
              overflow: "hidden",
              textOverflow: "ellipsis",
              whiteSpace: "nowrap",
            }}
          >
            {title}
          </span>
        </div>

        {/* Right: Window Controls */}
        <div className="flex items-center no-drag" style={{ gap: 2 }}>
          {/* Always on top / Pin toggle */}
          <button
            type="button"
            onClick={handleTogglePin}
            title={isAlwaysOnTop ? "Unpin (Disable always on top)" : "Pin (Keep always on top)"}
            className="xe-transition no-drag"
            style={{
              display: "flex",
              alignItems: "center",
              justifyContent: "center",
              width: 22,
              height: 22,
              background: isAlwaysOnTop ? "var(--xe-fg)" : "transparent",
              color: isAlwaysOnTop ? "var(--xe-bg)" : "var(--xe-muted)",
              borderRadius: 2,
              border: "none",
              cursor: "pointer",
            }}
          >
            <Pin size={11} />
          </button>

          {/* Open Main Dashboard */}
          <button
            type="button"
            onClick={handleOpenDashboard}
            title="Open XE Widgets Dashboard"
            className="xe-transition no-drag"
            style={{
              display: "flex",
              alignItems: "center",
              justifyContent: "center",
              width: 22,
              height: 22,
              background: "transparent",
              color: "var(--xe-muted)",
              borderRadius: 2,
              border: "none",
              cursor: "pointer",
            }}
            onMouseEnter={(e) =>
              ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)")
            }
            onMouseLeave={(e) =>
              ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-muted)")
            }
          >
            <LayoutDashboard size={11} />
          </button>

          {/* Close Window */}
          <button
            type="button"
            onClick={handleClose}
            title="Close desktop widget"
            className="xe-transition no-drag"
            style={{
              display: "flex",
              alignItems: "center",
              justifyContent: "center",
              width: 22,
              height: 22,
              background: "transparent",
              color: "var(--xe-muted)",
              borderRadius: 2,
              border: "none",
              cursor: "pointer",
            }}
            onMouseEnter={(e) => {
              (e.currentTarget as HTMLButtonElement).style.background = "var(--xe-border)";
              (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)";
            }}
            onMouseLeave={(e) => {
              (e.currentTarget as HTMLButtonElement).style.background = "transparent";
              (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-muted)";
            }}
          >
            <X size={12} />
          </button>
        </div>
      </div>

      {/* ── WIDGET CONTENT ─────────────────────────────────── */}
      <div className="flex-1 min-h-0 overflow-hidden no-drag" style={{ padding: 4 }}>
        {renderWidget()}
      </div>
    </div>
  );
}
