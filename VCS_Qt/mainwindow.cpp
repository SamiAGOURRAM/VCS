#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vcs.h"
#include <QInputDialog>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    addButton = new QPushButton("Add", this);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addButtonClicked);
    addButton->setGeometry(300, 200, 80, 30);
    initButton = new QPushButton("Init", this);
    connect(initButton, &QPushButton::clicked, this, &MainWindow::initButtonClicked);
    initButton->setGeometry(400, 200, 80, 30);
    commitButton = new QPushButton("Commit", this);
    connect(commitButton, &QPushButton::clicked, this, &MainWindow::commitButtonClicked);
    commitButton->setGeometry(250, 250, 80, 30);
    revertButton = new QPushButton("Revert", this);
    connect(revertButton, &QPushButton::clicked, this, &MainWindow::revertButtonClicked);
    revertButton->setGeometry(350, 250, 80, 30);
    logButton = new QPushButton("Log", this);
    connect(logButton, &QPushButton::clicked, this, &MainWindow::logButtonClicked);
    logButton->setGeometry(450, 250, 80, 30);
    // Create the QLineEdit for displaying the selected directory path
    directoryLineEdit = new QLineEdit(this);
    directoryLineEdit->setGeometry(240, 120, 310, 30);
    directoryLineEdit->setReadOnly(true);  // Make it read-only

    // Your other setup code goes here
    vcs = VCS();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::browseButtonClicked()
{

}

void MainWindow::on_pushButton_6_clicked()
{
    // Open a file dialog to let the user choose a directory
    QString directory = QFileDialog::getExistingDirectory(this, "Select Directory", QDir::homePath());
    gener = directory ;
    if (!directory.isEmpty()) {
        // Set the text of the QLineEdit to the selected directory path
        directoryLineEdit->setText(directory);
    }
}
void MainWindow::addButtonClicked()
{

    // Get user input for the file to be added
    QString qFileName = QInputDialog::getText(this, "Add File", "Enter the file name:");

    // Convert QString to std::string
    std::string fileName = qFileName.toStdString();

    // Check if the user pressed Cancel or entered an empty string
    if (fileName.empty()) {
        qDebug() << "Add operation canceled or no file name provided.";
        return;
    }
    std::cout << fileName << std::endl;
    // Call the add function with the provided file name
    vcs.add(fileName);
    qDebug() << "Git add successful for file: " << fileName.c_str();
}
void MainWindow::commitButtonClicked()
{

    // Get user input for the commit message
    QString qCommitMessage = QInputDialog::getText(this, "Commit", "Enter the commit message:");

    // Convert QString to std::string
    std::string commitMessage = qCommitMessage.toStdString();

    // Check if the user pressed Cancel or entered an empty string
    if (commitMessage.empty()) {
        qDebug() << "Commit operation canceled or no commit message provided.";
        return;
    }

    // Call the commit function with the provided commit message
    vcs.commit(commitMessage);
    qDebug() << "Git commit successful with message: " << commitMessage.c_str();
}
void MainWindow::revertButtonClicked()
{
    // Get user input for the revert message
    QString qRevertMessage = QInputDialog::getText(this, "Revert", "Enter the commit id:");

    // Convert QString to std::string
    std::string revertMessage = qRevertMessage.toStdString();

    // Check if the user pressed Cancel or entered an empty string
    if (revertMessage.empty()) {
        qDebug() << "Revert operation canceled or no revert message provided.";
        return;
    }

    // Call the revert function with the provided revert message
    vcs.revert(revertMessage);
    qDebug() << "Git revert successful with message: " << revertMessage.c_str();
}
void MainWindow::logButtonClicked()
{
    // Call the log function of the VCS class
    vcs.log();
}

void MainWindow::initButtonClicked()
{
    if (!gener.isEmpty()) {
        std::string geni = gener.toStdString();
        vcs.init(geni);
        qDebug() << "Git init successful";
    }
}

