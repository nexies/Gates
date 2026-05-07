//
// Created by nexie on 10.02.2026.
//

#ifndef GATES_QSETTINGS_TOML_HPP
#define GATES_QSETTINGS_TOML_HPP

#include <QSettings>

namespace qt_ext
{
    bool qsettings_toml_read(QIODevice &device, QSettings::SettingsMap & map);
    bool qsettings_toml_write(QIODevice & device, QSettings::SettingsMap & map);

    static QSettings::Format QSettingsTomlFormat = QSettings::Format::InvalidFormat;
    void registerQSettingsToml();
}


#endif //GATES_QSETTINGS_TOML_HPP