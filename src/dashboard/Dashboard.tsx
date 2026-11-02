
import {
  Clock3,
  Activity,
  StickyNote,
  CalendarDays,
  Music2,
  Plus,
} from "lucide-react";

import Header from "../components/layout/Header";
import WidgetCard from "../components/ui/WidgetCard";

const widgets = [
  {
    name: "XE Clock",
    description: "A customizable desktop clock with elegant styles.",
    icon: Clock3,
    category: "Productivity",
    status: "available" as const,
  },
  {
    name: "XE Monitor",
    description: "Keep an eye on CPU, memory and system performance.",
    icon: Activity,
    category: "System",
    status: "available" as const,
  },
  {
    name: "XE Notes",
    description: "Quick notes that stay accessible on your desktop.",
    icon: StickyNote,
    category: "Productivity",
    status: "available" as const,
  },
  {
    name: "XE Calendar",
    description: "Your schedule and upcoming events at a glance.",
    icon: CalendarDays,
    category: "Productivity",
    status: "available" as const,
  },
  {
    name: "XE Music",
    description: "Control your music without leaving your desktop.",
    icon: Music2,
    category: "Entertainment",
    status: "available" as const,
  },
];

export default function Dashboard() {
  return (
    <div className="min-h-screen">
      <Header
        title="Overview"
        description="Your desktop, your way."
      />

      <main className="space-y-9 p-8">
        <section className="relative overflow-hidden rounded-3xl border border-violet-400/15 bg-gradient-to-br from-violet-500/15 via-[#17151f] to-[#111114] p-8">
          <div className="relative z-10 max-w-xl">
            <span className="text-[11px] font-semibold uppercase tracking-[0.2em] text-violet-300">
              XE Desktop Environment
            </span>

            <h2 className="mt-4 text-3xl font-semibold tracking-tight text-white">
              Make your desktop
              <br />
              feel like yours.
            </h2>

            <p className="mt-3 max-w-md text-sm leading-6 text-zinc-400">
              Build your own desktop experience with beautiful,
              customizable XE widgets.
            </p>

            <button className="mt-6 flex items-center gap-2 rounded-xl bg-violet-500 px-4 py-3 text-xs font-semibold text-white transition hover:bg-violet-400">
              <Plus size={16} />
              Explore widgets
            </button>
          </div>

          <div className="pointer-events-none absolute -right-20 -top-32 h-80 w-80 rounded-full bg-violet-500/10 blur-3xl" />
        </section>

        <section>
          <div className="mb-5 flex items-center justify-between">
            <div>
              <h2 className="text-base font-semibold text-white">
                Widget Library
              </h2>
              <p className="mt-1 text-xs text-zinc-500">
                Discover your XE desktop tools.
              </p>
            </div>

            <span className="text-xs text-zinc-500">
              {widgets.length} widgets
            </span>
          </div>

          <div className="grid grid-cols-1 gap-4 md:grid-cols-2 xl:grid-cols-3">
            {widgets.map((widget) => (
              <WidgetCard key={widget.name} {...widget} />
            ))}
          </div>
        </section>
      </main>
    </div>
  );
}
