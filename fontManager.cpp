#include "fontManager.h"
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>

namespace FontManager {

bool setupGlobalFont(const QString &fontPath, int pointSize)
{
    // 1. 加载字体文件
    int fontId = QFontDatabase::addApplicationFont(fontPath);
    if (fontId == -1) {
        qWarning() << "字体加载失败：" << fontPath;
        return false;
    }

    // 2. 获取字体家族名称（Family Name）
    QStringList fontfamilies = QFontDatabase::applicationFontFamilies(fontId);
    if (fontfamilies.isEmpty()) {
        qWarning() << "字体加载成功，但无法获取字体家族名";
        return false;
    }
    QString family = fontfamilies.first();
    qDebug() << "字体加载成功：" << family;

    // 3. 创建字体对象并设置为全局默认
    QFont globalFont(family);
    globalFont.setPointSize(pointSize);
    QApplication::setFont(globalFont);

    return true;
}

} // namespace FontManager
