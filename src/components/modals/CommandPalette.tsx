import { useState, useEffect } from "react";
import * as Dialog from "@radix-ui/react-dialog";
import {
  Search,
  LayoutDashboard,
  Grid2X2,
  AppWindow,
  Settings2,
  Play,
  Pause,
  Sun,
  Moon,
} from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";

export default function CommandPalette() {
  const {
    isCommandPaletteOpen,
    setIsCommandPaletteOpen,
    setCurrentView,
    widgets,
    toggleWidget,
    isPlaying,
    togglePlay,
    updateSettings,
    settings,
    addNotification,
  } = useWidgets();

  const [query, setQuery] = useState("");

  useEffect(() => {
    if (!isCommandPaletteOpen) {
      setQuery("");
    }
  }, [isCommandPaletteOpen]);

  const runAction = (action: () => void) => {
    action();
    setIsCommandPaletteOpen(false);
  };

  const actions = [
    {
      id: "view-overview",
      label: "Go to Overview",
      category: "Navigation",
      icon: LayoutDashboard,
      perform: () => setCurrentView("overview"),
    },
    {
      id: "view-library",
      label: "Browse Widget Library",
      category: "Navigation",
      icon: Grid2X2,
      perform: () => setCurrentView("library"),
    },
    {
      id: "view-active",
      label: "View Active Widgets",
      category: "Navigation",
      icon: AppWindow,
      perform: () => setCurrentView("active"),
    },
    {
      id: "view-settings",
      label: "Open Settings",
      category: "Navigation",
      icon: Settings2,
      perform: () => setCurrentView("settings"),
    },
    {
      id: "toggle-music",
      label: isPlaying ? "Pause Playback" : "Play Media",
      category: "Media",
      icon: isPlaying ? Pause : Play,
      perform: () => togglePlay(),
    },
    {
      id: "switch-theme",
      label: settings.theme === "light" ? "Switch to Dark Theme" : "Switch to Light Theme",
      category: "Appearance",
      icon: settings.theme === "light" ? Moon : Sun,
      perform: () => {
        const next = settings.theme === "light" ? "dark" : "light";
        updateSettings({ theme: next });
        addNotification("Theme Changed", `${next === "light" ? "Light" : "Dark"} theme applied.`);
      },
    },
  ];

  const filteredActions = actions.filter((act) =>
    act.label.toLowerCase().includes(query.toLowerCase())
  );

  const filteredWidgets = widgets.filter(
    (w) =>
      w.name.toLowerCase().includes(query.toLowerCase()) ||
      w.description.toLowerCase().includes(query.toLowerCase()) ||
      w.tags.some((t) => t.toLowerCase().includes(query.toLowerCase()))
  );

  const groupLabel = (text: string) => (
    <div
      style={{
        padding: "8px 16px 4px",
        fontFamily: "var(--xe-font-mono)",
        fontSize: 9,
        color: "var(--xe-faint)",
        letterSpacing: "0.08em",
        textTransform: "uppercase",
      }}
    >
      {text}
    </div>
  );

  const itemStyle: React.CSSProperties = {
    display: "flex",
    width: "100%",
    alignItems: "center",
    justifyContent: "space-between",
    padding: "9px 16px",
    fontSize: 13,
    fontFamily: "var(--xe-font-body)",
    color: "var(--xe-fg)",
    background: "transparent",
    border: "none",
    borderRadius: 0,
    cursor: "pointer",
    textAlign: "left",
    transition: "background 120ms",
  };

  return (
    <Dialog.Root open={isCommandPaletteOpen} onOpenChange={setIsCommandPaletteOpen}>
      <Dialog.Portal>
        <Dialog.Overlay
          style={{
            position: "fixed",
            inset: 0,
            zIndex: 50,
            background: "rgba(0,0,0,0.25)",
          }}
        />
        <Dialog.Content
          style={{
            position: "fixed",
            left: "50%",
            top: "22%",
            zIndex: 51,
            width: 560,
            transform: "translateX(-50%)",
            background: "var(--xe-bg)",
            border: "1px solid var(--xe-border)",
            borderRadius: 8,
            boxShadow: "0 12px 40px rgba(0,0,0,0.12)",
            overflow: "hidden",
          }}
        >
          <Dialog.Title className="sr-only">Command Palette</Dialog.Title>

          {/* Search bar */}
          <div
            style={{
              display: "flex",
              alignItems: "center",
              gap: 10,
              padding: "14px 18px",
              borderBottom: "1px solid var(--xe-border)",
            }}
          >
            <Search size={14} style={{ color: "var(--xe-faint)", flexShrink: 0 }} />
            <input
              type="text"
              autoFocus
              placeholder="Type a command or search widgets..."
              value={query}
              onChange={(e) => setQuery(e.target.value)}
              style={{
                flex: 1,
                background: "transparent",
                border: "none",
                outline: "none",
                fontSize: 13,
                fontFamily: "var(--xe-font-body)",
                color: "var(--xe-fg)",
              }}
            />
            <kbd
              style={{
                fontFamily: "var(--xe-font-mono)",
                fontSize: 10,
                color: "var(--xe-faint)",
                background: "var(--xe-surface)",
                border: "1px solid var(--xe-border)",
                borderRadius: 3,
                padding: "1px 6px",
              }}
            >
              ESC
            </kbd>
          </div>

          {/* Results */}
          <div style={{ maxHeight: 340, overflowY: "auto" }}>
            {filteredActions.length > 0 && (
              <>
                {groupLabel("Actions")}
                {filteredActions.map((act) => {
                  const Icon = act.icon;
                  return (
                    <button
                      key={act.id}
                      type="button"
                      onClick={() => runAction(act.perform)}
                      style={itemStyle}
                      onMouseEnter={(e) =>
                        ((e.currentTarget as HTMLButtonElement).style.background =
                          "var(--xe-surface)")
                      }
                      onMouseLeave={(e) =>
                        ((e.currentTarget as HTMLButtonElement).style.background = "transparent")
                      }
                    >
                      <div style={{ display: "flex", alignItems: "center", gap: 10 }}>
                        <Icon size={14} style={{ color: "var(--xe-muted)", flexShrink: 0 }} />
                        <span>{act.label}</span>
                      </div>
                      <span
                        style={{
                          fontFamily: "var(--xe-font-mono)",
                          fontSize: 9,
                          color: "var(--xe-faint)",
                          letterSpacing: "0.06em",
                          textTransform: "uppercase",
                        }}
                      >
                        {act.category}
                      </span>
                    </button>
                  );
                })}
              </>
            )}

            {filteredWidgets.length > 0 && (
              <>
                {groupLabel("Widgets")}
                {filteredWidgets.map((widget) => (
                  <button
                    key={widget.id}
                    type="button"
                    onClick={() =>
                      runAction(() => {
                        toggleWidget(widget.id);
                        addNotification(
                          widget.isActive ? "Widget Deactivated" : "Widget Activated",
                          `${widget.name} status updated.`
                        );
                      })
                    }
                    style={itemStyle}
                    onMouseEnter={(e) =>
                      ((e.currentTarget as HTMLButtonElement).style.background =
                        "var(--xe-surface)")
                    }
                    onMouseLeave={(e) =>
                      ((e.currentTarget as HTMLButtonElement).style.background = "transparent")
                    }
                  >
                    <div>
                      <div
                        style={{
                          fontFamily: "var(--xe-font-body)",
                          fontSize: 13,
                          color: "var(--xe-fg)",
                          fontWeight: 500,
                        }}
                      >
                        {widget.name}
                      </div>
                      <div
                        style={{
                          fontFamily: "var(--xe-font-body)",
                          fontSize: 11,
                          color: "var(--xe-muted)",
                          marginTop: 1,
                        }}
                      >
                        {widget.description}
                      </div>
                    </div>
                    <span
                      style={{
                        fontFamily: "var(--xe-font-mono)",
                        fontSize: 9,
                        letterSpacing: "0.06em",
                        textTransform: "uppercase",
                        color: widget.isActive ? "var(--xe-fg)" : "var(--xe-faint)",
                        padding: "2px 6px",
                        border: "1px solid var(--xe-border)",
                        borderRadius: 2,
                        flexShrink: 0,
                        marginLeft: 12,
                      }}
                    >
                      {widget.isActive ? "ACTIVE" : "AVAILABLE"}
                    </span>
                  </button>
                ))}
              </>
            )}

            {filteredActions.length === 0 && filteredWidgets.length === 0 && (
              <div
                style={{
                  padding: "32px 16px",
                  textAlign: "center",
                  fontFamily: "var(--xe-font-body)",
                  fontSize: 13,
                  color: "var(--xe-faint)",
                }}
              >
                No results for "{query}"
              </div>
            )}
          </div>

          {/* Footer */}
          <div
            style={{
              display: "flex",
              justifyContent: "space-between",
              padding: "8px 18px",
              borderTop: "1px solid var(--xe-border)",
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
              letterSpacing: "0.04em",
            }}
          >
            <span>NAVIGATION & QUICK ACTIONS</span>
            <span>↵ SELECT</span>
          </div>
        </Dialog.Content>
      </Dialog.Portal>
    </Dialog.Root>
  );
}
