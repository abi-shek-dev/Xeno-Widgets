import { LayoutDashboard, Grid2X2, AppWindow, Settings2 } from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";
import type { NavView } from "../../types";

const NAV_ITEMS: { id: NavView; label: string; num: string; icon: React.ElementType }[] = [
  { id: "overview",  label: "Overview",       num: "01", icon: LayoutDashboard },
  { id: "library",   label: "Widget Library", num: "02", icon: Grid2X2 },
  { id: "active",    label: "Active Widgets", num: "03", icon: AppWindow },
  { id: "settings",  label: "Settings",       num: "04", icon: Settings2 },
];

export default function Sidebar() {
  const { currentView, setCurrentView } = useWidgets();

  return (
    <aside
      className="flex h-full shrink-0 flex-col"
      style={{
        width: 224,
        background: "var(--xe-sidebar-bg)",
        borderRight: "1px solid var(--xe-sidebar-border)",
      }}
    >
      {/* Wordmark */}
      <div style={{ padding: "20px 20px 0" }}>
        <div
          style={{
            fontFamily: "var(--xe-font-display)",
            fontSize: 22,
            fontWeight: 700,
            color: "var(--xe-fg)",
            letterSpacing: "-0.02em",
            lineHeight: 1,
          }}
        >
          XE.
        </div>
        <div
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 10,
            color: "var(--xe-faint)",
            letterSpacing: "0.06em",
            marginTop: 4,
          }}
        >
          XE WIDGETS
        </div>
      </div>

      {/* Section label */}
      <div
        className="xe-section-label"
        style={{ padding: "24px 20px 8px" }}
      >
        WORKSPACE
      </div>

      {/* Navigation */}
      <nav className="flex flex-col" style={{ gap: 1 }}>
        {NAV_ITEMS.map((item) => {
          const Icon = item.icon;
          const isActive = currentView === item.id;

          return (
            <button
              key={item.id}
              type="button"
              onClick={() => setCurrentView(item.id)}
              className="flex items-center gap-3 text-left xe-transition"
              style={{
                padding: "9px 20px",
                borderLeft: isActive ? "2px solid var(--xe-fg)" : "2px solid transparent",
                background: isActive ? "var(--xe-surface)" : "transparent",
                color: isActive ? "var(--xe-fg)" : "var(--xe-muted)",
                fontSize: 13,
                fontFamily: "var(--xe-font-body)",
                fontWeight: isActive ? 500 : 400,
                cursor: "pointer",
                width: "100%",
              }}
              onMouseEnter={(e) => {
                if (!isActive) {
                  (e.currentTarget as HTMLButtonElement).style.background = "var(--xe-surface-hover)";
                  (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)";
                }
              }}
              onMouseLeave={(e) => {
                if (!isActive) {
                  (e.currentTarget as HTMLButtonElement).style.background = "transparent";
                  (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-muted)";
                }
              }}
            >
              <span
                className="font-mono tabnum"
                style={{
                  fontSize: 10,
                  color: isActive ? "var(--xe-muted)" : "var(--xe-faint)",
                  fontFamily: "var(--xe-font-mono)",
                  letterSpacing: "0.04em",
                  minWidth: 18,
                }}
              >
                {item.num}
              </span>
              <Icon
                size={14}
                style={{ color: isActive ? "var(--xe-fg)" : "var(--xe-faint)", flexShrink: 0 }}
              />
              <span>{item.label}</span>
            </button>
          );
        })}
      </nav>

      {/* Footer */}
      <div
        className="mt-auto"
        style={{
          borderTop: "1px solid var(--xe-sidebar-border)",
          padding: "12px 20px",
        }}
      >
        <div
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 10,
            color: "var(--xe-faint)",
            letterSpacing: "0.04em",
            lineHeight: 1.7,
          }}
        >
          <div>XE Widgets</div>
          <div>v2.0.0</div>
          <div style={{ color: "var(--xe-faint)", opacity: 0.6 }}>Development</div>
        </div>
      </div>
    </aside>
  );
}
