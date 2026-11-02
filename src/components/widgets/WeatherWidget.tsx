import React from "react";
import {
  CloudSun,
  Sun,
  CloudRain,
  Cloud,
  Wind,
  Droplets,
  Thermometer,
} from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";

export default function WeatherWidget() {
  const { weather, settings, updateSettings } = useWidgets();

  const isCelsius = settings.tempUnit === "C";
  const displayTemp = isCelsius
    ? `${weather.tempC}°C`
    : `${Math.round((weather.tempC * 9) / 5 + 32)}°F`;

  const getConditionIcon = (code: string) => {
    switch (code) {
      case "sunny":
        return <Sun size={24} className="text-amber-400" />;
      case "rainy":
        return <CloudRain size={24} className="text-cyan-400" />;
      case "cloudy":
      default:
        return <CloudSun size={24} className="text-sky-300" />;
    }
  };

  return (
    <div className="glass-card flex h-full flex-col justify-between overflow-hidden rounded-2xl p-5">
      {/* Header */}
      <div className="flex items-center justify-between border-b border-white/[0.06] pb-3">
        <div className="flex items-center gap-2">
          <div className="flex h-7 w-7 items-center justify-center rounded-lg bg-cyan-500/10 text-cyan-400 border border-cyan-500/20">
            <CloudSun size={15} />
          </div>
          <span className="text-xs font-semibold text-white">Atmosphere & Weather</span>
        </div>

        {/* C/F Unit Toggle */}
        <button
          type="button"
          onClick={() =>
            updateSettings({ tempUnit: isCelsius ? "F" : "C" })
          }
          className="pressable-btn rounded-md border border-white/[0.08] bg-white/[0.04] px-1.5 py-0.5 text-[10px] font-mono text-zinc-300 hover:text-white"
        >
          {isCelsius ? "°C / °F" : "°F / °C"}
        </button>
      </div>

      {/* Main Condition Display */}
      <div className="my-auto py-2">
        <div className="flex items-center justify-between">
          <div>
            <div className="text-3xl font-bold tracking-tight text-white">
              {displayTemp}
            </div>
            <div className="text-xs font-medium text-cyan-300">
              {weather.condition}
            </div>
            <div className="mt-0.5 text-[11px] text-zinc-400">
              {weather.city}, {weather.country}
            </div>
          </div>

          <div className="flex h-14 w-14 items-center justify-center rounded-2xl bg-white/[0.03] border border-white/[0.06]">
            {getConditionIcon(weather.conditionCode)}
          </div>
        </div>

        {/* Environmental Telemetry */}
        <div className="mt-4 grid grid-cols-3 gap-2 border-t border-white/[0.06] pt-3 text-center">
          <div className="rounded-lg bg-white/[0.02] p-1.5">
            <div className="flex items-center justify-center gap-1 text-[10px] text-zinc-400">
              <Droplets size={11} className="text-cyan-400" /> Hum
            </div>
            <div className="mt-0.5 text-xs font-semibold text-zinc-200">
              {weather.humidity}%
            </div>
          </div>

          <div className="rounded-lg bg-white/[0.02] p-1.5">
            <div className="flex items-center justify-center gap-1 text-[10px] text-zinc-400">
              <Wind size={11} className="text-teal-400" /> Wind
            </div>
            <div className="mt-0.5 text-xs font-semibold text-zinc-200">
              {weather.windKmH} km/h
            </div>
          </div>

          <div className="rounded-lg bg-white/[0.02] p-1.5">
            <div className="flex items-center justify-center gap-1 text-[10px] text-zinc-400">
              <Thermometer size={11} className="text-amber-400" /> UV
            </div>
            <div className="mt-0.5 text-xs font-semibold text-zinc-200">
              {weather.uvIndex}
            </div>
          </div>
        </div>

        {/* 4-Day Forecast mini bar */}
        <div className="mt-3 flex items-center justify-between rounded-xl bg-white/[0.02] p-2 text-center">
          {weather.forecast.map((fc) => (
            <div key={fc.day} className="text-[10px]">
              <div className="text-zinc-500 font-medium">{fc.day}</div>
              <div className="my-0.5 flex justify-center">
                {fc.conditionCode === "sunny" ? (
                  <Sun size={12} className="text-amber-400" />
                ) : fc.conditionCode === "rainy" ? (
                  <CloudRain size={12} className="text-cyan-400" />
                ) : (
                  <Cloud size={12} className="text-zinc-400" />
                )}
              </div>
              <div className="font-mono text-zinc-300">
                {isCelsius
                  ? `${fc.tempC}°`
                  : `${Math.round((fc.tempC * 9) / 5 + 32)}°`}
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* Footer */}
      <div className="flex items-center justify-between border-t border-white/[0.06] pt-2 text-[10px] text-zinc-500">
        <span>Station: Meteorological Air</span>
        <span>High: {weather.highC}° · Low: {weather.lowC}°</span>
      </div>
    </div>
  );
}
