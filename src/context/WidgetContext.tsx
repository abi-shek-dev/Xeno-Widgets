import React, { createContext, useContext, useState, useEffect, useMemo, useCallback } from "react";
import type {
  NavView,
  WidgetMetadata,
  SystemMetrics,
  NoteItem,
  AudioTrack,
  WeatherData,
  AppSettings,
  NotificationItem,
} from "../types";

interface WidgetContextType {
  currentView: NavView;
  setCurrentView: (view: NavView) => void;
  searchQuery: string;
  setSearchQuery: (q: string) => void;
  isCommandPaletteOpen: boolean;
  setIsCommandPaletteOpen: (open: boolean) => void;

  // Widgets
  widgets: WidgetMetadata[];
  toggleWidget: (id: string) => void;
  togglePinWidget: (id: string) => void;
  activeWidgets: WidgetMetadata[];

  // System Metrics
  metrics: SystemMetrics;

  // Notes
  notes: NoteItem[];
  addNote: (title: string, content: string, color?: NoteItem["color"]) => void;
  deleteNote: (id: string) => void;
  toggleNoteCompleted: (id: string) => void;
  togglePinNote: (id: string) => void;

  // Audio / Media
  currentTrack: AudioTrack;
  isPlaying: boolean;
  togglePlay: () => void;
  trackProgress: number;
  setTrackProgress: (progress: number) => void;
  volume: number;
  setVolume: (v: number) => void;
  nextTrack: () => void;
  prevTrack: () => void;

  // Weather
  weather: WeatherData;

  // Settings
  settings: AppSettings;
  updateSettings: (newSettings: Partial<AppSettings>) => void;

  // Notifications
  notifications: NotificationItem[];
  addNotification: (title: string, message: string, type?: NotificationItem["type"]) => void;
  clearNotifications: () => void;
  unreadCount: number;
}

const DEFAULT_WIDGETS: WidgetMetadata[] = [
  {
    id: "clock",
    name: "XE Precision Clock",
    description: "Multi-timezone desktop clock with stopwatch & focus pomodoro.",
    category: "Productivity",
    iconName: "Clock3",
    version: "2.4.0",
    author: "XE Studio",
    tags: ["Time", "Pomodoro", "World Clock", "Timer"],
    defaultSize: "medium",
    isActive: true,
    isPinned: true,
  },
  {
    id: "monitor",
    name: "XE System Telemetry",
    description: "Live CPU, memory, GPU, disk, and real-time network throughput graphs.",
    category: "System",
    iconName: "Activity",
    version: "3.1.2",
    author: "XE Systems",
    tags: ["Hardware", "CPU", "RAM", "Network", "GPU"],
    defaultSize: "large",
    isActive: true,
    isPinned: false,
  },
  {
    id: "notes",
    name: "XE Quick Scratchpad",
    description: "Minimalist sticky notes and task checklist.",
    category: "Productivity",
    iconName: "StickyNote",
    version: "1.9.0",
    author: "XE Studio",
    tags: ["Notes", "Checklist", "Tasks", "Productivity"],
    defaultSize: "medium",
    isActive: true,
    isPinned: false,
  },
  {
    id: "music",
    name: "XE Ambient Sound & Media",
    description: "Desktop media controller with playback controls.",
    category: "Media",
    iconName: "Music2",
    version: "2.0.1",
    author: "XE Labs",
    tags: ["Audio", "Player", "Media"],
    defaultSize: "medium",
    isActive: true,
    isPinned: false,
  },
  {
    id: "weather",
    name: "XE Atmosphere & Forecast",
    description: "Real-time meteorological weather report with 4-day forecast.",
    category: "Utility",
    iconName: "CloudSun",
    version: "1.5.0",
    author: "XE Meteo",
    tags: ["Weather", "Climate", "Forecast", "Atmosphere"],
    defaultSize: "medium",
    isActive: true,
    isPinned: false,
  },
  {
    id: "calendar",
    name: "XE Event Horizon Calendar",
    description: "Compact desktop agenda showing upcoming milestones and schedule.",
    category: "Productivity",
    iconName: "CalendarDays",
    version: "1.2.0",
    author: "XE Studio",
    tags: ["Calendar", "Agenda", "Events", "Schedule"],
    defaultSize: "small",
    isActive: false,
    isPinned: false,
  },
];

