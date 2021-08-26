#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include "spline.h"
#include "qcustomplot.h"
#include "point.h"

class MainWindow;
class GLWidget;
class QCustomPlot;
class Population;


class Window : public QWidget
{
    Q_OBJECT
public:
    Window(MainWindow *mw);

    void showPlot();

    Spline getSpl() const;

    void setSpl(const Spline &value);

    QCustomPlot *getPlot() const;

    void convexe();

public slots:
    void updateErrors(double xErr, double yErr, double fitness);

private slots:
    void clickedGraph(QMouseEvent *event);
    void releasedGraph(QMouseEvent *event);
    void doubleClickedGraph(QMouseEvent *event);
    void clickMove(QMouseEvent *event);
    void bListChanged(QTableWidgetItem *item);
    void pointsChanged(QVector<Point> points);
    void listUpdated(QTableWidgetItem *item);
    void updateTitleLabels();
    void evaluatePressed();
    void optimisePressed();
    void calcTimePressed();

private:
    MainWindow *mainWindow;
    GLWidget *glWidget;
    QCustomPlot *plot;
    QVector<QCPItemText*> ti;
    double const tiYoffset = -0.03;
    QVector<double> vec_x, vec_y;
    QVector<int> selectedKnots;
    double selectedX;
    QTableWidget *bTableWidget, *tableWidget;
    QSpinBox *degreeBox, *populationBox, *knotBox;
    Spline spl;
    QSlider *bSmoothnessSlider, *pointScaleSlider;
    QPushButton *rescale, *adapt, *calcTime;
    QCheckBox *showLinesBtnCTRL, *showLinesBtnPASS;

    QLabel *controlPointsLabel, *knotsLabel;

    QRadioButton *cRadio, *pRadio;

    QPushButton *evalBtn, *optBtn2003, *optBtnNew;

    QLabel *errXLabel, *errYLabel, *fitnessLabel;
    double theoreticalMinErrX, theoreticalMinErrY, theoreticalMinFitness;

    QVector<double> tp;
    const double EPS = 0.0000001;
    const double EPS2 = 0.01;

    void movePoint(double x);
    void updateBList();
    void addToVecXAndSort(double x);
    void updateTi();
    void calculateErrors();
    void updatePopulation(const Population &pop);
};

#endif // WINDOW_H
