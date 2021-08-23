#include "window.h"
#include "glwidget.h"
#include "qcustomplot.h"

#include "interpolation.h"
#include "ap.h"
#include "alglibinternal.h"
#include "alglibmisc.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <string>
#include <vector>

#include "population.h"

Window::Window(MainWindow *mw) : mainWindow(mw), vec_x{}, vec_y{}, selectedKnots{}, spl(0,2,{})
{
    glWidget = new GLWidget(this);
    glWidget->setMouseTracking(true);
    QSizePolicy p = glWidget->sizePolicy();
    p.setHeightForWidth( true );
    glWidget->setSizePolicy( p );
    plot = new QCustomPlot;
    selectedKnots.resize(0);
    plot->addGraph();

    plot->yAxis->setRange(-0.07, 1);
    plot->xAxis->setRange(/*-0.05*/0, 1/*.05*/);

    plot->graph(0)->setScatterStyle(QCPScatterStyle::ScatterShape::ssPlus);
    plot->graph(0)->setLineStyle(QCPGraph::LineStyle::lsNone);

    //    plot->setInteraction(QCP::iRangeDrag, true);
    //    plot->axisRect()->setRangeDrag(Qt::Horizontal);
    plot->setInteraction(QCP::iRangeZoom, true);
    plot->axisRect()->setRangeZoom(Qt::Horizontal);



    QVBoxLayout *displayLayout = new QVBoxLayout;
    QVBoxLayout *manipulationLayout = new QVBoxLayout;
    QHBoxLayout *linesLayout = new QHBoxLayout;
    QHBoxLayout *cpButtonsLayout = new QHBoxLayout;
    QHBoxLayout *cpRadioButtonsLayout = new QHBoxLayout;
    QHBoxLayout *geneticLayout = new QHBoxLayout;
//    QHBoxLayout *bSplineButtonsLayout = new QHBoxLayout;
    QHBoxLayout *optBtnsLayout = new QHBoxLayout;
    QHBoxLayout *newBSplineLayout = new QHBoxLayout;
    QHBoxLayout *mainLayout = new QHBoxLayout;

    displayLayout->addWidget(glWidget);
    displayLayout->addWidget(plot);

    QWidget *w = new QWidget;
    w->setLayout(displayLayout);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(w);

    controlPointsLabel = new QLabel("Points de Contrôle :");
    controlPointsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    manipulationLayout->addWidget(controlPointsLabel);

    tableWidget = new QTableWidget;
    tableWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    tableWidget->setColumnCount(2);
    tableWidget->setRowCount(0);
    tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("x"));
    tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("y"));
    tableWidget->horizontalHeader()->setStyleSheet( "QHeaderView::section { border-bottom: 1px solid gray; }" );
    tableWidget->setVerticalHeaderItem(0, new QTableWidgetItem("c0"));
    manipulationLayout->addWidget(tableWidget);
    connect(tableWidget, &QTableWidget::itemChanged, this, &Window::listUpdated);

    cRadio = new QRadioButton("Contrôle");
    pRadio = new QRadioButton("Passage");
    cRadio->setChecked(true);
    cpRadioButtonsLayout->addWidget(cRadio);
    cpRadioButtonsLayout->addWidget(pRadio);
    connect(pRadio, &QRadioButton::toggled, glWidget, &GLWidget::changeTypeOfPoints);
    QWidget *cpRadioButtonsWidget = new QWidget;
    cpRadioButtonsWidget->setLayout(cpRadioButtonsLayout);
    manipulationLayout->addWidget(cpRadioButtonsWidget);

    showLinesBtnCTRL = new QCheckBox("Polygone de contrôle");
    showLinesBtnCTRL->setCheckState(Qt::CheckState::Checked);
    connect(showLinesBtnCTRL, &QCheckBox::toggled, glWidget, &GLWidget::setShowLinesCTRL);
    linesLayout->addWidget(showLinesBtnCTRL);

    showLinesBtnPASS = new QCheckBox("Lignes passage");
    showLinesBtnPASS->setCheckState(Qt::CheckState::Checked);
    connect(showLinesBtnPASS, &QCheckBox::toggled, glWidget, &GLWidget::setShowLinesPASS);
    linesLayout->addWidget(showLinesBtnPASS);

    QWidget *linesWidget = new QWidget;
    linesWidget->setLayout(linesLayout);
    linesWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    manipulationLayout->addWidget(linesWidget);

    adapt = new QPushButton("Adapter");
    connect(adapt, &QPushButton::pressed, glWidget, &GLWidget::adapt);
    cpButtonsLayout->addWidget(adapt);

    rescale = new QPushButton("Redimensionner");
    connect(rescale, &QPushButton::pressed, glWidget, &GLWidget::redimension);
    cpButtonsLayout->addWidget(rescale);

    QWidget *cpButtons = new QWidget;
    cpButtons->setLayout(cpButtonsLayout);
    cpButtons->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    manipulationLayout->addWidget(cpButtons);



    QLabel *scale = new QLabel("Taille des points");
    manipulationLayout->addWidget(scale);
    pointScaleSlider = new QSlider(Qt::Horizontal);
    pointScaleSlider->setRange(0, 1000);
    pointScaleSlider->setValue(50);
    pointScaleSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    connect(pointScaleSlider, &QSlider::valueChanged, glWidget, &GLWidget::setPointSizeMultiplier);
    manipulationLayout->addWidget(pointScaleSlider);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    manipulationLayout->addWidget(line);

    evalBtn = new QPushButton("Evaluer");
    connect(evalBtn, &QPushButton::pressed, this, &Window::evaluatePressed);
    manipulationLayout->addWidget(evalBtn);

    QLabel *population = new QLabel("Population :");
    populationBox = new QSpinBox();
    populationBox->setValue(50);
