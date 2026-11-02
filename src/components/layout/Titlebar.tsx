import React, { useState, useEffect } from "react";
import {
  Minus,
  Square,
  X,
  Search,
  Palette,
  Sparkles,
  Command,
} from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";

export default function Titlebar() {
  const {
    setIsCommandPaletteOpen,
    settings,
    updateSettings,
    metrics,
  } = useWidgets();

  const [timeString, setTimeString] = useState("");
  const [isMaximized, setIsMaximized] = useState(false);

  useEffect(() => {
    const updateTime = () => {
      const now = new Date();
      setTimeString(
        now.toLocaleTimeString([], {
          hour: "2-digit",
          minute: "2-digit",
          hour12: settings.timeFormat === "12h",
        })
      );
    };
    updateTime();
    const interval = setInterval(updateTime, 1000);
    return () => clearInterval(interval);
  }, [settings.timeFormat]);

  const cycleTheme = () => {
    const themes: Array<"slate" | "obsidian" | "aurora" | "cyberpunk" | "paper"> = [
      "slate",
      "obsidian",
      "aurora",
      "cyberpunk",
      "paper",
    ];
    const currentIndex = themes.indexOf(settings.themeId);
    const nextTheme = themes[(currentIndex + 1) % themes.length];
    updateSettings({ themeId: nextTheme });
    document.body.setAttribute("data-theme", nextTheme);
  };

  const handleMinimize = async () => {
    try {
      const { getCurrentWindow } = await import("@tauri-apps/api/window");
      getCurrentWindow().minimize();
    } catch {
      // In web dev environment
    }
  };

  const handleMaximize = async () => {
    try {
      const { getCurrentWindow } = await import("@tauri-apps/api/window");
      const win = getCurrentWindow();
      const maximized = await win.isMaximized();
      if (maximized) {
        await win.unmaximize();
        setIsMaximized(false);
      } else {
        await win.maximize();
        setIsMaximized(true);
      }
    } catch {
      setIsMaximized((prev) => !prev);
    }
  };

  const handleClose = async () => {
    try {
      const { getCurrentWindow } = await import("@tauri-apps/api/window");
      getCurrentWindow().close();
    } catch {
      // In web dev environment
    }
  };

  return (
    <div
      data-tauri-drag-region
      className="flex h-10 w-full shrink-0 select-none items-center justify-between border-b border-white/[0.07] bg-[#0c0e14]/90 px-3 text-xs text-zinc-400 backdrop-blur-xl"
    >
      {/* Brand & App Title */}
      <div className="flex items-center gap-2.5 no-drag">
        <div className="flex h-5 w-5 items-center justify-center rounded-md bg-gradient-to-tr from-cyan-500 to-purple-600 font-mono text-[10px] font-bold text-white shadow-sm shadow-cyan-500/30">
          XE
        </div>
        <span className="font-medium tracking-tight text-zinc-200">
          XE Widgets
        </span>
        <span className="rounded bg-white/[0.06] px-1.5 py-0.5 text-[9px] font-medium text-zinc-400">
          v2.0
        </span>
      </div>

      {/* Center Widget Info / Clock Pill */}
      <div className="flex items-center gap-2">
        <button
          type="button"
          onClick={() => setIsCommandPaletteOpen(true)}
          className="no-drag pressable-btn flex items-center gap-2 rounded-full border border-white/[0.08] bg-white/[0.03] px-2.5 py-0.5 text-[11px] text-zinc-400 transition hover:border-white/[0.18] hover:bg-white/[0.07] hover:text-zinc-200"
        >
          <Search size={11} className="text-zinc-500" />
          <span className="tabular-num font-mono text-[11px] font-medium text-zinc-300">
            {timeString}
          </span>
          <span className="h-1 w-1 rounded-full bg-emerald-400" />
          <span className="flex items-center gap-0.5 text-[10px] text-zinc-500">
            <Command size={9} />K
          </span>
        </button>

        <div className="hidden items-center gap-1.5 text-[11px] text-zinc-500 lg:flex">
          <span>CPU</span>
          <span className="tabular-num font-mono text-zinc-300">
            {metrics.cpuUsage}%
          </span>
        </div>
      </div>

      {/* Action Controls & Windows Buttons */}
      <div className="flex items-center gap-1 no-drag">
        {/* Quick Theme Switcher */}
        <button
          type="button"
          onClick={cycleTheme}
          title={`Current Theme: ${settings.themeId}. Click to switch.`}
          className="pressable-btn flex h-7 w-7 items-center justify-center rounded-md text-zinc-400 transition hover:bg-white/[0.08] hover:text-zinc-200"
        >
          <Palette size={13} />
        </button>

        {/* Windows Standard Controls */}
        <div className="ml-1 flex items-center">
          <button
            type="button"
            onClick={handleMinimize}
            aria-label="Minimize"
            className="flex h-7 w-9 items-center justify-center text-zinc-400 transition hover:bg-white/[0.08] hover:text-white"
          >
            <Minus size={13} />
          </button>
          <button
            type="button"
            onClick={handleMaximize}
            aria-label="Maximize"
            className="flex h-7 w-9 items-center justify-center text-zinc-400 transition hover:bg-white/[0.08] hover:text-white"
          >
            <Square size={11} />
          </button>
          <button
            type="button"
            onClick={handleClose}
            aria-label="Close"
            className="flex h-7 w-9 items-center justify-center text-zinc-400 transition hover:bg-rose-600 hover:text-white"
          >
            <X size={13} />
          </button>
        </div>
      </div>
    </div>
  );
}
