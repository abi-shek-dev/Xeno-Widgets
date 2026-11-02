
import {
  LayoutDashboard,
  Grid2X2,
  AppWindow,
  Settings2,
} from "lucide-react";

const navigation = [
  { label: "Overview", icon: LayoutDashboard },
  { label: "Widget Library", icon: Grid2X2 },
  { label: "Active Widgets", icon: AppWindow },
  { label: "Settings", icon: Settings2 },
];

export default function Sidebar() {
  return (
    <aside className="flex h-full w-64 flex-col border-r border-white/[0.07] bg-[#0d0d10] p-5">
      <div className="mb-12 flex items-center gap-3">
        <div className="flex h-10 w-10 items-center justify-center rounded-xl bg-violet-500 text-lg font-bold text-white">
          XE
        </div>

        <div>
          <h1 className="text-sm font-semibold text-white">XE Widgets</h1>
          <p className="text-xs text-zinc-500">Desktop environment</p>
        </div>
      </div>

      <p className="mb-4 px-3 text-[10px] font-semibold uppercase tracking-[0.2em] text-zinc-600">
        Workspace
      </p>

      <nav className="space-y-2">
        {navigation.map(({ label, icon: Icon }, index) => (
          <button
            key={label}
            className={`flex w-full items-center gap-3 rounded-xl px-3 py-3 text-sm transition-colors ${
              index === 0
                ? "bg-violet-500/10 text-violet-300"
                : "text-zinc-400 hover:bg-white/[0.04] hover:text-white"
            }`}
          >
            <Icon size={18} />
            {label}
          </button>
        ))}
      </nav>

      <div className="mt-auto rounded-xl border border-white/[0.07] bg-white/[0.02] p-4">
        <p className="text-xs font-medium text-white">XE Widgets</p>
        <p className="mt-1 text-[11px] text-zinc-500">
          Version 0.1.0 · Development
        </p>
      </div>
    </aside>
  );
}
