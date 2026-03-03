#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QString>
#include <QColor>
#include <QJsonObject>
#include <QRectF>
#include <QDateTime>

class Annotation
{
public:
    enum Type { Highlight, Note };

    Annotation(Type type, int page, const QRectF &rect, const QColor &color, const QString &text = QString());
    explicit Annotation(const QJsonObject &json);

    QJsonObject toJson() const;

    Type type() const { return m_type; }
    int page() const { return m_page; }
    QRectF rect() const { return m_rect; }
    QColor color() const { return m_color; }
    QString text() const { return m_text; }
    void setText(const QString &text) { m_text = text; }
    QDateTime created() const { return m_created; }

    void setRect(const QRectF &rect) { m_rect = rect; }

private:
    Type m_type;
    int m_page;
    QRectF m_rect;          // Coordenadas normalizadas (0-1)
    QColor m_color;
    QString m_text;
    QDateTime m_created;
};

#endif