import { useState } from "react";
import { Search } from "lucide-react";
import Header from "../components/layout/Header";
import { useWidgets } from "../context/WidgetContext";
import type { WidgetCategory } from "../types";

const CATEGORIES: WidgetCategory[] = ["All", "Productivity", "System", "Media", "Utility"];

const ICON_MAP: Record<string, string> = {
  clock: "◷",
  monitor: "◈",
  notes: "◻",
  music: "♪",
  weather: "◉",
  calendar: "▦",
};

export default function WidgetLibraryView() {
  const { widgets, toggleWidget, togglePinWidget, addNotification } = useWidgets();
  const [selectedCategory, setSelectedCategory] = useState<WidgetCategory>("All");
  const [search, setSearch] = useState("");

  const filteredWidgets = widgets.filter((w) => {
    const matchesCategory = selectedCategory === "All" || w.category === selectedCategory;
    const matchesSearch =
      w.name.toLowerCase().includes(search.toLowerCase()) ||
      w.description.toLowerCase().includes(search.toLowerCase()) ||
      w.tags.some((t) => t.toLowerCase().includes(search.toLowerCase()));
    return matchesCategory && matchesSearch;
  });

  return (
    <div className="flex h-full flex-col overflow-hidden">
      <Header
        title="Widget Library"
        subtitle="Discover, activate, and manage companion widgets."
      />

      <div className="flex-1 overflow-y-auto" style={{ padding: "28px 40px" }}>

        {/* Toolbar */}
        <div
          className="flex items-center justify-between"
          style={{ marginBottom: 28 }}
        >
          {/* Category tabs */}
          <div className="flex items-center" style={{ gap: 0 }}>
            {CATEGORIES.map((cat) => (
              <button
                key={cat}
                type="button"
                onClick={() => setSelectedCategory(cat)}
                className="xe-transition"
                style={{
                  padding: "6px 14px",
                  fontSize: 12,
                  fontFamily: "var(--xe-font-body)",
                  color: selectedCategory === cat ? "var(--xe-fg)" : "var(--xe-muted)",
                  fontWeight: selectedCategory === cat ? 500 : 400,
                  cursor: "pointer",
                  background: "transparent",
                  border: "none",
                  borderBottom: selectedCategory === cat
                    ? "2px solid var(--xe-fg)"
                    : "2px solid transparent",
                  borderRadius: 0,
                  marginBottom: 0,
                }}
              >
                {cat}
              </button>
            ))}
          </div>

          {/* Search */}
          <div className="relative" style={{ width: 220 }}>
            <Search
              size={12}
              style={{
                position: "absolute",
                left: 10,
                top: "50%",
                transform: "translateY(-50%)",
                color: "var(--xe-faint)",
              }}
            />
            <input
              type="text"
              placeholder="Filter catalog..."
              value={search}
              onChange={(e) => setSearch(e.target.value)}
              style={{
                width: "100%",
                height: 32,
                paddingLeft: 30,
                paddingRight: 10,
                fontSize: 12,
                fontFamily: "var(--xe-font-body)",
                color: "var(--xe-fg)",
                background: "var(--xe-surface)",
                border: "1px solid var(--xe-border)",
                borderRadius: 4,
                outline: "none",
              }}
            />
          </div>
        </div>

        {/* Divider */}
        <div className="xe-divider" style={{ marginBottom: 24 }} />

        {/* Count label */}
        <div className="xe-section-label" style={{ marginBottom: 16 }}>
          {filteredWidgets.length} WIDGET{filteredWidgets.length !== 1 ? "S" : ""} —{" "}
          {selectedCategory.toUpperCase()}
        </div>

        {/* Grid */}
        {filteredWidgets.length === 0 ? (
          <div
            style={{
              padding: "48px 24px",
              textAlign: "center",
              border: "1px dashed var(--xe-border)",
              borderRadius: 4,
              color: "var(--xe-faint)",
              fontSize: 13,
              fontFamily: "var(--xe-font-body)",
            }}
          >
            No widgets match your search.
          </div>
        ) : (
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
            {filteredWidgets.map((widget) => (
              <div
                key={widget.id}
                className="xe-transition"
                style={{
                  padding: "20px",
                  background: "var(--xe-bg)",
                  display: "flex",
                  flexDirection: "column",
                }}
                onMouseEnter={(e) =>
                  ((e.currentTarget as HTMLDivElement).style.background = "var(--xe-surface)")
                }
                onMouseLeave={(e) =>
                  ((e.currentTarget as HTMLDivElement).style.background = "var(--xe-bg)")
                }
              >
                {/* Top bar */}
                <div className="flex items-start justify-between" style={{ marginBottom: 12 }}>
                  <span
                    style={{
                      fontFamily: "var(--xe-font-mono)",
                      fontSize: 20,
                      color: "var(--xe-faint)",
                      lineHeight: 1,
                    }}
                  >
                    {ICON_MAP[widget.id] ?? "◆"}
                  </span>
                  <div className="flex items-center" style={{ gap: 6 }}>
                    {/* Pin button */}
                    <button
                      type="button"
                      onClick={() => {
                        togglePinWidget(widget.id);
                        addNotification(
                          widget.isPinned ? "Unpinned" : "Pinned",
                          `${widget.name} pin state changed.`
                        );
                      }}
                      title={widget.isPinned ? "Unpin" : "Pin to top"}
                      className="xe-transition"
                      style={{
                        width: 22,
                        height: 22,
                        display: "flex",
                        alignItems: "center",
                        justifyContent: "center",
                        border: "1px solid var(--xe-border)",
                        borderRadius: 3,
                        cursor: "pointer",
                        background: widget.isPinned ? "var(--xe-fg)" : "transparent",
                        color: widget.isPinned ? "var(--xe-bg)" : "var(--xe-faint)",
                        fontSize: 10,
                        fontFamily: "var(--xe-font-mono)",
                      }}
                    >
                      ◈
                    </button>
                    {/* Status badge */}
                    <span
                      style={{
                        fontFamily: "var(--xe-font-mono)",
                        fontSize: 9,
                        letterSpacing: "0.06em",
                        textTransform: "uppercase" as const,
                        color: widget.isActive ? "var(--xe-fg)" : "var(--xe-faint)",
                        padding: "2px 6px",
                        border: `1px solid ${widget.isActive ? "var(--xe-fg)" : "var(--xe-border)"}`,
                        borderRadius: 2,
                      }}
                    >
                      {widget.isActive ? "ACTIVE" : "AVAILABLE"}
                    </span>
                  </div>
                </div>

                {/* Name + version */}
                <div className="flex items-baseline" style={{ gap: 6, marginBottom: 6 }}>
                  <h4
                    style={{
                      fontFamily: "var(--xe-font-body)",
                      fontSize: 13,
                      fontWeight: 500,
                      color: "var(--xe-fg)",
                    }}
                  >
                    {widget.name}
                  </h4>
                  <span
                    style={{
                      fontFamily: "var(--xe-font-mono)",
                      fontSize: 10,
                      color: "var(--xe-faint)",
                    }}
                  >
                    v{widget.version}
                  </span>
                </div>

                {/* Description */}
                <p
                  style={{
                    fontFamily: "var(--xe-font-body)",
                    fontSize: 11,
                    color: "var(--xe-muted)",
                    lineHeight: 1.55,
                    marginBottom: 10,
                    flex: 1,
                    overflow: "hidden",
                    display: "-webkit-box",
                    WebkitLineClamp: 2,
                    WebkitBoxOrient: "vertical" as const,
                  }}
                >
                  {widget.description}
                </p>

                {/* Tags */}
                <div className="flex flex-wrap" style={{ gap: 4, marginBottom: 14 }}>
                  {widget.tags.map((tag) => (
                    <span
                      key={tag}
                      style={{
                        fontFamily: "var(--xe-font-mono)",
                        fontSize: 9,
                        color: "var(--xe-faint)",
                        letterSpacing: "0.04em",
                      }}
                    >
                      {tag}
                    </span>
                  ))}
                </div>

                {/* Footer */}
                <div
                  className="flex items-center justify-between"
                  style={{
                    paddingTop: 12,
                    borderTop: "1px solid var(--xe-border)",
                  }}
                >
                  <span
                    style={{
                      fontFamily: "var(--xe-font-body)",
                      fontSize: 11,
                      color: "var(--xe-faint)",
                    }}
                  >
                    {widget.author}
                  </span>
                  <button
                    type="button"
                    onClick={() => {
                      toggleWidget(widget.id);
                      addNotification(
                        widget.isActive ? "Widget Deactivated" : "Widget Activated",
                        `${widget.name} is now ${widget.isActive ? "disabled" : "running"}.`,
                        widget.isActive ? "info" : "success"
                      );
                    }}
                    className="xe-transition"
                    style={{
                      fontFamily: "var(--xe-font-mono)",
                      fontSize: 10,
                      letterSpacing: "0.04em",
                      textTransform: "uppercase" as const,
                      color: widget.isActive ? "var(--xe-muted)" : "var(--xe-fg)",
                      background: "transparent",
                      border: "1px solid var(--xe-border)",
                      borderRadius: 3,
                      padding: "4px 10px",
                      cursor: "pointer",
                    }}
                    onMouseEnter={(e) => {
                      (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-fg)";
                      (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)";
                    }}
                    onMouseLeave={(e) => {
                      (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-border)";
                      (e.currentTarget as HTMLButtonElement).style.color = widget.isActive
                        ? "var(--xe-muted)"
                        : "var(--xe-fg)";
                    }}
                  >
                    {widget.isActive ? "DISABLE" : "ENABLE"}
                  </button>
                </div>
              </div>
            ))}
          </div>
        )}
      </div>
    </div>
  );
}
