import React from "react";
import { Music2, Play, Pause, SkipBack, SkipForward, Volume2, VolumeX } from "lucide-react";
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

export default function MusicWidget() {
  const {
    currentTrack,
    isPlaying,
    togglePlay,
    trackProgress,
    setTrackProgress,
    volume,
    setVolume,
    nextTrack,
    prevTrack,
  } = useWidgets();

  const formatSeconds = (sec: number) => {
    const mins = Math.floor(sec / 60);
    const remaining = Math.floor(sec % 60);
    return `${mins}:${String(remaining).padStart(2, "0")}`;
  };

  const progressPercent = Math.min(100, (trackProgress / currentTrack.duration) * 100);

  const controlBtn = (onClick: () => void, children: React.ReactNode): React.ReactNode => (
    <button
      type="button"
      onClick={onClick}
      style={{
        display: "flex",
        alignItems: "center",
        justifyContent: "center",
        width: 28,
        height: 28,
        background: "transparent",
        border: "1px solid var(--xe-border)",
        borderRadius: 3,
        cursor: "pointer",
        color: "var(--xe-muted)",
        transition: "all 150ms",
      }}
      onMouseEnter={(e) => {
        (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-fg)";
        (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-fg)";
      }}
      onMouseLeave={(e) => {
        (e.currentTarget as HTMLButtonElement).style.borderColor = "var(--xe-border)";
        (e.currentTarget as HTMLButtonElement).style.color = "var(--xe-muted)";
      }}
    >
      {children}
    </button>
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
          flexShrink: 0,
        }}
      >
        <div style={{ display: "flex", alignItems: "center", gap: 6 }}>
          <Music2 size={13} style={{ color: "var(--xe-muted)" }} />
          <span
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
              letterSpacing: "0.08em",
              textTransform: "uppercase",
            }}
          >
            MEDIA PLAYER
          </span>
        </div>
        {/* Simple playing indicator: 3 static bars */}
        <div
          style={{
            display: "flex",
            alignItems: "flex-end",
            gap: 2,
            height: 14,
            opacity: isPlaying ? 1 : 0.3,
          }}
        >
          {[6, 10, 7, 12, 8].map((h, i) => (
            <div
              key={i}
              style={{
                width: 2,
                height: h,
                background: "var(--xe-muted)",
                borderRadius: 1,
              }}
            />
          ))}
        </div>
      </div>

      {/* Body */}
      <div style={{ flex: 1, padding: "10px 12px", overflow: "hidden", display: "flex", flexDirection: "column", justifyContent: "space-between" }}>
        {/* Track info */}
        <div>
          <div
            style={{
              fontFamily: "var(--xe-font-body)",
              fontSize: 12,
              fontWeight: 500,
              color: "var(--xe-fg)",
              overflow: "hidden",
              textOverflow: "ellipsis",
              whiteSpace: "nowrap",
            }}
          >
            {currentTrack.title}
          </div>
          <div
            style={{
              fontFamily: "var(--xe-font-body)",
              fontSize: 11,
              color: "var(--xe-muted)",
              marginTop: 2,
              overflow: "hidden",
              textOverflow: "ellipsis",
              whiteSpace: "nowrap",
            }}
          >
            {currentTrack.artist}
          </div>
          <div
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
              marginTop: 2,
              letterSpacing: "0.04em",
            }}
          >
            {currentTrack.album}
          </div>
        </div>

        {/* Progress bar */}
        <div style={{ marginTop: 10 }}>
          <div
            style={{
              height: 3,
              background: "var(--xe-border)",
              borderRadius: 2,
              cursor: "pointer",
              overflow: "hidden",
            }}
            onClick={(e) => {
              const rect = e.currentTarget.getBoundingClientRect();
              const ratio = Math.max(0, Math.min(1, (e.clientX - rect.left) / rect.width));
              setTrackProgress(Math.floor(ratio * currentTrack.duration));
            }}
          >
            <div
              style={{
                height: "100%",
                width: `${progressPercent}%`,
                background: "var(--xe-fg)",
                borderRadius: 2,
                transition: "width 300ms linear",
              }}
            />
          </div>
          <div
            className="flex justify-between tabnum"
            style={{
              marginTop: 4,
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
            }}
          >
            <span>{formatSeconds(trackProgress)}</span>
            <span>{formatSeconds(currentTrack.duration)}</span>
          </div>
        </div>

        {/* Controls */}
        <div
          style={{
            display: "flex",
            alignItems: "center",
            justifyContent: "space-between",
            marginTop: 8,
          }}
        >
          {/* Volume */}
          <div style={{ display: "flex", alignItems: "center", gap: 6 }}>
            <button
              type="button"
              onClick={() => setVolume(volume > 0 ? 0 : 75)}
              style={{ color: "var(--xe-faint)", cursor: "pointer" }}
            >
              {volume === 0 ? <VolumeX size={13} /> : <Volume2 size={13} />}
            </button>
            <input
              type="range"
              min="0"
              max="100"
              value={volume}
              onChange={(e) => setVolume(Number(e.target.value))}
              style={{ width: 48, height: 3, cursor: "pointer", accentColor: "var(--xe-fg)" }}
            />
          </div>
          {/* Transport */}
          <div style={{ display: "flex", alignItems: "center", gap: 4 }}>
            {controlBtn(prevTrack, <SkipBack size={12} />)}
            <button
              type="button"
              onClick={togglePlay}
              aria-label={isPlaying ? "Pause" : "Play"}
              style={{
                display: "flex",
                alignItems: "center",
                justifyContent: "center",
                width: 32,
                height: 32,
                background: "var(--xe-fg)",
                color: "var(--xe-bg)",
                border: "none",
                borderRadius: 3,
                cursor: "pointer",
              }}
            >
              {isPlaying ? <Pause size={13} /> : <Play size={13} />}
            </button>
            {controlBtn(nextTrack, <SkipForward size={12} />)}
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
          flexShrink: 0,
        }}
      >
        <span style={{ fontFamily: "var(--xe-font-mono)", fontSize: 9, color: "var(--xe-faint)" }}>
          DIRECTSOUND
        </span>
        <span style={{ fontFamily: "var(--xe-font-mono)", fontSize: 9, color: "var(--xe-faint)" }}>
          24-BIT
        </span>
      </div>
    </div>
  );
}
