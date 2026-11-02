import { CloudSun, Sun, CloudRain, Wind, Droplets, Thermometer } from "lucide-react";
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

export default function WeatherWidget() {
  const { weather, settings, updateSettings } = useWidgets();
  const isCelsius = settings.tempUnit === "C";

  const displayTemp = isCelsius
    ? `${weather.tempC}°C`
    : `${Math.round((weather.tempC * 9) / 5 + 32)}°F`;

  const getConditionSymbol = (code: string) => {
    switch (code) {
      case "sunny":  return "○";
      case "rainy":  return "≋";
      default:       return "◑";
    }
  };

  const getConditionIcon = (code: string) => {
    switch (code) {
      case "sunny":  return <Sun size={18} style={{ color: "var(--xe-muted)" }} />;
      case "rainy":  return <CloudRain size={18} style={{ color: "var(--xe-muted)" }} />;
      default:       return <CloudSun size={18} style={{ color: "var(--xe-muted)" }} />;
    }
  };

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
          <CloudSun size={13} style={{ color: "var(--xe-muted)" }} />
          <span
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
              letterSpacing: "0.08em",
              textTransform: "uppercase",
            }}
          >
            ATMOSPHERE
          </span>
        </div>
        <button
          type="button"
          onClick={() => updateSettings({ tempUnit: isCelsius ? "F" : "C" })}
          style={{
            padding: "2px 6px",
            fontSize: 9,
            fontFamily: "var(--xe-font-mono)",
            letterSpacing: "0.06em",
            cursor: "pointer",
            background: "transparent",
            color: "var(--xe-muted)",
            border: "1px solid var(--xe-border)",
            borderRadius: 2,
          }}
        >
          {isCelsius ? "°C" : "°F"}
        </button>
      </div>

      {/* Body */}
      <div style={{ flex: 1, padding: "10px 12px", display: "flex", flexDirection: "column", justifyContent: "space-between", overflow: "hidden" }}>
        {/* Main temp display */}
        <div style={{ display: "flex", alignItems: "flex-start", justifyContent: "space-between" }}>
          <div>
            <div
              className="tabnum"
              style={{
                fontFamily: "var(--xe-font-mono)",
                fontSize: 32,
                fontWeight: 400,
                color: "var(--xe-fg)",
                lineHeight: 1,
                letterSpacing: "-0.02em",
              }}
            >
              {displayTemp}
            </div>
            <div
              style={{
                fontFamily: "var(--xe-font-body)",
                fontSize: 11,
                color: "var(--xe-muted)",
                marginTop: 4,
              }}
            >
              {weather.condition}
            </div>
            <div
              style={{
                fontFamily: "var(--xe-font-mono)",
                fontSize: 10,
                color: "var(--xe-faint)",
                marginTop: 2,
                letterSpacing: "0.02em",
              }}
            >
              {weather.city}, {weather.country}
            </div>
          </div>
          <div
            style={{
              width: 44,
              height: 44,
              display: "flex",
              alignItems: "center",
              justifyContent: "center",
              border: "1px solid var(--xe-border)",
              borderRadius: 4,
              background: "var(--xe-bg)",
            }}
          >
            {getConditionIcon(weather.conditionCode)}
          </div>
        </div>

        {/* Telemetry row */}
        <div
          style={{
            display: "grid",
            gridTemplateColumns: "1fr 1fr 1fr",
            gap: 1,
            border: "1px solid var(--xe-border)",
            borderRadius: 3,
            overflow: "hidden",
            background: "var(--xe-border)",
          }}
        >
          {[
            { Icon: Droplets, label: "HUM", value: `${weather.humidity}%` },
            { Icon: Wind, label: "WIND", value: `${weather.windKmH}km/h` },
            { Icon: Thermometer, label: "UV", value: String(weather.uvIndex) },
          ].map(({ Icon, label, value }) => (
            <div
              key={label}
              style={{
                padding: "6px 8px",
                background: "var(--xe-bg)",
                textAlign: "center",
              }}
            >
              <div
                style={{
                  fontFamily: "var(--xe-font-mono)",
                  fontSize: 9,
                  color: "var(--xe-faint)",
                  letterSpacing: "0.06em",
                  display: "flex",
                  alignItems: "center",
                  justifyContent: "center",
                  gap: 3,
                  marginBottom: 3,
                }}
              >
                <Icon size={9} />
                {label}
              </div>
              <div
                className="tabnum"
                style={{
                  fontFamily: "var(--xe-font-mono)",
                  fontSize: 11,
                  color: "var(--xe-fg)",
                }}
              >
                {value}
              </div>
            </div>
          ))}
        </div>

        {/* Forecast */}
        <div
          style={{
            display: "flex",
            justifyContent: "space-between",
            padding: "6px 0 0",
          }}
        >
          {weather.forecast.map((fc) => (
            <div key={fc.day} style={{ textAlign: "center" }}>
              <div
                style={{
                  fontFamily: "var(--xe-font-mono)",
                  fontSize: 9,
                  color: "var(--xe-faint)",
                  letterSpacing: "0.04em",
                }}
              >
                {fc.day}
              </div>
              <div
                style={{
                  fontFamily: "var(--xe-font-mono)",
                  fontSize: 12,
                  color: "var(--xe-muted)",
                  margin: "3px 0",
                }}
              >
                {getConditionSymbol(fc.conditionCode)}
              </div>
              <div
                className="tabnum"
                style={{
                  fontFamily: "var(--xe-font-mono)",
                  fontSize: 10,
                  color: "var(--xe-fg)",
                }}
              >
                {isCelsius ? `${fc.tempC}°` : `${Math.round((fc.tempC * 9) / 5 + 32)}°`}
              </div>
            </div>
          ))}
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
          METEOROLOGICAL
        </span>
        <span style={{ fontFamily: "var(--xe-font-mono)", fontSize: 9, color: "var(--xe-faint)" }}>
          H:{weather.highC}° L:{weather.lowC}°
        </span>
      </div>
    </div>
  );
}
