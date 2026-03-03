#ifndef PDFVIEWER_H
#define PDFVIEWER_H

#include <QWidget>
#include <QList>
#include <memory>
#include <QQueue>
#include <QMap>
#include "Annotation.h"
#include "PDFPageWidget.h"

class QScrollArea;
class QVBoxLayout;
class QTimer;

namespace Poppler {
    class Document;
}

class PDFViewer : public QWidget
{
    Q_OBJECT
public:
    explicit PDFViewer(QWidget *parent = nullptr);
    ~PDFViewer();

    void loadDocument(const QString &fileName);
    void goToPage(int page);

    void setAnnotationMode(PDFPageWidget::Mode mode);
    PDFPageWidget::Mode annotationMode() const { return m_annotationMode; }

    void saveAnnotations();
    void loadAnnotations();

    // Zoom
    void zoomIn();
    void zoomOut();
    void resetZoom();

signals:
    void pageChanged(int currentPage, int totalPages);
    void annotationModeChanged(PDFPageWidget::Mode mode);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onScrollValueChanged(int value);
    void processNextPage();
    void onAnnotationCreated(Annotation* annotation);
    void onAnnotationClicked(Annotation* annotation);
    void onAnnotationRemoved(Annotation* annotation);
    void onTextSelected(int pageNumber, QRectF normalizedRect);

private:
    void clearPages();
    void startRendering();
    int currentPageFromScroll() const;
    QString annotationFilePath() const;
    void updateZoom();

    std::unique_ptr<Poppler::Document> m_document;
    QScrollArea *m_scrollArea;
    QWidget *m_pagesContainer;
    QVBoxLayout *m_pagesLayout;
    QList<PDFPageWidget*> m_pages;
    int m_currentPage;
    int m_totalPages;

    QTimer *m_renderTimer;
    QQueue<int> m_pagesToRender;

    QList<Annotation*> m_allAnnotations;
    QMap<int, QList<Annotation*>> m_annotationsByPage;
    PDFPageWidget::Mode m_annotationMode;
    QString m_currentFile;

    double m_zoomFactor;
};

#endif