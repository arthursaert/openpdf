#include "PDFPageWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>
#include <QCursor>
#include <QInputDialog>

PDFPageWidget::PDFPageWidget(const QImage &image, int pageNumber,
                             std::vector<std::unique_ptr<Poppler::Link>> links,
                             QWidget *parent)
    : QWidget(parent)
    , m_originalImage(image)
    , m_pageNumber(pageNumber)
    , m_scale(1.0)
    , m_links(std::move(links))
    , m_mode(ModeSelect)
    , m_drawing(false)
    , m_currentRectPixels()
    , m_selectionRectPixels()
    , m_selectionVisible(false)
{
    setMouseTracking(true);
    setFixedSize(image.size());
}

PDFPageWidget::~PDFPageWidget() {}

void PDFPageWidget::setScale(double scale)
{
    if (scale == m_scale) return;
    m_scale = scale;
    setFixedSize(m_originalImage.size() * scale);
    update();
}

void PDFPageWidget::setAnnotations(const QList<Annotation*>& annotations)
{
    m_annotations = annotations;
    update();
}

void PDFPageWidget::addAnnotation(Annotation* annotation)
{
    m_annotations.append(annotation);
    update();
}

void PDFPageWidget::removeAnnotation(Annotation* annotation)
{
    m_annotations.removeOne(annotation);
    update();
}

QPointF PDFPageWidget::widgetToPage(const QPoint &pos) const
{
    qreal x = pos.x() / (qreal)width();
    qreal y = pos.y() / (qreal)height();
    return QPointF(qBound(0.0, x, 1.0), qBound(0.0, y, 1.0));
}

void PDFPageWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Desenhar a imagem do PDF
    QImage scaled = m_originalImage.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter.drawImage(0, 0, scaled);

    // Desenhar anotações (destaques e notas)
    for (Annotation *ann : m_annotations) {
        QRectF rect(ann->rect().x() * width(),
                    ann->rect().y() * height(),
                    ann->rect().width() * width(),
                    ann->rect().height() * height());

        if (ann->type() == Annotation::Highlight) {
            painter.setBrush(QColor(ann->color().red(), ann->color().green(), ann->color().blue(), 80));
            painter.setPen(Qt::NoPen);
            painter.drawRect(rect);
        } else if (ann->type() == Annotation::Note) {
            painter.setBrush(QColor(255, 255, 0, 200));
            painter.setPen(Qt::black);
            painter.drawEllipse(rect.topLeft(), 10, 10);
            painter.drawText(rect.topLeft() + QPointF(5, 5), "!");
        }
    }

    // Desenhar retângulo temporário durante o arrasto (para destaque ou seleção)
    if (m_drawing && !m_currentRectPixels.isNull()) {
        if (m_mode == ModeHighlight) {
            painter.setBrush(QColor(255, 255, 0, 50));
            painter.setPen(Qt::yellow);
        } else if (m_mode == ModeSelect) {
            painter.setBrush(QColor(0, 120, 215, 50));  // azul claro semitransparente
            painter.setPen(Qt::blue);
        }
        painter.drawRect(m_currentRectPixels);
    }

    // Desenhar a seleção ativa (se houver e o modo for seleção)
    if (m_mode == ModeSelect && m_selectionVisible && !m_selectionRectPixels.isNull()) {
        painter.setBrush(QColor(0, 120, 215, 80));  // azul um pouco mais forte
        painter.setPen(Qt::blue);
        painter.drawRect(m_selectionRectPixels);
    }
}

void PDFPageWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF pagePoint = widgetToPage(event->pos());

        // Verificar se clicou em anotação
        for (Annotation *ann : m_annotations) {
            if (ann->rect().contains(pagePoint)) {
                emit annotationClicked(ann);
                return;
            }
        }

        // Verificar links
        for (const auto &linkPtr : m_links) {
            if (linkPtr->linkArea().contains(pagePoint)) {
                if (linkPtr->linkType() == Poppler::Link::Browse) {
                    Poppler::LinkBrowse *browse = static_cast<Poppler::LinkBrowse*>(linkPtr.get());
                    QString url = browse->url();
                    if (!url.isEmpty())
                        QDesktopServices::openUrl(QUrl(url));
                }
                return;
            }
        }

        // Iniciar ação conforme o modo
        if (m_mode == ModeHighlight) {
            m_drawing = true;
            m_dragStart = event->pos();
            m_currentRectPixels = QRect();
            // Se houver uma seleção ativa, esconder
            m_selectionVisible = false;
            update();
        } else if (m_mode == ModeNote) {
            QString text = QInputDialog::getMultiLineText(this, "Nova Nota", "Texto da nota:");
            if (!text.isEmpty()) {
                Annotation *ann = new Annotation(Annotation::Note, m_pageNumber,
                                                 QRectF(pagePoint.x(), pagePoint.y(), 0.02, 0.02),
                                                 QColor(Qt::yellow), text);
                emit annotationCreated(ann);
            }
        } else if (m_mode == ModeSelect) {
            m_drawing = true;
            m_dragStart = event->pos();
            m_currentRectPixels = QRect();
            // Esconder seleção anterior enquanto arrasta
            m_selectionVisible = false;
            update();
        }
    } else if (event->button() == Qt::RightButton) {
        QPointF pagePoint = widgetToPage(event->pos());
        // Verificar se clicou em alguma anotação para remover
        for (Annotation *ann : m_annotations) {
            if (ann->rect().contains(pagePoint)) {
                emit annotationRemoved(ann);
                return;
            }
        }
        // Se não, copiar imagem
        QApplication::clipboard()->setImage(m_originalImage);
    }
}

void PDFPageWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pagePoint = widgetToPage(event->pos());

    bool overLink = false;
    for (const auto &linkPtr : m_links) {
        if (linkPtr->linkArea().contains(pagePoint)) {
            overLink = true;
            break;
        }
    }
    for (Annotation *ann : m_annotations) {
        if (ann->rect().contains(pagePoint)) {
            overLink = true;
            break;
        }
    }
    setCursor(overLink ? Qt::PointingHandCursor : Qt::ArrowCursor);

    if (m_drawing && (m_mode == ModeHighlight || m_mode == ModeSelect)) {
        m_currentRectPixels = QRect(m_dragStart, event->pos()).normalized();
        update();
    }
}

void PDFPageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_drawing && event->button() == Qt::LeftButton) {
        if (m_mode == ModeHighlight) {
            m_drawing = false;
            if (m_currentRectPixels.width() > 5 && m_currentRectPixels.height() > 5) {
                QRectF normRect(
                    (qreal)m_currentRectPixels.x() / width(),
                    (qreal)m_currentRectPixels.y() / height(),
                    (qreal)m_currentRectPixels.width() / width(),
                    (qreal)m_currentRectPixels.height() / height()
                );
                Annotation *ann = new Annotation(Annotation::Highlight, m_pageNumber,
                                                 normRect, QColor(Qt::yellow));
                emit annotationCreated(ann);
            }
            m_currentRectPixels = QRect();
            update();
        } else if (m_mode == ModeSelect) {
            m_drawing = false;
            if (m_currentRectPixels.width() > 5 && m_currentRectPixels.height() > 5) {
                // Salvar o retângulo da seleção para exibir
                m_selectionRectPixels = m_currentRectPixels;
                m_selectionVisible = true;

                QRectF normRect(
                    (qreal)m_currentRectPixels.x() / width(),
                    (qreal)m_currentRectPixels.y() / height(),
                    (qreal)m_currentRectPixels.width() / width(),
                    (qreal)m_currentRectPixels.height() / height()
                );
                emit textSelected(m_pageNumber, normRect);
            } else {
                m_selectionVisible = false;
            }
            m_currentRectPixels = QRect();
            update();
        }
    }
}

void PDFPageWidget::leaveEvent(QEvent *event)
{
    setCursor(Qt::ArrowCursor);
    QWidget::leaveEvent(event);
}