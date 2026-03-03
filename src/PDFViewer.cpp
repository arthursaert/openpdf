#include "PDFViewer.h"
#include "PDFPageWidget.h"
#include <poppler-qt6.h>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QWheelEvent>
#include <QKeyEvent>

PDFViewer::PDFViewer(QWidget *parent)
    : QWidget(parent)
    , m_currentPage(0)
    , m_totalPages(0)
    , m_renderTimer(new QTimer(this))
    , m_annotationMode(PDFPageWidget::ModeSelect)
    , m_zoomFactor(1.0)
{
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setAlignment(Qt::AlignHCenter);

    m_pagesContainer = new QWidget;
    m_pagesLayout = new QVBoxLayout(m_pagesContainer);
    m_pagesLayout->setAlignment(Qt::AlignHCenter);
    m_pagesLayout->setSpacing(10);

    m_scrollArea->setWidget(m_pagesContainer);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_scrollArea);

    connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &PDFViewer::onScrollValueChanged);

    m_renderTimer->setInterval(200);
    connect(m_renderTimer, &QTimer::timeout, this, &PDFViewer::processNextPage);
}

PDFViewer::~PDFViewer()
{
    qDeleteAll(m_allAnnotations);
}

void PDFViewer::setAnnotationMode(PDFPageWidget::Mode mode)
{
    m_annotationMode = mode;
    for (PDFPageWidget *page : m_pages) {
        page->setMode(mode);
    }
    emit annotationModeChanged(mode);
}

void PDFViewer::loadDocument(const QString &fileName)
{
    clearPages();
    m_renderTimer->stop();
    m_pagesToRender.clear();
    m_currentFile = fileName;

    m_document = Poppler::Document::load(fileName);
    if (!m_document || m_document->isLocked()) {
        QMessageBox::warning(this, "Erro", "Não foi possível abrir o PDF ou o arquivo está protegido.");
        return;
    }

    m_document->setRenderHint(Poppler::Document::Antialiasing);
    m_document->setRenderHint(Poppler::Document::TextAntialiasing);

    m_totalPages = m_document->numPages();
    emit pageChanged(0, m_totalPages);

    loadAnnotations();

    for (int i = 0; i < m_totalPages; ++i) {
        m_pagesToRender.enqueue(i);
    }

    startRendering();
}

void PDFViewer::clearPages()
{
    qDeleteAll(m_pages);
    m_pages.clear();
}

void PDFViewer::startRendering()
{
    if (!m_pagesToRender.isEmpty()) {
        m_renderTimer->start();
    }
}

void PDFViewer::processNextPage()
{
    if (m_pagesToRender.isEmpty()) {
        m_renderTimer->stop();
        return;
    }

    int pageIndex = m_pagesToRender.dequeue();
    std::unique_ptr<Poppler::Page> page(m_document->page(pageIndex));
    if (!page) return;

    QImage image = page->renderToImage(72, 72);
    if (!image.isNull()) {
        auto links = page->links();
        PDFPageWidget *pageWidget = new PDFPageWidget(
            image, pageIndex + 1, std::move(links), m_pagesContainer);
        pageWidget->setMode(m_annotationMode);
        pageWidget->setAnnotations(m_annotationsByPage.value(pageIndex + 1));

        connect(pageWidget, &PDFPageWidget::annotationCreated,
                this, &PDFViewer::onAnnotationCreated);
        connect(pageWidget, &PDFPageWidget::annotationClicked,
                this, &PDFViewer::onAnnotationClicked);
        connect(pageWidget, &PDFPageWidget::annotationRemoved,
                this, &PDFViewer::onAnnotationRemoved);
        connect(pageWidget, &PDFPageWidget::textSelected,
                this, &PDFViewer::onTextSelected);

        m_pagesLayout->addWidget(pageWidget);
        m_pages.append(pageWidget);
    }

    if (m_pages.size() == 1) {
        onScrollValueChanged(0);
    }
}

void PDFViewer::onScrollValueChanged(int value)
{
    Q_UNUSED(value);
    int newPage = currentPageFromScroll();
    if (newPage != m_currentPage) {
        m_currentPage = newPage;
        emit pageChanged(m_currentPage + 1, m_totalPages);
    }
}

int PDFViewer::currentPageFromScroll() const
{
    if (m_pages.isEmpty()) return 0;

    int scrollPos = m_scrollArea->verticalScrollBar()->value();
    int accumulated = 0;
    for (int i = 0; i < m_pages.size(); ++i) {
        int pageHeight = m_pages[i]->height() + m_pagesLayout->spacing();
        if (scrollPos < accumulated + pageHeight / 2)
            return i;
        accumulated += pageHeight;
    }
    return m_pages.size() - 1;
}

void PDFViewer::goToPage(int page)
{
    if (page < 1 || page > m_totalPages) return;
    int index = page - 1;

    if (index >= m_pages.size()) {
        int yPos = 0;
        for (int i = 0; i < m_pages.size(); ++i) {
            yPos += m_pages[i]->height() + m_pagesLayout->spacing();
        }
        m_scrollArea->verticalScrollBar()->setValue(yPos);
    } else {
        int yPos = 0;
        for (int i = 0; i < index; ++i) {
            yPos += m_pages[i]->height() + m_pagesLayout->spacing();
        }
        m_scrollArea->verticalScrollBar()->setValue(yPos);
    }
}

QString PDFViewer::annotationFilePath() const
{
    if (m_currentFile.isEmpty()) return QString();
    QFileInfo info(m_currentFile);
    return info.absolutePath() + "/" + info.completeBaseName() + ".annot";
}

