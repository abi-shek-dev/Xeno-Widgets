import { ArrowRight, Plus } from "lucide-react";
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

function renderWidgetPreview(id: string) {
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

export default function OverviewView() {
  const { setCurrentView, activeWidgets, widgets, metrics, toggleWidget, addNotification } =
    useWidgets();

  const available = widgets.length - activeWidgets.length;

  const statCells = [
    { value: String(widgets.length), label: "Total Widgets" },
    { value: String(activeWidgets.length), label: "Active" },
    { value: String(available), label: "Available" },
    { value: `${metrics.cpuUsage}%`, label: "CPU Load" },
  ];

  const btnPrimary: React.CSSProperties = {
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
    letterSpacing: "0.01em",
  };

  const btnSecondary: React.CSSProperties = {
    height: 32,
    padding: "0 16px",
    fontSize: 12,
    fontFamily: "var(--xe-font-body)",
    background: "transparent",
    color: "var(--xe-muted)",
    border: "1px solid var(--xe-border)",
    borderRadius: 4,
    cursor: "pointer",
    display: "flex",
    alignItems: "center",
    gap: 4,
  };

  return (
    <div className="flex h-full flex-col overflow-hidden">
      <Header title="Overview" subtitle="Your desktop workspace at a glance." />

      <div className="flex-1 overflow-y-auto" style={{ padding: "36px 40px" }}>

        {/* ── HERO ─────────────────────────────────────────── */}
        <section style={{ marginBottom: 40 }}>
          <h1
            style={{
              fontFamily: "var(--xe-font-display)",
              fontSize: 38,
              lineHeight: 1.05,
              color: "var(--xe-fg)",
              letterSpacing: "-0.01em",
            }}
          >
            YOUR DESKTOP.
            <br />
            YOUR RULES.
          </h1>
          <p
            style={{
              marginTop: 14,
              fontSize: 13,
              color: "var(--xe-muted)",
              fontFamily: "var(--xe-font-body)",
              maxWidth: 400,
              lineHeight: 1.65,
            }}
          >
            A minimal companion for your workflow. Precision widgets, zero distractions,
            complete control.
          </p>
          <div className="flex items-center" style={{ marginTop: 20, gap: 8 }}>
            <button type="button" onClick={() => setCurrentView("library")} style={btnPrimary}>
              <Plus size={12} style={{ display: "inline", marginRight: 5 }} />
              Browse Widgets
            </button>
            <button
              type="button"
              onClick={() => setCurrentView("active")}
              style={btnSecondary}
            >
              Manage Active
              <ArrowRight size={12} />
            </button>
          </div>
        </section>

        <div className="xe-divider" style={{ marginBottom: 36 }} />

        {/* ── 01 STATISTICS ────────────────────────────────── */}
        <section style={{ marginBottom: 40 }}>
          <div className="xe-section-label" style={{ marginBottom: 14 }}>
            01 — Statistics
          </div>
          <div
            style={{
              display: "grid",
              gridTemplateColumns: "repeat(4, 1fr)",
              border: "1px solid var(--xe-border)",
            }}
          >
            {statCells.map((cell, i) => (
              <div
                key={cell.label}
                style={{
                  padding: "20px 24px",
                  borderRight: i < statCells.length - 1 ? "1px solid var(--xe-border)" : "none",
                }}
              >
                <div
                  className="tabnum"
                  style={{
                    fontFamily: "var(--xe-font-mono)",
                    fontSize: 28,
                    fontWeight: 500,
                    color: "var(--xe-fg)",
                    lineHeight: 1,
                  }}
                >
                  {cell.value}
                </div>
                <div
                  style={{
                    marginTop: 8,
                    fontFamily: "var(--xe-font-body)",
                    fontSize: 11,
                    color: "var(--xe-muted)",
                  }}
                >
                  {cell.label}
                </div>
              </div>
            ))}
          </div>
        </section>

        {/* ── 02 ACTIVE WIDGETS ────────────────────────────── */}
        <section style={{ marginBottom: 40 }}>
          <div
            className="flex items-center justify-between"
            style={{ marginBottom: 14 }}
          >
            <div className="xe-section-label">02 — Active Widgets</div>
            <button
              type="button"
              onClick={() => setCurrentView("active")}
              className="flex items-center xe-transition"
              style={{
                gap: 4,
                fontSize: 11,
                fontFamily: "var(--xe-font-mono)",
                color: "var(--xe-faint)",
                cursor: "pointer",
                letterSpacing: "0.04em",
              }}
              onMouseEnter={(e) =>
                ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)")
              }
              onMouseLeave={(e) =>
                ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-faint)")
              }
            >
              MANAGE ALL
              <ArrowRight size={11} />
            </button>
          </div>

          {activeWidgets.length === 0 ? (
            <div
              style={{
                padding: "24px",
                border: "1px dashed var(--xe-border)",
                borderRadius: 4,
                textAlign: "center",
                color: "var(--xe-faint)",
                fontSize: 12,
                fontFamily: "var(--xe-font-body)",
              }}
            >
              No active widgets. Browse the library to get started.
            </div>
          ) : (
            <div style={{ border: "1px solid var(--xe-border)", borderRadius: 4, overflow: "hidden" }}>
              {activeWidgets.map((w, i) => (
                <div
                  key={w.id}
                  className="flex items-center xe-transition"
                  style={{
                    padding: "10px 16px",
                    borderBottom: i < activeWidgets.length - 1 ? "1px solid var(--xe-border)" : "none",
                    background: "transparent",
                  }}
                  onMouseEnter={(e) =>
                    ((e.currentTarget as HTMLDivElement).style.background = "var(--xe-surface)")
                  }
                  onMouseLeave={(e) =>
                    ((e.currentTarget as HTMLDivElement).style.background = "transparent")
                  }
                >
                  {/* Status dot */}
                  <div
                    className="xe-status-dot active"
                    style={{ marginRight: 10, flexShrink: 0 }}
                  />
                  {/* Symbol */}
                  <span
                    style={{
                      fontFamily: "var(--xe-font-mono)",
                      fontSize: 14,
                      color: "var(--xe-faint)",
                      marginRight: 10,
                      lineHeight: 1,
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
                  {/* Category label */}
                  <span
                    style={{
                      fontFamily: "var(--xe-font-mono)",
                      fontSize: 9,
                      color: "var(--xe-faint)",
                      textTransform: "uppercase",
                      letterSpacing: "0.06em",
                      marginRight: 12,
                    }}
                  >
                    {w.category}
                  </span>
                  {/* Running label */}
                  <span
                    style={{
                      fontFamily: "var(--xe-font-mono)",
                      fontSize: 10,
                      color: "var(--xe-muted)",
                      letterSpacing: "0.04em",
                    }}
                  >
                    RUNNING
                  </span>
                </div>
              ))}
            </div>
          )}
        </section>

        {/* ── 03 WIDGET LIBRARY PREVIEW ────────────────────── */}
        <section style={{ marginBottom: 40 }}>
          <div
            className="flex items-center justify-between"
            style={{ marginBottom: 14 }}
          >
            <div className="xe-section-label">03 — Widget Library</div>
            <button
              type="button"
              onClick={() => setCurrentView("library")}
              className="flex items-center xe-transition"
              style={{
                gap: 4,
                fontSize: 11,
                fontFamily: "var(--xe-font-mono)",
                color: "var(--xe-faint)",
                cursor: "pointer",
                letterSpacing: "0.04em",
              }}
              onMouseEnter={(e) =>
                ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)")
              }
              onMouseLeave={(e) =>
                ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-faint)")
              }
            >
              VIEW ALL
              <ArrowRight size={11} />
            </button>
          </div>

          <div
            style={{
              display: "grid",
              gridTemplateColumns: "repeat(3, 1fr)",
              gap: 1,
              border: "1px solid var(--xe-border)",
              borderRadius: 4,
              overflow: "hidden",
              background: "var(--xe-border)",
            }}
          >
            {widgets.slice(0, 3).map((w) => (
              <div
                key={w.id}
                className="xe-transition"
                style={{
                  padding: "16px",
                  background: "var(--xe-bg)",
                  cursor: "default",
                }}
                onMouseEnter={(e) =>
                  ((e.currentTarget as HTMLDivElement).style.background = "var(--xe-surface)")
                }
                onMouseLeave={(e) =>
                  ((e.currentTarget as HTMLDivElement).style.background = "var(--xe-bg)")
                }
              >
                <div
                  className="flex items-start justify-between"
                  style={{ marginBottom: 10 }}
                >
                  <span
                    style={{
                      fontFamily: "var(--xe-font-mono)",
                      fontSize: 16,
                      color: "var(--xe-faint)",
                    }}
                  >
                    {ICON_MAP[w.id] ?? "◆"}
                  </span>
                  <span
                    style={{
                      fontFamily: "var(--xe-font-mono)",
                      fontSize: 9,
                      color: w.isActive ? "var(--xe-fg)" : "var(--xe-faint)",
                      textTransform: "uppercase",
                      letterSpacing: "0.06em",
                      padding: "2px 6px",
                      border: "1px solid var(--xe-border)",
                      borderRadius: 2,
                    }}
                  >
                    {w.isActive ? "ACTIVE" : "AVAILABLE"}
                  </span>
                </div>
                <div
                  style={{
                    fontFamily: "var(--xe-font-body)",
                    fontSize: 13,
                    fontWeight: 500,
                    color: "var(--xe-fg)",
                    marginBottom: 4,
                  }}
                >
                  {w.name}
                </div>
                <div
                  style={{
                    fontFamily: "var(--xe-font-body)",
                    fontSize: 11,
                    color: "var(--xe-muted)",
                    lineHeight: 1.5,
                    marginBottom: 12,
                  }}
                >
                  {w.description}
                </div>
                <button
                  type="button"
                  onClick={() => {
                    toggleWidget(w.id);
                    addNotification(
                      w.isActive ? "Widget Deactivated" : "Widget Activated",
                      `${w.name} is now ${w.isActive ? "disabled" : "running"}.`,
                      w.isActive ? "info" : "success"
                    );
                  }}
                  className="xe-transition"
                  style={{
                    fontFamily: "var(--xe-font-mono)",
                    fontSize: 10,
                    letterSpacing: "0.04em",
                    color: w.isActive ? "var(--xe-muted)" : "var(--xe-fg)",
                    background: "transparent",
                    border: "1px solid var(--xe-border)",
                    borderRadius: 3,
                    padding: "3px 8px",
                    cursor: "pointer",
                    textTransform: "uppercase",
                  }}
                >
                  {w.isActive ? "DISABLE" : "ENABLE"}
                </button>
              </div>
            ))}
          </div>
        </section>

        {/* ── 04 LIVE WIDGET PREVIEWS ──────────────────────── */}
        <section style={{ marginBottom: 8 }}>
          <div className="xe-section-label" style={{ marginBottom: 14 }}>
            04 — Live Previews
          </div>
          <div
            style={{
              display: "grid",
              gridTemplateColumns: "repeat(3, 1fr)",
              gap: 12,
            }}
          >
            {activeWidgets.slice(0, 6).map((w) => (
              <div key={w.id} style={{ height: 240 }}>
                {renderWidgetPreview(w.id)}
              </div>
            ))}
          </div>
        </section>

      </div>
    </div>
  );
}
