import { Check, ShieldCheck, HardDrive, Cpu } from "lucide-react";
import Header from "../components/layout/Header";
import { useWidgets } from "../context/WidgetContext";

function ToggleSwitch({
  checked,
  onChange,
}: {
  checked: boolean;
  onChange: (v: boolean) => void;
}) {
  return (
    <button
      type="button"
      role="switch"
      aria-checked={checked}
      onClick={() => onChange(!checked)}
      className="xe-transition"
      style={{
        width: 36,
        height: 20,
        borderRadius: 10,
        background: checked ? "var(--xe-fg)" : "var(--xe-border)",
        border: "none",
        cursor: "pointer",
        position: "relative",
        flexShrink: 0,
        transition: "background 200ms var(--xe-ease)",
      }}
    >
      <span
        style={{
          position: "absolute",
          top: 3,
          left: checked ? "calc(100% - 17px)" : 3,
          width: 14,
          height: 14,
          borderRadius: "50%",
          background: "var(--xe-bg)",
          transition: "left 200ms var(--xe-ease)",
        }}
      />
    </button>
  );
}

function SettingRow({
  label,
  description,
  children,
}: {
  label: string;
  description?: string;
  children: React.ReactNode;
}) {
  return (
    <div
      className="flex items-center justify-between xe-transition"
      style={{
        padding: "13px 18px",
        borderBottom: "1px solid var(--xe-border)",
      }}
    >
      <div style={{ paddingRight: 16 }}>
        <div
          style={{
            fontFamily: "var(--xe-font-body)",
            fontSize: 13,
            color: "var(--xe-fg)",
            fontWeight: 400,
          }}
        >
          {label}
        </div>
        {description && (
          <div
            style={{
              fontFamily: "var(--xe-font-body)",
              fontSize: 11,
              color: "var(--xe-muted)",
              marginTop: 2,
              lineHeight: 1.45,
            }}
          >
            {description}
          </div>
        )}
      </div>
      {children}
    </div>
  );
}

function SectionLabel({ num, title }: { num: string; title: string }) {
  return (
    <div style={{ marginBottom: 12 }}>
      <div className="xe-section-label">
        {num} — {title}
      </div>
    </div>
  );
}

function SegmentControl({
  options,
  value,
  onChange,
}: {
  options: { label: string; value: string }[];
  value: string;
  onChange: (v: string) => void;
}) {
  return (
    <div
      className="flex"
      style={{
        border: "1px solid var(--xe-border)",
        borderRadius: 4,
        overflow: "hidden",
        flexShrink: 0,
      }}
    >
      {options.map((opt, i) => (
        <button
          key={opt.value}
          type="button"
          onClick={() => onChange(opt.value)}
          className="xe-transition"
          style={{
            padding: "5px 14px",
            fontSize: 11,
            fontFamily: "var(--xe-font-mono)",
            letterSpacing: "0.04em",
            cursor: "pointer",
            background: value === opt.value ? "var(--xe-fg)" : "transparent",
            color: value === opt.value ? "var(--xe-bg)" : "var(--xe-muted)",
            border: "none",
            borderLeft: i > 0 ? "1px solid var(--xe-border)" : "none",
          }}
        >
          {opt.label}
        </button>
      ))}
    </div>
  );
}

