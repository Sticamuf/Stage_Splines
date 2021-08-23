#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "window.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Window *window;
    QString currentFile = "";

private slots:
//    void onNouveau();
    void onEnregistrerCTRLsous();
    void onEnregistrerPASSsous();
    void onEnregistrerSPLNsous();
    void onOuvrir();
    void onOuvrirNoeuds();
    void onEnregistrerNoeudsSous();
    void onExporterSVG();
};
#endif // MAINWINDOW_H