//    QLabel *lambda = new QLabel("Lambda :");
//    lambdaBox = new QDoubleSpinBox();
//    lambdaBox->setDecimals(3);
//    lambdaBox->setMaximum(0.499);
//    lambdaBox->setSingleStep(0.01);
    QLabel *knots = new QLabel("Nb noeuds");
    knotBox = new QSpinBox();

    geneticLayout->addWidget(population);
    geneticLayout->addWidget(populationBox);
    geneticLayout->addWidget(knots);
//    geneticLayout->addWidget(lambda);
    geneticLayout->addWidget(knotBox);
//    geneticLayout->addWidget(lambdaBox);
    QWidget *geneticWidget = new QWidget;
    geneticWidget->setLayout(geneticLayout);
    geneticWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    manipulationLayout->addWidget(geneticWidget);

    optBtn2003 = new QPushButton("Optimiser 2003");
    optBtn2003->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(optBtn2003, &QPushButton::pressed, this, &Window::optimisePressed);

    optBtnsLayout->addWidget(optBtn2003);

    optBtnNew = new QPushButton("Optimiser moderne");
    optBtnNew->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(optBtnNew, &QPushButton::pressed, this, &Window::optimisePressed);

    optBtnsLayout->addWidget(optBtnNew);

    QWidget *optBtnsWidget = new QWidget;
    optBtnsWidget->setLayout(optBtnsLayout);
    manipulationLayout->addWidget(optBtnsWidget);


    errXLabel = new QLabel("Erreur X : ");
    errYLabel = new QLabel("Erreur Y : ");
    fitnessLabel = new QLabel("Fitness : ");
    manipulationLayout->addWidget(errXLabel);
    manipulationLayout->addWidget(errYLabel);
    manipulationLayout->addWidget(fitnessLabel);

    calcTime = new QPushButton("calc temps");
    manipulationLayout->addWidget(calcTime);
    connect(calcTime, &QPushButton::pressed, this, &Window::calcTimePressed);

    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    manipulationLayout->addWidget(line2);

    knotsLabel = new QLabel("Noeuds :");
    controlPointsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    manipulationLayout->addWidget(knotsLabel);

    bTableWidget = new QTableWidget;
    bTableWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    bTableWidget->setColumnCount(1);
    bTableWidget->setRowCount(0);
    bTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("x"));
    bTableWidget->horizontalHeader()->setStyleSheet( "QHeaderView::section { border-bottom: 1px solid gray; }" );
    connect(bTableWidget, &QTableWidget::itemChanged, this, &Window::bListChanged);
    manipulationLayout->addWidget(bTableWidget);

    QLabel *smoothnessSliderLabel = new QLabel("Smoothness");
    manipulationLayout->addWidget(smoothnessSliderLabel);

    bSmoothnessSlider = new QSlider;
    bSmoothnessSlider->setOrientation(Qt::Horizontal);
    bSmoothnessSlider->setRange(15,100);
    bSmoothnessSlider->setSingleStep(5);
    bSmoothnessSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    connect(bSmoothnessSlider, &QSlider::valueChanged, this, &Window::showPlot);
    manipulationLayout->addWidget(bSmoothnessSlider);

    QLabel *degree = new QLabel("Degré :");
    degree->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    newBSplineLayout->addWidget(degree);
    degreeBox = new QSpinBox();
    connect(degreeBox, &QSpinBox::textChanged, this, &Window::showPlot);
    connect(degreeBox, &QSpinBox::textChanged, this, &Window::updateTitleLabels);
    newBSplineLayout->addWidget(degreeBox);
//    QLabel *dimension = new QLabel("Dimensions :");
//    dimension->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
//    newBSplineLayout->addWidget(dimension);
//    dimensionBox = new QSpinBox();
//    connect(dimensionBox, &QSpinBox::textChanged, this, &Window::showPlot);
//    newBSplineLayout->addWidget(dimensionBox);


    QWidget *newBSplineWidget = new QWidget;
    newBSplineWidget->setLayout(newBSplineLayout);
    manipulationLayout->addWidget(newBSplineWidget);

    QWidget *w2 = new QWidget;
    w2->setLayout(manipulationLayout);
    mainLayout->addWidget(w2);

    connect(plot, SIGNAL(mousePress(QMouseEvent*)), SLOT(clickedGraph(QMouseEvent*)));
    connect(plot, SIGNAL(mouseRelease(QMouseEvent*)), SLOT(releasedGraph(QMouseEvent*)));
    connect(plot, SIGNAL(mouseMove(QMouseEvent*)), SLOT(clickMove(QMouseEvent*)));
    connect(plot, SIGNAL(mouseDoubleClick(QMouseEvent*)), SLOT(doubleClickedGraph(QMouseEvent*)));
    connect(glWidget, &GLWidget::pointsChanged, this, &Window::pointsChanged);



    setLayout(mainLayout);

}

