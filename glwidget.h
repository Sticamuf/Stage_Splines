#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "point.h"
#include "spline.h"


class GLWidget : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT
public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();


    /**
     * @brief getPPoints
     * @return les points de passage de la spline
     */
    QVector<Point> getPPoints() const;
    /**
     * @brief getCPoints
     * @return les points de coontrôle de la spline
     */
    QVector<Point> getCPoints() const;

    /**
     * @brief setControlPointAt
     * si l'indexe est valide remplace à l'indexe "index" le point de contrôle par "p"
     * @param index
     * @param p
     */
    void setControlPointAt(int index, Point p);
    /**
     * @brief setPassingPointAt
     * si l'indexe est valide remplace à l'indexe "index" le point de passage par "p"
     * @param index
     * @param p
     */
    void setPassingPointAt(int index, Point p);
    /**
     * @brief setSpl
     * @param value la nouvelle spline a afficher dans la fenêtre OpenGL
     */
    void setSpl(Spline value);
    /**
     * @brief getSpl
     * @return la spline affichée dans la fenêtre OpenGL
     */
    Spline getSpl() const;

public slots:
    /**
     * @brief setPointSizeMultiplier
     * met le multiplicateur de la taille des points à value
     * @param value le nouveau multiplicateur
     */
    void setPointSizeMultiplier(double value);
    /**
     * @brief setShowLinesCTRL
     * allume ou éteint l'affichage des lignes entre les points de contrôle en fonction du paramètre value
     * @param value
     */
    void setShowLinesCTRL(bool value);
    /**
     * @brief setShowLinesPASS
     * allume ou éteint l'affichage des lignes entre les points de passage en fonction du paramètre value
     * @param value
     */
    void setShowLinesPASS(bool value);
    /**
     * @brief redimension
     * réinitialise l'affichage pour avoir un repère orthonormal
     */
    void redimension();
    /**
     * @brief adapt
     * adapte l'affichage pour afficher tous les points de contrôle et de passage disponibles
     */
    void adapt();
    /**
     * @brief changeTypeOfPoints
     * change le type de point auquel l'utilisateur a accès
     */
    void changeTypeOfPoints();

    /**
     * @brief resizeEvent
     * override de l'evenement pour garder la géométrie des objets dans la fenêtre
     * @param e l'evenement déclanchant la fonction
     */
    void resizeEvent(QResizeEvent *e) override;
signals:
    /**
     * @brief pointsChanged
     * signal émis dès que le type ou la valeur des points est modifiée
     * @param pts les points auquels l'utilisateur a accès au moment où le signal est émis
     */
    void pointsChanged(QVector<Point> pts);

protected:
    void initializeGL() override;

    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent* event) override;

    void drawSquareAtCenter(double x, double y, double xMult, double yMult);

private:
    Spline spl;
    int selectedPointIndex = -1;
    int setRedPointIndex = -1, setRedLineIndex = -1;
    double const EPS = 0.05;
    double const EPS1 = 0.1;
    double const EPS2 = 5;
    double const EPS3 = 500;
    double const EPS4 = 0.00001;


    /**
     * @brief getShifted
     * prends t appartenant à [a,b] et retourne la valeur de la projection dans [c,d]
     * @param t dans l'intervalle [a,b]
     * @param a debut de l'intervalle initial
     * @param b fin de l'interval initial
     * @param c debut de l'interval final
     * @param d fin de l'interval final
     * @return la projection de t depuis l'intervalle [a,b] dans l'intervalle [c,d]
     */
    double getShifted(double t, double a, double b, double c, double d);

    bool showLinesCTRL = true;
    bool showLinesPASS = true;
    double pointSizeMultiplier = 50;
    double translateOriginX = 0, translateOriginY = 0, translateEndX = 0, translateEndY = 0;
    double left, right, bottom, top;
    double previousLeft, previousRight, previousBottom, previousTop;

    bool controlP = true;
    double xMult, yMult, dWid, dHei;

    void calculateTotalTranslate();
    void initialisePreviousBorders();


};

#endif // GLWIDGET_H
