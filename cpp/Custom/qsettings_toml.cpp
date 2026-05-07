//
// Created by nexie on 10.02.2026.
//

#include "qsettings_toml.hpp"

#include <QIODevice>
#include <QDebug>

#include "third_party/tomlplusplus/toml.hpp"


using namespace qt_ext;

bool qsettings_toml_read(QIODevice& device, QSettings::SettingsMap& map)
{
    QByteArray ba = device.readAll();
    toml::parse_result parsed;

    try
    {
        parsed = toml::parse(ba.data());
    }catch (std::exception& e)
    {
        qCritical() << e.what();
        return false;
    }

    if (!parsed.is_table())
    {
        return false;
    }

    

    return true;
}