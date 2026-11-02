
import { Search, Bell, Command } from "lucide-react";

interface HeaderProps {
  title: string;
  description: string;
}

export default function Header({ title, description }: HeaderProps) {
  return (
    <header className="flex h-[88px] items-center justify-between border-b border-white/[0.06] px-8">
      <div>
        <h1 className="text-xl font-semibold tracking-tight text-white">
          {title}
        </h1>

        <p className="mt-1 text-xs text-zinc-500">
          {description}
        </p>
      </div>

      <div className="flex items-center gap-3">
        <button
          aria-label="Search"
          className="flex h-10 items-center gap-3 rounded-xl border border-white/[0.07] bg-white/[0.02] px-3 text-zinc-500 transition hover:bg-white/[0.05]"
        >
          <Search size={16} />
          <span className="text-xs">Search widgets...</span>
          <span className="ml-5 flex items-center gap-1 rounded-md bg-white/[0.06] px-2 py-1 text-[10px]">
            <Command size={11} /> K
          </span>
        </button>

        <button
          aria-label="Notifications"
          className="relative flex h-10 w-10 items-center justify-center rounded-xl border border-white/[0.07] text-zinc-400 transition hover:bg-white/[0.05] hover:text-white"
        >
          <Bell size={17} />
          <span className="absolute right-2 top-2 h-1.5 w-1.5 rounded-full bg-violet-400" />
        </button>

        <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-violet-400/20 bg-violet-500/10 text-sm font-semibold text-violet-300">
          XE
        </div>
      </div>
    </header>
  );
}
