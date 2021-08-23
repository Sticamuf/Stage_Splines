#include "individual.h"

#include "interpolation.h"
#include "ap.h"
#include "alglibinternal.h"
#include "alglibmisc.h"

#include "point.h"

#include <iostream>

#include<qdebug.h>

std::mt19937 Individual::mt = std::mt19937{static_cast<std::uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};

Individual::Individual(int length, int degree) : genes{}
{
    for(int i = 0 ; i<degree+1 ; i++)
    {
        genes.insert(0);
        genes.insert(1);
    }

    for(int i = 0 ; i<length ; i++)
    {
        genes.insert(generateRand());
    }
}

void Individual::calculateFitness(const QVector<double>& tp, const double *xPoints, const double *yPoints, int degree)
{
    //alglib
    alglib::ae_int_t nbFunctions = genes.size()-(degree+1),
            info0, info1;

    alglib::lsfitreport rep0, rep1;


    alglib::real_2d_array fmatrix;
    alglib::real_1d_array y0, y1, c0, c1;

    int nbPoints = tp.length();

    y0.setcontent(nbPoints, xPoints);
    y1.setcontent(nbPoints, yPoints);

    //calculer le Y de chaque B-Spline en chaque temps tp
    QVector<double> Ytp;
    Ytp.reserve(nbPoints*nbFunctions);
    for(int i = 0 ; i<nbPoints ; i++){
        for(int j = 0 ; j<nbFunctions ; j++)
        {
            Ytp.push_back(calculateBSplineY(degree,j,tp[i]));
        }
    }
    const double *YtpCDP = Ytp.data();

    fmatrix.setcontent(nbPoints, nbFunctions, YtpCDP);

    alglib::lsfitlinear(y0,fmatrix,nbPoints,nbFunctions,info0,c0,rep0);
    alglib::lsfitlinear(y1,fmatrix,nbPoints,nbFunctions,info1,c1,rep1);

    if(info0 != 4 && info1 != 4){

        errX = pow(rep0.rmserror,2)*nbPoints;
        errY = pow(rep1.rmserror,2)*nbPoints;

        ctrlPtsX = c0;
        ctrlPtsY = c1;

        int N = nbPoints;
        int n = genes.size()-(2*(degree+1));
        int m = degree+1;
        double knotError = log(N)*(2*n+m);

        double xErr = (N*(log(errX+1))) + knotError;
        double yErr = (N*(log(errY+1))) + knotError;

        fitness = xErr+yErr;
    }else{
        qDebug()<<"erreur";
    }

}

double Individual::calculateBSplineY(int k, int i, double t)
{
    //fonction récursive calculant le y d'une b-spline pour un temps t donné et un degré donné
    if(k==0)
    {
        double d = *std::next(genes.begin(),i);
        double dPlus1 = *std::next(genes.begin(), i+1);
        if(d <= t && t < dPlus1)
        {
            return 1;
        }else
        {
            return 0;
        }
    }else
    {
        double d = *std::next(genes.begin(),i);
        double dPlusK = *std::next(genes.begin(),i+k);
        double dPlus1 = *std::next(genes.begin(), i+1);
        double dPlusKPlus1 = *std::next(genes.begin(),i+k+1);
        if(dPlusK != d)
        {
            if(dPlusKPlus1 != dPlus1)
            {
                return (((t-d) / (dPlusK-d)) * calculateBSplineY(k-1, i, t)) + (((dPlusKPlus1-t) / (dPlusKPlus1-dPlus1)) *  calculateBSplineY(k-1, i+1, t));
            }else{
                return ((t-d) / (dPlusK-d)) * calculateBSplineY(k-1, i, t);
            }
        }else
        {
            if(dPlusKPlus1 != dPlus1)
            {
                return ((dPlusKPlus1-t) / (dPlusKPlus1-dPlus1)) *  calculateBSplineY(k-1, i+1, t);
            }else
            {
                return 0;
            }
        }
    }
}

double Individual::getFitness() const
{
    return fitness;
}

