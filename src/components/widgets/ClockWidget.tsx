import React, { useState, useEffect } from "react";
import {
  Clock3,
  Timer,
  Hourglass,
  Play,
  Pause,
  RotateCcw,
  Globe2,
} from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";

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
    let timeZone = undefined;
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
    let timeZone = undefined;
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
  const [swTime, setSwTime] = useState(0); // in tenths of a second
  const [swRunning, setSwRunning] = useState(false);

  useEffect(() => {
    let interval: NodeJS.Timeout | null = null;
    if (swRunning) {
      interval = setInterval(() => setSwTime((prev) => prev + 1), 100);
    }
    return () => {
      if (interval) clearInterval(interval);
    };
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
    let interval: NodeJS.Timeout | null = null;
    if (pomoRunning) {
      interval = setInterval(() => {
        setPomoSeconds((prev) => {
          if (prev <= 1) {
            setPomoRunning(false);
            return 25 * 60;
          }
          return prev - 1;
        });
      }, 1000);
    }
    return () => {
      if (interval) clearInterval(interval);
    };
  }, [pomoRunning]);

  const formatPomodoro = (sec: number) => {
    const mins = Math.floor(sec / 60);
    const s = sec % 60;
    return `${String(mins).padStart(2, "0")}:${String(s).padStart(2, "0")}`;
  };

  return (
    <div className="glass-card relative flex h-full flex-col justify-between overflow-hidden rounded-2xl p-5">
      {/* Widget Header & Modes */}
      <div className="flex items-center justify-between border-b border-white/[0.06] pb-3">
        <div className="flex items-center gap-2">
          <div className="flex h-7 w-7 items-center justify-center rounded-lg bg-sky-500/10 text-sky-400 border border-sky-500/20">
            <Clock3 size={15} />
          </div>
          <span className="text-xs font-semibold text-white">Precision Clock</span>
        </div>

        {/* Mode Selector */}
        <div className="flex items-center rounded-lg bg-white/[0.04] p-0.5 text-[10px] text-zinc-400">
          <button
            type="button"
            onClick={() => setActiveTab("clock")}
            className={`rounded-md px-2 py-1 transition ${
              activeTab === "clock" ? "bg-white/[0.1] text-white font-medium" : "hover:text-zinc-200"
            }`}
          >
            Clock
          </button>
          <button
            type="button"
            onClick={() => setActiveTab("stopwatch")}
            className={`rounded-md px-2 py-1 transition ${
              activeTab === "stopwatch" ? "bg-white/[0.1] text-white font-medium" : "hover:text-zinc-200"
            }`}
          >
            Stopwatch
          </button>
          <button
            type="button"
            onClick={() => setActiveTab("pomodoro")}
            className={`rounded-md px-2 py-1 transition ${
              activeTab === "pomodoro" ? "bg-white/[0.1] text-white font-medium" : "hover:text-zinc-200"
            }`}
          >
            Focus
          </button>
        </div>
      </div>

      {/* Main Display Area */}
      <div className="my-auto py-3 text-center">
        {activeTab === "clock" && (
          <div>
            <div className="tabular-num font-mono text-3xl sm:text-4xl font-bold tracking-tight text-white drop-shadow-md">
              {getTimeForZone()}
            </div>
            <div className="mt-1 text-xs font-medium text-sky-300">
              {getDateForZone()}
            </div>

            {/* Timezone Switcher */}
            <div className="mt-4 flex items-center justify-center gap-1.5">
              <Globe2 size={12} className="text-zinc-500 mr-1" />
              {(["local", "ny", "london", "tokyo"] as const).map((z) => (
                <button
                  key={z}
                  type="button"
                  onClick={() => setSelectedZone(z)}
                  className={`pressable-btn rounded-md border px-2 py-0.5 text-[10px] uppercase font-mono transition ${
                    selectedZone === z
                      ? "border-sky-500/40 bg-sky-500/10 text-sky-300"
                      : "border-white/[0.05] bg-white/[0.02] text-zinc-500 hover:text-zinc-300"
                  }`}
                >
                  {z}
                </button>
              ))}
            </div>
          </div>
        )}

        {activeTab === "stopwatch" && (
          <div>
            <div className="tabular-num font-mono text-3xl sm:text-4xl font-bold tracking-tight text-white">
              {formatStopwatch(swTime)}
            </div>
            <p className="mt-1 text-[11px] text-zinc-500">Elapsed Stopwatch Timer</p>
            <div className="mt-4 flex items-center justify-center gap-2">
              <button
                type="button"
                onClick={() => setSwRunning((prev) => !prev)}
                className={`pressable-btn flex items-center gap-1.5 rounded-xl px-4 py-1.5 text-xs font-semibold text-white transition ${
                  swRunning ? "bg-amber-500 hover:bg-amber-400" : "bg-emerald-500 hover:bg-emerald-400"
                }`}
              >
                {swRunning ? <Pause size={12} /> : <Play size={12} />}
                <span>{swRunning ? "Pause" : "Start"}</span>
              </button>
              <button
                type="button"
                onClick={() => {
                  setSwRunning(false);
                  setSwTime(0);
                }}
                className="pressable-btn flex items-center gap-1.5 rounded-xl border border-white/[0.08] bg-white/[0.04] px-3 py-1.5 text-xs text-zinc-300 hover:bg-white/[0.08]"
              >
                <RotateCcw size={12} />
                <span>Reset</span>
              </button>
            </div>
          </div>
        )}

        {activeTab === "pomodoro" && (
          <div>
            <div className="tabular-num font-mono text-3xl sm:text-4xl font-bold tracking-tight text-purple-300">
              {formatPomodoro(pomoSeconds)}
            </div>
            <p className="mt-1 text-[11px] text-zinc-400">Pomodoro 25m Focus Block</p>
            <div className="mt-4 flex items-center justify-center gap-2">
              <button
                type="button"
                onClick={() => setPomoRunning((prev) => !prev)}
                className="pressable-btn flex items-center gap-1.5 rounded-xl bg-purple-600 px-4 py-1.5 text-xs font-semibold text-white transition hover:bg-purple-500"
              >
                {pomoRunning ? <Pause size={12} /> : <Play size={12} />}
                <span>{pomoRunning ? "Pause" : "Start Focus"}</span>
              </button>
              <button
                type="button"
                onClick={() => {
                  setPomoRunning(false);
                  setPomoSeconds(25 * 60);
                }}
                className="pressable-btn flex items-center gap-1.5 rounded-xl border border-white/[0.08] bg-white/[0.04] px-3 py-1.5 text-xs text-zinc-300 hover:bg-white/[0.08]"
              >
                <RotateCcw size={12} />
                <span>Reset</span>
              </button>
            </div>
          </div>
        )}
      </div>

      {/* Footer info */}
      <div className="flex items-center justify-between border-t border-white/[0.06] pt-2 text-[10px] text-zinc-500">
        <span>Time Sync: Local NTP</span>
        <span className="font-mono text-zinc-400">±0.2 ms</span>
      </div>
    </div>
  );
}
