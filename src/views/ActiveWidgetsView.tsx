import { Plus, X, Pin, ExternalLink, Focus } from "lucide-react";
import Header from "../components/layout/Header";
import { useWidgets } from "../context/WidgetContext";
import ClockWidget from "../components/widgets/ClockWidget";
import SystemMonitorWidget from "../components/widgets/SystemMonitorWidget";
import NotesWidget from "../components/widgets/NotesWidget";
import MusicWidget from "../components/widgets/MusicWidget";
import WeatherWidget from "../components/widgets/WeatherWidget";
import CalendarWidget from "../components/widgets/CalendarWidget";

const ICON_MAP: Record<string, string> = {
  clock: "◷",
  monitor: "◈",
  notes: "◻",
  music: "♪",
  weather: "◉",
  calendar: "▦",
};

function renderWidgetComponent(id: string) {
  switch (id) {
    case "clock":    return <ClockWidget />;
    case "monitor":  return <SystemMonitorWidget />;
    case "notes":    return <NotesWidget />;
    case "music":    return <MusicWidget />;
    case "weather":  return <WeatherWidget />;
    case "calendar": return <CalendarWidget />;
    default:         return null;
  }
}

export default function ActiveWidgetsView() {
  const {
    activeWidgets,
    toggleWidget,
    togglePinWidget,
    setCurrentView,
    addNotification,
    isWidgetOnDesktop,
    openWidgetOnDesktop,
    focusWidgetOnDesktop,
    closeWidgetOnDesktop,
    openDesktopWidgets,
  } = useWidgets();

  const addBtn = (
    <button
      type="button"
      onClick={() => setCurrentView("library")}
      className="flex items-center xe-transition no-drag"
      style={{
        gap: 5,
        height: 32,
        padding: "0 14px",
        fontSize: 12,
        fontFamily: "var(--xe-font-body)",
        color: "var(--xe-muted)",
        background: "transparent",
        border: "1px solid var(--xe-border)",
        borderRadius: 4,
        cursor: "pointer",
      }}
      onMouseEnter={(e) => {
        (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-fg)";
        (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)";
      }}
      onMouseLeave={(e) => {
        (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-border)";
        (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-muted)";
      }}
    >
      <Plus size={13} />
      Add Widget
    </button>
  );

  const iconBtnStyle = (active = false): React.CSSProperties => ({
    display: "flex",
    alignItems: "center",
    justifyContent: "center",
    width: 26,
    height: 26,
    background: "transparent",
    border: "1px solid transparent",
    borderRadius: 3,
    cursor: "pointer",
    color: active ? "var(--xe-fg)" : "var(--xe-faint)",
    transition: "all 150ms",
  });

  return (
    <div className="flex h-full flex-col overflow-hidden">
      <Header
        title="Active Widgets"
        subtitle="Manage active canvas widgets and independent floating desktop windows."
        actionButton={addBtn}
      />

      <div className="flex-1 overflow-y-auto" style={{ padding: "28px 40px" }}>
        {activeWidgets.length === 0 ? (
          /* ── Empty state ── */
          <div
            className="flex flex-col items-center justify-center"
            style={{
              height: 360,
              border: "1px dashed var(--xe-border)",
              borderRadius: 4,
              textAlign: "center",
              padding: 32,
            }}
          >
            <div
              style={{
                fontFamily: "var(--xe-font-mono)",
                fontSize: 32,
                color: "var(--xe-faint)",
                marginBottom: 16,
                lineHeight: 1,
              }}
            >
              ◻
            </div>
            <h3
              style={{
                fontFamily: "var(--xe-font-display)",
                fontSize: 16,
                color: "var(--xe-fg)",
                marginBottom: 8,
              }}
            >
              No Active Widgets
            </h3>
            <p
              style={{
                fontFamily: "var(--xe-font-body)",
                fontSize: 12,
                color: "var(--xe-muted)",
                maxWidth: 300,
                lineHeight: 1.6,
                marginBottom: 20,
              }}
            >
              Your workspace is empty. Browse the widget library to activate
              productivity, system, and media tools.
            </p>
            <button
              type="button"
              onClick={() => setCurrentView("library")}
              style={{
                height: 32,
                padding: "0 16px",
                fontSize: 12,
                fontFamily: "var(--xe-font-body)",
                fontWeight: 500,
                background: "var(--xe-fg)",
                color: "var(--xe-bg)",
                border: "none",
                borderRadius: 4,
                cursor: "pointer",
              }}
            >
              Browse Library
            </button>
          </div>
        ) : (
          <>
            {/* ── Active widget list ── */}
            <section style={{ marginBottom: 36 }}>
              <div
                className="flex items-center justify-between"
                style={{ marginBottom: 14 }}
              >
                <div className="xe-section-label">
                  ACTIVE CANVAS ({activeWidgets.length}) · ON DESKTOP ({openDesktopWidgets.length})
                </div>
              </div>

              <div
                style={{
                  border: "1px solid var(--xe-border)",
                  borderRadius: 4,
                  overflow: "hidden",
                }}
              >
                {activeWidgets.map((w, i) => {
                  const onDesktop = isWidgetOnDesktop(w.id);

                  return (
                    <div
                      key={w.id}
                      className="flex items-center xe-transition"
                      style={{
                        padding: "10px 16px",
                        borderBottom:
                          i < activeWidgets.length - 1
                            ? "1px solid var(--xe-border)"
                            : "none",
                        background: "transparent",
                      }}
                      onMouseEnter={(e) =>
                        ((e.currentTarget as HTMLDivElement).style.background =
                          "var(--xe-surface)")
                      }
                      onMouseLeave={(e) =>
                        ((e.currentTarget as HTMLDivElement).style.background = "transparent")
                      }
                    >
                      {/* Status dot */}
                      <div
                        className="xe-status-dot active"
                        style={{ marginRight: 10 }}
                      />
                      {/* Symbol */}
                      <span
                        style={{
                          fontFamily: "var(--xe-font-mono)",
                          fontSize: 14,
                          color: "var(--xe-faint)",
                          marginRight: 10,
                          lineHeight: 1,
                          minWidth: 18,
                        }}
                      >
                        {ICON_MAP[w.id] ?? "◆"}
                      </span>
                      {/* Name */}
                      <span
                        style={{
                          fontFamily: "var(--xe-font-body)",
                          fontSize: 13,
                          color: "var(--xe-fg)",
                          flex: 1,
                        }}
                      >
                        {w.name}
                      </span>
                      {/* Desktop Status Tag */}
                      {onDesktop ? (
                        <span
                          style={{
                            fontFamily: "var(--xe-font-mono)",
                            fontSize: 9,
                            color: "var(--xe-fg)",
                            background: "var(--xe-border)",
                            padding: "2px 6px",
                            borderRadius: 2,
                            letterSpacing: "0.06em",
                            textTransform: "uppercase",
                            marginRight: 12,
                            display: "flex",
                            alignItems: "center",
                            gap: 4,
                          }}
                        >
                          <span className="xe-status-dot active" style={{ width: 4, height: 4 }} />
                          ON DESKTOP
                        </span>
                      ) : (
                        <span
                          style={{
                            fontFamily: "var(--xe-font-mono)",
                            fontSize: 9,
                            color: "var(--xe-faint)",
                            letterSpacing: "0.06em",
                            textTransform: "uppercase",
                            marginRight: 12,
                          }}
                        >
                          DASHBOARD ONLY
                        </span>
                      )}
                      {/* Category */}
                      <span
                        style={{
                          fontFamily: "var(--xe-font-mono)",
                          fontSize: 9,
                          color: "var(--xe-faint)",
                          textTransform: "uppercase",
                          letterSpacing: "0.06em",
                          marginRight: 16,
                        }}
                      >
                        {w.category}
                      </span>
                      {/* Version */}
                      <span
                        style={{
                          fontFamily: "var(--xe-font-mono)",
                          fontSize: 10,
                          color: "var(--xe-faint)",
                          marginRight: 16,
                        }}
                      >
                        v{w.version}
                      </span>

                      {/* Desktop Window Launch / Focus Button */}
                      <button
                        type="button"
                        onClick={() => {
                          if (onDesktop) {
                            focusWidgetOnDesktop(w.id);
                          } else {
                            openWidgetOnDesktop(w.id);
                          }
                        }}
                        title={onDesktop ? "Focus desktop window" : "Open as floating desktop window"}
                        style={{
                          ...iconBtnStyle(onDesktop),
                          marginRight: 6,
                        }}
                        onMouseEnter={(e) =>
                          ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)")
                        }
                        onMouseLeave={(e) =>
                          ((e.currentTarget as HTMLButtonElement).style.color = onDesktop
                            ? "var(--xe-fg)"
                            : "var(--xe-faint)")
                        }
                      >
                        {onDesktop ? <Focus size={13} /> : <ExternalLink size={13} />}
                      </button>

                      {/* Pin */}
                      <button
                        type="button"
                        onClick={() => {
                          togglePinWidget(w.id);
                          addNotification(
                            w.isPinned ? "Unpinned" : "Pinned",
                            `${w.name} pin toggled.`
                          );
                        }}
                        title={w.isPinned ? "Unpin" : "Pin"}
                        style={iconBtnStyle(w.isPinned)}
                        onMouseEnter={(e) =>
                          ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)")
                        }
                        onMouseLeave={(e) =>
                          ((e.currentTarget as HTMLButtonElement).style.color = w.isPinned
                            ? "var(--xe-fg)"
                            : "var(--xe-faint)")
                        }
                      >
                        <Pin size={12} />
                      </button>

                      {/* Remove / Close */}
                      <button
                        type="button"
                        onClick={() => {
                          if (onDesktop) {
                            closeWidgetOnDesktop(w.id);
                          }
                          toggleWidget(w.id);
                          addNotification("Removed", `${w.name} removed from canvas.`, "info");
                        }}
                        title="Deactivate widget"
                        style={iconBtnStyle()}
                        onMouseEnter={(e) =>
                          ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)")
                        }
                        onMouseLeave={(e) =>
                          ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-faint)")
                        }
                      >
                        <X size={12} />
                      </button>
                    </div>
                  );
                })}
              </div>
            </section>

            <div className="xe-divider" style={{ marginBottom: 36 }} />

            {/* ── Live widget previews ── */}
            <section>
              <div className="xe-section-label" style={{ marginBottom: 14 }}>
                LIVE PREVIEWS
              </div>
              <div
                style={{
                  display: "grid",
                  gridTemplateColumns: "repeat(3, 1fr)",
                  gap: 12,
                }}
              >
                {activeWidgets.map((w) => {
                  const onDesktop = isWidgetOnDesktop(w.id);

                  return (
                    <div key={w.id} className="flex flex-col" style={{ height: 260 }}>
                      <div
                        className="flex items-center justify-between"
                        style={{
                          fontFamily: "var(--xe-font-mono)",
                          fontSize: 9,
                          color: "var(--xe-faint)",
                          textTransform: "uppercase",
                          letterSpacing: "0.06em",
                          marginBottom: 6,
                        }}
                      >
                        <span>{ICON_MAP[w.id]} {w.name}</span>
                        <button
                          type="button"
                          onClick={() => {
                            if (onDesktop) {
                              focusWidgetOnDesktop(w.id);
                            } else {
                              openWidgetOnDesktop(w.id);
                            }
                          }}
                          className="xe-transition"
                          style={{
                            fontFamily: "var(--xe-font-mono)",
                            fontSize: 9,
                            color: onDesktop ? "var(--xe-fg)" : "var(--xe-muted)",
                            cursor: "pointer",
                            background: "transparent",
                            border: "none",
                            display: "flex",
                            alignItems: "center",
                            gap: 3,
                          }}
                        >
                          <ExternalLink size={9} />
                          {onDesktop ? "ON DESKTOP" : "FLOAT"}
                        </button>
                      </div>
                      <div className="flex-1 min-h-0">
                        {renderWidgetComponent(w.id)}
                      </div>
                    </div>
                  );
                })}
              </div>
            </section>
          </>
        )}
      </div>
    </div>
  );
}