void Individual::crossover2003(Individual &indi)
{
    //échange tous les gènes entre deux bornes entre cet individu et l'individu en donnée
    double r1 = generateRand();
    double r2 = generateRand();

    if(r1 > r2){
        std::swap(r1,r2);
    }

    std::multiset<double> temp1;

    temp1.insert(genes.lower_bound(r1), genes.upper_bound(r2));
    genes.erase(genes.lower_bound(r1), genes.upper_bound(r2));
    genes.insert(indi.genes.lower_bound(r1), indi.genes.upper_bound(r2));
    indi.genes.erase(indi.genes.lower_bound(r1), indi.genes.upper_bound(r2));
    indi.genes.insert(temp1.begin(), temp1.end());


}



void Individual::mutate2003(double probability, int degree)
{
    //supprimer des gènes existants ou ajouter des gènes aléatoires un nombre de fois proportionnel à la probabilité en paramètre et à la taille de l'individu
    int end = genes.size()-(2*(degree+1));
    double mutate;
    double addOrRem;
    for(int i = 0 ; i<end ; i++){
        mutate = generateRand();
        if(mutate <= probability){
            addOrRem = generateRand();
            if(addOrRem <= 0.5){
                genes.insert(generateRand());
            }else{
                if(genes.size() > static_cast<double>((degree+1)*2)){
                    int index = rand() % (genes.size()-(2*(degree+1)));
                    genes.erase(std::next(genes.begin(),index+degree+1));
                }
            }
        }
    }
}

void Individual::crossoverEqNbGenes_1(Individual& indi, int degree)
{
    //echange le même nombre de gênes des deux individus entre deux bornes

    //generer deux bornes
    double r1 = generateRand();
    double r2 = generateRand();
    if(r1 > r2){
        std::swap(r1,r2);
    }

    std::multiset<double> temp1;
    std::multiset<double> temp2;

    //inserer dans les temporaires les noeuds entre les bornes de chacun des individus
    temp1.insert(genes.lower_bound(r1), genes.upper_bound(r2));
    temp2.insert(indi.genes.lower_bound(r1), indi.genes.upper_bound(r2));
    int length = std::min(temp1.size(), temp2.size());

    if(temp1.size() < temp2.size()){
        int diff = temp2.size()-temp1.size();
        int r = rand() % diff;

        //supprimer les noeuds du plus grand temporaire
        for(int i = 0 ; i<diff ; i++){
            if(static_cast<int>(temp2.size()) > r){
                temp2.erase(std::next(temp2.begin(),r));
            }else{
                temp2.erase(std::prev(temp2.end()));
            }
        }

        //supprimer dans les individus les noeuds qui sont encore dans les temporaires
        genes.erase(genes.lower_bound(r1),genes.upper_bound(r2));
        indi.genes.erase(std::next(indi.genes.begin(),r+degree+1),std::next(indi.genes.begin(), r+length+degree+1));

    }else if(temp2.size() < temp1.size()){
        int diff = temp1.size()-temp2.size();
        int r = rand() % diff;

        //suppimer les noeuds du plus grand temporaire
        for(int i = 0 ; i<diff ; i++){
            if(static_cast<int>(temp1.size()) > r){
                temp1.erase(std::next(temp1.begin(),r));
            }else{
                temp1.erase(std::prev(temp1.end()));
            }
        }

        //supprimer dans les individus les noeuds qui sont encore dans les temporaires
        indi.genes.erase(indi.genes.lower_bound(r1), indi.genes.upper_bound(r2));
        genes.erase(std::next(genes.begin(),r+degree+1), std::next(genes.begin(), r+length+degree+1));
    }else{
        //supprimer dans les individus les neouds qui soont aussi dans les temporaires
        genes.erase(genes.lower_bound(r1), genes.upper_bound(r2));
        indi.genes.erase(indi.genes.lower_bound(r1),indi.genes.upper_bound(r2));
    }

    //réinsérer les noeuds des temporaires dans les individus en croisant
    genes.insert(temp2.begin(), temp2.end());
    indi.genes.insert(temp1.begin(), temp1.end());

}

