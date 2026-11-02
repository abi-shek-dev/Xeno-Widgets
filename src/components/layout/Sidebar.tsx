import React from "react";
import {
  LayoutDashboard,
  Grid2X2,
  AppWindow,
  Settings2,
  Cpu,
  Layers,
  Sparkles,
} from "lucide-react";
import { motion } from "motion/react";
import { useWidgets } from "../../context/WidgetContext";
import type { NavView } from "../../types";

const NAV_ITEMS: { id: NavView; label: string; icon: React.ElementType }[] = [
  { id: "overview", label: "Overview", icon: LayoutDashboard },
  { id: "library", label: "Widget Library", icon: Grid2X2 },
  { id: "active", label: "Active Widgets", icon: AppWindow },
  { id: "settings", label: "Settings", icon: Settings2 },
];

export default function Sidebar() {
  const { currentView, setCurrentView, activeWidgets, metrics, widgets } =
    useWidgets();

  return (
    <aside className="flex h-full w-60 shrink-0 flex-col border-r border-white/[0.08] bg-[#0c0e14]/95 p-4 text-zinc-300 backdrop-blur-2xl">
      {/* Brand Workspace Header */}
      <div className="mb-6 flex items-center gap-3 px-2">
        <div className="relative flex h-9 w-9 items-center justify-center rounded-xl bg-gradient-to-br from-cyan-500/20 via-purple-500/10 to-transparent border border-cyan-500/30 text-cyan-400 shadow-inner">
          <Layers size={18} />
          <span className="absolute -bottom-0.5 -right-0.5 flex h-2.5 w-2.5">
            <span className="absolute inline-flex h-full w-full animate-ping rounded-full bg-cyan-400 opacity-75" />
            <span className="relative inline-flex h-2.5 w-2.5 rounded-full bg-cyan-500" />
          </span>
        </div>
        <div className="min-w-0 flex-1">
          <h2 className="truncate text-sm font-semibold tracking-tight text-white">
            Desktop Studio
          </h2>
          <p className="truncate text-[11px] text-zinc-500">
            Fluent Suite Engine
          </p>
        </div>
      </div>

      {/* Navigation section */}
      <div className="mb-2 px-2 text-[10px] font-semibold uppercase tracking-wider text-zinc-500">
        Workspace
      </div>

      <nav className="space-y-1">
        {NAV_ITEMS.map((item) => {
          const Icon = item.icon;
          const isActive = currentView === item.id;

          let badgeContent: React.ReactNode = null;
          if (item.id === "active") {
            badgeContent = (
              <span className="ml-auto rounded-full bg-cyan-500/20 px-1.5 py-0.5 text-[10px] font-medium text-cyan-300">
                {activeWidgets.length}
              </span>
            );
          } else if (item.id === "library") {
            badgeContent = (
              <span className="ml-auto text-[10px] text-zinc-500">
                {widgets.length}
              </span>
            );
          }

          return (
            <button
              key={item.id}
              type="button"
              onClick={() => setCurrentView(item.id)}
              className={`pressable-btn group relative flex w-full items-center gap-3 rounded-xl px-3 py-2.5 text-left text-xs font-medium transition-all ${
                isActive
                  ? "bg-white/[0.08] text-white shadow-sm shadow-black/40"
                  : "text-zinc-400 hover:bg-white/[0.04] hover:text-zinc-200"
              }`}
            >
              {isActive && (
                <motion.div
                  layoutId="sidebarActivePill"
                  className="absolute left-0 top-2 bottom-2 w-1 rounded-r-full bg-cyan-400 shadow-[0_0_10px_rgba(56,189,248,0.7)]"
                  transition={{ type: "spring", stiffness: 350, damping: 30 }}
                />
              )}

              <Icon
                size={16}
                className={`transition-colors ${
                  isActive ? "text-cyan-400" : "text-zinc-500 group-hover:text-zinc-300"
                }`}
              />

              <span>{item.label}</span>
              {badgeContent}
            </button>
          );
        })}
      </nav>

      {/* Live System Diagnostics Quick Card */}
      <div className="mt-auto rounded-2xl border border-white/[0.07] bg-white/[0.02] p-3 backdrop-blur-md">
        <div className="mb-2.5 flex items-center justify-between">
          <div className="flex items-center gap-1.5 text-[11px] font-medium text-zinc-300">
            <Cpu size={13} className="text-purple-400" />
            <span>Telemetry</span>
          </div>
          <span className="flex items-center gap-1 rounded bg-purple-500/10 px-1.5 py-0.5 text-[9px] font-medium text-purple-300">
            LIVE
          </span>
        </div>

        {/* CPU Progress */}
        <div className="space-y-1">
          <div className="flex justify-between text-[10px] text-zinc-400">
            <span>CPU Usage</span>
            <span className="tabular-num font-mono text-zinc-200">
              {metrics.cpuUsage}%
            </span>
          </div>
          <div className="h-1.5 w-full overflow-hidden rounded-full bg-white/[0.06]">
            <motion.div
              className="h-full rounded-full bg-gradient-to-r from-cyan-500 to-purple-500"
              animate={{ width: `${metrics.cpuUsage}%` }}
              transition={{ ease: "easeOut", duration: 0.6 }}
            />
          </div>
        </div>

        {/* RAM Progress */}
        <div className="mt-2.5 space-y-1">
          <div className="flex justify-between text-[10px] text-zinc-400">
            <span>RAM</span>
            <span className="tabular-num font-mono text-zinc-200">
              {metrics.ramUsedGB} / {metrics.ramTotalGB} GB
            </span>
          </div>
          <div className="h-1.5 w-full overflow-hidden rounded-full bg-white/[0.06]">
            <motion.div
              className="h-full rounded-full bg-gradient-to-r from-purple-500 to-pink-500"
              animate={{
                width: `${(metrics.ramUsedGB / metrics.ramTotalGB) * 100}%`,
              }}
              transition={{ ease: "easeOut", duration: 0.6 }}
            />
          </div>
        </div>
      </div>
    </aside>
  );
}
