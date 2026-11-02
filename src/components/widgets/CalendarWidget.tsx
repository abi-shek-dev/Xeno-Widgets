import React, { useState } from "react";
import {
  CalendarDays,
  ChevronLeft,
  ChevronRight,
  Clock,
  Plus,
} from "lucide-react";

interface CalendarEvent {
  id: string;
  title: string;
  time: string;
  category: "Meeting" | "Focus" | "Personal";
  color: string;
}

const SAMPLE_EVENTS: CalendarEvent[] = [
  {
    id: "ev-1",
    title: "Sprint Review & Release",
    time: "10:30 AM",
    category: "Meeting",
    color: "bg-purple-500",
  },
  {
    id: "ev-2",
    title: "Deep Work - UI Polish Pass",
    time: "02:00 PM",
    category: "Focus",
    color: "bg-cyan-500",
  },
  {
    id: "ev-3",
    title: "Desktop Widgets Sync",
    time: "04:45 PM",
    category: "Personal",
    color: "bg-emerald-500",
  },
];

export default function CalendarWidget() {
  const [currentDate] = useState(new Date());
  const [selectedDay, setSelectedDay] = useState(currentDate.getDate());

  const daysInMonth = 30;
  const startDayOffset = 2; // Tuesday
  const today = currentDate.getDate();

  const days = Array.from({ length: daysInMonth }, (_, i) => i + 1);

  return (
    <div className="glass-card flex h-full flex-col justify-between overflow-hidden rounded-2xl p-5">
      {/* Header */}
      <div className="flex items-center justify-between border-b border-white/[0.06] pb-3">
        <div className="flex items-center gap-2">
          <div className="flex h-7 w-7 items-center justify-center rounded-lg bg-pink-500/10 text-pink-400 border border-pink-500/20">
            <CalendarDays size={15} />
          </div>
          <span className="text-xs font-semibold text-white">Event Horizon</span>
        </div>

        <div className="flex items-center gap-1 text-[11px] font-medium text-zinc-300">
          <span>November 2026</span>
        </div>
      </div>

      {/* Mini Calendar Grid */}
      <div className="my-auto py-2">
        <div className="grid grid-cols-7 gap-1 text-center text-[10px] font-semibold text-zinc-500 mb-1">
          <span>Su</span>
          <span>Mo</span>
          <span>Tu</span>
          <span>We</span>
          <span>Th</span>
          <span>Fr</span>
          <span>Sa</span>
        </div>

        <div className="grid grid-cols-7 gap-1 text-center">
          {Array.from({ length: startDayOffset }).map((_, i) => (
            <div key={`empty-${i}`} className="h-6 w-full" />
          ))}

          {days.slice(0, 14).map((d) => {
            const isToday = d === today;
            const isSelected = d === selectedDay;

            return (
              <button
                key={d}
                type="button"
                onClick={() => setSelectedDay(d)}
                className={`flex h-6 w-full items-center justify-center rounded-md text-[11px] font-mono transition ${
                  isSelected
                    ? "bg-pink-500 font-bold text-white shadow-sm"
                    : isToday
                    ? "border border-pink-500/50 bg-pink-500/10 text-pink-300"
                    : "text-zinc-400 hover:bg-white/[0.05] hover:text-white"
                }`}
              >
                {d}
              </button>
            );
          })}
        </div>

        {/* Selected Day Agenda */}
        <div className="mt-3 space-y-1.5 border-t border-white/[0.06] pt-2">
          <div className="text-[10px] font-semibold uppercase tracking-wider text-zinc-500">
            Agenda · Nov {selectedDay}
          </div>

          {SAMPLE_EVENTS.map((ev) => (
            <div
              key={ev.id}
              className="flex items-center justify-between rounded-lg bg-white/[0.02] p-1.5 text-xs"
            >
              <div className="flex items-center gap-2">
                <span className={`h-2 w-2 rounded-full ${ev.color}`} />
                <span className="text-[11px] font-medium text-zinc-200">
                  {ev.title}
                </span>
              </div>
              <div className="flex items-center gap-1 text-[10px] font-mono text-zinc-500">
                <Clock size={10} />
                <span>{ev.time}</span>
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* Footer */}
      <div className="flex items-center justify-between border-t border-white/[0.06] pt-2 text-[10px] text-zinc-500">
        <span>3 events pending today</span>
        <button
          type="button"
          className="flex items-center gap-0.5 text-pink-400 hover:text-pink-300"
        >
          <Plus size={10} /> Add
        </button>
      </div>
    </div>
  );
}
