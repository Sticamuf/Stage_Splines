#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "window.h"
#include "QSvgGenerator"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    window = new Window(this);
    setCentralWidget(window);

    QMenuBar *menuBar = new QMenuBar;

    QMenu *menuFichier = menuBar->addMenu("Fichier");

//    QAction *nouveau = new QAction(menuFichier);
//    nouveau->setText("Nouveau");
//    menuFichier->addAction(nouveau);
//    connect(nouveau, &QAction::triggered, this, &MainWindow::onNouveau);

    QAction *ouvrir = new QAction(menuFichier);
    ouvrir->setText("Ouvrir");
    menuFichier->addAction(ouvrir);
    connect(ouvrir, &QAction::triggered, this, &MainWindow::onOuvrir);

    QAction *ouvrirNoeuds = new QAction(menuFichier);
    ouvrirNoeuds->setText("Ouvrir noeuds");
    menuFichier->addAction(ouvrirNoeuds);
    connect(ouvrirNoeuds, &QAction::triggered, this, &MainWindow::onOuvrirNoeuds);

    QAction *enregistrerCTRLsous = new QAction(menuFichier);
    enregistrerCTRLsous->setText("Enregistrer CTRL Sous");
    menuFichier->addAction(enregistrerCTRLsous);
    connect(enregistrerCTRLsous, &QAction::triggered, this, &MainWindow::onEnregistrerCTRLsous);

    QAction *enregistrerPASSsous = new QAction(menuFichier);
    enregistrerPASSsous->setText("Enregistrer PASS Sous");
    menuFichier->addAction(enregistrerPASSsous);
    connect(enregistrerPASSsous, &QAction::triggered, this, &MainWindow::onEnregistrerPASSsous);

    QAction *enregistrerSPLNsous = new QAction(menuFichier);
    enregistrerSPLNsous->setText("Enregistrer SPLN Sous");
    menuFichier->addAction(enregistrerSPLNsous);
    connect(enregistrerSPLNsous, &QAction::triggered, this, &MainWindow::onEnregistrerSPLNsous);

    QAction *enregistrerNoeudsSous = new QAction(menuFichier);
    enregistrerNoeudsSous->setText("Enregistrer noeuds Sous");
    menuFichier->addAction(enregistrerNoeudsSous);
    connect(enregistrerNoeudsSous, &QAction::triggered, this, &MainWindow::onEnregistrerNoeudsSous);

    QAction *exporterSVG = new QAction(menuFichier);
    exporterSVG->setText("Exporter SVG");
    menuFichier->addAction(exporterSVG);
    connect(exporterSVG, &QAction::triggered, this, &MainWindow::onExporterSVG);

//    QAction *quitter = new QAction(menuFichier);
//    quitter->setText("Quitter");
//    menuFichier->addAction(quitter);


//    QMenu *menuEdition = menuBar->addMenu("Edition");

//    QAction *annuler = new QAction(menuEdition);
//    annuler->setText("Annuler");
//    menuEdition->addAction(annuler);
//    QAction *retablir = new QAction(menuEdition);
//    retablir->setText("Rétablir");
//    menuEdition->addAction(retablir);

    setMenuBar(menuBar);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete window;
}

//void MainWindow::onNouveau()
//{
//    window = new Window(this);
//}

void MainWindow::onEnregistrerCTRLsous()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer sous");
    currentFile = fileName;
    window->getSpl().saveCTRLinFile(fileName.toStdString());
    std::string str = fileName.toStdString() + ".txt";
}

void MainWindow::onEnregistrerPASSsous()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer sous");
    currentFile = fileName;
    window->getSpl().savePASSinFile(fileName.toStdString());
    std::string str = fileName.toStdString() + ".txt";
}

void MainWindow::onEnregistrerSPLNsous()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer sous");
    currentFile = fileName;
    window->getSpl().saveSPLNinFileAsPASS(fileName.toStdString());
    std::string str = fileName.toStdString() + ".txt";
}

void MainWindow::onOuvrir()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Ouvrir le fichier");
    QFile file(fileName);
    currentFile = fileName;
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Attention", "N'a pas pu ouvrir le fichier");
        return;
    }
    //setWindowTitle(fileName);
    //QTextStream in(&file);
    Spline spl = Spline(fileName.toStdString());
    window->setSpl(spl);

    file.close();
}

void MainWindow::onOuvrirNoeuds()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Ouvrir le fichier");
    QFile file(fileName);
    currentFile = fileName;
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Attention", "N'a pas pu ouvrir le fichier");
        return;
    }

    Spline spl = Spline(window->getSpl());
    spl.setKnots(fileName);
    window->setSpl(spl);
    file.close();
}

void MainWindow::onEnregistrerNoeudsSous()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer sous");
    currentFile = fileName;
    window->getSpl().saveKnotsInFile(fileName.toStdString());
    std::string str = fileName.toStdString() + ".txt";
}

void MainWindow::onExporterSVG()
{
//    https://www.qcustomplot.com/index.php/support/forum/1677
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter vers");
    fileName += ".svg";
    QSvgGenerator svggenerator;
    svggenerator.setFileName(fileName);
    QCPPainter qcpPainter;
    qcpPainter.begin(&svggenerator);
    window->getPlot()->toPainter(&qcpPainter, window->getPlot()->width(), window->getPlot()->height());
    qcpPainter.end();

//    https://www.qtcentre.org/threads/38099-Exporting-QGLWidget-content-in-a-vector-graphics-format
//    "OpenGL outputs a raster (bitmap) and not a vector thus it is not possible to save the output of the GL pipeline to any vector format."
}
