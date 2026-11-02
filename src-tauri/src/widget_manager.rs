use std::collections::HashMap;
use std::fs;
use std::path::PathBuf;
use std::sync::{Arc, Mutex};
use serde::{Deserialize, Serialize};
use tauri::{AppHandle, Manager, WebviewUrl, WebviewWindowBuilder};

pub const REGISTERED_WIDGET_IDS: &[&str] = &[
    "clock",
    "monitor",
    "notes",
    "music",
    "weather",
    "calendar",
];

fn default_layer() -> String {
    "normal".to_string()
}

fn default_opacity() -> f64 {
    1.0
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct WidgetConfig {
    pub id: String,
    pub enabled: bool,
    pub visible: bool,
    pub x: Option<i32>,
    pub y: Option<i32>,
    pub width: f64,
    pub height: f64,
    pub always_on_top: bool,
    #[serde(default = "default_layer")]
    pub layer: String, // "bottom" (desktop layer), "normal", "top" (stay on top)
    #[serde(default)]
    pub locked: bool, // locked position prevents dragging
    #[serde(default)]
    pub click_through: bool, // click through to desktop
    #[serde(default = "default_opacity")]
    pub opacity: f64, // 0.2 to 1.0
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct WidgetStore {
    pub version: u32,
    pub widgets: HashMap<String, WidgetConfig>,
}

impl Default for WidgetStore {
    fn default() -> Self {
        let mut widgets = HashMap::new();
        for &id in REGISTERED_WIDGET_IDS {
            let (w, h) = get_default_dimensions(id);
            widgets.insert(
                id.to_string(),
                WidgetConfig {
                    id: id.to_string(),
                    enabled: id == "clock" || id == "monitor",
                    visible: false,
                    x: None,
                    y: None,
                    width: w,
                    height: h,
                    always_on_top: false,
                    layer: "normal".to_string(),
                    locked: false,
                    click_through: false,
                    opacity: 1.0,
                },
            );
        }
        Self {
            version: 1,
            widgets,
        }
    }
}

pub struct WidgetManager {
    store: Arc<Mutex<WidgetStore>>,
    config_path: PathBuf,
}

impl WidgetManager {
    pub fn new(app: &AppHandle) -> Self {
        let config_dir = app
            .path()
            .app_config_dir()
            .unwrap_or_else(|_| PathBuf::from("."));

        let config_path = config_dir.join("widgets_config.json");
        let store = Self::load_store(&config_path);

        Self {
            store: Arc::new(Mutex::new(store)),
            config_path,
        }
    }

    fn load_store(path: &PathBuf) -> WidgetStore {
        if let Ok(data) = fs::read_to_string(path) {
            if let Ok(store) = serde_json::from_str::<WidgetStore>(&data) {
                return store;
            }
        }
        WidgetStore::default()
    }

    pub fn save_store(&self) {
        if let Ok(store) = self.store.lock() {
            if let Some(parent) = self.config_path.parent() {
                let _ = fs::create_dir_all(parent);
            }
            if let Ok(json) = serde_json::to_string_pretty(&*store) {
                let _ = fs::write(&self.config_path, json);
            }
        }
    }

    pub fn get_config(&self, id: &str) -> WidgetConfig {
        if let Ok(store) = self.store.lock() {
            if let Some(cfg) = store.widgets.get(id) {
                return cfg.clone();
            }
        }
        let (w, h) = get_default_dimensions(id);
        WidgetConfig {
            id: id.to_string(),
            enabled: true,
            visible: false,
            x: None,
            y: None,
            width: w,
            height: h,
            always_on_top: false,
            layer: "normal".to_string(),
            locked: false,
            click_through: false,
            opacity: 1.0,
        }
    }

    pub fn update_config<F>(&self, id: &str, f: F)
    where
        F: FnOnce(&mut WidgetConfig),
    {
        if let Ok(mut store) = self.store.lock() {
            let entry = store
                .widgets
                .entry(id.to_string())
                .or_insert_with(|| {
                    let (w, h) = get_default_dimensions(id);
                    WidgetConfig {
                        id: id.to_string(),
                        enabled: true,
                        visible: false,
                        x: None,
                        y: None,
                        width: w,
                        height: h,
                        always_on_top: false,
                        layer: "normal".to_string(),
                        locked: false,
                        click_through: false,
                        opacity: 1.0,
                    }
                });
            f(entry);
        }
        self.save_store();
    }

    pub fn get_all_configs(&self) -> HashMap<String, WidgetConfig> {
        if let Ok(store) = self.store.lock() {
            store.widgets.clone()
        } else {
            HashMap::new()
        }
    }
}

pub fn is_valid_widget_id(id: &str) -> bool {
    REGISTERED_WIDGET_IDS.contains(&id)
}

pub fn window_label_for_widget(id: &str) -> String {
    format!("xe-widget-{}", id)
}

pub fn get_widget_title(id: &str) -> &'static str {
    match id {
        "clock" => "XE Precision Clock",
        "monitor" => "XE System Telemetry",
        "notes" => "XE Quick Scratchpad",
        "music" => "XE Ambient Sound & Media",
        "weather" => "XE Atmosphere & Forecast",
        "calendar" => "XE Event Horizon Calendar",
        _ => "XE Widget",
    }
}

pub fn get_default_dimensions(id: &str) -> (f64, f64) {
    match id {
        "clock" => (320.0, 180.0),
        "monitor" => (380.0, 280.0),
        "notes" => (360.0, 320.0),
        "music" => (360.0, 260.0),
        "weather" => (380.0, 340.0),
        "calendar" => (420.0, 360.0),
        _ => (320.0, 240.0),
    }
}

pub fn get_min_dimensions(id: &str) -> (f64, f64) {
    match id {
        "clock" => (240.0, 120.0),
        "monitor" => (280.0, 180.0),
        "notes" => (240.0, 180.0),
        "music" => (260.0, 160.0),
        "weather" => (260.0, 200.0),
        "calendar" => (280.0, 220.0),
        _ => (200.0, 140.0),
    }
}

/// Validates whether a saved (x, y) coordinate falls within any currently connected display monitor.
fn is_position_visible(app: &AppHandle, x: i32, y: i32) -> bool {
    if let Ok(monitors) = app.available_monitors() {
        for monitor in monitors {
            let pos = monitor.position();
            let size = monitor.size();
            let mx = pos.x;
            let my = pos.y;
            let mw = size.width as i32;
            let mh = size.height as i32;

            if x >= mx - 100 && x < (mx + mw - 50) && y >= my - 50 && y < (my + mh - 50) {
                return true;
            }
        }
    }
    false
}

/// Opens or focuses an independent floating Tauri window for a registered widget
pub fn open_widget_window(app: &AppHandle, widget_id: &str) -> Result<(), String> {
    if !is_valid_widget_id(widget_id) {
        return Err(format!("Unrecognized widget identifier: {}", widget_id));
    }

    let label = window_label_for_widget(widget_id);

    // If window already exists, unminimize, show and focus it
    if let Some(existing_window) = app.get_webview_window(&label) {
        let _ = existing_window.unminimize();
        let _ = existing_window.show();
        let _ = existing_window.set_focus();
        return Ok(());
    }

    // Window doesn't exist; get stored config or defaults
    let manager = app.state::<WidgetManager>();
    let config = manager.get_config(widget_id);

    let title = get_widget_title(widget_id);
    let (min_w, min_h) = get_min_dimensions(widget_id);
    let width = if config.width >= min_w { config.width } else { min_w };
    let height = if config.height >= min_h { config.height } else { min_h };

    let is_top = config.layer == "top" || config.always_on_top;
    let is_bottom = config.layer == "bottom";

    let init_script = format!(
        "window.__XE_WIDGET_ID__ = '{}';\
         window.__XE_INITIAL_LAYER__ = '{}';\
         window.__XE_INITIAL_LOCKED__ = {};\
         window.__XE_INITIAL_CLICK_THROUGH__ = {};\
         window.__XE_INITIAL_OPACITY__ = {};\
         window.__XE_INITIAL_ALWAYS_ON_TOP__ = {};",
        widget_id,
        config.layer,
        config.locked,
        config.click_through,
        config.opacity,
        is_top
    );

    let mut builder = WebviewWindowBuilder::new(app, &label, WebviewUrl::default())
        .title(title)
        .initialization_script(&init_script)
        .inner_size(width, height)
        .min_inner_size(min_w, min_h)
        .resizable(true)
        .decorations(false) // Frameless Rainmeter skin
        .transparent(true) // Transparent backing so skin floats directly on wallpaper
        .shadow(false)
        .always_on_top(is_top)
        .skip_taskbar(true); // Never pollute taskbar like Rainmeter

    // Apply saved coordinates if they are within a currently visible monitor
    if let (Some(x), Some(y)) = (config.x, config.y) {
        if is_position_visible(app, x, y) {
            builder = builder.position(x as f64, y as f64);
        } else {
            builder = builder.center();
        }
    } else {
        builder = builder.center();
    }

    let window = builder
        .build()
        .map_err(|e| format!("Failed to create widget window {}: {}", widget_id, e))?;

    // Apply bottom layer if requested
    if is_bottom {
        let _ = window.set_always_on_bottom(true);
    }

    // Apply click through if configured
    if config.click_through {
        let _ = window.set_ignore_cursor_events(true);
    }

    // Mark as visible in config store
    manager.update_config(widget_id, |c| {
        c.visible = true;
        c.enabled = true;
    });

    let _ = window.show();
    let _ = window.set_focus();

    Ok(())
}

/// Closes the native widget window if it is open
pub fn close_widget_window(app: &AppHandle, widget_id: &str) -> Result<(), String> {
    let label = window_label_for_widget(widget_id);
    if let Some(window) = app.get_webview_window(&label) {
        if let Ok(pos) = window.outer_position() {
            if let Ok(size) = window.inner_size() {
                let manager = app.state::<WidgetManager>();
                manager.update_config(widget_id, |c| {
                    c.x = Some(pos.x);
                    c.y = Some(pos.y);
                    c.width = size.width as f64;
                    c.height = size.height as f64;
                    c.visible = false;
                });
            }
        }
        window.close().map_err(|e| format!("Error closing widget window: {}", e))?;
    } else {
        let manager = app.state::<WidgetManager>();
        manager.update_config(widget_id, |c| {
            c.visible = false;
        });
    }

    Ok(())
}

/// Focuses an open widget window
pub fn focus_widget_window(app: &AppHandle, widget_id: &str) -> Result<(), String> {
    let label = window_label_for_widget(widget_id);
    if let Some(window) = app.get_webview_window(&label) {
        let _ = window.unminimize();
        let _ = window.show();
        let _ = window.set_focus();
        Ok(())
    } else {
        Err(format!("Widget window {} is not open", widget_id))
    }
}

/// Toggles open/close state of a widget window
pub fn toggle_widget_window(app: &AppHandle, widget_id: &str) -> Result<bool, String> {
    let label = window_label_for_widget(widget_id);
    if app.get_webview_window(&label).is_some() {
        close_widget_window(app, widget_id)?;
        Ok(false)
    } else {
        open_widget_window(app, widget_id)?;
        Ok(true)
    }
}

/// Sets layer/z-order: "bottom" (on desktop wallpaper), "normal", or "top" (stay on top)
pub fn set_widget_layer(app: &AppHandle, widget_id: &str, layer: &str) -> Result<(), String> {
    let label = window_label_for_widget(widget_id);
    if let Some(window) = app.get_webview_window(&label) {
        match layer {
            "bottom" => {
                let _ = window.set_always_on_top(false);
                let _ = window.set_always_on_bottom(true);
            }
            "top" => {
                let _ = window.set_always_on_bottom(false);
                let _ = window.set_always_on_top(true);
            }
            _ => {
                let _ = window.set_always_on_top(false);
                let _ = window.set_always_on_bottom(false);
            }
        }
    }

    let manager = app.state::<WidgetManager>();
    manager.update_config(widget_id, |c| {
        c.layer = layer.to_string();
        c.always_on_top = layer == "top";
    });

    Ok(())
}

/// Toggles locked position (disables or enables window movement)
pub fn set_widget_locked(app: &AppHandle, widget_id: &str, locked: bool) -> Result<(), String> {
    let manager = app.state::<WidgetManager>();
    manager.update_config(widget_id, |c| {
        c.locked = locked;
    });
    Ok(())
}

/// Sets click-through mode (transparent to mouse clicks)
pub fn set_widget_click_through(app: &AppHandle, widget_id: &str, click_through: bool) -> Result<(), String> {
    let label = window_label_for_widget(widget_id);
    if let Some(window) = app.get_webview_window(&label) {
        window
            .set_ignore_cursor_events(click_through)
            .map_err(|e| format!("Failed to set click-through: {}", e))?;
    }

    let manager = app.state::<WidgetManager>();
    manager.update_config(widget_id, |c| {
        c.click_through = click_through;
    });

    Ok(())
}

/// Sets opacity (0.2 to 1.0)
pub fn set_widget_opacity(app: &AppHandle, widget_id: &str, opacity: f64) -> Result<(), String> {
    let manager = app.state::<WidgetManager>();
    manager.update_config(widget_id, |c| {
        c.opacity = opacity.clamp(0.2, 1.0);
    });
    Ok(())
}

/// Returns a list of currently open widget IDs
pub fn get_open_widget_windows(app: &AppHandle) -> Vec<String> {
    REGISTERED_WIDGET_IDS
        .iter()
        .filter_map(|&id| {
            let label = window_label_for_widget(id);
            if app.get_webview_window(&label).is_some() {
                Some(id.to_string())
            } else {
                None
            }
        })
        .collect()
}

/// Restores all previously open widgets on startup
pub fn restore_saved_widgets(app: &AppHandle) {
    let manager = app.state::<WidgetManager>();
    let configs = manager.get_all_configs();
    for (id, config) in configs {
        if config.visible && config.enabled && is_valid_widget_id(&id) {
            let _ = open_widget_window(app, &id);
        }
    }
}

/// Brings the main application dashboard to the front
pub fn show_main_window(app: &AppHandle) -> Result<(), String> {
    if let Some(main) = app.get_webview_window("main") {
        let _ = main.show();
        let _ = main.unminimize();
        let _ = main.set_focus();
        Ok(())
    } else {
        Err("Main window not found".to_string())
    }
}
