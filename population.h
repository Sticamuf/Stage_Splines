#ifndef POPULATION_H
#define POPULATION_H

#include<QVector>
#include"individual.h"

class Population
{
public:
    Population(int populationSize, /*double lambda*/int length, int degree, const QVector<Point>& points,bool oldMethod = false);

    /**
     * @brief compute2003
     * lance l'algorithme d'optimisation de 2003
     */
    void compute2003();
    /**
     * @brief computeNew
     * lance l'algorithme d'optimisation amélioré
     */
    void computeNew();

    Individual *getBestIndividual() const;

    QVector<double> getTp() const;

private:
    std::vector<Individual> individuals;
    double avgFitness;
    Individual *bestIndividual;
    double *xPoints, *yPoints;
    QVector<double> tp;
    int degree;
    const double EPS = 0.0000001;
    QVector<Point> pPoints;

    void calcFitness2003(int index1, int index2, int gen);
    void selection2003(int &index1, int &index2);
    void crossover2003(int index1, int index2);
    void mutation2003(int index);

    Individual child1, child2;
    std::vector<Individual> nextGeneration;
    /**
     * @brief selectionNew
     * choisis par tournoi binaire deux indexes différents
     * @param index1
     * @param index2
     */
    void selectionNew(int &index1, int &index2);

};

#endif // POPULATION_H
