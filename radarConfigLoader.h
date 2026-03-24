#ifndef RADAR_CONFIG_LOADER_H
#define RADAR_CONFIG_LOADER_H
#include "radarDataModel.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

class RadarConfigLoader
{
public:
    // 加载单个 XML 文件
    static ExperimentConfig loadConfig(const QString& xmlPath) {
        ExperimentConfig config;
        QFile file(xmlPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Error opening:" << xmlPath;
            return config;
        }

        QXmlStreamReader xml(&file);
        while (!xml.atEnd() && !xml.hasError()) {
            QXmlStreamReader::TokenType token = xml.readNext();
            if (token == QXmlStreamReader::StartElement) {
                if (xml.name() == QStringLiteral("Experiment")) {
                    QXmlStreamAttributes attrs = xml.attributes();
                    config.id = attrs.value("id").toInt();
                    config.name = attrs.value("name").toString();
                    config.fixedBottomImage = attrs.value("bottomImagePath").toString();
                    config.expType = attrs.value("type").toString();       // "demo"/"test"
                    config.txCount = attrs.value("txCount").toInt();       // 12/4/7...
                }
                // 解析四个组
                else if (xml.name() == QStringLiteral("GroupRadar")) {
                    parseGroup(xml, config.radarGroup, "Component");
                }
                else if (xml.name() == QStringLiteral("GroupPrinciple")) {
                    parseGroup(xml, config.principleGroup, "Component");
                }
                else if (xml.name() == QStringLiteral("GroupStep")) {
                    parseGroup(xml, config.stepGroup, "Step");

                }
                else if (xml.name() == QStringLiteral("GroupCourse")) {
                    parseGroup(xml, config.courseGroup, "Course");
                }
            }
        }
        return config;
    }

private:
    // 通用的解析组函数
    static void parseGroup(QXmlStreamReader& xml, GroupData& groupData, const QString& type) {
        // 1. 读取可见性
        QXmlStreamAttributes attrs = xml.attributes();
        if (attrs.hasAttribute("visible")) {
            groupData.isVisible = (attrs.value("visible").toString() != "false");
        }

        // 2. 解析组内的 Item
        QString groupTagName = xml.name().toString();
        while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == groupTagName)) {
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == QStringLiteral("Item")) {
                ExperimentContentItem item;
                QXmlStreamAttributes itemAttrs = xml.attributes();
                item.id = itemAttrs.value("id").toString();
                item.title = itemAttrs.value("title").toString();
                item.moduleType = type;
                item.txBit = itemAttrs.value("txBit").toString();  // "s1:1" 等

                // 解析 Item 内部
                while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("Item"))) {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement) {
                        if (xml.name() == QStringLiteral("Image"))
                            item.imagePath = xml.readElementText();
                        else if (xml.name() == QStringLiteral("Description"))
                            item.description = xml.readElementText();
                        else
                            xml.skipCurrentElement();
                    }
                }
                groupData.items.append(item);
            }
        }
    }
};
#endif
