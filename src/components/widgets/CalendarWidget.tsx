import { useState } from "react";
import { CalendarDays, Clock } from "lucide-react";

interface CalendarEvent {
  id: string;
  title: string;
  time: string;
  category: "Meeting" | "Focus" | "Personal";
}

const SAMPLE_EVENTS: CalendarEvent[] = [
  { id: "ev-1", title: "Sprint Review & Release", time: "10:30", category: "Meeting" },
  { id: "ev-2", title: "Deep Work — UI Polish Pass", time: "14:00", category: "Focus" },
  { id: "ev-3", title: "Desktop Widgets Sync", time: "16:45", category: "Personal" },
];

const widgetContainer: React.CSSProperties = {
  height: "100%",
  display: "flex",
  flexDirection: "column",
  border: "1px solid var(--xe-border)",
  borderRadius: 4,
  background: "var(--xe-surface)",
  overflow: "hidden",
};

export default function CalendarWidget() {
  const [currentDate] = useState(new Date());
  const [selectedDay, setSelectedDay] = useState(currentDate.getDate());

  const daysInMonth = 30;
  const startDayOffset = 2;
  const today = currentDate.getDate();
  const days = Array.from({ length: daysInMonth }, (_, i) => i + 1);

  return (
    <div style={widgetContainer}>
      {/* Header */}
      <div
        style={{
          display: "flex",
          alignItems: "center",
          justifyContent: "space-between",
          padding: "8px 12px",
          borderBottom: "1px solid var(--xe-border)",
          flexShrink: 0,
        }}
      >
        <div style={{ display: "flex", alignItems: "center", gap: 6 }}>
          <CalendarDays size={13} style={{ color: "var(--xe-muted)" }} />
          <span
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
              letterSpacing: "0.08em",
              textTransform: "uppercase",
            }}
          >
            CALENDAR
          </span>
        </div>
        <span
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 10,
            color: "var(--xe-muted)",
          }}
        >
          Nov 2026
        </span>
      </div>

      {/* Body */}
      <div style={{ flex: 1, padding: "8px 12px", overflow: "hidden", display: "flex", flexDirection: "column", gap: 8 }}>
        {/* Day headers */}
        <div style={{ display: "grid", gridTemplateColumns: "repeat(7, 1fr)", gap: 2 }}>
          {["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"].map((d) => (
            <div
              key={d}
              style={{
                textAlign: "center",
                fontFamily: "var(--xe-font-mono)",
                fontSize: 8,
                color: "var(--xe-faint)",
                letterSpacing: "0.04em",
              }}
            >
              {d}
            </div>
          ))}
        </div>

        {/* Day grid */}
        <div style={{ display: "grid", gridTemplateColumns: "repeat(7, 1fr)", gap: 2 }}>
          {Array.from({ length: startDayOffset }).map((_, i) => (
            <div key={`e-${i}`} />
          ))}
          {days.slice(0, 14).map((d) => {
            const isToday = d === today;
            const isSelected = d === selectedDay;
            return (
              <button
                key={d}
                type="button"
                onClick={() => setSelectedDay(d)}
                style={{
                  display: "flex",
                  alignItems: "center",
                  justifyContent: "center",
                  height: 22,
                  fontFamily: "var(--xe-font-mono)",
                  fontSize: 10,
                  cursor: "pointer",
                  borderRadius: 2,
                  border: isToday && !isSelected ? "1px solid var(--xe-fg)" : "1px solid transparent",
                  background: isSelected ? "var(--xe-fg)" : "transparent",
                  color: isSelected ? "var(--xe-bg)" : isToday ? "var(--xe-fg)" : "var(--xe-muted)",
                  transition: "all 120ms",
                }}
              >
                {d}
              </button>
            );
          })}
        </div>

        {/* Agenda */}
        <div style={{ flex: 1, overflow: "hidden" }}>
          <div
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
              letterSpacing: "0.06em",
              textTransform: "uppercase",
              marginBottom: 5,
            }}
          >
            AGENDA · NOV {selectedDay}
          </div>
          <div style={{ display: "flex", flexDirection: "column", gap: 4 }}>
            {SAMPLE_EVENTS.map((ev) => (
              <div
                key={ev.id}
                style={{
                  display: "flex",
                  alignItems: "center",
                  justifyContent: "space-between",
                  padding: "5px 8px",
                  border: "1px solid var(--xe-border)",
                  borderRadius: 3,
                  background: "var(--xe-bg)",
                }}
              >
                <div style={{ display: "flex", alignItems: "center", gap: 6 }}>
                  <div
                    style={{
                      width: 4,
                      height: 4,
                      borderRadius: "50%",
                      background: "var(--xe-muted)",
                      flexShrink: 0,
                    }}
                  />
                  <span
                    style={{
                      fontFamily: "var(--xe-font-body)",
                      fontSize: 10,
                      color: "var(--xe-fg)",
                      overflow: "hidden",
                      textOverflow: "ellipsis",
                      whiteSpace: "nowrap",
                      maxWidth: 100,
                    }}
                  >
                    {ev.title}
                  </span>
                </div>
                <div
                  style={{
                    display: "flex",
                    alignItems: "center",
                    gap: 3,
                    fontFamily: "var(--xe-font-mono)",
                    fontSize: 9,
                    color: "var(--xe-faint)",
                    flexShrink: 0,
                  }}
                >
                  <Clock size={9} />
                  {ev.time}
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>

      {/* Footer */}
      <div
        style={{
          display: "flex",
          justifyContent: "space-between",
          padding: "5px 12px",
          borderTop: "1px solid var(--xe-border)",
          flexShrink: 0,
        }}
      >
        <span style={{ fontFamily: "var(--xe-font-mono)", fontSize: 9, color: "var(--xe-faint)" }}>
          {SAMPLE_EVENTS.length} EVENTS
        </span>
        <span style={{ fontFamily: "var(--xe-font-mono)", fontSize: 9, color: "var(--xe-faint)" }}>
          TODAY
        </span>
      </div>
    </div>
  );
}
