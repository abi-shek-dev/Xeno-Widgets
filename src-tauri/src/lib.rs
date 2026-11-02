mod widget_manager;

use std::collections::HashMap;
use tauri::{AppHandle, Manager};
use widget_manager::{WidgetConfig, WidgetManager};

#[derive(serde::Serialize)]
#[serde(rename_all = "camelCase")]
pub struct CurrentWindowInfo {
    pub label: String,
    pub is_widget: bool,
    pub widget_id: Option<String>,
}

#[tauri::command]
fn get_current_window_info(window: tauri::Window) -> CurrentWindowInfo {
    let label = window.label().to_string();
    let is_widget = label.starts_with("xe-widget-");
    let widget_id = if is_widget {
        Some(label.trim_start_matches("xe-widget-").to_string())
    } else {
        None
    };
    CurrentWindowInfo {
        label,
        is_widget,
        widget_id,
    }
}

#[tauri::command]
fn open_widget_window(app: AppHandle, widget_id: String) -> Result<(), String> {
    widget_manager::open_widget_window(&app, &widget_id)
}

#[tauri::command]
fn close_widget_window(app: AppHandle, widget_id: String) -> Result<(), String> {
    widget_manager::close_widget_window(&app, &widget_id)
}

#[tauri::command]
fn focus_widget_window(app: AppHandle, widget_id: String) -> Result<(), String> {
    widget_manager::focus_widget_window(&app, &widget_id)
}

#[tauri::command]
fn toggle_widget_window(app: AppHandle, widget_id: String) -> Result<bool, String> {
    widget_manager::toggle_widget_window(&app, &widget_id)
}

#[tauri::command]
fn set_widget_always_on_top(
    app: AppHandle,
    widget_id: String,
    always_on_top: bool,
) -> Result<(), String> {
    let layer = if always_on_top { "top" } else { "normal" };
    widget_manager::set_widget_layer(&app, &widget_id, layer)
}

#[tauri::command]
fn set_widget_layer(app: AppHandle, widget_id: String, layer: String) -> Result<(), String> {
    widget_manager::set_widget_layer(&app, &widget_id, &layer)
}

#[tauri::command]
fn set_widget_locked(app: AppHandle, widget_id: String, locked: bool) -> Result<(), String> {
    widget_manager::set_widget_locked(&app, &widget_id, locked)
}

#[tauri::command]
fn set_widget_click_through(
    app: AppHandle,
    widget_id: String,
    click_through: bool,
) -> Result<(), String> {
    widget_manager::set_widget_click_through(&app, &widget_id, click_through)
}

#[tauri::command]
fn set_widget_opacity(app: AppHandle, widget_id: String, opacity: f64) -> Result<(), String> {
    widget_manager::set_widget_opacity(&app, &widget_id, opacity)
}

#[tauri::command]
fn get_open_widget_windows(app: AppHandle) -> Result<Vec<String>, String> {
    Ok(widget_manager::get_open_widget_windows(&app))
}

#[tauri::command]
fn get_widget_configs(app: AppHandle) -> Result<HashMap<String, WidgetConfig>, String> {
    let manager = app.state::<WidgetManager>();
    Ok(manager.get_all_configs())
}

#[tauri::command]
fn show_main_window(app: AppHandle) -> Result<(), String> {
    widget_manager::show_main_window(&app)
}

#[tauri::command]
fn save_widget_bounds(
    app: AppHandle,
    widget_id: String,
    x: i32,
    y: i32,
    width: f64,
    height: f64,
) -> Result<(), String> {
    let manager = app.state::<WidgetManager>();
    manager.update_config(&widget_id, |c| {
        c.x = Some(x);
        c.y = Some(y);
        c.width = width;
        c.height = height;
    });
    Ok(())
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_opener::init())
        .setup(|app| {
            let manager = WidgetManager::new(app.handle());
            app.manage(manager);

            // Restore any widgets that were active and visible before restart with a slight delay
            let handle = app.handle().clone();
            std::thread::spawn(move || {
                std::thread::sleep(std::time::Duration::from_millis(700));
                widget_manager::restore_saved_widgets(&handle);
            });

            Ok(())
        })
        .on_window_event(|window, event| {
            if let tauri::WindowEvent::CloseRequested { api, .. } = event {
                let app = window.app_handle();
                if window.label() == "main" {
                    let open_widgets = widget_manager::get_open_widget_windows(app);
                    if !open_widgets.is_empty() {
                        // Desktop widgets are still active; hide main window instead of killing app
                        let _ = window.hide();
                        api.prevent_close();
                    }
                } else if window.label().starts_with("xe-widget-") {
                    let widget_id = window.label().trim_start_matches("xe-widget-");
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
                }
            }
        })
        .invoke_handler(tauri::generate_handler![
            get_current_window_info,
            open_widget_window,
            close_widget_window,
            focus_widget_window,
            toggle_widget_window,
            set_widget_always_on_top,
            set_widget_layer,
            set_widget_locked,
            set_widget_click_through,
            set_widget_opacity,
            get_open_widget_windows,
            get_widget_configs,
            show_main_window,
            save_widget_bounds,
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
