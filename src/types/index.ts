export type NavView = "overview" | "library" | "active" | "settings";

export type WidgetCategory = "All" | "Productivity" | "System" | "Media" | "Utility";

export type WidgetSize = "small" | "medium" | "large";

export interface DesktopWidgetConfig {
  id: string;
  enabled: boolean;
  visible: boolean;
  x?: number;
  y?: number;
  width: number;
  height: number;
  alwaysOnTop: boolean;
}

export interface WidgetMetadata {
  id: string;
  name: string;
  description: string;
  category: "Productivity" | "System" | "Media" | "Utility";
  iconName: string;
  version: string;
  author: string;
  tags: string[];
  defaultSize: WidgetSize;
  isActive: boolean;
  isPinned: boolean;
}

export interface SystemMetrics {
  cpuUsage: number;
  cpuHistory: number[];
  ramUsedGB: number;
  ramTotalGB: number;
  diskUsedPercent: number;
  gpuUsage: number;
  networkDownMbps: number;
  networkUpMbps: number;
  batteryPercent: number;
  isCharging: boolean;
  uptimeHours: number;
}

export interface NoteItem {
  id: string;
  title: string;
  content: string;
  color: "amber" | "emerald" | "violet" | "sky" | "rose";
  isPinned: boolean;
  completed?: boolean;
  updatedAt: string;
}

export interface AudioTrack {
  title: string;
  artist: string;
  album: string;
  duration: number; // in seconds
  coverUrl: string;
}

export interface WeatherData {
  city: string;
  country: string;
  tempC: number;
  condition: string;
  conditionCode: "sunny" | "cloudy" | "rainy" | "storm" | "snow";
  highC: number;
  lowC: number;
  humidity: number;
  windKmH: number;
  uvIndex: number;
  forecast: {
    day: string;
    tempC: number;
    conditionCode: "sunny" | "cloudy" | "rainy" | "storm" | "snow";
  }[];
}

export interface AppSettings {
  theme: "light" | "dark";
  enableSounds: boolean;
  alwaysOnTop: boolean;
  startWithWindows: boolean;
  minimizeToTray: boolean;
  refreshRateMs: number;
  tempUnit: "C" | "F";
  timeFormat: "12h" | "24h";
}

export interface NotificationItem {
  id: string;
  title: string;
  message: string;
  type: "info" | "success" | "warning";
  timestamp: string;
}
