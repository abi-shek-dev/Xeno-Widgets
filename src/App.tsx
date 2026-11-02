import { WidgetProvider, useWidgets } from "./context/WidgetContext";
import Sidebar from "./components/layout/Sidebar";
import CommandPalette from "./components/modals/CommandPalette";
import OverviewView from "./views/OverviewView";
import WidgetLibraryView from "./views/WidgetLibraryView";
import ActiveWidgetsView from "./views/ActiveWidgetsView";
import SettingsView from "./views/SettingsView";

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
  return (
    <WidgetProvider>
      <MainContent />
    </WidgetProvider>
  );
}
