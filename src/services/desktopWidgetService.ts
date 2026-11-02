import { invoke } from "@tauri-apps/api/core";
import type { DesktopWidgetConfig } from "../types";

export interface CurrentWindowInfo {
  label: string;
  isWidget: boolean;
  widgetId?: string;
}

/**
 * Checks if the current environment is running inside a native Tauri runtime.
 */
export function isTauriEnvironment(): boolean {
  return (
    typeof window !== "undefined" &&
    Boolean((window as unknown as { __TAURI_INTERNALS__?: unknown }).__TAURI_INTERNALS__)
  );
}

/**
 * Detects whether this window should render as a desktop widget.
 * Checks synchronously for injected global, URL query params, hash params, or Tauri window label.
 */
export function detectWidgetId(): string | null {
  // 1. Injected global variable from Tauri initialization_script
  if (typeof window !== "undefined") {
    const injected = (window as unknown as { __XE_WIDGET_ID__?: string }).__XE_WIDGET_ID__;
    if (injected && injected.trim()) {
      return injected.trim();
    }
  }

  // 2. URL search parameters (e.g. ?widget=clock for browser testing)
  if (typeof window !== "undefined" && window.location) {
    const searchParams = new URLSearchParams(window.location.search);
    const param = searchParams.get("widget");
    if (param && param.trim()) {
      return param.trim();
    }

    // 3. URL hash fallback (e.g. #widget=clock)
    if (window.location.hash) {
      const hashParams = new URLSearchParams(window.location.hash.replace(/^#/, "?"));
      const hashWidget = hashParams.get("widget");
      if (hashWidget && hashWidget.trim()) {
        return hashWidget.trim();
      }
    }
  }

  // 4. Tauri window label fallback
  try {
    const internals = (
      window as unknown as {
        __TAURI_INTERNALS__?: {
          metadata?: {
            currentWindow?: { label?: string };
          };
        };
      }
    ).__TAURI_INTERNALS__;
    const label = internals?.metadata?.currentWindow?.label;
    if (label && label.startsWith("xe-widget-")) {
      return label.replace("xe-widget-", "");
    }
  } catch {
    // Ignore error
  }

  return null;
}

export async function getCurrentWindowInfo(): Promise<CurrentWindowInfo> {
  if (!isTauriEnvironment()) {
    const widgetId = detectWidgetId();
    return {
      label: widgetId ? `xe-widget-${widgetId}` : "main",
      isWidget: Boolean(widgetId),
      widgetId: widgetId || undefined,
    };
  }
  try {
    return await invoke<CurrentWindowInfo>("get_current_window_info");
  } catch (error) {
    console.error("Failed to get current window info:", error);
    const widgetId = detectWidgetId();
    return {
      label: widgetId ? `xe-widget-${widgetId}` : "main",
      isWidget: Boolean(widgetId),
      widgetId: widgetId || undefined,
    };
  }
}

export async function openWidgetWindow(widgetId: string): Promise<void> {
  if (!isTauriEnvironment()) {
    console.info(`[DesktopWidgetService] openWidgetWindow: ${widgetId} (browser preview)`);
    return;
  }
  try {
    await invoke("open_widget_window", { widgetId });
  } catch (error) {
    console.error(`Failed to open widget window ${widgetId}:`, error);
    throw error;
  }
}

export async function closeWidgetWindow(widgetId: string): Promise<void> {
  if (!isTauriEnvironment()) {
    console.info(`[DesktopWidgetService] closeWidgetWindow: ${widgetId} (browser preview)`);
    return;
  }
  try {
    await invoke("close_widget_window", { widgetId });
  } catch (error) {
    console.error(`Failed to close widget window ${widgetId}:`, error);
    throw error;
  }
}

export async function focusWidgetWindow(widgetId: string): Promise<void> {
  if (!isTauriEnvironment()) {
    console.info(`[DesktopWidgetService] focusWidgetWindow: ${widgetId} (browser preview)`);
    return;
  }
  try {
    await invoke("focus_widget_window", { widgetId });
  } catch (error) {
    console.error(`Failed to focus widget window ${widgetId}:`, error);
    throw error;
  }
}

export async function toggleWidgetWindow(widgetId: string): Promise<boolean> {
  if (!isTauriEnvironment()) {
    console.info(`[DesktopWidgetService] toggleWidgetWindow: ${widgetId} (browser preview)`);
    return true;
  }
  try {
    return await invoke<boolean>("toggle_widget_window", { widgetId });
  } catch (error) {
    console.error(`Failed to toggle widget window ${widgetId}:`, error);
    throw error;
  }
}

export async function setWidgetAlwaysOnTop(widgetId: string, alwaysOnTop: boolean): Promise<void> {
  if (!isTauriEnvironment()) {
    console.info(`[DesktopWidgetService] setWidgetAlwaysOnTop: ${widgetId} = ${alwaysOnTop}`);
    return;
  }
  try {
    await invoke("set_widget_always_on_top", { widgetId, alwaysOnTop });
  } catch (error) {
    console.error(`Failed to set always on top for ${widgetId}:`, error);
    throw error;
  }
}

export async function getOpenWidgetWindows(): Promise<string[]> {
  if (!isTauriEnvironment()) {
    return [];
  }
  try {
    return await invoke<string[]>("get_open_widget_windows");
  } catch (error) {
    console.error("Failed to query open widget windows:", error);
    return [];
  }
}

export async function getWidgetConfigs(): Promise<Record<string, DesktopWidgetConfig>> {
  if (!isTauriEnvironment()) {
    return {};
  }
  try {
    return await invoke<Record<string, DesktopWidgetConfig>>("get_widget_configs");
  } catch (error) {
    console.error("Failed to get widget configs:", error);
    return {};
  }
}

export async function showMainWindow(): Promise<void> {
  if (!isTauriEnvironment()) {
    console.info("[DesktopWidgetService] showMainWindow (browser preview)");
    return;
  }
  try {
    await invoke("show_main_window");
  } catch (error) {
    console.error("Failed to show main window:", error);
    throw error;
  }
}
