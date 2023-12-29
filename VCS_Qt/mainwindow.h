#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QPalette>
#include <QDebug>
#include <QPushButton>
#include <QFileDialog>
#include <QLineEdit>
#include "vcs.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void browseButtonClicked();
    void addButtonClicked();
    void on_pushButton_6_clicked();
    void initButtonClicked();
    void commitButtonClicked();
    void revertButtonClicked();
    void logButtonClicked();


private:
    Ui::MainWindow *ui;
    QPushButton *addButton;
    QPushButton *initButton;
    QPushButton *commitButton;
    QPushButton *browseButton;
    QPushButton *revertButton;
    QPushButton *logButton;
    QLineEdit *directoryLineEdit;
    QString gener;
    VCS vcs;
};

#endif // MAINWINDOW_H
