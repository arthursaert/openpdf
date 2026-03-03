#include "MainWindow.h"
#include "PDFViewer.h"
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QApplication>
#include <QActionGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_viewer(nullptr)
    , pageLabel(nullptr)
    , pageEdit(nullptr)
    , totalPagesLabel(nullptr)
    , m_annotationModeGroup(nullptr)
{
    createActions();
    createMenus();
    createToolBar();
    createAnnotationToolBar();

    m_viewer = new PDFViewer(this);
    setCentralWidget(m_viewer);

    connect(m_viewer, &PDFViewer::pageChanged,
            this, &MainWindow::updatePageInfo);

    setWindowTitle("OpenPDF");
    resize(900, 700);
}

MainWindow::~MainWindow() {}

void MainWindow::createActions()
{
    openAction = new QAction("&Abrir...", this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    exitAction = new QAction("&Sair", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    aboutAction = new QAction("&Sobre", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

    aboutQtAction = new QAction("Sobre &Qt", this);
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);

    m_selectModeAction = new QAction("Selecionar", this);
    m_selectModeAction->setCheckable(true);
    m_selectModeAction->setChecked(true);

    m_highlightModeAction = new QAction("Destacar", this);
    m_highlightModeAction->setCheckable(true);

    m_noteModeAction = new QAction("Nota", this);
    m_noteModeAction->setCheckable(true);

    m_annotationModeGroup = new QActionGroup(this);
    m_annotationModeGroup->addAction(m_selectModeAction);
    m_annotationModeGroup->addAction(m_highlightModeAction);
    m_annotationModeGroup->addAction(m_noteModeAction);

    connect(m_annotationModeGroup, &QActionGroup::triggered,
            this, &MainWindow::setAnnotationMode);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("&Arquivo");
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu *helpMenu = menuBar()->addMenu("A&juda");
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBar()
{
    QToolBar *toolBar = addToolBar("Navegação");
    toolBar->setMovable(false);

    QLabel *prefixLabel = new QLabel("Página: ", this);
    pageLabel = new QLabel(this);
    pageEdit = new QLineEdit(this);
    pageEdit->setFixedWidth(50);
    pageEdit->setValidator(new QIntValidator(1, 1, this));
    totalPagesLabel = new QLabel(this);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    toolBar->addWidget(prefixLabel);
    toolBar->addWidget(pageLabel);
    toolBar->addWidget(pageEdit);
    toolBar->addWidget(totalPagesLabel);
    toolBar->addWidget(spacer);
    toolBar->addAction(openAction);

    connect(pageEdit, &QLineEdit::returnPressed, this, &MainWindow::goToPage);
    pageEdit->setEnabled(false);
}

void MainWindow::createAnnotationToolBar()
{
    QToolBar *annToolBar = addToolBar("Anotações");
    annToolBar->addAction(m_selectModeAction);
    annToolBar->addAction(m_highlightModeAction);
    annToolBar->addAction(m_noteModeAction);
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Abrir PDF", QString(), "Arquivos PDF (*.pdf)");
    if (!fileName.isEmpty()) {
        m_viewer->loadDocument(fileName);
        setWindowTitle("OpenPDF - " + fileName);
        pageEdit->setEnabled(true);
    }
}

void MainWindow::updatePageInfo(int currentPage, int totalPages)
{
    pageLabel->setText(QString::number(currentPage));
    totalPagesLabel->setText(" de " + QString::number(totalPages));
    pageEdit->setValidator(new QIntValidator(1, totalPages, this));
    pageEdit->setText(QString::number(currentPage));
}

void MainWindow::goToPage()
{
    if (!pageEdit->text().isEmpty()) {
        int page = pageEdit->text().toInt();
        m_viewer->goToPage(page);
    }
}

void MainWindow::setAnnotationMode(QAction *action)
{
    if (action == m_selectModeAction) {
        m_viewer->setAnnotationMode(PDFPageWidget::ModeSelect);
    } else if (action == m_highlightModeAction) {
        m_viewer->setAnnotationMode(PDFPageWidget::ModeHighlight);
    } else if (action == m_noteModeAction) {
        m_viewer->setAnnotationMode(PDFPageWidget::ModeNote);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, "Sobre OpenPDF",
        "OpenPDF é um leitor de PDF de código aberto feito para ler PDFs de forma simplificada.\n"
        "Versão 3.0 com suporte a anotações (destaques e notas).");
}