#pragma once
class QWindow;
class BlurBehindHelper
{
public:
    static bool applyBlur(QWindow * target);
    static bool disableBlur(QWindow * target);
};
