#ifndef PDFPAGEWIDGET_H
#define PDFPAGEWIDGET_H

#include <QWidget>
#include <QImage>
#include <memory>
#include <vector>
#include <poppler-qt6.h>
#include "Annotation.h"

class PDFPageWidget : public QWidget
{
    Q_OBJECT
public:
    enum Mode { ModeSelect, ModeHighlight, ModeNote };

    explicit PDFPageWidget(const QImage &image, int pageNumber,
                           std::vector<std::unique_ptr<Poppler::Link>> links,
                           QWidget *parent = nullptr);
    ~PDFPageWidget();

    void setScale(double scale);
    double scale() const { return m_scale; }
    int pageNumber() const { return m_pageNumber; }
    QImage image() const { return m_originalImage; }

    void setAnnotations(const QList<Annotation*>& annotations);
    void addAnnotation(Annotation* annotation);
    void removeAnnotation(Annotation* annotation);
    QList<Annotation*>& annotations() { return m_annotations; }

    void setMode(Mode mode) { m_mode = mode; }

signals:
    void annotationCreated(Annotation* annotation);
    void annotationClicked(Annotation* annotation);
    void annotationRemoved(Annotation* annotation);
    void textSelected(int pageNumber, QRectF normalizedRect);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QPointF widgetToPage(const QPoint &pos) const;

    QImage m_originalImage;
    int m_pageNumber;
    double m_scale;
    std::vector<std::unique_ptr<Poppler::Link>> m_links;
    QList<Annotation*> m_annotations;

    Mode m_mode;
    bool m_drawing;
    QPoint m_dragStart;
    QRect m_currentRectPixels;      // retângulo temporário durante arrasto
    QRect m_selectionRectPixels;     // retângulo da última seleção (para exibir)
    bool m_selectionVisible;         // se deve mostrar a seleção atual
};

#endif