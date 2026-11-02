import React from "react";
import {
  Music2,
  Play,
  Pause,
  SkipBack,
  SkipForward,
  Volume2,
  VolumeX,
  Heart,
} from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";

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

  const [isLiked, setIsLiked] = React.useState(false);

  const formatSeconds = (sec: number) => {
    const mins = Math.floor(sec / 60);
    const remaining = Math.floor(sec % 60);
    return `${mins}:${String(remaining).padStart(2, "0")}`;
  };

  const progressPercent = Math.min(
    100,
    (trackProgress / currentTrack.duration) * 100
  );

  return (
    <div className="glass-card flex h-full flex-col justify-between overflow-hidden rounded-2xl p-5">
      {/* Header */}
      <div className="flex items-center justify-between border-b border-white/[0.06] pb-3">
        <div className="flex items-center gap-2">
          <div className="flex h-7 w-7 items-center justify-center rounded-lg bg-emerald-500/10 text-emerald-400 border border-emerald-500/20">
            <Music2 size={15} />
          </div>
          <span className="text-xs font-semibold text-white">Audio & Media</span>
        </div>

        {/* Animated Equalizer Visualizer */}
        <div className="flex h-4 items-end gap-0.5 px-2">
          <div
            className={`w-0.5 rounded-full bg-emerald-400 ${
              isPlaying ? "animate-eq-1" : "h-1"
            }`}
          />
          <div
            className={`w-0.5 rounded-full bg-emerald-400 ${
              isPlaying ? "animate-eq-2" : "h-2"
            }`}
          />
          <div
            className={`w-0.5 rounded-full bg-emerald-400 ${
              isPlaying ? "animate-eq-3" : "h-1.5"
            }`}
          />
          <div
            className={`w-0.5 rounded-full bg-emerald-400 ${
              isPlaying ? "animate-eq-4" : "h-2.5"
            }`}
          />
          <div
            className={`w-0.5 rounded-full bg-emerald-400 ${
              isPlaying ? "animate-eq-5" : "h-1"
            }`}
          />
        </div>
      </div>

      {/* Track Display */}
      <div className="my-auto py-2">
        <div className="flex items-center gap-3">
          <img
            src={currentTrack.coverUrl}
            alt={currentTrack.album}
            className="h-14 w-14 rounded-xl object-cover shadow-md shadow-black/50 ring-1 ring-white/10"
          />

          <div className="min-w-0 flex-1">
            <div className="flex items-center justify-between">
              <h4 className="truncate text-xs font-semibold text-white">
                {currentTrack.title}
              </h4>
              <button
                type="button"
                onClick={() => setIsLiked((prev) => !prev)}
                className={`p-1 transition ${
                  isLiked ? "text-rose-400" : "text-zinc-500 hover:text-white"
                }`}
              >
                <Heart size={13} fill={isLiked ? "currentColor" : "none"} />
              </button>
            </div>
            <p className="truncate text-[11px] text-zinc-400">
              {currentTrack.artist}
            </p>
            <p className="truncate text-[10px] text-zinc-500">
              {currentTrack.album}
            </p>
          </div>
        </div>

        {/* Scrubber Progress Bar */}
        <div className="mt-3">
          <div
            className="group relative h-1.5 w-full cursor-pointer rounded-full bg-white/[0.08]"
            onClick={(e) => {
              const rect = e.currentTarget.getBoundingClientRect();
              const clickX = e.clientX - rect.left;
              const ratio = Math.max(0, Math.min(1, clickX / rect.width));
              setTrackProgress(Math.floor(ratio * currentTrack.duration));
            }}
          >
            <div
              className="h-full rounded-full bg-gradient-to-r from-emerald-500 to-teal-400 transition-all"
              style={{ width: `${progressPercent}%` }}
            />
          </div>
          <div className="mt-1 flex justify-between text-[10px] font-mono text-zinc-500">
            <span>{formatSeconds(trackProgress)}</span>
            <span>{formatSeconds(currentTrack.duration)}</span>
          </div>
        </div>

        {/* Playback Controls */}
        <div className="mt-2 flex items-center justify-between">
          {/* Volume */}
          <div className="flex items-center gap-1.5 text-zinc-400">
            <button
              type="button"
              onClick={() => setVolume(volume > 0 ? 0 : 75)}
              className="hover:text-white"
            >
              {volume === 0 ? <VolumeX size={13} /> : <Volume2 size={13} />}
            </button>
            <input
              type="range"
              min="0"
              max="100"
              value={volume}
              onChange={(e) => setVolume(Number(e.target.value))}
              className="h-1 w-14 cursor-pointer accent-emerald-400"
            />
          </div>

          {/* Transport buttons */}
          <div className="flex items-center gap-2">
            <button
              type="button"
              onClick={prevTrack}
              aria-label="Previous track"
              className="pressable-btn flex h-7 w-7 items-center justify-center rounded-lg text-zinc-400 transition hover:bg-white/[0.08] hover:text-white"
            >
              <SkipBack size={14} />
            </button>

            <button
              type="button"
              onClick={togglePlay}
              aria-label={isPlaying ? "Pause" : "Play"}
              className="pressable-btn flex h-9 w-9 items-center justify-center rounded-xl bg-emerald-500 text-black shadow-md shadow-emerald-500/20 transition hover:bg-emerald-400"
            >
              {isPlaying ? <Pause size={15} /> : <Play size={15} className="ml-0.5" />}
            </button>

            <button
              type="button"
              onClick={nextTrack}
              aria-label="Next track"
              className="pressable-btn flex h-7 w-7 items-center justify-center rounded-lg text-zinc-400 transition hover:bg-white/[0.08] hover:text-white"
            >
              <SkipForward size={14} />
            </button>
          </div>
        </div>
      </div>

      {/* Footer */}
      <div className="flex items-center justify-between border-t border-white/[0.06] pt-2 text-[10px] text-zinc-500">
        <span>Output: Hi-Fi DirectSound</span>
        <span>Lossless 24-bit</span>
      </div>
    </div>
  );
}
