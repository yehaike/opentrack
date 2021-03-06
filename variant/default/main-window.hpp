/* Copyright (c) 2013-2016, Stanislaw Halik <sthalik@misaki.pl>

 * Permission to use, copy, modify, and/or distribute this
 * software for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission
 * notice appear in all copies.
 */

#pragma once

#include "api/plugin-support.hpp"
#include "gui/mapping-dialog.hpp"
#include "gui/settings.hpp"
#include "gui/process_detector.h"
#include "logic/main-settings.hpp"
#include "logic/pipeline.hpp"
#include "logic/shortcuts.h"
#include "logic/work.hpp"
#include "logic/state.hpp"
#include "options/options.hpp"

#include <QApplication>
#include <QMainWindow>
#include <QKeySequence>
#include <QShortcut>
#include <QPixmap>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QString>
#include <QMenu>
#include <QAction>
#include <QEvent>
#include <QCloseEvent>

#include <vector>
#include <tuple>
#include <memory>

#include "ui_main-window.h"

using namespace options;

class main_window : public QMainWindow, private State
{
    Q_OBJECT

    Ui::main_window ui;

    Shortcuts global_shortcuts;
    module_settings m;
    std::unique_ptr<QSystemTrayIcon> tray;
    QMenu tray_menu;
    QTimer pose_update_timer { this };
    QTimer det_timer;
    QTimer config_list_timer;
    std::unique_ptr<options_dialog> options_widget;
    std::unique_ptr<mapping_dialog> mapping_widget;
    QShortcut kbd_quit { QKeySequence("Ctrl+Q"), this };
    std::unique_ptr<IFilterDialog> pFilterDialog;
    std::unique_ptr<IProtocolDialog> pProtocolDialog;
    std::unique_ptr<ITrackerDialog> pTrackerDialog;

    process_detector_worker det;
    QMenu profile_menu;

    QAction menu_action_header   { &tray_menu },
            menu_action_show     { &tray_menu },
            menu_action_exit     { &tray_menu },
            menu_action_tracker  { &tray_menu },
            menu_action_filter   { &tray_menu },
            menu_action_proto    { &tray_menu },
            menu_action_options  { &tray_menu },
            menu_action_mappings { &tray_menu };

    std::shared_ptr<dylib> current_tracker()
    {
        return modules.trackers().value(ui.iconcomboTrackerSource->currentIndex(), nullptr);
    }
    std::shared_ptr<dylib> current_protocol()
    {
        return modules.protocols().value(ui.iconcomboProtocol->currentIndex(), nullptr);
    }
    std::shared_ptr<dylib> current_filter()
    {
        return modules.filters().value(ui.iconcomboFilter->currentIndex(), nullptr);
    }

    void update_button_state(bool running, bool inertialp);
    void display_pose(const double* mapped, const double* raw);
    void set_title(const QString& game_title = QString());
    static bool get_new_config_name_from_dialog(QString &ret);
    void set_profile_in_registry(const QString& profile);
    void register_shortcuts();
    void set_keys_enabled(bool flag);
    bool is_config_listed(const QString& name);

    void init_tray();

    void changeEvent(QEvent* e) override;
    bool event(QEvent *event) override;
    bool maybe_hide_to_tray(QEvent* e);
#if !defined _WIN32 && !defined __APPLE__
    void annoy_if_root();
#endif

    // only use in impl file since no definition in header!
    template<typename t>
    bool mk_dialog(std::shared_ptr<dylib> lib, std::unique_ptr<t>& d);

    // idem
    template<typename t, typename... Args>
    inline bool mk_window(std::unique_ptr<t>& place, Args&&... params);

    // idem
    template<typename t, typename F>
    bool mk_window_common(std::unique_ptr<t>& d, F&& ctor);

    void closeEvent(QCloseEvent *event) override;

private slots:
    void save_modules();
    void exit(int status = EXIT_SUCCESS);
    bool set_profile(const QString& new_name);

    void show_tracker_settings();
    void show_proto_settings();
    void show_filter_settings();
    void show_options_dialog();
    void show_mapping_window();
    void show_pose();

    void maybe_start_profile_from_executable();

    void make_empty_config();
    void make_copied_config();
    void open_config_directory();
    bool refresh_config_list();

    void start_tracker_();
    void stop_tracker_();

    void ensure_tray();

    void toggle_restore_from_tray(QSystemTrayIcon::ActivationReason e);

signals:
    void start_tracker();
    void stop_tracker();
    void toggle_tracker();
    void restart_tracker();
public:
    main_window();
    ~main_window();
    static void set_working_directory();
    bool maybe_die_on_config_not_writable(const QString& current, QStringList* ini_list);
    void die_on_config_not_writable();
    bool is_tray_enabled();
    bool start_in_tray();
};
