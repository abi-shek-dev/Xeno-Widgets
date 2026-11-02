import React, { useState, useRef, useEffect } from "react";
import {
  Search,
  Bell,
  Command,
  Plus,
  CheckCircle2,
  AlertTriangle,
  Info,
  Trash2,
} from "lucide-react";
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
    setCurrentView,
  } = useWidgets();

  const [isNotifOpen, setIsNotifOpen] = useState(false);
  const notifRef = useRef<HTMLDivElement>(null);

  // Close notifications dropdown on outside click
  useEffect(() => {
    function handleClickOutside(event: MouseEvent) {
      if (notifRef.current && !notifRef.current.contains(event.target as Node)) {
        setIsNotifOpen(false);
      }
    }
    document.addEventListener("mousedown", handleClickOutside);
    return () => document.removeEventListener("mousedown", handleClickOutside);
  }, []);

  return (
    <header className="flex h-16 shrink-0 items-center justify-between border-b border-white/[0.08] bg-[#0c0e14]/60 px-6 backdrop-blur-md">
      <div>
        <h1 className="text-base font-semibold tracking-tight text-white">
          {title}
        </h1>
        <p className="text-xs text-zinc-400">
          {subtitle}
        </p>
      </div>

      <div className="flex items-center gap-3">
        {/* Spotlight Quick Search Pill */}
        <button
          type="button"
          onClick={() => setIsCommandPaletteOpen(true)}
          className="pressable-btn flex h-9 items-center gap-2 rounded-xl border border-white/[0.08] bg-white/[0.03] px-3 text-xs text-zinc-400 transition hover:border-white/[0.16] hover:bg-white/[0.06] hover:text-zinc-200"
        >
          <Search size={14} className="text-zinc-500" />
          <span className="hidden sm:inline">Search widgets, actions...</span>
          <span className="sm:hidden">Search</span>
          <span className="ml-2 flex items-center gap-0.5 rounded-md bg-white/[0.06] px-1.5 py-0.5 text-[10px] font-mono text-zinc-400">
            <Command size={10} /> K
          </span>
        </button>

        {/* Notifications Dropdown */}
        <div className="relative" ref={notifRef}>
          <button
            type="button"
            onClick={() => setIsNotifOpen((prev) => !prev)}
            aria-label="Notifications"
            className="pressable-btn relative flex h-9 w-9 items-center justify-center rounded-xl border border-white/[0.08] bg-white/[0.03] text-zinc-400 transition hover:border-white/[0.16] hover:bg-white/[0.06] hover:text-zinc-200"
          >
            <Bell size={15} />
            {unreadCount > 0 && (
              <span className="absolute right-2 top-2 h-2 w-2 rounded-full bg-cyan-400 ring-2 ring-[#0c0e14]" />
            )}
          </button>

          {isNotifOpen && (
            <div className="absolute right-0 top-11 z-50 w-80 rounded-2xl border border-white/[0.1] bg-[#12141c]/95 p-3 shadow-2xl shadow-black/80 backdrop-blur-2xl">
              <div className="mb-2 flex items-center justify-between border-b border-white/[0.06] pb-2 px-1">
                <span className="text-xs font-semibold text-white">
                  Notifications ({notifications.length})
                </span>
                {notifications.length > 0 && (
                  <button
                    type="button"
                    onClick={clearNotifications}
                    className="flex items-center gap-1 text-[11px] text-zinc-400 hover:text-rose-400"
                  >
                    <Trash2 size={11} /> Clear
                  </button>
                )}
              </div>

              <div className="max-h-64 space-y-1.5 overflow-y-auto">
                {notifications.length === 0 ? (
                  <p className="py-6 text-center text-xs text-zinc-500">
                    No new notifications
                  </p>
                ) : (
                  notifications.map((n) => (
                    <div
                      key={n.id}
                      className="rounded-xl border border-white/[0.04] bg-white/[0.02] p-2.5 text-xs transition hover:bg-white/[0.05]"
                    >
                      <div className="flex items-center gap-1.5 font-medium text-zinc-200">
                        {n.type === "success" && (
                          <CheckCircle2 size={13} className="text-emerald-400" />
                        )}
                        {n.type === "warning" && (
                          <AlertTriangle size={13} className="text-amber-400" />
                        )}
                        {n.type === "info" && (
                          <Info size={13} className="text-cyan-400" />
                        )}
                        <span>{n.title}</span>
                        <span className="ml-auto text-[10px] text-zinc-500 font-mono">
                          {n.timestamp}
                        </span>
                      </div>
                      <p className="mt-1 text-[11px] text-zinc-400 leading-relaxed">
                        {n.message}
                      </p>
                    </div>
                  ))
                )}
              </div>
            </div>
          )}
        </div>

        {/* Custom Header Action button (if any) or Default "Browse Library" button */}
        {actionButton ? (
          actionButton
        ) : (
          <button
            type="button"
            onClick={() => setCurrentView("library")}
            className="pressable-btn flex h-9 items-center gap-1.5 rounded-xl bg-gradient-to-r from-cyan-500 to-blue-600 px-3.5 text-xs font-semibold text-white shadow-lg shadow-cyan-500/20 transition hover:from-cyan-400 hover:to-blue-500"
          >
            <Plus size={14} />
            <span>Add Widget</span>
          </button>
        )}
      </div>
    </header>
  );
}
