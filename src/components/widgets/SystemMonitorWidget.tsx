import { Activity, Cpu, HardDrive, Wifi, ArrowDown, ArrowUp, Zap } from "lucide-react";
import { motion } from "motion/react";
import { useWidgets } from "../../context/WidgetContext";

const widgetContainer: React.CSSProperties = {
  height: "100%",
  display: "flex",
  flexDirection: "column",
  border: "1px solid var(--xe-border)",
  borderRadius: 4,
  background: "var(--xe-surface)",
  overflow: "hidden",
};

export default function SystemMonitorWidget() {
  const { metrics } = useWidgets();

  const box: React.CSSProperties = {
    border: "1px solid var(--xe-border)",
    borderRadius: 3,
    padding: "8px 10px",
    background: "var(--xe-bg)",
  };

  const metaLabel: React.CSSProperties = {
    fontFamily: "var(--xe-font-mono)",
    fontSize: 9,
    color: "var(--xe-faint)",
    letterSpacing: "0.06em",
    textTransform: "uppercase",
  };

  const valueText: React.CSSProperties = {
    fontFamily: "var(--xe-font-mono)",
    fontSize: 13,
    fontWeight: 500,
    color: "var(--xe-fg)",
  };

  const bar = (pct: number) => (
    <div
      style={{
        height: 3,
        width: "100%",
        background: "var(--xe-border)",
        borderRadius: 2,
        marginTop: 6,
        overflow: "hidden",
      }}
    >
      <motion.div
        style={{ height: "100%", background: "var(--xe-fg)", borderRadius: 2 }}
        animate={{ width: `${pct}%` }}
        transition={{ ease: "easeOut", duration: 0.5 }}
      />
    </div>
  );

  return (
    <div style={widgetContainer}>
      {/* Header */}
      <div
        style={{
          display: "flex",
          alignItems: "center",
          justifyContent: "space-between",
          padding: "8px 12px",
          borderBottom: "1px solid var(--xe-border)",
        }}
      >
        <div style={{ display: "flex", alignItems: "center", gap: 6 }}>
          <Activity size={13} style={{ color: "var(--xe-muted)" }} />
          <span
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
              letterSpacing: "0.08em",
              textTransform: "uppercase",
            }}
          >
            SYSTEM TELEMETRY
          </span>
        </div>
        <span
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 9,
            color: "var(--xe-faint)",
            letterSpacing: "0.04em",
          }}
        >
          LIVE
        </span>
      </div>

      {/* Body */}
      <div
        style={{
          flex: 1,
          padding: "10px",
          display: "grid",
          gridTemplateColumns: "1fr 1fr",
          gap: 6,
          overflow: "hidden",
        }}
      >
        {/* CPU */}
        <div style={box}>
          <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
            <span style={{ ...metaLabel, display: "flex", alignItems: "center", gap: 4 }}>
              <Cpu size={10} style={{ color: "var(--xe-muted)" }} />
              CPU
            </span>
            <span className="tabnum" style={valueText}>
              {metrics.cpuUsage}%
            </span>
          </div>
          {/* Sparkline */}
          <div
            style={{
              marginTop: 8,
              display: "flex",
              alignItems: "flex-end",
              height: 20,
              gap: 2,
            }}
          >
            {metrics.cpuHistory.map((val, idx) => (
              <motion.div
                key={idx}
                style={{
                  flex: 1,
                  background: "var(--xe-muted)",
                  borderRadius: 1,
                  minHeight: 2,
                }}
                animate={{ height: `${Math.max(8, val)}%` }}
                transition={{ ease: "easeOut", duration: 0.3 }}
              />
            ))}
          </div>
        </div>

        {/* RAM */}
        <div style={box}>
          <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
            <span style={{ ...metaLabel, display: "flex", alignItems: "center", gap: 4 }}>
              <Zap size={10} style={{ color: "var(--xe-muted)" }} />
              RAM
            </span>
            <span className="tabnum" style={valueText}>
              {Math.round((metrics.ramUsedGB / metrics.ramTotalGB) * 100)}%
            </span>
          </div>
          <div
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
              marginTop: 2,
            }}
          >
            {metrics.ramUsedGB} / {metrics.ramTotalGB} GB
          </div>
          {bar((metrics.ramUsedGB / metrics.ramTotalGB) * 100)}
        </div>

        {/* Disk */}
        <div style={box}>
          <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
            <span style={{ ...metaLabel, display: "flex", alignItems: "center", gap: 4 }}>
              <HardDrive size={10} style={{ color: "var(--xe-muted)" }} />
              DISK
            </span>
            <span className="tabnum" style={valueText}>
              {metrics.diskUsedPercent}%
            </span>
          </div>
          {bar(metrics.diskUsedPercent)}
        </div>

        {/* Network */}
        <div style={box}>
          <span style={{ ...metaLabel, display: "flex", alignItems: "center", gap: 4 }}>
            <Wifi size={10} style={{ color: "var(--xe-muted)" }} />
            NETWORK
          </span>
          <div style={{ marginTop: 6, display: "flex", flexDirection: "column", gap: 3 }}>
            <div
              className="tabnum"
              style={{
                display: "flex",
                alignItems: "center",
                gap: 4,
                fontFamily: "var(--xe-font-mono)",
                fontSize: 10,
                color: "var(--xe-fg)",
              }}
            >
              <ArrowDown size={10} style={{ color: "var(--xe-muted)" }} />
              {metrics.networkDownMbps} Mb/s
            </div>
            <div
              className="tabnum"
              style={{
                display: "flex",
                alignItems: "center",
                gap: 4,
                fontFamily: "var(--xe-font-mono)",
                fontSize: 10,
                color: "var(--xe-muted)",
              }}
            >
              <ArrowUp size={10} style={{ color: "var(--xe-faint)" }} />
              {metrics.networkUpMbps} Mb/s
            </div>
          </div>
        </div>
      </div>

      {/* Footer */}
      <div
        style={{
          display: "flex",
          justifyContent: "space-between",
          padding: "5px 12px",
          borderTop: "1px solid var(--xe-border)",
        }}
      >
        <span
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 9,
            color: "var(--xe-faint)",
          }}
        >
          GPU {metrics.gpuUsage}%
        </span>
        <span
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 9,
            color: "var(--xe-faint)",
          }}
        >
          UP {metrics.uptimeHours}h
        </span>
      </div>
    </div>
  );
}