const INITIAL_NOTES: NoteItem[] = [
  {
    id: "note-1",
    title: "Project Milestone",
    content: "Ship XE Widgets v2.0 with editorial design & live widgets.",
    color: "amber",
    isPinned: true,
    completed: false,
    updatedAt: "Today, 17:45",
  },
  {
    id: "note-2",
    title: "UI Polish Checklist",
    content: "Thin borders, generous whitespace, precise alignment.",
    color: "sky",
    isPinned: false,
    completed: true,
    updatedAt: "Today, 16:20",
  },
  {
    id: "note-3",
    title: "Design Guidelines",
    content: "Keep shadows for elevation, borders for structure. Optical alignment.",
    color: "emerald",
    isPinned: false,
    completed: false,
    updatedAt: "Today, 14:10",
  },
];

const PLAYLIST: AudioTrack[] = [
  {
    title: "Neon Horizon (Synthwave Mix)",
    artist: "Kavinsky & The Midnight",
    album: "Endless Summer",
    duration: 218,
    coverUrl: "https://images.unsplash.com/photo-1618005182384-a83a8bd57fbe?w=150&auto=format&fit=crop&q=80",
  },
  {
    title: "Cybernetic Pulse",
    artist: "Lorn & Gunship",
    album: "Dark All Day",
    duration: 195,
    coverUrl: "https://images.unsplash.com/photo-1550684848-fac1c5b4e853?w=150&auto=format&fit=crop&q=80",
  },
  {
    title: "Quantum State",
    artist: "Tycho & Com Truise",
    album: "Galactic Drift",
    duration: 244,
    coverUrl: "https://images.unsplash.com/photo-1518709268805-4e9042af9f23?w=150&auto=format&fit=crop&q=80",
  },
];

const INITIAL_WEATHER: WeatherData = {
  city: "San Francisco",
  country: "CA, USA",
  tempC: 19,
  condition: "Partly Cloudy",
  conditionCode: "cloudy",
  highC: 22,
  lowC: 13,
  humidity: 64,
  windKmH: 18,
  uvIndex: 4,
  forecast: [
    { day: "Tue", tempC: 20, conditionCode: "sunny" },
    { day: "Wed", tempC: 18, conditionCode: "cloudy" },
    { day: "Thu", tempC: 16, conditionCode: "rainy" },
    { day: "Fri", tempC: 21, conditionCode: "sunny" },
  ],
};

const DEFAULT_SETTINGS: AppSettings = {
  theme: "light",
  enableSounds: true,
  alwaysOnTop: false,
  startWithWindows: true,
  minimizeToTray: true,
  refreshRateMs: 1000,
  tempUnit: "C",
  timeFormat: "24h",
};

const WidgetContext = createContext<WidgetContextType | undefined>(undefined);

