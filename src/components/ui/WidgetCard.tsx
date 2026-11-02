
import type { LucideIcon } from "lucide-react";
import { ArrowUpRight } from "lucide-react";
import { motion } from "motion/react";

interface WidgetCardProps {
  name: string;
  description: string;
  icon: LucideIcon;
  category: string;
  status?: "available" | "active";
  onClick?: () => void;
}

export default function WidgetCard({
  name,
  description,
  icon: Icon,
  category,
  status = "available",
  onClick,
}: WidgetCardProps) {
  return (
    <motion.button
      type="button"
      onClick={onClick}
      whileHover={{ y: -4 }}
      whileTap={{ scale: 0.98 }}
      transition={{ duration: 0.2 }}
      className="group relative flex min-h-48 flex-col rounded-2xl border border-white/[0.07] bg-[#141418] p-5 text-left transition-colors hover:border-violet-400/30 hover:bg-[#19191f]"
    >
      <div className="flex items-start justify-between">
        <div className="flex h-11 w-11 items-center justify-center rounded-xl border border-violet-400/10 bg-violet-500/10 text-violet-300">
          <Icon size={21} />
        </div>

        <ArrowUpRight
          size={17}
          className="text-zinc-600 transition group-hover:text-violet-300"
        />
      </div>

      <div className="mt-auto pt-7">
        <div className="mb-2 flex items-center gap-2">
          <h3 className="text-sm font-semibold text-zinc-100">
            {name}
          </h3>

          <span
            className={`h-1.5 w-1.5 rounded-full ${
              status === "active" ? "bg-emerald-400" : "bg-zinc-600"
            }`}
          />
        </div>

        <p className="text-xs leading-relaxed text-zinc-500">
          {description}
        </p>

        <div className="mt-4">
          <span className="rounded-md bg-white/[0.04] px-2 py-1 text-[10px] text-zinc-500">
            {category}
          </span>
        </div>
      </div>
    </motion.button>
  );
}
