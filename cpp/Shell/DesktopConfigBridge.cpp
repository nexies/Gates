#include "DesktopConfigBridge.h"
#include "cpp/Config/ConfigManager.h"

namespace Gates {

DesktopConfigBridge * DesktopConfigBridge::_instance = nullptr;

DesktopConfigBridge & DesktopConfigBridge::instance()
{
    if (!_instance)
        _instance = new DesktopConfigBridge();
    return *_instance;
}

DesktopConfigBridge::DesktopConfigBridge(QObject * parent) : QObject(parent)
{
    // configChanged covers external file edits; desktopLayoutChanged covers live settings changes.
    connect(&ConfigManager::instance(), &ConfigManager::configChanged,
            this, &DesktopConfigBridge::layoutChanged);
    connect(&ConfigManager::instance(), &ConfigManager::desktopLayoutChanged,
            this, &DesktopConfigBridge::layoutChanged);
}

int  DesktopConfigBridge::cellW()         const { return ConfigManager::instance().config().desktopLayout.cellW;  }
int  DesktopConfigBridge::cellH()         const { return ConfigManager::instance().config().desktopLayout.cellH;  }
int  DesktopConfigBridge::gapX()          const { return ConfigManager::instance().config().desktopLayout.gapX;   }
int  DesktopConfigBridge::gapY()          const { return ConfigManager::instance().config().desktopLayout.gapY;   }
int  DesktopConfigBridge::margin()        const { return ConfigManager::instance().config().desktopLayout.margin; }
int  DesktopConfigBridge::stepX()         const { return ConfigManager::instance().config().desktopLayout.stepX(); }
int  DesktopConfigBridge::stepY()         const { return ConfigManager::instance().config().desktopLayout.stepY(); }
bool DesktopConfigBridge::showDebugGrid()    const { return ConfigManager::instance().config().desktopLayout.showDebugGrid;    }
bool DesktopConfigBridge::snapFramesToGrid() const { return ConfigManager::instance().config().desktopLayout.snapFramesToGrid; }

} // namespace Gates
