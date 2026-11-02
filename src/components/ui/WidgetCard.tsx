import type { LucideIcon } from "lucide-react";
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
      whileHover={{ y: -2 }}
      transition={{ duration: 0.15, ease: [0.16, 1, 0.3, 1] }}
      style={{
        display: "flex",
        flexDirection: "column",
        minHeight: 160,
        padding: 16,
        textAlign: "left",
        background: "var(--xe-bg)",
        border: "1px solid var(--xe-border)",
        borderRadius: 4,
        cursor: "pointer",
        transition: "border-color 150ms, background 150ms",
      }}
      onMouseEnter={(e) => {
        (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-fg)";
        (e.currentTarget as HTMLButtonElement).style.background = "var(--xe-surface)";
      }}
      onMouseLeave={(e) => {
        (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-border)";
        (e.currentTarget as HTMLButtonElement).style.background = "var(--xe-bg)";
      }}
    >
      {/* Top row */}
      <div style={{ display: "flex", alignItems: "center", justifyContent: "space-between", marginBottom: "auto" }}>
        <Icon size={18} style={{ color: "var(--xe-muted)" }} />
        <div
          style={{
            width: 6,
            height: 6,
            borderRadius: "50%",
            background: status === "active" ? "var(--xe-fg)" : "var(--xe-border)",
            flexShrink: 0,
          }}
        />
      </div>

      {/* Bottom info */}
      <div style={{ marginTop: 24 }}>
        <h3
          style={{
            fontFamily: "var(--xe-font-body)",
            fontSize: 13,
            fontWeight: 500,
            color: "var(--xe-fg)",
            marginBottom: 4,
          }}
        >
          {name}
        </h3>
        <p
          style={{
            fontFamily: "var(--xe-font-body)",
            fontSize: 11,
            color: "var(--xe-muted)",
            lineHeight: 1.5,
            marginBottom: 10,
          }}
        >
          {description}
        </p>
        <span
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 9,
            color: "var(--xe-faint)",
            letterSpacing: "0.06em",
            textTransform: "uppercase",
          }}
        >
          {category}
        </span>
      </div>
    </motion.button>
  );
}
