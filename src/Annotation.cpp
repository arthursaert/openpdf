#include "Annotation.h"
#include <QJsonDocument>
#include <QJsonArray>

Annotation::Annotation(Type type, int page, const QRectF &rect, const QColor &color, const QString &text)
    : m_type(type)
    , m_page(page)
    , m_rect(rect)
    , m_color(color)
    , m_text(text)
    , m_created(QDateTime::currentDateTime())
{
}

Annotation::Annotation(const QJsonObject &json)
{
    m_type = static_cast<Type>(json["type"].toInt());
    m_page = json["page"].toInt();
    m_rect = QRectF(json["x"].toDouble(), json["y"].toDouble(),
                    json["width"].toDouble(), json["height"].toDouble());
    m_color = QColor(json["color"].toString());
    m_text = json["text"].toString();
    m_created = QDateTime::fromString(json["created"].toString(), Qt::ISODate);
}

QJsonObject Annotation::toJson() const
{
    QJsonObject obj;
    obj["type"] = static_cast<int>(m_type);
    obj["page"] = m_page;
    obj["x"] = m_rect.x();
    obj["y"] = m_rect.y();
    obj["width"] = m_rect.width();
    obj["height"] = m_rect.height();
    obj["color"] = m_color.name();
    obj["text"] = m_text;
    obj["created"] = m_created.toString(Qt::ISODate);
    return obj;
}