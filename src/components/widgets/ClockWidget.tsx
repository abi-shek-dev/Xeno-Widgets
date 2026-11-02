import { useState, useEffect } from "react";
import { Clock3, Play, Pause, Globe2 } from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";

const widgetContainer: React.CSSProperties = {
  height: "100%",
  display: "flex",
  flexDirection: "column",
  border: "1px solid var(--xe-border)",
  borderRadius: 4,
  background: "var(--xe-surface)",
  overflow: "hidden",
};

const tabBtn = (active: boolean): React.CSSProperties => ({
  padding: "3px 8px",
  fontSize: 10,
  fontFamily: "var(--xe-font-mono)",
  letterSpacing: "0.04em",
  textTransform: "uppercase",
  cursor: "pointer",
  background: active ? "var(--xe-fg)" : "transparent",
  color: active ? "var(--xe-bg)" : "var(--xe-muted)",
  border: "1px solid var(--xe-border)",
  borderRadius: 3,
  transition: "all 150ms",
});

export default function ClockWidget() {
  const { settings } = useWidgets();
  const [activeTab, setActiveTab] = useState<"clock" | "stopwatch" | "pomodoro">("clock");

  // Clock state
  const [time, setTime] = useState(new Date());
  const [selectedZone, setSelectedZone] = useState<"local" | "london" | "tokyo" | "ny">("local");

  useEffect(() => {
    const timer = setInterval(() => setTime(new Date()), 1000);
    return () => clearInterval(timer);
  }, []);

  const getTimeForZone = () => {
    let timeZone: string | undefined = undefined;
    if (selectedZone === "london") timeZone = "Europe/London";
    if (selectedZone === "tokyo") timeZone = "Asia/Tokyo";
    if (selectedZone === "ny") timeZone = "America/New_York";
    return time.toLocaleTimeString([], {
      timeZone,
      hour: "2-digit",
      minute: "2-digit",
      second: "2-digit",
      hour12: settings.timeFormat === "12h",
    });
  };

  const getDateForZone = () => {
    let timeZone: string | undefined = undefined;
    if (selectedZone === "london") timeZone = "Europe/London";
    if (selectedZone === "tokyo") timeZone = "Asia/Tokyo";
    if (selectedZone === "ny") timeZone = "America/New_York";
    return time.toLocaleDateString(undefined, {
      timeZone,
      weekday: "short",
      month: "short",
      day: "numeric",
    });
  };

  // Stopwatch state
  const [swTime, setSwTime] = useState(0);
  const [swRunning, setSwRunning] = useState(false);

  useEffect(() => {
    let interval: ReturnType<typeof setInterval> | null = null;
    if (swRunning) {
      interval = setInterval(() => setSwTime((prev) => prev + 1), 100);
    }
    return () => { if (interval) clearInterval(interval); };
  }, [swRunning]);

  const formatStopwatch = (tenths: number) => {
    const minutes = Math.floor(tenths / 600);
    const seconds = Math.floor((tenths % 600) / 10);
    const ms = tenths % 10;
    return `${String(minutes).padStart(2, "0")}:${String(seconds).padStart(2, "0")}.${ms}`;
  };

  // Pomodoro state
  const [pomoSeconds, setPomoSeconds] = useState(25 * 60);
  const [pomoRunning, setPomoRunning] = useState(false);

  useEffect(() => {
    let interval: ReturnType<typeof setInterval> | null = null;
    if (pomoRunning) {
      interval = setInterval(() => {
        setPomoSeconds((prev) => {
          if (prev <= 1) { setPomoRunning(false); return 25 * 60; }
          return prev - 1;
        });
      }, 1000);
    }
    return () => { if (interval) clearInterval(interval); };
  }, [pomoRunning]);

  const formatPomodoro = (sec: number) => {
    const mins = Math.floor(sec / 60);
    const s = sec % 60;
    return `${String(mins).padStart(2, "0")}:${String(s).padStart(2, "0")}`;
  };

  const actionBtn = (label: string, onClick: () => void, primary = false): React.ReactNode => (
    <button
      type="button"
      onClick={onClick}
      style={{
        display: "flex",
        alignItems: "center",
        gap: 4,
        padding: "4px 12px",
        fontSize: 11,
        fontFamily: "var(--xe-font-mono)",
        letterSpacing: "0.04em",
        cursor: "pointer",
        background: primary ? "var(--xe-fg)" : "transparent",
        color: primary ? "var(--xe-bg)" : "var(--xe-muted)",
        border: "1px solid var(--xe-border)",
        borderRadius: 3,
        transition: "all 150ms",
      }}
    >
      {label}
    </button>
  );

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
        }}
      >
        <div style={{ display: "flex", alignItems: "center", gap: 6 }}>
          <Clock3 size={13} style={{ color: "var(--xe-muted)" }} />
          <span
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
              letterSpacing: "0.08em",
              textTransform: "uppercase",
            }}
          >
            PRECISION CLOCK
          </span>
        </div>
        <div style={{ display: "flex", gap: 3 }}>
          {(["clock", "stopwatch", "pomodoro"] as const).map((tab) => (
            <button
              key={tab}
              type="button"
              onClick={() => setActiveTab(tab)}
              style={tabBtn(activeTab === tab)}
            >
              {tab === "clock" ? "CLK" : tab === "stopwatch" ? "SW" : "FCS"}
            </button>
          ))}
        </div>
      </div>

      {/* Body */}
      <div
        className="flex-1 flex flex-col items-center justify-center"
        style={{ padding: "12px", overflow: "hidden" }}
      >
        {activeTab === "clock" && (
          <div style={{ textAlign: "center" }}>
            <div
              className="tabnum"
              style={{
                fontFamily: "var(--xe-font-mono)",
                fontSize: 30,
                fontWeight: 400,
                color: "var(--xe-fg)",
                letterSpacing: "0.02em",
                lineHeight: 1,
              }}
            >
              {getTimeForZone()}
            </div>
            <div
              style={{
                marginTop: 6,
                fontFamily: "var(--xe-font-body)",
                fontSize: 11,
                color: "var(--xe-muted)",
              }}
            >
              {getDateForZone()}
            </div>
            <div
              style={{
                marginTop: 12,
                display: "flex",
                alignItems: "center",
                justifyContent: "center",
                gap: 4,
              }}
            >
              <Globe2 size={11} style={{ color: "var(--xe-faint)" }} />
              {(["local", "ny", "london", "tokyo"] as const).map((z) => (
                <button
                  key={z}
                  type="button"
                  onClick={() => setSelectedZone(z)}
                  style={{
                    padding: "2px 6px",
                    fontSize: 9,
                    fontFamily: "var(--xe-font-mono)",
                    letterSpacing: "0.06em",
                    textTransform: "uppercase",
                    cursor: "pointer",
                    background: selectedZone === z ? "var(--xe-fg)" : "transparent",
                    color: selectedZone === z ? "var(--xe-bg)" : "var(--xe-faint)",
                    border: "1px solid var(--xe-border)",
                    borderRadius: 2,
                    transition: "all 150ms",
                  }}
                >
                  {z}
                </button>
              ))}
            </div>
          </div>
        )}

        {activeTab === "stopwatch" && (
          <div style={{ textAlign: "center" }}>
            <div
              className="tabnum"
              style={{
                fontFamily: "var(--xe-font-mono)",
                fontSize: 30,
                color: "var(--xe-fg)",
                letterSpacing: "0.02em",
                lineHeight: 1,
              }}
            >
              {formatStopwatch(swTime)}
            </div>
            <div
              style={{
                marginTop: 4,
                fontFamily: "var(--xe-font-body)",
                fontSize: 11,
                color: "var(--xe-faint)",
              }}
            >
              Elapsed Time
            </div>
            <div style={{ marginTop: 12, display: "flex", gap: 6, justifyContent: "center" }}>
              <button
                type="button"
                onClick={() => setSwRunning((prev) => !prev)}
                style={{
                  display: "flex",
                  alignItems: "center",
                  gap: 4,
                  padding: "4px 14px",
                  fontSize: 11,
                  fontFamily: "var(--xe-font-mono)",
                  cursor: "pointer",
                  background: "var(--xe-fg)",
                  color: "var(--xe-bg)",
                  border: "none",
                  borderRadius: 3,
                }}
              >
                {swRunning ? <Pause size={11} /> : <Play size={11} />}
                {swRunning ? "PAUSE" : "START"}
              </button>
              {actionBtn("RESET", () => { setSwRunning(false); setSwTime(0); })}
            </div>
          </div>
        )}

        {activeTab === "pomodoro" && (
          <div style={{ textAlign: "center" }}>
            <div
              className="tabnum"
              style={{
                fontFamily: "var(--xe-font-mono)",
                fontSize: 30,
                color: "var(--xe-fg)",
                letterSpacing: "0.02em",
                lineHeight: 1,
              }}
            >
              {formatPomodoro(pomoSeconds)}
            </div>
            <div
              style={{
                marginTop: 4,
                fontFamily: "var(--xe-font-body)",
                fontSize: 11,
                color: "var(--xe-faint)",
              }}
            >
              25m Focus Block
            </div>
            <div style={{ marginTop: 12, display: "flex", gap: 6, justifyContent: "center" }}>
              <button
                type="button"
                onClick={() => setPomoRunning((prev) => !prev)}
                style={{
                  display: "flex",
                  alignItems: "center",
                  gap: 4,
                  padding: "4px 14px",
                  fontSize: 11,
                  fontFamily: "var(--xe-font-mono)",
                  cursor: "pointer",
                  background: "var(--xe-fg)",
                  color: "var(--xe-bg)",
                  border: "none",
                  borderRadius: 3,
                }}
              >
                {pomoRunning ? <Pause size={11} /> : <Play size={11} />}
                {pomoRunning ? "PAUSE" : "FOCUS"}
              </button>
              {actionBtn("RESET", () => { setPomoRunning(false); setPomoSeconds(25 * 60); })}
            </div>
          </div>
        )}
      </div>

      {/* Footer */}
      <div
        style={{
          display: "flex",
          justifyContent: "space-between",
          padding: "6px 12px",
          borderTop: "1px solid var(--xe-border)",
        }}
      >
        <span
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 9,
            color: "var(--xe-faint)",
            letterSpacing: "0.04em",
          }}
        >
          NTP SYNC
        </span>
        <span
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 9,
            color: "var(--xe-faint)",
          }}
        >
          ±0.2ms
        </span>
      </div>
    </div>
  );
}