void Individual::crossoverEqNbGenes_2(Individual &indi, int degree)
{
    //vérifier s'il y a un noeud internne dans chacun des individus
    if(genes.size()-(2*(degree+1)) > 0 && indi.genes.size()-(2*(degree+1)) > 0){
        //trouver un index d'un noeud interne dans l'individu avec le moins de genes
        int index = degree+1 + (rand() % std::min(genes.size()-(2*(degree+1)), indi.genes.size()-(2*(degree+1))));
        //trouver une longueur correspondant au nombre de genes à échanger
        int length = 1 + (rand() % std::min(genes.size()-(degree+1)-index, indi.genes.size()-(degree+1)-index));

        std::multiset<double> temp;

        temp.insert(std::next(genes.begin(),index), std::next(genes.begin(), index+length));
        genes.erase(std::next(genes.begin(),index), std::next(genes.begin(), index+length));
        genes.insert(std::next(indi.genes.begin(), index), std::next(indi.genes.begin(), index+length));
        indi.genes.erase(std::next(indi.genes.begin(), index), std::next(indi.genes.begin(), index+length));
        indi.genes.insert(temp.begin(), temp.end());
    }
}

void Individual::crossoverIdea(Individual &indi)
{
    //l'idée est d'échanger des gênes entre deux bornes dans deux individus, les bornes étant différentes entre les individus
    //j'ai peu d'espoir pour cette idée

    double r1 = generateRand();
    double r2 = generateRand();

    if(r1 > r2){
        std::swap(r1,r2);
    }

    double r3 = generateRand();
    double r4 = generateRand();

    if(r3 > r4){
        std::swap(r3,r4);
    }

    std::multiset<double> temp;

    temp.insert(genes.lower_bound(r1), genes.upper_bound(r2));
    genes.erase(genes.lower_bound(r1), genes.upper_bound(r2));
    genes.insert(indi.genes.lower_bound(r3), indi.genes.upper_bound(r4));
    indi.genes.erase(indi.genes.lower_bound(r3), indi.genes.upper_bound(r4));
    indi.genes.insert(temp.begin(), temp.end());
}

void Individual::mutateNew(double probability, int degree)
{
    //déplacer des gènes un nombre de fois proportionnel à la probabilité en paramètre et à la taille de l'individu
    if( genes.size() > 2.0*(degree+1) ){//éviter modulo 0
        int end = genes.size()-(2*(degree+1));
        double mutate;
        for(int i = 0 ; i<end ; i++){
            mutate = generateRand();
            if(mutate <= probability){

                int index = rand() % (genes.size()-(2*(degree+1)));
                double ui = *std::next(genes.begin(), index+degree+1);

                double sigma = generateRand(); // ici ]0;1[ et pas [0;1]
                double delta = std::min(ui,1-ui);
                ui -= (delta/2.0)*(sigma-0.5);
                genes.erase(std::next(genes.begin(),index+degree+1));
                genes.insert(ui);
            }
        }
    }
}

double Individual::getErrX() const
{
    return errX;
}

double Individual::getErrY() const
{
    return errY;
}

QVector<Point> Individual::getCtrlPts()
{

    QVector<Point> ctrlPts;
    ctrlPts.reserve(ctrlPtsX.length());

    for(int i = 0 ; i<ctrlPtsX.length() ; i++)
    {
        Point p({ctrlPtsX[i], ctrlPtsY[i]});
        ctrlPts.push_back(p);
    }

    return ctrlPts;
}



QVector<double> Individual::getKnots()
{
    QVector<double> knots;

    for(auto it : genes){
        knots.push_back(it);
    }
    return knots;
}

void Individual::setGenes(const std::multiset<double> &newGenes)
{
    genes = newGenes;
}

void Individual::setGenes(const QVector<double> &newGenes)
{
    genes.clear();
    for(int i = 0 ; i < newGenes.length() ; ++i)
    {
        genes.insert(newGenes[i]);
    }
}

void Individual::setMt(const std::mt19937 &newMt)
{
    mt = newMt;
}

bool Individual::operator<(const Individual indi)
{
    return fitness<indi.fitness;
}

double Individual::generateRand()
{
    double r;
    std::uniform_real_distribution<double> dist(0.0,1.0);
    do{
        r = dist(mt);
    }while(r == 0);
    return r;
}