void Window::showPlot()
{
    plot->graph(0)->setData(vec_x, vec_y);
    //plot->graph(0)->rescaleAxes(true);
    spl.setDegree(degreeBox->value());
    if(degreeBox->value()+2 <= vec_x.size() )
    {
//        spl->setDimension(dimensionBox->value());

        spl.setKnots(vec_x);
        spl.setControlPoints(glWidget->getCPoints());
        spl.setPassingPoints(glWidget->getPPoints());
        spl.calculateBSplineY(bSmoothnessSlider->value());
        spl.calculateSplinePoints(bSmoothnessSlider->value());
        glWidget->setSpl(spl);


        QVector<QVector<double>> bSplineY = spl.getBSplineY();
        QVector<QVector<double>> bSplineX = spl.getBSplineX();

        //ajouter des graphes pour afficher les b-splines
        while (plot->graphCount() <= bSplineY.size())
        {
            plot->addGraph();
        }

        //afficher les b-splines
        for(int i = 0 ; i<bSplineY.size() ; i++)
        {
            if(i%5==0)
                plot->graph(i+1)->setPen(QPen(Qt::blue));
            else if(i%4==0)
                plot->graph(i+1)->setPen(QPen(Qt::black));
            else if(i%3==0)
                plot->graph(i+1)->setPen(QPen(Qt::green));
            else if(i%2==0)
                plot->graph(i+1)->setPen(QPen(Qt::red));
            else if(i%1==0)
                plot->graph(i+1)->setPen(QPen(Qt::magenta));

            plot->graph(i+1)->setScatterStyle(QCPScatterStyle::ssNone);
            plot->graph(i+1)->setLineStyle(QCPGraph::LineStyle::lsLine);
            plot->graph(i+1)->setData(bSplineX[i], bSplineY[i]);
            //plot->graph(i+1)->rescaleAxes(true);
        }

        //enlever les graphes qui n'ont pas de b-spline à afficher
        for(int i = bSplineY.size()+1 ; i<plot->graphCount() ; i++)
        {
            plot->removeGraph(i);
        }
    }else{
        //enlever les graphes qui n'ont pas de b-spline à afficher
        for(int i = 1 ; i<plot->graphCount() ; i++)
        {
            plot->removeGraph(i);
        }
    }

    plot->replot();
    plot->update();
    glWidget->update();
}

void Window::clickedGraph(QMouseEvent *event)
{
    QPoint point = event->pos();
    double x = plot->xAxis->pixelToCoord(point.x());
    double y = plot->yAxis->pixelToCoord(point.y());

    //    selectedKnot = -1;
    selectedKnots.clear();

    //pour chaque noeud
    for(int i = 0 ; i<vec_x.size() ; ++i)
    {
        //si le click est suffisamment proche
        if(abs(vec_x[i]-x) < 0.01*(plot->xAxis->range().upper - plot->xAxis->range().lower) && abs(vec_y[i]-y) < 0.02)
        {
            //s'il y avait un noeud selectionné
            if(selectedKnots.size() != 0)//s'il y avait deja noeud selectionné
            {
                //si le nouveau noeud est plus proche que les anciens
                if(abs(vec_x[i]-x) < abs(vec_x[selectedKnots[0]]-x) && abs(vec_y[i]-y) < abs(vec_y[selectedKnots[0]]-y))
                {
                    //on enleve les anciens noeuds
                    selectedKnots.clear();
                    //on ajoute celui qui est plus proche
                    selectedKnots.push_back(i);
                }
                //si le nouveau noeud est au même endroit que les anciens
                else if(abs(vec_x[i]-x) == abs(vec_x[selectedKnots[0]]-x) && abs(vec_y[i]-y) == abs(vec_y[selectedKnots[0]]-y))
                {
                    //on ajoute le noeud
                    selectedKnots.push_back(i);
                }
            }else //pas encore de noeud selectionné
            {
                //on le selectionne
                selectedKnots.push_back(i);
            }
        }
    }
    if(event->buttons() == Qt::RightButton)
    {
        //supprimer le noeud le plus proche de la souris
        //supprimer plusieurs noeuds s'ils sont confondus
        for(int i = selectedKnots.size()-1 ; i>=0 ; i--){
            vec_x.remove(selectedKnots[i]);
            vec_y.remove(selectedKnots[i]);
            selectedKnots.remove(i);
        }
        updateBList();
        updateTi();
        showPlot();
    }
    if(!selectedKnots.isEmpty())
        selectedX = vec_x[selectedKnots.front()];
}

void Window::releasedGraph(QMouseEvent *event)
{

    movePoint(plot->xAxis->pixelToCoord(event->pos().x()));

    selectedKnots.clear();

    showPlot();

}

void Window::doubleClickedGraph(QMouseEvent *event)
{
    //ajouter un noeud
    QPoint point = event->pos();
    double x = plot->xAxis->pixelToCoord(point.x());

    addToVecXAndSort(x);
    vec_y.append(0);
    updateTi();
    updateBList();

    selectedKnots.clear();
    showPlot();
}

void Window::clickMove(QMouseEvent *event)
{
    movePoint(plot->xAxis->pixelToCoord(event->pos().x()));
    showPlot();
}


void Window::bListChanged(QTableWidgetItem *item)
{
    //modifier le noeud en fonction du changmeent dans la liste de noeuds
    if(item != nullptr && selectedKnots.isEmpty())
    {
        double value = std::stod(item->text().toLocal8Bit().data());
        int index = bTableWidget->row(item);

        vec_x[index] = value;

        if(index > 0 && vec_x[index-1] > value){
            vec_x[index] = vec_x[index-1];
        }else if (index < vec_x.size()-1 && vec_x[index+1] < value){
            vec_x[index] = vec_x[index+1];
        }

        bTableWidget->item(index,0)->setText((std::to_string(vec_x[index])).c_str());
        updateTi();
        showPlot();
    }
}

