#ifndef RADAR_CONFIG_LOADER_H
#define RADAR_CONFIG_LOADER_H

#include "radarDataModel.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

class RadarConfigLoader
{
public:
    static ExperimentConfig loadConfig(const QString &xmlPath)
    {
        ExperimentConfig config;
        QFile file(xmlPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Error opening:" << xmlPath;
            return config;
        }

        QXmlStreamReader xml(&file);
        while (!xml.atEnd() && !xml.hasError()) {
            if (xml.readNext() != QXmlStreamReader::StartElement) continue;

            if (xml.name() == QStringLiteral("Experiment")) {
                QXmlStreamAttributes attrs = xml.attributes();
                config.id               = attrs.value("id").toInt();
                config.name             = attrs.value("name").toString();
                config.fixedBottomImage = attrs.value("bottomImagePath").toString();
                config.expType          = attrs.value("type").toString();
                config.txCount          = attrs.value("txCount").toInt();
            }
            else if (xml.name() == QStringLiteral("GroupRadar"))
                parseGroup(xml, config.radarGroup, "Component");
            else if (xml.name() == QStringLiteral("GroupPrinciple"))
                parseGroup(xml, config.principleGroup, "Component");
            else if (xml.name() == QStringLiteral("GroupStep"))
                parseGroup(xml, config.stepGroup, "Step");
            else if (xml.name() == QStringLiteral("GroupCourse"))
                parseGroup(xml, config.courseGroup, "Course");
        }
        return config;
    }

private:
    static void parseGroup(QXmlStreamReader &xml, GroupData &groupData, const QString &type)
    {
        QXmlStreamAttributes attrs = xml.attributes();
        if (attrs.hasAttribute("visible"))
            groupData.isVisible = (attrs.value("visible").toString() != "false");

        QString groupTagName = xml.name().toString();
        while (!(xml.tokenType() == QXmlStreamReader::EndElement
                 && xml.name() == groupTagName)) {
            xml.readNext();
            if (xml.tokenType() != QXmlStreamReader::StartElement || xml.name() != QStringLiteral("Item")) continue;

            ExperimentContentItem item;
            QXmlStreamAttributes itemAttrs = xml.attributes();
            item.id         = itemAttrs.value("id").toString();
            item.title      = itemAttrs.value("title").toString();
            item.moduleType = type;
            item.txBit      = itemAttrs.value("txBit").toString();
            item.waveform   = itemAttrs.value("waveform").toString();  // 新增
            item.special    = itemAttrs.value("special").toString();   // 新增

            // 解析 Item 内部子元素
            while (!(xml.tokenType() == QXmlStreamReader::EndElement
                     && xml.name() == QStringLiteral("Item"))) {
                xml.readNext();
                if (xml.tokenType() != QXmlStreamReader::StartElement) continue;

                if (xml.name() == QStringLiteral("Image"))
                    item.imagePath = xml.readElementText();

                else if (xml.name() == QStringLiteral("Description"))
                    item.description = xml.readElementText();

                else
                    xml.skipCurrentElement();
            }
            groupData.items.append(item);
        }
    }
};

#endif // RADAR_CONFIG_LOADER_H
