import React from "react";
import {
  Activity,
  Cpu,
  HardDrive,
  Wifi,
  ArrowDown,
  ArrowUp,
  Zap,
} from "lucide-react";
import { motion } from "motion/react";
import { useWidgets } from "../../context/WidgetContext";

export default function SystemMonitorWidget() {
  const { metrics } = useWidgets();

  return (
    <div className="glass-card flex h-full flex-col justify-between overflow-hidden rounded-2xl p-5">
      {/* Widget Header */}
      <div className="flex items-center justify-between border-b border-white/[0.06] pb-3">
        <div className="flex items-center gap-2">
          <div className="flex h-7 w-7 items-center justify-center rounded-lg bg-purple-500/10 text-purple-400 border border-purple-500/20">
            <Activity size={15} />
          </div>
          <span className="text-xs font-semibold text-white">System Telemetry</span>
        </div>
        <div className="flex items-center gap-1.5 rounded-full bg-emerald-500/10 px-2 py-0.5 text-[10px] font-medium text-emerald-400">
          <span className="h-1.5 w-1.5 rounded-full bg-emerald-400 animate-pulse" />
          Optimal
        </div>
      </div>

      {/* Grid of Gauges */}
      <div className="grid grid-cols-2 gap-3 my-auto py-2">
        {/* CPU Box */}
        <div className="rounded-xl border border-white/[0.06] bg-white/[0.02] p-3">
          <div className="flex items-center justify-between text-[11px] text-zinc-400">
            <span className="flex items-center gap-1.5">
              <Cpu size={12} className="text-cyan-400" /> CPU
            </span>
            <span className="tabular-num font-mono font-semibold text-white">
              {metrics.cpuUsage}%
            </span>
          </div>

          {/* Sparkline bars */}
          <div className="mt-2.5 flex h-7 items-end gap-1">
            {metrics.cpuHistory.map((val, idx) => (
              <motion.div
                key={idx}
                className="w-full rounded-sm bg-cyan-400/80"
                style={{ height: `${Math.max(15, val)}%` }}
                animate={{ height: `${Math.max(15, val)}%` }}
                transition={{ ease: "easeOut", duration: 0.3 }}
              />
            ))}
          </div>
        </div>

        {/* RAM Box */}
        <div className="rounded-xl border border-white/[0.06] bg-white/[0.02] p-3">
          <div className="flex items-center justify-between text-[11px] text-zinc-400">
            <span className="flex items-center gap-1.5">
              <Zap size={12} className="text-purple-400" /> RAM
            </span>
            <span className="tabular-num font-mono font-semibold text-white">
              {Math.round((metrics.ramUsedGB / metrics.ramTotalGB) * 100)}%
            </span>
          </div>
          <p className="mt-1 text-[10px] font-mono text-zinc-400">
            {metrics.ramUsedGB} GB of {metrics.ramTotalGB} GB
          </p>
          <div className="mt-2.5 h-1.5 w-full overflow-hidden rounded-full bg-white/[0.06]">
            <motion.div
              className="h-full rounded-full bg-purple-500"
              animate={{
                width: `${(metrics.ramUsedGB / metrics.ramTotalGB) * 100}%`,
              }}
              transition={{ ease: "easeOut", duration: 0.5 }}
            />
          </div>
        </div>

        {/* Storage Disk */}
        <div className="rounded-xl border border-white/[0.06] bg-white/[0.02] p-3">
          <div className="flex items-center justify-between text-[11px] text-zinc-400">
            <span className="flex items-center gap-1.5">
              <HardDrive size={12} className="text-amber-400" /> NVMe SSD
            </span>
            <span className="tabular-num font-mono font-semibold text-white">
              {metrics.diskUsedPercent}%
            </span>
          </div>
          <div className="mt-3 h-1.5 w-full overflow-hidden rounded-full bg-white/[0.06]">
            <div
              className="h-full rounded-full bg-amber-500"
              style={{ width: `${metrics.diskUsedPercent}%` }}
            />
          </div>
        </div>

        {/* Network Throughput */}
        <div className="rounded-xl border border-white/[0.06] bg-white/[0.02] p-3">
          <div className="flex items-center justify-between text-[11px] text-zinc-400">
            <span className="flex items-center gap-1.5">
              <Wifi size={12} className="text-emerald-400" /> Network
            </span>
          </div>
          <div className="mt-2 flex items-center justify-between text-[10px] font-mono">
            <span className="flex items-center gap-0.5 text-emerald-400">
              <ArrowDown size={11} /> {metrics.networkDownMbps} M/s
            </span>
            <span className="flex items-center gap-0.5 text-zinc-400">
              <ArrowUp size={11} /> {metrics.networkUpMbps} M/s
            </span>
          </div>
        </div>
      </div>

      {/* Footer */}
      <div className="flex items-center justify-between border-t border-white/[0.06] pt-2 text-[10px] text-zinc-500">
        <span>GPU Core: {metrics.gpuUsage}%</span>
        <span>Uptime: {metrics.uptimeHours} hrs</span>
      </div>
    </div>
  );
}
