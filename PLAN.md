# Gates — план разработки

> Цель: Fences-клон на Qt6/QML для Windows.
> Полупрозрачные фреймы на рабочем столе с icon grid, blur (Acrylic/Mica),
> полной Shell-интеграцией (контекстное меню, DnD, файловые операции) и персистентным конфигом.

---

## Архитектура (коротко)

```
Z-order (снизу вверх):
  [Wallpaper]
  [WorkerW / Progman]  ← нативный SysListView32 спрятан
  [GatesDesktopLayer]  ← полноэкранный прозрачный слой (HWND_BOTTOM)
    ├─ VirtualDesktop  ← иконки не в фреймах, свободное позиционирование
    └─ GatesFrame[]    ← фреймы с blur, grid-раскладкой иконок
  [Обычные приложения]
```

Данные хранятся в `%APPDATA%\Gates\config.toml`.
Физические файлы и ярлыки на диске **не перемещаются**.

---

## Фаза 1 — Основа

| # | Задача | Статус |
|---|--------|--------|
| 1 | **ConfigManager** — чтение/запись TOML (`%APPDATA%\Gates\config.toml`). Структуры: `GlobalConfig`, `FrameConfig` (id, name, monitor, x, y, w, h, collapsed, docked_edge, style), `IconEntry` (path, row, col), `DesktopIconEntry` (path, x, y). Методы: `load()`, `save()`, `QFileSystemWatcher` для hot-reload. | ✅ |
| 2 | **GatesFrameDispatcher** — создание/удаление экземпляров `GatesFrameForeign` по данным из `ConfigManager`. Синхронизация позиции/размера/состояния с конфигом при каждом изменении. | ✅ |

---

## Фаза 2 — Десктоп-слой

| # | Задача | Статус |
|---|--------|--------|
| 3 | **DesktopShellManager: захват позиций при первом запуске** — при отсутствии конфига найти `SysListView32` внутри `Progman/WorkerW`, прочитать позиции и пути всех иконок через `LVM_GETITEM` + `LVM_GETITEMPOSITION`, сохранить в `config.toml` как `desktop.icons[]`. | ✅ |
| 4 | **DesktopShellManager: скрытие/восстановление нативных иконок** — `ShowWindow(SW_HIDE)` при старте, `SW_SHOW` при выходе. Обработка крэша через `SetUnhandledExceptionFilter`. Поддержка Win10 (Progman) и Win11 (WorkerW) топологий. | ✅ |
| 5 | **GatesDesktopLayer** — `QQuickWindow` на весь экран, прозрачный фон, без рамки. `SetWindowPos(HWND_BOTTOM)` + повтор при `WM_WINDOWPOSCHANGING`. `WS_EX_TOOLWINDOW` (нет в таскбаре), `WS_EX_NOACTIVATE` (не крадёт фокус). По одному экземпляру на монитор. | ✅ |
| 6 | **VirtualDesktop QML** — компонент внутри `GatesDesktopLayer`. Показывает иконки из `config.desktop.icons[]` со свободным позиционированием (x, y). Drag иконок сохраняет позицию в конфиг. Иконки, перемещённые в фрейм, исчезают отсюда. | ✅ |

---

## Фаза 3 — Shell-интеграция

| # | Задача | Статус |
|---|--------|--------|
| 7 | **Shell Context Menu** — через COM: `IShellFolder::GetUIObjectOf()` → `IContextMenu` → `QueryContextMenu()` → `TrackPopupMenu()` → `InvokeCommand()`. Вызов по ПКМ на любой иконке. Меню идентично нативному Explorer. | ✅ |
| 8 | **DnD внутри Gates** — перетаскивание иконок между фреймами и `VirtualDesktop`. При drop: обновить `IconEntry` (убрать из источника, добавить в цель), сохранить конфиг. `QML DragHandler` + логика на C++. | ⬜ |
| 9 | **DnD ↔ Windows Shell** — `IDropSource` + `IDataObject` с `CFSTR_SHELLIDLIST` (drag FROM Gates → Explorer). `IDropTarget` + `RegisterDragDrop` (drop INTO Gates из Explorer). Полная совместимость с OLE Shell DnD. | ⬜ |
| 10 | **Файловые операции** — `ShellExecute` (open), `IFileOperation` (copy/move/delete), inline rename через Shell. Горячие клавиши: `F2` = rename, `Del` = delete, `Enter` = open. | ⬜ |