void Window::pointsChanged(QVector<Point> points)
{
    //afficher les bons points dans la liste de points (contrôle ou pasage)
    tableWidget->setRowCount(points.length());
    updateTitleLabels();
    for(int i = 0 ; i<points.length() ; i++)
    {
        tableWidget->setItem(i,0, new QTableWidgetItem(std::to_string(points[i].getValueAtDimension(0)).c_str()));
        tableWidget->setItem(i,1, new QTableWidgetItem(std::to_string(points[i].getValueAtDimension(1)).c_str()));
    }

    if(cRadio->isChecked()){
        for(int i = 0 ; i<points.length() ; i++){
            tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(("c" + std::to_string(i)).c_str()));
        }
        calculateErrors();
    }else{
        for(int i = 0 ; i<points.length() ; i++){
            tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(("p" + std::to_string(i)).c_str()));
        }
        updateErrors(-1,-1, 0);
    }
    showPlot();
}

void Window::listUpdated(QTableWidgetItem *item)
{
    //modifier le bon point (contrôle / passage) en fonction des changements dans la liste de points
    if(item != nullptr && selectedKnots.isEmpty())
    {
        //std::stod(tableWidget->item(index, 0)->text().toLocal8Bit().data())
        int index = tableWidget->row(item);
        if(tableWidget->item(index, 1) != 0) {
            Point p({tableWidget->item(index, 0)->text().toDouble(),
                     std::stod(tableWidget->item(index, 1)->text().toLocal8Bit().data())});
            if(cRadio->isChecked()){
                glWidget->setControlPointAt(index,p);
            }else{
                glWidget->setPassingPointAt(index,p);
            }
        }
    }
}

void Window::updateTitleLabels()
{
    //affiche le nombre optimal de noeuds en fonction du nombre de points de contrôle et inversement
    if(cRadio->isChecked()){
        int min = degreeBox->value()*2+2;
        int opt = std::max(tableWidget->rowCount()+degreeBox->value()+1, min);
//        knotsLabel->setText(("Noeuds : minimum : " + std::to_string(min) + ", optimal : " + std::to_string(opt)).c_str());
        knotsLabel->setText(("Noeuds : optimal : " + std::to_string(opt)).c_str());

        int min2 = degreeBox->value()+1;
        int opt2 = std::max(bTableWidget->rowCount()-degreeBox->value()-1, min2);
//        controlPointsLabel->setText(("Points de Contrôle : minimum : " + std::to_string(min2) + ", optimal : " + std::to_string(opt2)).c_str());
        controlPointsLabel->setText(("Points de Contrôle : optimal : " + std::to_string(opt2)).c_str());
    }else{
        knotsLabel->setText("Noeuds :");
        controlPointsLabel->setText("Points de passage :");
    }
}

