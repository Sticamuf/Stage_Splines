#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <set>
#include<QVector>
#include <random>
#include<chrono>

#include "interpolation.h"
#include "ap.h"
#include "alglibinternal.h"
#include "alglibmisc.h"

class Point;

class Individual
{
public:
    Individual(int length, int degree);

    //tp le tableau des temps auxquels doit passer la spline pour chaque point de passage
    void calculateFitness(const QVector<double>& tp, const double *xPoints, const double *yPoints, int degree);

    //calcule la fitness de l'individu en adaptant le tableau des temps
    void calculateFitnessAdaptTime(QVector<double> tp, const double *xPoints, const double *yPoints, int degree, QVector<Point> pPoints);

    //renvoie le point de la plsine au temps t
    Point deBoor(int indexOfIntervalOfT, double t, int degree, const QVector<Point> controlPoints);

    //k = degré, t = temps, i = i-ème b-spline
    double calculateBSplineY(int k,int i, double t);

    /**
     * @brief getFitness
     * @return fitness de l'individu
     */
    double getFitness() const;

    /**
     * @brief crossover2003
     * échanger tous les genes entre deux bornes aléatoires de cet indiidu et celui en paramètre
     * @param indi autre individu avec lequel on fait le croisement
     */
    void crossover2003(Individual& indi);
    /**
     * @brief mutate2003
     * mute l'individu à la manière de l'article "Data fitting with a spline using a real-coded genetic algorithm"
     * @param probability la probabilité qu'un gêne soit muté
     * @param degree le degré de la spline
     */
    void mutate2003(double probability, int degree);

    /**
     * @brief crossoverEqNbGenes_1
     * échanger un nombre égal de gênes de cet individu avec l'individu en paramêtre en utilisant deux bornes aléatoires
     * @param indi
     * @param degree
     */
    void crossoverEqNbGenes_1(Individual& indi, int degree);
    /**
     * @brief crossoverEqNbGenes_2
     * échanger un nombre égal de gênes entre cet individu et l'individu en paramètre en utilisant un index et une taille
     * @param indi
     * @param degree
     */
    void crossoverEqNbGenes_2(Individual& indi, int degree);

    /**
     * @brief crossoverIdea
     * échanger tous les genes entre deux bornes aléatoires de cet individu avec tous les gênes entre deux autres bornes aléatoires de l'individu en paramètre
     * @param indi
     */
    void crossoverIdea(Individual &indi);

    /**
     * @brief mutateNew
     * déplace les gênes selon une probabilité
     * @param probability
     * @param degree
     */
    void mutateNew(double probability, int degree);

    double getErrX() const;

    double getErrY() const;

    QVector<Point> getCtrlPts();
    QVector<double> getKnots();

    void setGenes(const std::multiset<double> &newGenes);
    void setGenes(const QVector<double> &newGenes);

    static void setMt(const std::mt19937 &newMt);

    bool operator<(const Individual indi);

private:
    std::multiset<double> genes;
    double errX, errY;
    double fitness;

    alglib::real_1d_array ctrlPtsX;
    alglib::real_1d_array ctrlPtsY;

    static std::mt19937 mt;
    static double EPS;

    /**
     * @brief generateRand
     * @return un nombre aléatoire dans l'intervalle ]0;1[
     */
    double generateRand();
};

#endif // INDIVIDUAL_H