---

## Фаза 4 — UX управления фреймами

| # | Задача | Статус |
|---|--------|--------|
| 11 | **SystemTrayIcon** — `QSystemTrayIcon` с меню: "Создать фрейм", "Настройки", "Показать/скрыть все", "Выход". Двойной клик → настройки. | ⬜ |
| 12 | **Frame UX** — rename (двойной клик на заголовке → inline edit), collapse (двойной клик → сворачивается до высоты заголовка с анимацией), dock-to-edge (прилипание к краю + hover-reveal). | ⬜ |
| 13 | **AutostartManager** — запись/удаление `HKCU\Software\Microsoft\Windows\CurrentVersion\Run\Gates`. Проверка актуальности пути при старте. | ⬜ |

---

## Фаза 5 — Мульти-монитор

| # | Задача | Статус |
|---|--------|--------|
| 14 | **MonitorManager** — привязка фрейма к `QScreen` по имени/серийному номеру. При `screenRemoved`: переносить фреймы на primary screen с сохранением относительных координат. `GatesDesktopLayer` — по одному на каждый монитор. | ⬜ |

---

## Фаза 6 — Визуал

| # | Задача | Статус |
|---|--------|--------|
| 15 | **BlurManager** — определять версию Windows (`RtlGetVersion`). Win11 build ≥ 22000: Mica через `DwmSetWindowAttribute(DWMWA_SYSTEMBACKDROP_TYPE)`. Win10: Acrylic через `SetWindowCompositionAttribute`. Режим `auto` выбирает лучший доступный. Per-frame override в конфиге. | ⬜ |

---

## Фаза 7 — Настройки

| # | Задача | Статус |
|---|--------|--------|
| 16 | **SettingsUI** — отдельное QML-окно: глобальные настройки (автозапуск, blur preference), список фреймов (создать/удалить), редактор стиля каждого фрейма (цвет, opacity, blur mode, radius). Открывается из трея или из контекстного меню фрейма (ПКМ на заголовке). | ⬜ |

---

## Зависимости

```
1 (ConfigManager)
└─► 2 (Dispatcher)
    └─► 3 (захват иконок)
        └─► 4 (скрыть нативные)
            └─► 5 (DesktopLayer)
                └─► 6 (VirtualDesktop)
                    └─► 7 (ContextMenu)
                        └─► 8 (DnD внутри)
                            └─► 9 (DnD ↔ Shell)
                                └─► 10 (файловые операции)

Параллельно с Ф3+:
  11 (Tray)
  12 (Frame UX)
  13 (Autostart)
  14 (MultiMonitor)
  15 (BlurManager)
  16 (SettingsUI)
```

---

## Формат config.toml

```toml
[global]
autostart       = true
blur_preference = "auto"   # "mica" | "acrylic" | "none" | "auto"

[[desktop.icons]]
path = "C:\\Users\\nexie\\Desktop\\This PC.lnk"
x    = 1820
y    = 80

[[frames]]
id          = "550e8400-e29b-frame1"
name        = "Work"
monitor     = "\\.\DISPLAY1"
x           = 120
y           = 80
width       = 380
height      = 280
collapsed   = false
docked_edge = "none"       # "left" | "right" | "top" | "bottom" | "none"

[frames.style]
blur    = "mica"
color   = "#1a1a2e"
opacity = 0.72
radius  = 12

[[frames.icons]]
path = "C:\\Users\\nexie\\Desktop\\project.lnk"
row  = 0
col  = 0

[[frames.icons]]
path = "C:\\Users\\nexie\\Desktop\\Visual Studio.lnk"
row  = 0
col  = 1
```