void Window::evaluatePressed()
{
    //fonction qui calcule les points de contrôle optimaux en fonction des points de passage et des b-splines

    spl.setPassingPoints(glWidget->getSpl().getPassingPoints());

    //facilite la lecture
    QVector<Point> points = spl.getPassingPoints();

    //éviter le crash
    if(points.length() == 0)
    {
        qDebug()<<"erreur : pas de points de passage";
        return;
    }
    if(spl.getKnots().length() == 0)
    {
        qDebug()<<"erreur : pas de noeuds";
        return;
    }

    //transformer le tableau de points de passage en deux tableaux de double
    QVector<double> xPoints, yPoints;
    xPoints.reserve(points.length());
    yPoints.reserve(points.length());

    for(int i = 0 ; i<points.length() ; i++)
    {
        xPoints.push_back(points[i].getValueAtDimension(0));
        yPoints.push_back(points[i].getValueAtDimension(1));
    }

    const double *xPointsCDP = xPoints.data();
    const double *yPointsCDP = yPoints.data();

    alglib::real_1d_array y0, y1, c0, c1;
    y0.setcontent(points.length(), xPointsCDP);
    y1.setcontent(points.length(), yPointsCDP);


    alglib::ae_int_t nbFunctions = spl.getBSplineY().length(),
            nbPoints = points.length(),
            info0, info1;

    //calcule les t auxquels la spline devrait passer par le point de passage
    tp.clear();
    tp.resize(points.length());
    tp[0] = 0;
//    double alpha = 0.5;
    for(int i = 1 ; i<points.length() ; i++){
        //methode corde
        tp[i] = tp[i-1] + sqrt(pow(points[i].getValueAtDimension(1)-points[i-1].getValueAtDimension(1),2) +
                pow(points[i].getValueAtDimension(0)-points[i-1].getValueAtDimension(0),2));

//        //methode centripetale
//        tp[i] = tp[i-1] + pow(sqrt(pow(points[i].getValueAtDimension(1)-points[i-1].getValueAtDimension(1),2) +
//                              pow(points[i].getValueAtDimension(0)-points[i-1].getValueAtDimension(0),2)),alpha);

//        tp[i] = points[i].getValueAtDimension(0);
    }

    //range les t dans [0,1[ inclure 1 fait dégénerer le calcul
    //sans EPS, si l'on met degré+1 noeuds sur le dernier noeud ça dégénère
    for(int i = 0 ; i<points.length()-1 ; i++){
        tp[i] = tp[i]/(tp[points.length()-1]);
    }
    tp[points.length()-1] = 1-EPS;

//    //temps pour "exempleSplinePtsPassage.txt", X temps entre chaque noeud, ici X = 16
//    tp = {0, 0.0133333, 0.0266667, 0.04, 0.0533333, 0.0666667, 0.08, 0.0933333, 0.106667, 0.12, 0.133333, 0.146667, 0.16, 0.173333, 0.186667, 0.2,
//          0.2, 0.206667, 0.213333, 0.22, 0.226667, 0.233333, 0.24, 0.246667, 0.253333, 0.26, 0.266667, 0.273333, 0.28, 0.286667, 0.293333, 0.3,
//          0.3, 0.306667, 0.313333, 0.32, 0.326667, 0.333333, 0.34, 0.346667, 0.353333, 0.36, 0.366667, 0.373333, 0.38, 0.386667, 0.393333, 0.4,
//          0.4, 0.413333, 0.426667, 0.44, 0.453333, 0.466667, 0.48, 0.493333, 0.506667, 0.52, 0.533333, 0.546667, 0.56, 0.573333, 0.586667, 0.6,
//          0.6, 0.601333, 0.602667, 0.604, 0.605333, 0.606667, 0.608, 0.609333, 0.610667, 0.612, 0.613333, 0.614667, 0.616, 0.617333, 0.618667, 0.62,
//          0.62, 0.621333, 0.622667, 0.624, 0.625333, 0.626667, 0.628, 0.629333, 0.630667, 0.632, 0.633333, 0.634667, 0.636, 0.637333, 0.638667, 0.64,
//          0.64, 0.664, 0.688, 0.712, 0.736, 0.76, 0.784, 0.808, 0.832, 0.856, 0.88, 0.904, 0.928, 0.952, 0.976, 1-EPS};

    //calculer le Y de chaque B-Spline en chaque temps tp
    QVector<double> Ytp;
    Ytp.reserve(tp.length()*spl.getBSplineY().length());
    for(int i = 0 ; i<tp.length() ; i++){
        for(int j = 0 ; j<spl.getBSplineY().length() ; j++)
        {
            Ytp.push_back(spl.calculateBSplineY(spl.getDegree(),j,tp[i]));
        }
    }

    const double *YtpCDP = Ytp.data();

    alglib::real_2d_array fmatrix;
    fmatrix.setcontent(tp.length(), spl.getBSplineY().length() ,YtpCDP);

    alglib::lsfitreport rep0, rep1;

    alglib::lsfitlinear(y0,fmatrix,nbPoints,nbFunctions,info0,c0,rep0);
    alglib::lsfitlinear(y1,fmatrix,nbPoints,nbFunctions,info1,c1,rep1);


    if(info0 == 1 && info1 == 1){
        QVector<Point> ctrlPts;
        ctrlPts.reserve(c0.length());

        for(int i = 0 ; i<c0.length() ; i++)
        {
            Point p = Point({c0[i], c1[i]});
            ctrlPts.push_back(p);
        }

        double err0 = pow(rep0.rmserror,2)*points.length(),
                err1 = pow(rep1.rmserror,2)*points.length();



        errXLabel->setStyleSheet("QLabel { background-color : transparent;}");
        errYLabel->setStyleSheet("QLabel { background-color : transparent;}");
        fitnessLabel->setStyleSheet("QLabel { background-color : transparent;}");
        theoreticalMinErrX = err0;
        theoreticalMinErrY = err1;

        int N = nbPoints;
        int n = spl.getKnots().size()-(2*(degreeBox->value()+1));
        int m = spl.getDegree()+1;
        double knotError = log(N)*(2*n+m);

        double xErr = (N*(log(err0+1))) + knotError;
        double yErr = (N*(log(err1+1))) + knotError;

        theoreticalMinFitness = xErr+yErr;
        updateErrors(err0,err1, theoreticalMinFitness);

        spl.setControlPoints(ctrlPts);
        glWidget->setSpl(spl);

        if(cRadio){
            pointsChanged(spl.getControlPoints());
        }

        qDebug()<<"error after eval pressed, X:"<<err0<<"Y:"<<err1;
//        calculateErrors();

        showPlot();
    }else{
        qDebug()<<"erreur";
    }
}

void Window::optimisePressed()
{

    //éviter le crash
    if(glWidget->getSpl().getPassingPoints().size() == 0)
    {
        qDebug()<<"erreur : pas de points de passage";
        return;
    }

    //https://stackoverflow.com/questions/37938304/how-to-know-which-button-was-clicked-in-qt/37938556
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender()); // retrieve the button you have clicked

    spl.setPassingPoints(glWidget->getSpl().getPassingPoints());

    if(buttonSender == optBtn2003){
        Population pop(populationBox->value(),/*lambdaBox->value()*/knotBox->value(),spl.getDegree(),glWidget->getSpl().getPassingPoints(),true);
        updatePopulation(pop);
    }else if(buttonSender == optBtnNew){
        Population pop(populationBox->value(), /*lambdaBox->value()*/knotBox->value(), spl.getDegree(), glWidget->getSpl().getPassingPoints());
        updatePopulation(pop);
    }
}

