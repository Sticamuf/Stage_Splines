#ifndef SPLINE_H
#define SPLINE_H

#include <vector>
#include <string>
#include <fstream>
#include <QWidget>

class Point;

class Spline
{
public:

    Spline(const std::string& fileName);

    Spline(int degree, int dimension, QVector<double> knots);
    ~Spline();


    /**
     * @brief deBoor
     * https://en.wikipedia.org/wiki/De_Boor's_algorithm
     * @param indexOfIntervalOfX l'indexe du noeud inférieur ou égal au temps t
     * @param t le temps
     * @return le point de la Spline calculé à partir des B-Splines au temps t
     */
    Point deBoor(int indexOfIntervalOfX, double t);

    /**
     * @brief saveInFile
     * enregistre les noeuds et points de contrôle dans le ficher fileName.txt
     * @param fileName le nom du fichier auquel on ajoute ".txt"
     */
    void saveCTRLinFile(const std::string& fileName);

    /**
     * @brief savePASSinFile
     * enregistre les points de passage dans les fichier fileName.txt
     * @param fileName le nom du fichier auquel on ajoute ".txt"
     */
    void savePASSinFile(const std::string &fileName);

    /**
     * @brief saveSPLNinFileAsPASS
     * entregistre les points de la spline dans le fichier fileName.txt, à l'ouverture ces points seront interprétés comme des points de passage
     * s'il faut garder les points de la spline en tant que points de spline utiliser saveCTRLinFile
     * @param fileName les nom du fichier auquel on ajoute ".txt"
     */
    void saveSPLNinFileAsPASS(const std::string &fileName);

    /**
     * @brief saveKnotsInFile
     * enregistre les noeuds dans le fichier
     * @param fileName
     */
    void saveKnotsInFile(const std::string &fileName);

    /**
     * @brief getKnotAt
     * @param index
     * @return le noeud à l'indexe "index", si cet index n'existe pas retourne INT_MAX
     */
    double getKnotAt(int index) const;
    /**
     * @brief getNumberOfKnots
     * @return ne nombre de noeuds
     */
    int getNumberOfKnots() const;
    /**
     * @brief getControlPointAt
     * @param index
     * @return le point de contrôle à l'indexe "index", si cet indexe n'existe pas retourne le Point (0,0)
     */
    Point getControlPointAt(int index) const;
    /**
     * @brief setControlPointAt
     * si l'indexe est valide, remplace le point courant à l'indexe "index" avec le point "p"
     * @param p
     * @param index
     */
    void setControlPointAt(Point p, int index);
    /**
     * @brief getNumberOfControlPoints
     * @return le nombre de points de contrôle
     */
    int getNumberOfControlPoints() const;
    /**
     * @brief getDimension
     * @return le nombre de dimensions de la spline
     */
    int getDimension() const;

    /**
     * @brief calculateBSplineY
     * remplit les tableaux des B-Splines en dimension X et Y
     * @param detail plus cette valeur est grande plus il y aura de valeurs calculées
     */
    void calculateBSplineY(int detail);
    /**
     * @brief calculateBSplineY
     * fonction de récurence pour remplir les tableaux des B-Splines en dimension X et Y
     * @param k le degré
     * @param i la i-ème B-Spline
     * @param t le temps
     * @return
     */
    double calculateBSplineY(int k, int i, double t);
    /**
     * @brief getBSplineY
     * @return le tableau des Y de chaque B-Spline
     */
    QVector<QVector<double>> getBSplineY() const;
    /**
     * @brief getBSplineX
     * @return le tableau des X de chaque B-Spline
     */
    QVector<QVector<double>> getBSplineX() const;
    /**
     * @brief getKnots
     * @return les noeuds
     */
    QVector<double> getKnots() const;
    /**
     * @brief getControlPoints
     * @return les points de contrôle
     */
    QVector<Point> getControlPoints() const;
    /**
     * @brief addControlPoint
     * ajoute le point "p" aux points de controle
     * @param p
     */
    void addControlPoint(Point p);
    /**
     * @brief insertControlPoint
     * si l'indexe est valide insère le point "p" à l'indexe "index"
     * @param index
     * @param p
     */
    void insertControlPoint(int index, Point p);
    /**
     * @brief removeControlPointAt
     * si l'indexe est valide supprime le point de contrôle à l'indexe "index"
     * @param index
     */
    void removeControlPointAt(int index);
    /**
     * @brief setControlPoints
     * remplace les points de contrôle par ceux donnés en paramètre
     * @param value le nouveau tableau de points de contrôle
     */
    void setControlPoints(const QVector<Point> &value);
    /**
     * @brief getDegree
     * @return le dergé de la Spline / des B-Splines
     */
    int getDegree();
    /**
     * @brief setKnots
     * remplace les noeuds par ceux donnés en paramètre
     * @param value les nouveaux noeuds
     */
    void setKnots(const QVector<double> &value);
    /**
     * @brief setKnots
     * remplace les noeuds par ceux dans le fichier en paramètre
     * @param fileName
     */
    void setKnots(QString fileName);
    /**
     * @brief setDegree
     * met le degré à la valeur donnée en paramètre
     * @param value le nouveau degré
     */
    void setDegree(int value);
    /**
     * @brief calculateSplinePoints
     * calcule les points de la spline
     * @param detail plus cette valeur est grande plus il y aura de valeurs calculées
     */
    void calculateSplinePoints(int detail);
    /**
     * @brief getSplinePoints
     * @return les points de la spline
     */
    QVector<Point> getSplinePoints() const;
    /**
     * @brief getSplinePointsAt
     * @param index
     * @return le point de la pline à l'indexe "index", le point (0,0) si l'index est invalide
     */
    Point getSplinePointsAt(int index) const;
    /**
     * @brief setPassingPoints
     * @param pts nouveaus points de passage
     */
    void setPassingPoints(const QVector<Point> &pts);
    /**
     * @brief getPassingPoints
     * @return les points de passage
     */
    QVector<Point> getPassingPoints() const;
    /**
     * @brief getPassingPointAt
     * @param index
     * @return le point de passage à l'indexe "index", le point (0,0) si l'indexe est invalide
     */
    Point getPassingPointAt(int index) const;
    /**
     * @brief insertPassingPoint
     * si l'indexe est valide insère le point "p" à l'indexe "index"
     * @param index
     * @param p
     */
    void insertPassingPoint(int index, Point p);
    /**
     * @brief addPassingPoint
     * ajoute p aux points de passage
     * @param p
     */
    void addPassingPoint(Point p);
    /**
     * @brief setPassingPointsAt
     * si l'indexe est valide remplace le point de passage à l'indexe "index" par le point "p"
     * @param index
     * @param p
     */
    void setPassingPointsAt(int index, Point p);
    /**
     * @brief removePassingPointAt
     * si l'indexe est valide supprime le point de passage à l'indexe "index"
     * @param index
     */
    void removePassingPointAt(int index);
    /**
     * @brief clear
     */
    void clear();

private:
    int degree, dimension, numberOfKnots, numberOfControlPoints;
    QVector<double> knots;
    QVector<Point> controlPoints, splinePoints;
    QVector<QVector<double>> bSplineY, bSplineX; //tableau de tableau de valeurs, premier tableau correspond aux valeurs discrétisées de la premiere b-spline etc.
    QVector<Point> passingPoints;

    double EPS = 0.0000001;


};

#endif // SPLINE_H
