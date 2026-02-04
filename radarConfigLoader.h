#ifndef RADAR_CONFIG_LOADER_H
#define RADAR_CONFIG_LOADER_H
#include "radarDataModel.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

class RadarConfigLoader
{

public:
    // 加载整个配置文件
    static QMap<int, ExperimentRadarConfig> loadAllConfigs(const QString& xmlPath) {
        QMap<int, ExperimentRadarConfig> configs;
        QFile file(xmlPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "无法打开雷达配置文件:" << xmlPath;
            return configs;
        }

        QXmlStreamReader xml(&file);
        while (!xml.atEnd() && !xml.hasError()) {
            QXmlStreamReader::TokenType token = xml.readNext();
            if (token == QXmlStreamReader::StartElement && xml.name() == QStringLiteral("Experiment")) {
                parseExperiment(xml, configs);
            }
        }
        return configs;
    }

private:
    static void parseExperiment(QXmlStreamReader& xml, QMap<int, ExperimentRadarConfig>& configs) {
        QXmlStreamAttributes attrs = xml.attributes();
        int expId = attrs.value("id").toInt();

        ExperimentRadarConfig config;
        config.isVisible = (attrs.value("visible").toString() != "false");

        // 如果可见，继续解析子节点
        if (config.isVisible) {
            while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("Experiment"))) {
                if (xml.readNext() == QXmlStreamReader::StartElement && xml.name() == QStringLiteral("Component")) {
                    RadarComponentItem item;
                    QXmlStreamAttributes itemAttrs = xml.attributes();
                    item.id = itemAttrs.value("id").toString();
                    item.title = itemAttrs.value("title").toString();

                    // 解析内部标签 Image 和 Description
                    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("Component"))) {
                        xml.readNext();
                        if (xml.name() == QStringLiteral("Image"))
                            item.imagePath = xml.readElementText();
                        else if (xml.name() == QStringLiteral("Description"))
                            item.description = xml.readElementText();
                    }
                    config.items.append(item);
                }
            }
        } else {
            // 如果不可见，跳过直到结束标签
            xml.skipCurrentElement();
        }

        configs.insert(expId, config);
    }
};
#endif