void Window::calcTimePressed()
{
    //objectif de la fonction :
    /**
     *
     *   - utiliser l'exemple des points de passage que vous avez récupérés sur une spline (je les notes p_0, p_1, ..., p_m)
     *   - calculer le temps tp_i en chaque p_i avec la méthode actuelle (chordale)
     *   - placer les nœuds là où ils doivent être
     *   - faire un certains nombre de fois
     *     - calculer les points de contrôle (qui nous donnent donc l'équation S de la spline)
     *     - pour chaque point pi avec i variant de 1 à m-1
     *       - choisir un temps t' au hasard entre tp_(i-1) et tp_i
     *       - choisir un temps t" au hasard entre tp_i et tp_ (i+1)
     *       - calculer l'erreur entre p_i et S(tp_i)
     *       - calculer l'erreur entre p_i et S(t')
     *       - calculer l'erreur entre p_i et S(t")
     *       - affecter à tp_i celui des 3 temps ci-dessus qui génère la plus petite erreur
    **/

    spl.setPassingPoints(glWidget->getSpl().getPassingPoints());

    //facilite la lecture
    QVector<Point> points = spl.getPassingPoints();

    //éviter le crash
    if(points.length() == 0)
    {
        qDebug()<<"erreur : pas de points de passage";
        return;
    }
    if(spl.getKnots().length() == 0)
    {
        qDebug()<<"erreur : pas de noeuds";
        return;
    }

    //transformer le tableau de points de passage en deux tableaux de double
    QVector<double> xPoints, yPoints;
    xPoints.reserve(points.length());
    yPoints.reserve(points.length());

    for(int i = 0 ; i<points.length() ; i++)
    {
        xPoints.push_back(points[i].getValueAtDimension(0));
        yPoints.push_back(points[i].getValueAtDimension(1));
    }

    const double *xPointsCDP = xPoints.data();
    const double *yPointsCDP = yPoints.data();

    alglib::real_1d_array y0, y1, c0, c1;
    y0.setcontent(points.length(), xPointsCDP);
    y1.setcontent(points.length(), yPointsCDP);


    alglib::ae_int_t nbFunctions = spl.getBSplineY().length(),
            nbPoints = points.length(),
            info0, info1;

    //calcule les t auxquels la spline devrait passer par le point de passage
    tp.clear();
    tp.resize(points.length());
    tp[0] = 0;

    for(int i = 1 ; i<points.length() ; i++){
        //methode corde
        tp[i] = tp[i-1] + sqrt(pow(points[i].getValueAtDimension(1)-points[i-1].getValueAtDimension(1),2) +
                pow(points[i].getValueAtDimension(0)-points[i-1].getValueAtDimension(0),2));
    }
    //ranger les t dans [0;1[
    for(int i = 0 ; i<points.length()-1 ; i++){
        tp[i] = tp[i]/(tp[points.length()-1]);
    }
    tp[points.length()-1] = 1-EPS;

    alglib::lsfitreport rep0, rep1;
    QVector<Point> ctrlPts;

    QVector<Point> pPoints = spl.getPassingPoints();

    int nbDeTours = 100; //pour tester
    for(int nbt = 0 ; nbt<nbDeTours ; nbt++){

        //calculer le Y de chaque B-Spline en chaque temps tp
        QVector<double> Ytp;
        Ytp.reserve(tp.length()*spl.getBSplineY().length());
        for(int i = 0 ; i<tp.length() ; i++){
            for(int j = 0 ; j<spl.getBSplineY().length() ; j++)
            {
                Ytp.push_back(spl.calculateBSplineY(spl.getDegree(),j,tp[i]));
            }
        }

        const double *YtpCDP = Ytp.data();

        alglib::real_2d_array fmatrix;
        fmatrix.setcontent(tp.length(), spl.getBSplineY().length() ,YtpCDP);

        //calculer les points de contrôle
        alglib::lsfitlinear(y0,fmatrix,nbPoints,nbFunctions,info0,c0,rep0);
        alglib::lsfitlinear(y1,fmatrix,nbPoints,nbFunctions,info1,c1,rep1);

        ctrlPts.clear();
        ctrlPts.reserve(c0.length());
        for(int i = 0 ; i<c0.length() ; i++)
        {
            Point p = Point({c0[i], c1[i]});
            ctrlPts.push_back(p);
        }
        spl.setControlPoints(ctrlPts);
        glWidget->setSpl(spl);


        for(int j = 1 ; j<tp.length()-1 ; j++)
        {
            double t_p = (rand() / static_cast<double>(RAND_MAX)) * (tp[j]-tp[j-1]) + tp[j-1];
            double t_pp = (rand() / static_cast<double>(RAND_MAX)) * (tp[j+1]-tp[j]) + tp[j];

            //rechercher l'indice du noeud tel que ce noeud <= au temps utilisé pour calculer le point de passage
            int index = -1;
            for(int k = spl.getDegree() ; k<spl.getKnots().length()-spl.getDegree()-1 ; k++)
            {
                if(spl.getKnotAt(j) <= tp[k])
                {
                    index = k;
                }
            }
            //si l'indice existe comparer l'erreur pour chaque temps et remplacer ce temps par celui qui a la plus petite erreur
            if(index != -1)
            {
                Point point_t = spl.deBoor(index,tp[j]);
                Point point_t_p = spl.deBoor(index,t_p);
                Point point_t_pp = spl.deBoor(index, t_pp);

                double err_t = pow(point_t.getValueAtDimension(0) - pPoints[j].getValueAtDimension(0), 2) +
                        pow(point_t.getValueAtDimension(1) - pPoints[j].getValueAtDimension(1), 2);

                double err_t_p = pow(point_t_p.getValueAtDimension(0) - pPoints[j].getValueAtDimension(0), 2) +
                        pow(point_t_p.getValueAtDimension(1) - pPoints[j].getValueAtDimension(1), 2);

                double err_t_pp = pow(point_t_pp.getValueAtDimension(0) - pPoints[j].getValueAtDimension(0), 2) +
                        pow(point_t_pp.getValueAtDimension(1) - pPoints[j].getValueAtDimension(1), 2);

                if(err_t_p < err_t && err_t_p < err_t_pp){
                    tp[j] = t_p;
                }else if(err_t_pp < err_t && err_t_pp < err_t_p){
                    tp[j] = t_pp;
                }

                if(tp[j] == 1)
                    tp[j] -= EPS;
            }
        }


    }


    ctrlPts.clear();
    ctrlPts.reserve(c0.length());

    for(int i = 0 ; i<c0.length() ; i++)
    {
        Point p = Point({c0[i], c1[i]});
        ctrlPts.push_back(p);
    }

    double err0 = pow(rep0.rmserror,2)*points.length(),
            err1 = pow(rep1.rmserror,2)*points.length();

    errXLabel->setStyleSheet("QLabel { background-color : transparent;}");
    errYLabel->setStyleSheet("QLabel { background-color : transparent;}");
    fitnessLabel->setStyleSheet("QLabel { background-color : transparent;}");
    theoreticalMinErrX = err0;
    theoreticalMinErrY = err1;

    int N = nbPoints;
    int n = spl.getKnots().size()-(2*(degreeBox->value()+1));
    int m = spl.getDegree()+1;
    double knotError = log(N)*(2*n+m);

    double xErr = (N*(log(err0+1))) + knotError;
    double yErr = (N*(log(err1+1))) + knotError;

    theoreticalMinFitness = xErr+yErr;
    updateErrors(err0,err1, theoreticalMinFitness);

    spl.setControlPoints(ctrlPts);
    glWidget->setSpl(spl);

    qDebug()<<"error after calc time pressed, X:"<<err0<<"Y:"<<err1;

    if(cRadio){
        pointsChanged(spl.getControlPoints());
    }

    showPlot();


}

