#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <QFont>

namespace FontManager {

/**
 * @brief 加载并设置全局默认字体（如 PingFang）
 * @param fontPath 字体资源路径
 * @param pointSize 默认字号（可选，默认 12）
 * @return 成功返回 true，否则 false
 */
bool setupGlobalFont(const QString &fontPath, int pointSize = 12);

} // namespace FontManager

#endif // FONTMANAGER_H
