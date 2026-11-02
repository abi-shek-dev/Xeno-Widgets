import { useState, useEffect } from "react";
import { WidgetProvider, useWidgets } from "./context/WidgetContext";
import Sidebar from "./components/layout/Sidebar";
import CommandPalette from "./components/modals/CommandPalette";
import FloatingWidgetFrame from "./components/desktop/FloatingWidgetFrame";
import OverviewView from "./views/OverviewView";
import WidgetLibraryView from "./views/WidgetLibraryView";
import ActiveWidgetsView from "./views/ActiveWidgetsView";
import SettingsView from "./views/SettingsView";
import { detectWidgetId, getCurrentWindowInfo } from "./services/desktopWidgetService";

function MainContent() {
  const { currentView } = useWidgets();

  return (
    <div
      className="flex h-screen w-screen overflow-hidden select-none"
      style={{ background: "var(--xe-bg)", color: "var(--xe-fg)", fontFamily: "var(--xe-font-body)" }}
    >
      <Sidebar />
      <main
        className="flex-1 overflow-hidden flex flex-col"
        style={{ background: "var(--xe-bg)" }}
      >
        {currentView === "overview" && <OverviewView />}
        {currentView === "library" && <WidgetLibraryView />}
        {currentView === "active" && <ActiveWidgetsView />}
        {currentView === "settings" && <SettingsView />}
      </main>
      <CommandPalette />
    </div>
  );
}

export default function App() {
  // Synchronous detection first (from injected global, label or URL)
  const [widgetId, setWidgetId] = useState<string | null>(() => detectWidgetId());

  // Asynchronous verification via Tauri command as secondary fallback
  useEffect(() => {
    if (!widgetId) {
      getCurrentWindowInfo()
        .then((info) => {
          if (info.isWidget && info.widgetId) {
            setWidgetId(info.widgetId);
          }
        })
        .catch(() => {});
    }
  }, [widgetId]);

  if (widgetId) {
    return (
      <WidgetProvider>
        <FloatingWidgetFrame widgetId={widgetId} />
      </WidgetProvider>
    );
  }

  return (
    <WidgetProvider>
      <MainContent />
    </WidgetProvider>
  );
}