export default function SettingsView() {
  const { settings, updateSettings, addNotification } = useWidgets();

  const handleThemeChange = (theme: "light" | "dark") => {
    updateSettings({ theme });
    addNotification("Theme Changed", `${theme === "light" ? "Light" : "Dark"} theme applied.`);
  };

  const sectionCard = (content: React.ReactNode): React.ReactNode => (
    <div
      style={{
        border: "1px solid var(--xe-border)",
        borderRadius: 4,
        overflow: "hidden",
        background: "var(--xe-surface)",
      }}
    >
      {content}
    </div>
  );

  const THEMES: { id: "light" | "dark"; label: string; description: string; bg: string; fg: string }[] = [
    {
      id: "light",
      label: "Light",
      description: "Warm off-white. Editorial and spacious.",
      bg: "#EBE9E4",
      fg: "#121212",
    },
    {
      id: "dark",
      label: "Dark",
      description: "Deep near-black. Focused and precise.",
      bg: "#111110",
      fg: "#F0EEE9",
    },
  ];

  return (
    <div className="flex h-full flex-col overflow-hidden">
      <Header
        title="Settings"
        subtitle="Configure appearance, behaviour, and preferences."
      />

      <div
        className="flex-1 overflow-y-auto"
        style={{ padding: "32px 40px" }}
      >
        {/* Two-Column Responsive Grid Layout filling the entire desktop workspace */}
        <div
          style={{
            display: "grid",
            gridTemplateColumns: "repeat(auto-fit, minmax(440px, 1fr))",
            gap: "28px",
            alignItems: "start",
          }}
        >
          {/* ── LEFT COLUMN ───────────────────────────────── */}
          <div style={{ display: "flex", flexDirection: "column", gap: "28px" }}>
            
            {/* 01 APPEARANCE */}
            <div>
              <SectionLabel num="01" title="Appearance" />
              {sectionCard(
                <>
                  <div
                    style={{
                      padding: "16px",
                      display: "grid",
                      gridTemplateColumns: "1fr 1fr",
                      gap: 10,
                    }}
                  >
                    {THEMES.map((th) => {
                      const isActive = settings.theme === th.id;
                      return (
                        <button
                          key={th.id}
                          type="button"
                          onClick={() => handleThemeChange(th.id)}
                          className="xe-transition"
                          style={{
                            padding: "16px",
                            border: isActive ? "2px solid var(--xe-fg)" : "1px solid var(--xe-border)",
                            borderRadius: 4,
                            background: isActive ? "var(--xe-bg)" : "transparent",
                            cursor: "pointer",
                            textAlign: "left",
                          }}
                        >
                          <div
                            className="flex items-center justify-between"
                            style={{ marginBottom: 12 }}
                          >
                            {/* Colour swatch */}
                            <div
                              style={{
                                width: 32,
                                height: 22,
                                borderRadius: 3,
                                background: th.bg,
                                border: "1px solid var(--xe-border)",
                                position: "relative",
                                overflow: "hidden",
                              }}
                            >
                              <div
                                style={{
                                  position: "absolute",
                                  bottom: 4,
                                  left: 4,
                                  right: 4,
                                  height: 2,
                                  background: th.fg,
                                  borderRadius: 1,
                                  opacity: 0.6,
                                }}
                              />
                            </div>
                            {isActive && (
                              <Check
                                size={14}
                                style={{ color: "var(--xe-fg)", flexShrink: 0 }}
                              />
                            )}
                          </div>
                          <div
                            style={{
                              fontFamily: "var(--xe-font-body)",
                              fontSize: 13,
                              fontWeight: 500,
                              color: "var(--xe-fg)",
                              marginBottom: 4,
                            }}
                          >
                            {th.label}
                          </div>
                          <div
                            style={{
                              fontFamily: "var(--xe-font-body)",
                              fontSize: 11,
                              color: "var(--xe-muted)",
                              lineHeight: 1.45,
                            }}
                          >
                            {th.description}
                          </div>
                        </button>
                      );
                    })}
                  </div>

                  <div
                    style={{
                      padding: "10px 16px",
                      borderTop: "1px solid var(--xe-border)",
                      background: "var(--xe-bg)",
                      display: "flex",
                      alignItems: "center",
                      justifyContent: "space-between",
                    }}
                  >
                    <span
                      style={{
                        fontFamily: "var(--xe-font-mono)",
                        fontSize: 10,
                        color: "var(--xe-faint)",
                        letterSpacing: "0.04em",
                      }}
                    >
                      COLOUR SYSTEM
                    </span>
                    <span
                      style={{
                        fontFamily: "var(--xe-font-mono)",
                        fontSize: 10,
                        color: "var(--xe-muted)",
                      }}
                    >
                      Monochromatic Editorial
                    </span>
                  </div>
                </>
              )}
            </div>

            {/* 02 TYPOGRAPHY */}
            <div>
              <SectionLabel num="02" title="Typography" />
              {sectionCard(
                <>
                  {[
                    {
                      role: "DISPLAY",
                      family: "var(--xe-font-display)",
                      sample: "Geist Pixel",
                      desc: "Headings, display titles, hero typography",
                    },
                    {
                      role: "BODY",
                      family: "var(--xe-font-body)",
                      sample: "Geist Sans",
                      desc: "Navigation, descriptions, general interface",
                    },
                    {
                      role: "MONO",
                      family: "var(--xe-font-mono)",
                      sample: "Geist Mono",
                      desc: "Labels, version numbers, system telemetry",
                    },
                  ].map((font, i, arr) => (
                    <div
                      key={font.role}
                      style={{
                        padding: "16px 18px",
                        borderBottom: i < arr.length - 1 ? "1px solid var(--xe-border)" : "none",
                        display: "flex",
                        alignItems: "center",
                        justifyContent: "space-between",
                        gap: 16,
                      }}
                    >
                      <div>
                        <div className="xe-section-label" style={{ marginBottom: 2 }}>
                          {font.role}
                        </div>
                        <div
                          style={{
                            fontFamily: "var(--xe-font-body)",
                            fontSize: 11,
                            color: "var(--xe-muted)",
                            lineHeight: 1.4,
                          }}
                        >
                          {font.desc}
                        </div>
                      </div>
                      <div
                        style={{
                          fontFamily: font.family,
                          fontSize: 15,
                          color: "var(--xe-fg)",
                          letterSpacing: font.role === "MONO" ? "0.04em" : undefined,
                          flexShrink: 0,
                        }}
                      >
                        {font.sample}
                      </div>
                    </div>
                  ))}

                  <div
                    style={{
                      padding: "10px 18px",
                      borderTop: "1px solid var(--xe-border)",
                      background: "var(--xe-bg)",
                      display: "flex",
                      alignItems: "center",
                      justifyContent: "space-between",
                    }}
                  >
                    <span
                      style={{
                        fontFamily: "var(--xe-font-mono)",
                        fontSize: 10,
                        color: "var(--xe-faint)",
                        letterSpacing: "0.04em",
                      }}
                    >
                      ASSET STATUS
                    </span>
                    <span
                      style={{
                        fontFamily: "var(--xe-font-mono)",
                        fontSize: 10,
                        color: "var(--xe-muted)",
                      }}
                    >
                      Bundled Locally (WOFF2)
                    </span>
                  </div>
                </>
              )}
            </div>

          </div>

          {/* ── RIGHT COLUMN ──────────────────────────────── */}
          <div style={{ display: "flex", flexDirection: "column", gap: "28px" }}>
            
            {/* 03 WIDGET BEHAVIOUR */}
            <div>
              <SectionLabel num="03" title="Widget Behaviour" />
              {sectionCard(
                <>
                  <SettingRow
                    label="Always on Top"
                    description="Float desktop widgets above other foreground application windows."
                  >
                    <ToggleSwitch
                      checked={settings.alwaysOnTop}
                      onChange={(v) => updateSettings({ alwaysOnTop: v })}
                    />
                  </SettingRow>
                  <SettingRow
                    label="Launch on Startup"
                    description="Automatically run XE Widgets when your computer starts up."
                  >
                    <ToggleSwitch
                      checked={settings.startWithWindows}
                      onChange={(v) => updateSettings({ startWithWindows: v })}
                    />
                  </SettingRow>
                  <SettingRow
                    label="Minimize to Tray"
                    description="Keep running silently in the notification tray when closed."
                  >
                    <ToggleSwitch
                      checked={settings.minimizeToTray}
                      onChange={(v) => updateSettings({ minimizeToTray: v })}
                    />
                  </SettingRow>
                  <SettingRow
                    label="Clock Time Format"
                    description="Choose between 12-hour AM/PM and 24-hour military notation."
                  >
                    <SegmentControl
                      options={[
                        { label: "12H", value: "12h" },
                        { label: "24H", value: "24h" },
                      ]}
                      value={settings.timeFormat}
                      onChange={(v) => updateSettings({ timeFormat: v as "12h" | "24h" })}
                    />
                  </SettingRow>
                  <SettingRow
                    label="Temperature Unit"
                    description="Measurement unit for meteorological and atmosphere telemetry."
                  >
                    <SegmentControl
                      options={[
                        { label: "°C", value: "C" },
                        { label: "°F", value: "F" },
                      ]}
                      value={settings.tempUnit}
                      onChange={(v) => updateSettings({ tempUnit: v as "C" | "F" })}
                    />
                  </SettingRow>
                </>
              )}
            </div>

            {/* 04 APPLICATION & PLATFORM */}
            <div>
              <SectionLabel num="04" title="Application & Platform" />
              {sectionCard(
                <div style={{ padding: "18px" }}>
                  <div
                    className="flex items-center justify-between"
                    style={{ marginBottom: 12 }}
                  >
                    <div className="flex items-center gap-2">
                      <ShieldCheck size={16} style={{ color: "var(--xe-muted)" }} />
                      <span
                        style={{
                          fontFamily: "var(--xe-font-body)",
                          fontSize: 13,
                          fontWeight: 500,
                          color: "var(--xe-fg)",
                        }}
                      >
                        XE Desktop Widgets Platform
                      </span>
                    </div>
                    <span
                      style={{
                        fontFamily: "var(--xe-font-mono)",
                        fontSize: 11,
                        color: "var(--xe-muted)",
                        background: "var(--xe-bg)",
                        padding: "2px 8px",
                        borderRadius: 3,
                        border: "1px solid var(--xe-border)",
                      }}
                    >
                      v2.0.0 Stable
                    </span>
                  </div>

                  <p
                    style={{
                      fontFamily: "var(--xe-font-body)",
                      fontSize: 11,
                      color: "var(--xe-muted)",
                      lineHeight: 1.6,
                    }}
                  >
                    Engineered with Tauri 2, Rust, React 19, and Vite. Styled with Tailwind CSS
                    v4 and the Geist type family. Built with strict editorial minimalism principles,
                    subtle dividers, and zero decorative noise.
                  </p>

                  <div
                    style={{
                      marginTop: 16,
                      paddingTop: 14,
                      borderTop: "1px solid var(--xe-border)",
                      display: "grid",
                      gridTemplateColumns: "repeat(4, 1fr)",
                      gap: 12,
                    }}
                  >
                    {[
                      ["Runtime", "Tauri 2"],
                      ["Backend", "Rust"],
                      ["Frontend", "React 19"],
                      ["Styling", "Tailwind 4"],
                    ].map(([k, v]) => (
                      <div key={k}>
                        <div className="xe-section-label" style={{ marginBottom: 3 }}>
                          {k}
                        </div>
                        <div
                          style={{
                            fontFamily: "var(--xe-font-mono)",
                            fontSize: 11,
                            color: "var(--xe-fg)",
                          }}
                        >
                          {v}
                        </div>
                      </div>
                    ))}
                  </div>

                  <div
                    style={{
                      marginTop: 14,
                      paddingTop: 12,
                      borderTop: "1px solid var(--xe-border)",
                      display: "flex",
                      alignItems: "center",
                      justifyContent: "space-between",
                    }}
                  >
                    <span
                      style={{
                        fontFamily: "var(--xe-font-mono)",
                        fontSize: 10,
                        color: "var(--xe-faint)",
                        display: "flex",
                        alignItems: "center",
                        gap: 4,
                      }}
                    >
                      <HardDrive size={11} /> Storage: LocalStorage Sync
                    </span>
                    <span
                      style={{
                        fontFamily: "var(--xe-font-mono)",
                        fontSize: 10,
                        color: "var(--xe-faint)",
                        display: "flex",
                        alignItems: "center",
                        gap: 4,
                      }}
                    >
                      <Cpu size={11} /> Graphics: Native DirectDraw
                    </span>
                  </div>
                </div>
              )}
            </div>

          </div>
        </div>

      </div>
    </div>
  );
}
