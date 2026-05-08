#include "SelectionService.h"

SelectionService * SelectionService::_inst = nullptr;

SelectionService & SelectionService::instance()
{
    if (!_inst) _inst = new SelectionService();
    return *_inst;
}

void SelectionService::select(const QString & path)
{
    if (_path == path) return;
    _path = path;
    emit selectedPathChanged();
}

void SelectionService::deselect()
{
    if (_path.isEmpty()) return;
    _path.clear();
    emit selectedPathChanged();
}
