#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "PDFPageWidget.h"

class QAction;
class QLabel;
class QLineEdit;
class QActionGroup;
class PDFViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void updatePageInfo(int currentPage, int totalPages);
    void goToPage();
    void about();
    void setAnnotationMode(QAction *action);

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createAnnotationToolBar();

    PDFViewer *m_viewer;
    QAction *openAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *aboutQtAction;

    QLabel *pageLabel;
    QLineEdit *pageEdit;
    QLabel *totalPagesLabel;

    QActionGroup *m_annotationModeGroup;
    QAction *m_selectModeAction;
    QAction *m_highlightModeAction;
    QAction *m_noteModeAction;
};

#endif