QCustomPlot *Window::getPlot() const
{
    return plot;
}

void Window::setSpl(const Spline &value)
{
//    spl.clear();
    vec_x.clear();
    vec_y.clear();
    glWidget->setSpl(spl);
    spl = value;
    glWidget->setSpl(spl);
    degreeBox->setValue(spl.getDegree());
    vec_x = spl.getKnots();
    vec_y.resize(vec_x.length());
    showPlot();
    updateTi();
    updateBList();
    pointsChanged(spl.getControlPoints());
}

void Window::updateErrors(double xErr, double yErr, double fitness)
{
    //met à jour les labels affichant les erreurs
    if(xErr == -1 || yErr == -1 || fitness < 0){
        errXLabel->setText("Erreur X : ");
        errYLabel->setText("Erreur Y : ");
        fitnessLabel->setText("Fitness : ");
    }else{
        if( xErr+EPS2 < theoreticalMinErrX){
            errXLabel->setStyleSheet("QLabel { background-color : red;}");
        }
        if( yErr+EPS2 < theoreticalMinErrY){
            errYLabel->setStyleSheet("QLabel { background-color : red;}");
        }
        if( fitness+EPS2 < theoreticalMinFitness){
            fitnessLabel->setStyleSheet("QLabel { background-color : red;}");
        }
        std::string s = "Erreur X : "+std::to_string(xErr);
        errXLabel->setText(s.c_str());
        s = "Erreur Y : "+std::to_string(yErr);
        errYLabel->setText(s.c_str());
        s = "Fitness : "+std::to_string(fitness);
        fitnessLabel->setText(s.c_str());
    }
}

Spline Window::getSpl() const
{
    return spl;
}

void Window::movePoint(double x)
{

    if(!selectedKnots.isEmpty())
    {
        //si la touche CTRL est maintenue
        if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
        {
            //si on déplace la souris à la gauche du tas on déplace le plus petit t du tas selectionné
            if(x < selectedX)
            {
                //mettre à jour le noeud
                vec_x[selectedKnots.front()] = x;

                //corriger erreur de déplacement de la droite à la gacuhe du tas ou appui sur ctrl pendant le déplacement
                if(selectedKnots.size() > 1)
                {
                    for(int i = 1 ; i<selectedKnots.size() ; i++)
                    {
                        vec_x[selectedKnots[i]] = selectedX;
                    }
                }

                //bloquer le noeud entre ses voisins
                if(vec_x.size() > selectedKnots.front()+1 && x > vec_x[selectedKnots.front()+1])
                    vec_x[selectedKnots.front()] = vec_x[selectedKnots.front()+1];
                else if(0 <= selectedKnots.front()-1 && x < vec_x[selectedKnots.front()-1])
                    vec_x[selectedKnots.front()] = vec_x[selectedKnots.front()-1];

            }

            //si on déplace la souris à la droite du tas on déplace le plus grand t du tas selectionné
            if(x > selectedX)
            {
                //mettre à jour le noeud
                vec_x[selectedKnots.back()] = x;

                //corriger erreur de déplacement de la gauche à la droite du tas ou appui sur ctrl pendant le déplacement
                if(selectedKnots.size() > 1)
                {
                    for(int i = 0 ; i<selectedKnots.size()-1 ; i++)
                    {
                        vec_x[selectedKnots[i]] = selectedX;
                    }
                }

                //bloquer le noeud entre ses voisins
                if( 0 <= selectedKnots.back()-1 && x < vec_x[selectedKnots.back()-1])
                    vec_x[selectedKnots.back()] = vec_x[selectedKnots.back()-1];
                else if( vec_x.size() > selectedKnots.back()+1 && x > vec_x[selectedKnots.back()+1])
                    vec_x[selectedKnots.back()] = vec_x[selectedKnots.back()+1];

            }

        }
        else
        {

            //on déplace tout le tas selectionné
            for(int i = 0 ; i < selectedKnots.size() ; ++i)
            {
                vec_x[selectedKnots[i]] = x;
                ti[selectedKnots[i]]->position->setCoords(vec_x[selectedKnots[i]],tiYoffset);
            }

            //on bloque le tas entre ses voisins
            if(0 <= selectedKnots.front()-1 && x < vec_x[selectedKnots.front()-1])
            {
                double x1 = vec_x[selectedKnots.front()-1];
                for(int i = 0 ; i < selectedKnots.size() ; ++i)
                {
                    vec_x[selectedKnots[i]] = x1;
                    ti[selectedKnots[i]]->position->setCoords(x1,tiYoffset);
                }
            }
            else{
                if(vec_x.size() > selectedKnots.back()+1 && x > vec_x[selectedKnots.back()+1])
                {
                    for(int i = 0 ; i < selectedKnots.size() ; ++i)
                    {
                        vec_x[selectedKnots[i]] = vec_x[selectedKnots.back()+1];
                        ti[selectedKnots[i]]->position->setCoords(vec_x[selectedKnots[i]],tiYoffset);
                    }
                }
            }

        }
        updateBList();
        for(int i = 0 ; i<selectedKnots.size() ; i++)
            ti[selectedKnots[i]]->position->setCoords(vec_x[selectedKnots[i]],tiYoffset);
    }

}