export function WidgetProvider({ children }: { children: React.ReactNode }) {
  const [currentView, setCurrentView] = useState<NavView>("overview");
  const [searchQuery, setSearchQuery] = useState("");
  const [isCommandPaletteOpen, setIsCommandPaletteOpen] = useState(false);

  // Widgets state
  const [widgets, setWidgets] = useState<WidgetMetadata[]>(() => {
    try {
      const saved = localStorage.getItem("xe_widgets_meta");
      if (saved) {
        const parsed: WidgetMetadata[] = JSON.parse(saved);
        // Migrate old data: strip accentColor if present
        return parsed.map(({ ...w }) => w);
      }
      return DEFAULT_WIDGETS;
    } catch {
      return DEFAULT_WIDGETS;
    }
  });

  useEffect(() => {
    try {
      localStorage.setItem("xe_widgets_meta", JSON.stringify(widgets));
    } catch (e) {
      console.error(e);
    }
  }, [widgets]);

  const toggleWidget = useCallback((id: string) => {
    setWidgets((prev) =>
      prev.map((w) => (w.id === id ? { ...w, isActive: !w.isActive } : w))
    );
  }, []);

  const togglePinWidget = useCallback((id: string) => {
    setWidgets((prev) =>
      prev.map((w) => (w.id === id ? { ...w, isPinned: !w.isPinned } : w))
    );
  }, []);

  const activeWidgets = useMemo(() => widgets.filter((w) => w.isActive), [widgets]);

  // System Metrics live update
  const [metrics, setMetrics] = useState<SystemMetrics>({
    cpuUsage: 28,
    cpuHistory: [22, 25, 29, 34, 28, 24, 30, 28],
    ramUsedGB: 11.4,
    ramTotalGB: 32.0,
    diskUsedPercent: 44,
    gpuUsage: 18,
    networkDownMbps: 42.5,
    networkUpMbps: 12.8,
    batteryPercent: 92,
    isCharging: true,
    uptimeHours: 38,
  });

  useEffect(() => {
    const timer = setInterval(() => {
      setMetrics((prev) => {
        const deltaCpu = (Math.random() - 0.48) * 8;
        const newCpu = Math.min(96, Math.max(8, Math.round(prev.cpuUsage + deltaCpu)));
        const newHistory = [...prev.cpuHistory.slice(1), newCpu];

        const deltaRam = (Math.random() - 0.5) * 0.2;
        const newRam = Math.min(31.5, Math.max(8.0, Number((prev.ramUsedGB + deltaRam).toFixed(1))));

        const deltaNetDown = (Math.random() - 0.5) * 6;
        const newDown = Math.max(1.2, Number((prev.networkDownMbps + deltaNetDown).toFixed(1)));

        const deltaNetUp = (Math.random() - 0.5) * 2;
        const newUp = Math.max(0.5, Number((prev.networkUpMbps + deltaNetUp).toFixed(1)));

        const deltaGpu = (Math.random() - 0.49) * 6;
        const newGpu = Math.min(99, Math.max(5, Math.round(prev.gpuUsage + deltaGpu)));

        return {
          ...prev,
          cpuUsage: newCpu,
          cpuHistory: newHistory,
          ramUsedGB: newRam,
          gpuUsage: newGpu,
          networkDownMbps: newDown,
          networkUpMbps: newUp,
        };
      });
    }, 1200);

    return () => clearInterval(timer);
  }, []);

  // Notes state
  const [notes, setNotes] = useState<NoteItem[]>(() => {
    try {
      const saved = localStorage.getItem("xe_widgets_notes");
      return saved ? JSON.parse(saved) : INITIAL_NOTES;
    } catch {
      return INITIAL_NOTES;
    }
  });

  useEffect(() => {
    try {
      localStorage.setItem("xe_widgets_notes", JSON.stringify(notes));
    } catch (e) {
      console.error(e);
    }
  }, [notes]);

  const addNote = useCallback((title: string, content: string, color: NoteItem["color"] = "amber") => {
    const newNote: NoteItem = {
      id: "note-" + Date.now(),
      title: title.trim() || "Quick Note",
      content: content.trim(),
      color,
      isPinned: false,
      completed: false,
      updatedAt: "Just now",
    };
    setNotes((prev) => [newNote, ...prev]);
  }, []);

  const deleteNote = useCallback((id: string) => {
    setNotes((prev) => prev.filter((n) => n.id !== id));
  }, []);

  const toggleNoteCompleted = useCallback((id: string) => {
    setNotes((prev) =>
      prev.map((n) => (n.id === id ? { ...n, completed: !n.completed } : n))
    );
  }, []);

  const togglePinNote = useCallback((id: string) => {
    setNotes((prev) =>
      prev.map((n) => (n.id === id ? { ...n, isPinned: !n.isPinned } : n))
    );
  }, []);

  // Audio state
  const [trackIndex, setTrackIndex] = useState(0);
  const [isPlaying, setIsPlaying] = useState(false);
  const [trackProgress, setTrackProgress] = useState(48);
  const [volume, setVolume] = useState(75);

  const currentTrack = PLAYLIST[trackIndex] || PLAYLIST[0];

  const togglePlay = useCallback(() => {
    setIsPlaying((prev) => !prev);
  }, []);

  const nextTrack = useCallback(() => {
    setTrackIndex((prev) => (prev + 1) % PLAYLIST.length);
    setTrackProgress(0);
  }, []);

  const prevTrack = useCallback(() => {
    setTrackIndex((prev) => (prev - 1 + PLAYLIST.length) % PLAYLIST.length);
    setTrackProgress(0);
  }, []);

  useEffect(() => {
    if (!isPlaying) return;
    const interval = setInterval(() => {
      setTrackProgress((p) => {
        if (p >= currentTrack.duration) {
          nextTrack();
          return 0;
        }
        return p + 1;
      });
    }, 1000);
    return () => clearInterval(interval);
  }, [isPlaying, currentTrack.duration, nextTrack]);

  // Weather state
  const [weather] = useState<WeatherData>(INITIAL_WEATHER);

  // Settings state
  const [settings, setSettings] = useState<AppSettings>(() => {
    try {
      const saved = localStorage.getItem("xe_widgets_settings");
      if (saved) {
        const parsed = JSON.parse(saved);
        // Migrate old settings: convert themeId to theme
        if (parsed.themeId && !parsed.theme) {
          parsed.theme = parsed.themeId === "paper" ? "light" : "dark";
          delete parsed.themeId;
          delete parsed.glassOpacity;
          delete parsed.blurStrength;
        }
        return { ...DEFAULT_SETTINGS, ...parsed };
      }
      return DEFAULT_SETTINGS;
    } catch {
      return DEFAULT_SETTINGS;
    }
  });

  // Apply theme on mount
  useEffect(() => {
    document.documentElement.setAttribute("data-theme", settings.theme);
  }, []);

  useEffect(() => {
    try {
      localStorage.setItem("xe_widgets_settings", JSON.stringify(settings));
    } catch (e) {
      console.error(e);
    }
  }, [settings]);

  const updateSettings = useCallback((newSettings: Partial<AppSettings>) => {
    setSettings((prev) => {
      const merged = { ...prev, ...newSettings };
      if (newSettings.theme) {
        document.documentElement.setAttribute("data-theme", newSettings.theme);
      }
      return merged;
    });
  }, []);

  // Notifications
  const [notifications, setNotifications] = useState<NotificationItem[]>([
    {
      id: "notif-1",
      title: "XE Engine Ready",
      message: "Widgets successfully initialized and synced.",
      type: "success",
      timestamp: "17:50",
    },
    {
      id: "notif-2",
      title: "Design System Applied",
      message: "Editorial minimalism theme loaded.",
      type: "info",
      timestamp: "17:42",
    },
  ]);

  const addNotification = useCallback((title: string, message: string, type: NotificationItem["type"] = "info") => {
    const item: NotificationItem = {
      id: "notif-" + Date.now(),
      title,
      message,
      type,
      timestamp: "Just now",
    };
    setNotifications((prev) => [item, ...prev]);
  }, []);

  const clearNotifications = useCallback(() => {
    setNotifications([]);
  }, []);

  // Hotkey listener for Command Palette (Ctrl+K)
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "k") {
        e.preventDefault();
        setIsCommandPaletteOpen((prev) => !prev);
      }
      if (e.key === "Escape") {
        setIsCommandPaletteOpen(false);
      }
    };
    window.addEventListener("keydown", handleKeyDown);
    return () => window.removeEventListener("keydown", handleKeyDown);
  }, []);

  const value = {
    currentView,
    setCurrentView,
    searchQuery,
    setSearchQuery,
    isCommandPaletteOpen,
    setIsCommandPaletteOpen,
    widgets,
    toggleWidget,
    togglePinWidget,
    activeWidgets,
    metrics,
    notes,
    addNote,
    deleteNote,
    toggleNoteCompleted,
    togglePinNote,
    currentTrack,
    isPlaying,
    togglePlay,
    trackProgress,
    setTrackProgress,
    volume,
    setVolume,
    nextTrack,
    prevTrack,
    weather,
    settings,
    updateSettings,
    notifications,
    addNotification,
    clearNotifications,
    unreadCount: notifications.length,
  };

  return <WidgetContext.Provider value={value}>{children}</WidgetContext.Provider>;
}

export function useWidgets() {
  const context = useContext(WidgetContext);
  if (!context) {
    throw new Error("useWidgets must be used within a WidgetProvider");
  }
  return context;
}