void PDFViewer::saveAnnotations()
{
    QString path = annotationFilePath();
    if (path.isEmpty()) return;

    QJsonArray annArray;
    for (Annotation *ann : m_allAnnotations) {
        annArray.append(ann->toJson());
    }

    QJsonDocument doc(annArray);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Não foi possível salvar anotações em" << path;
        return;
    }
    file.write(doc.toJson());
}

void PDFViewer::loadAnnotations()
{
    QString path = annotationFilePath();
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) return;

    qDeleteAll(m_allAnnotations);
    m_allAnnotations.clear();
    m_annotationsByPage.clear();

    for (const QJsonValue &val : doc.array()) {
        if (!val.isObject()) continue;
        Annotation *ann = new Annotation(val.toObject());
        m_allAnnotations.append(ann);
        m_annotationsByPage[ann->page()].append(ann);
    }

    for (PDFPageWidget *page : m_pages) {
        page->setAnnotations(m_annotationsByPage.value(page->pageNumber()));
    }
}

void PDFViewer::onAnnotationCreated(Annotation *annotation)
{
    m_allAnnotations.append(annotation);
    m_annotationsByPage[annotation->page()].append(annotation);

    for (PDFPageWidget *page : m_pages) {
        if (page->pageNumber() == annotation->page()) {
            page->addAnnotation(annotation);
            break;
        }
    }

    saveAnnotations();
}

void PDFViewer::onAnnotationClicked(Annotation *annotation)
{
    if (annotation->type() == Annotation::Note && !annotation->text().isEmpty()) {
        QMessageBox::information(this, "Nota", annotation->text());
    }
}

void PDFViewer::onAnnotationRemoved(Annotation *annotation)
{
    if (m_allAnnotations.removeOne(annotation)) {
        int page = annotation->page();
        m_annotationsByPage[page].removeOne(annotation);

        for (PDFPageWidget *pageWidget : m_pages) {
            if (pageWidget->pageNumber() == page) {
                pageWidget->removeAnnotation(annotation);
                break;
            }
        }

        delete annotation;
        saveAnnotations();
    }
}

void PDFViewer::onTextSelected(int pageNumber, QRectF normalizedRect)
{
    if (!m_document) return;

    std::unique_ptr<Poppler::Page> page(m_document->page(pageNumber - 1));
    if (!page) return;

    QSizeF pageSize = page->pageSize();
    QRectF rectPoints(
        normalizedRect.x() * pageSize.width(),
        normalizedRect.y() * pageSize.height(),
        normalizedRect.width() * pageSize.width(),
        normalizedRect.height() * pageSize.height()
    );

    QString text = page->text(rectPoints);
    if (!text.isEmpty()) {
        QApplication::clipboard()->setText(text);
    }
}

// ====================== ZOOM CORRIGIDO ======================

void PDFViewer::updateZoom()
{
    if (m_pages.isEmpty()) return;

    // 1. Descobrir a página atual e a posição do scroll dentro dela
    QScrollBar *vbar = m_scrollArea->verticalScrollBar();
    int currentScroll = vbar->value();

    int pageIndex = 0;
    int accumulated = 0;
    for (int i = 0; i < m_pages.size(); ++i) {
        int pageHeight = m_pages[i]->height() + m_pagesLayout->spacing();
        if (currentScroll < accumulated + pageHeight) {
            pageIndex = i;
            break;
        }
        accumulated += pageHeight;
    }

    // 2. Calcular o offset dentro da página (em pixels)
    int topOfPage = accumulated;
    int offsetInPage = currentScroll - topOfPage;
    if (offsetInPage < 0) offsetInPage = 0;
    int pageHeightBefore = m_pages[pageIndex]->height();

    // 3. Salvar a proporção (0 a 1)
    double proportion = (pageHeightBefore > 0) ? (double)offsetInPage / pageHeightBefore : 0.0;

    // 4. Aplicar o zoom a todas as páginas
    for (PDFPageWidget *page : m_pages) {
        page->setScale(m_zoomFactor);
    }

    // 5. Forçar a atualização do layout (os tamanhos mudaram)
    m_pagesContainer->adjustSize();
    m_pagesLayout->invalidate();
    QApplication::processEvents();

    // 6. Recalcular a posição após o zoom
    accumulated = 0;
    for (int i = 0; i < pageIndex; ++i) {
        accumulated += m_pages[i]->height() + m_pagesLayout->spacing();
    }
    int pageHeightAfter = m_pages[pageIndex]->height();
    int newOffsetInPage = static_cast<int>(proportion * pageHeightAfter);
    int newScroll = accumulated + newOffsetInPage;

    vbar->setValue(newScroll);
}

void PDFViewer::zoomIn()
{
    m_zoomFactor *= 1.2;
    if (m_zoomFactor > 5.0) m_zoomFactor = 5.0;
    updateZoom();
}

void PDFViewer::zoomOut()
{
    m_zoomFactor /= 1.2;
    if (m_zoomFactor < 0.2) m_zoomFactor = 0.2;
    updateZoom();
}

void PDFViewer::resetZoom()
{
    m_zoomFactor = 1.0;
    updateZoom();
}

void PDFViewer::wheelEvent(QWheelEvent *event)
{
    QWidget::wheelEvent(event);
}

void PDFViewer::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
            zoomIn();
            event->accept();
            return;
        } else if (event->key() == Qt::Key_Minus) {
            zoomOut();
            event->accept();
            return;
        } else if (event->key() == Qt::Key_0) {
            resetZoom();
            event->accept();
            return;
        }
    }
    QWidget::keyPressEvent(event);
}

void PDFViewer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}