void Window::updateBList()
{
    bTableWidget->clear();
    bTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("x"));
    bTableWidget->setRowCount(vec_x.size());
    updateTitleLabels();
    for(int i = 0 ; i<vec_x.size() ; i++)
    {
        bTableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(("t" + std::to_string(i)).c_str()));
        bTableWidget->setItem(i, 0, new QTableWidgetItem(std::to_string(vec_x[i]).c_str()));
    }
    updateErrors(-1,-1, 0);
//    calculateErrors();
}

void Window::addToVecXAndSort(double x)
{
    int i = 0;
    while(i<vec_x.size() && vec_x[i] < x)
    {
        i++;
    }

    vec_x.insert(i,x);
}

void Window::updateTi()
{
    ti.clear();
    plot->clearItems();
    for(int i = 0 ; i<vec_x.size() ; i++)
    {
        ti.push_back(new QCPItemText(plot));
        ti[i]->position->setCoords(vec_x[i],tiYoffset);
        std::string s = "t"+std::to_string(i);
        ti[i]->setText(s.c_str());
    }
}

void Window::calculateErrors()
{
    if(tp.length() >= spl.getPassingPoints().size() && spl.getKnots().length() > 0 && spl.getNumberOfControlPoints() > spl.getNumberOfKnots()-spl.getDegree()){ //éviter le crash
        spl = glWidget->getSpl();
        QVector<Point> pPoints = spl.getPassingPoints();
        double xErr = 0, yErr = 0;
        for(int i = 0 ; i<pPoints.length() ; i++)
        {
            //rechercher l'indice du noeud tel que ce noeud <= au temps utilisé pour calculer le point de passage
            int index = -1;
            for(int j = spl.getDegree() ; j<spl.getKnots().length()-spl.getDegree()-1 ; j++)
            {
                if(spl.getKnotAt(j) <= tp[i])
                {
                    index = j;
                }
            }
            //si l'indice existe ajouter aux erreurs la différence au carré entre la spline et le point de passage
            if(index != -1)
            {
                Point point = spl.deBoor(index,tp[i]);
                xErr += pow(point.getValueAtDimension(0) - pPoints[i].getValueAtDimension(0), 2);
                yErr += pow(point.getValueAtDimension(1) - pPoints[i].getValueAtDimension(1), 2);
            }
        }
        //calc fitness
        int N = spl.getPassingPoints().length();
        int n = spl.getKnots().size()-(2*(degreeBox->value()+1));
        int m = spl.getDegree()+1;
        double knotError = log(N)*(2*n+m);

        double e1 = (N*(log(xErr+1))) + knotError;
        double e2 = (N*(log(yErr+1))) + knotError;

        double fitness = e1+e2;
        qDebug()<<xErr<<","<<yErr<<","<<fitness;
        updateErrors(xErr,yErr, fitness);
    }
}

void Window::updatePopulation(const Population &pop)
{

    tp = pop.getTp();

    double err0 = pop.getBestIndividual()->getErrX(),
            err1 = pop.getBestIndividual()->getErrY();
    errXLabel->setStyleSheet("QLabel { background-color : transparent;}");
    errYLabel->setStyleSheet("QLabel { background-color : transparent;}");
    fitnessLabel->setStyleSheet("QLabel { background-color : transparent;}");
    theoreticalMinErrX = err0;
    theoreticalMinErrY = err1;
    theoreticalMinFitness = pop.getBestIndividual()->getFitness();

    QVector<double> knots = pop.getBestIndividual()->getKnots();

    vec_x = knots;
    vec_y.resize(vec_x.length());
    updateBList();

    QVector<Point> ctrlPts = pop.getBestIndividual()->getCtrlPts();

    updateTi();
    spl.setControlPoints(ctrlPts);
    glWidget->setSpl(spl);
    showPlot();

    updateErrors(err0,err1, pop.getBestIndividual()->getFitness());
}


