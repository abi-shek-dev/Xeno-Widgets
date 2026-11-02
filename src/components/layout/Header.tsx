import React, { useState, useRef, useEffect } from "react";
import { Search, Bell, Sun, Moon, Trash2, X } from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";

interface HeaderProps {
  title: string;
  subtitle: string;
  actionButton?: React.ReactNode;
}

export default function Header({ title, subtitle, actionButton }: HeaderProps) {
  const {
    setIsCommandPaletteOpen,
    notifications,
    clearNotifications,
    unreadCount,
    settings,
    updateSettings,
  } = useWidgets();

  const [isNotifOpen, setIsNotifOpen] = useState(false);
  const notifRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    function handleClickOutside(event: MouseEvent) {
      if (notifRef.current && !notifRef.current.contains(event.target as Node)) {
        setIsNotifOpen(false);
      }
    }
    document.addEventListener("mousedown", handleClickOutside);
    return () => document.removeEventListener("mousedown", handleClickOutside);
  }, []);

  const handleThemeToggle = () => {
    updateSettings({ theme: settings.theme === "light" ? "dark" : "light" });
  };

  const iconBtnStyle: React.CSSProperties = {
    display: "flex",
    alignItems: "center",
    justifyContent: "center",
    width: 32,
    height: 32,
    border: "1px solid var(--xe-border)",
    background: "var(--xe-surface)",
    borderRadius: 4,
    cursor: "pointer",
    color: "var(--xe-muted)",
    flexShrink: 0,
    transition: "all 200ms var(--xe-ease)",
  };

  return (
    <header
      className="flex shrink-0 items-center justify-between"
      style={{
        height: 56,
        padding: "0 32px",
        borderBottom: "1px solid var(--xe-border)",
        background: "var(--xe-bg)",
      }}
      data-tauri-drag-region
    >
      {/* Left: title + subtitle */}
      <div data-tauri-drag-region>
        <h1
          style={{
            fontFamily: "var(--xe-font-display)",
            fontSize: 15,
            color: "var(--xe-fg)",
            lineHeight: 1.2,
            letterSpacing: "-0.01em",
          }}
        >
          {title}
        </h1>
        <p
          style={{
            fontFamily: "var(--xe-font-body)",
            fontSize: 11,
            color: "var(--xe-muted)",
            marginTop: 1,
          }}
        >
          {subtitle}
        </p>
      </div>

      {/* Right: controls */}
      <div className="flex items-center no-drag" style={{ gap: 8 }}>
        {/* Search trigger */}
        <button
          type="button"
          onClick={() => setIsCommandPaletteOpen(true)}
          className="flex items-center xe-transition"
          style={{
            height: 32,
            padding: "0 10px",
            gap: 6,
            border: "1px solid var(--xe-border)",
            background: "var(--xe-surface)",
            borderRadius: 4,
            cursor: "pointer",
            color: "var(--xe-faint)",
            fontFamily: "var(--xe-font-body)",
            fontSize: 12,
          }}
          onMouseEnter={(e) => {
            (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-fg)";
            (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-muted)";
          }}
          onMouseLeave={(e) => {
            (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-border)";
            (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-faint)";
          }}
        >
          <Search size={12} style={{ color: "var(--xe-faint)" }} />
          <span style={{ minWidth: 120 }}>Search widgets...</span>
          <span
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 10,
              background: "var(--xe-border)",
              padding: "1px 5px",
              borderRadius: 3,
              color: "var(--xe-muted)",
              letterSpacing: "0.04em",
            }}
          >
            ⌘K
          </span>
        </button>

        {/* Notifications */}
        <div className="relative" ref={notifRef}>
          <button
            type="button"
            onClick={() => setIsNotifOpen((prev) => !prev)}
            aria-label="Notifications"
            className="relative xe-transition"
            style={iconBtnStyle}
            onMouseEnter={(e) => {
              (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-fg)";
              (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)";
            }}
            onMouseLeave={(e) => {
              (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-border)";
              (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-muted)";
            }}
          >
            <Bell size={14} />
            {unreadCount > 0 && (
              <span
                className="absolute"
                style={{
                  top: 7,
                  right: 7,
                  width: 5,
                  height: 5,
                  borderRadius: "50%",
                  background: "var(--xe-fg)",
                }}
              />
            )}
          </button>

          {isNotifOpen && (
            <div
              className="absolute"
              style={{
                right: 0,
                top: 40,
                zIndex: 50,
                width: 300,
                background: "var(--xe-bg)",
                border: "1px solid var(--xe-border)",
                borderRadius: 6,
                boxShadow: "0 8px 24px rgba(0,0,0,0.08)",
                overflow: "hidden",
              }}
            >
              <div
                className="flex items-center justify-between"
                style={{
                  padding: "10px 14px",
                  borderBottom: "1px solid var(--xe-border)",
                }}
              >
                <span
                  style={{
                    fontFamily: "var(--xe-font-mono)",
                    fontSize: 10,
                    color: "var(--xe-muted)",
                    letterSpacing: "0.06em",
                    textTransform: "uppercase",
                  }}
                >
                  Notifications ({notifications.length})
                </span>
                <div className="flex items-center" style={{ gap: 8 }}>
                  {notifications.length > 0 && (
                    <button
                      type="button"
                      onClick={clearNotifications}
                      className="flex items-center xe-transition"
                      style={{
                        gap: 4,
                        fontSize: 11,
                        color: "var(--xe-faint)",
                        cursor: "pointer",
                        fontFamily: "var(--xe-font-body)",
                      }}
                      onMouseEnter={(e) =>
                        ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)")
                      }
                      onMouseLeave={(e) =>
                        ((e.currentTarget as HTMLButtonElement).style.color = "var(--xe-faint)")
                      }
                    >
                      <Trash2 size={11} />
                      Clear
                    </button>
                  )}
                  <button
                    type="button"
                    onClick={() => setIsNotifOpen(false)}
                    style={{ color: "var(--xe-faint)", cursor: "pointer" }}
                  >
                    <X size={13} />
                  </button>
                </div>
              </div>

              <div style={{ maxHeight: 260, overflowY: "auto" }}>
                {notifications.length === 0 ? (
                  <div
                    style={{
                      padding: "24px 14px",
                      textAlign: "center",
                      fontSize: 12,
                      color: "var(--xe-faint)",
                      fontFamily: "var(--xe-font-body)",
                    }}
                  >
                    No notifications
                  </div>
                ) : (
                  notifications.map((n) => (
                    <div
                      key={n.id}
                      style={{
                        padding: "10px 14px",
                        borderBottom: "1px solid var(--xe-border)",
                      }}
                    >
                      <div className="flex items-center justify-between">
                        <span
                          style={{
                            fontSize: 12,
                            fontFamily: "var(--xe-font-body)",
                            fontWeight: 500,
                            color: "var(--xe-fg)",
                          }}
                        >
                          {n.title}
                        </span>
                        <span
                          style={{
                            fontFamily: "var(--xe-font-mono)",
                            fontSize: 10,
                            color: "var(--xe-faint)",
                          }}
                        >
                          {n.timestamp}
                        </span>
                      </div>
                      <p
                        style={{
                          marginTop: 3,
                          fontSize: 11,
                          color: "var(--xe-muted)",
                          fontFamily: "var(--xe-font-body)",
                          lineHeight: 1.5,
                        }}
                      >
                        {n.message}
                      </p>
                    </div>
                  ))
                )}
              </div>
            </div>
          )}
        </div>

        {/* Theme toggle */}
        <button
          type="button"
          onClick={handleThemeToggle}
          aria-label="Toggle theme"
          className="xe-transition"
          style={iconBtnStyle}
          onMouseEnter={(e) => {
            (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-fg)";
            (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)";
          }}
          onMouseLeave={(e) => {
            (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-border)";
            (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-muted)";
          }}
        >
          {settings.theme === "dark" ? <Sun size={14} /> : <Moon size={14} />}
        </button>

        {/* Optional action button slot */}
        {actionButton}
      </div>
    </header>
  );
}
