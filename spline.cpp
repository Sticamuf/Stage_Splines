#include "spline.h"
#include "point.h"

Spline::Spline(const std::string& fileName): knots{}, controlPoints{}, passingPoints{}
{
    degree = 0;
    numberOfKnots = 0;
    dimension = 0;
    std::ifstream ist(fileName);

    if(ist)
    {
        double boolVal;
        ist>>boolVal;
        if(boolVal == 0){
            ist>>degree>>numberOfKnots>>dimension;
            if(numberOfKnots>=degree+2 && dimension >= 2)
            {
                knots.resize(numberOfKnots);
                numberOfControlPoints = numberOfKnots-(degree+1);
                controlPoints.resize(numberOfControlPoints);
                //read knots
                for(int i = 0; i<numberOfKnots ; ++i)
                {
                    ist>>knots[i];
                }
                //read control points in all dimensions
                for(int i = 0 ; i<numberOfControlPoints ; ++i)
                {
                    controlPoints[i].setNumberOfDimensions(dimension);
                    for(int j = 0 ; j<dimension ; ++j)
                    {
                        double k;
                        ist>>k;
                        controlPoints[i].setValueAtDimension(j,k); //value number i, dimension j
                    }
                }
            }
        }else{
            double numberOfPassingPoints;
            degree=0;
            ist>>numberOfPassingPoints>>dimension;
            passingPoints.resize(numberOfPassingPoints);
            for(int i = 0 ; i<numberOfPassingPoints ; ++i)
            {
                passingPoints[i].setNumberOfDimensions(dimension);
                for(int j = 0 ; j<dimension ; ++j)
                {
                    double k;
                    ist>>k;
//                    if(j == 0){
                        passingPoints[i].setValueAtDimension(j,k);
//                    }else{
//                        //insère une erreur de mesure aux points de passage
//                        passingPoints[i].setValueAtDimension(j,k+(2*rand()/static_cast<double>(RAND_MAX)-1)*1.5);
//                    }
                }
            }
        }
    }
}

Spline::Spline(int degree, int dimension, QVector<double> knots)
{
    this->degree = degree;
    this->dimension = dimension;
    this->knots = knots;
    numberOfKnots = knots.size();
    numberOfControlPoints = 0;
}



Spline::~Spline()
{

}

void Spline::calculateBSplineY(int detail)
{
    //calculer des valeurs des b-splines pour l'affichage

    int numberOfBSplines = numberOfKnots-(degree+1);

    bSplineY.clear();
    bSplineX.clear();

    bSplineX.resize(numberOfBSplines);
    bSplineY.resize(numberOfBSplines);

    double t;

    for(int i = 0 ; i<numberOfBSplines ; i++)
    {
        t = knots[i];
        for(int j = 0 ; j<=degree+1 ; j++){
            double pas = (knots[i+j]-knots[i])/(1.0*detail);
            while (t<knots[i+j])
            {
                double y = calculateBSplineY(degree, i, t);
                bSplineX[i].push_back(t);
                bSplineY[i].push_back(y);
                t += pas;
            }
            //pas sur d'arriver -> dernier point calculé entre knots[i+degree+1]-EPS*pas et knots[i+degree+1]
            if(t-pas < knots[i+j]-EPS*pas){
                double y = calculateBSplineY(degree, i, knots[i+j]-EPS*pas);
                bSplineX[i].push_back(knots[i+j]);
                bSplineY[i].push_back(y);
            }
        }
    }

}

//fonction de récurrence pour calculer les B-Splines, k le degré, i la i-ième B-Spline, t le temps pour lequel on veut le Y
double Spline::calculateBSplineY(int k, int i, double t)
{
    if(k==0)
    {
        if(knots[i]<= t && t < knots[i+1])
        {
            return 1;
        }else
        {
            return 0;
        }
    }else
    {
        if(knots[i+k] != knots[i])
        {
            if(knots[i+k+1] != knots[i+1])
            {
                return (((t-knots[i]) / (knots[i+k]-knots[i])) * calculateBSplineY(k-1, i, t)) + (((knots[i+k+1]-t) / (knots[i+k+1]-knots[i+1])) *  calculateBSplineY(k-1, i+1, t));
            }else{
                return ((t-knots[i]) / (knots[i+k]-knots[i])) * calculateBSplineY(k-1, i, t);
            }
        }else
        {
            if(knots[i+k+1] != knots[i+1])
            {
                return ((knots[i+k+1]-t) / (knots[i+k+1]-knots[i+1])) *  calculateBSplineY(k-1, i+1, t);
            }else
            {
                return 0;
            }
        }
    }
}



Point Spline::deBoor(int indexOfIntervalOfT, double t)
{
    //renvoie le point de la spline au temps t
    QVector<Point> d;
    d.reserve(degree+1);
    for(int j = 0 ; j<degree+1 ; j++)
    {
        d.push_back(controlPoints[j+indexOfIntervalOfT-degree]);
    }
    for(int r = 1 ; r<degree+1 ; r++)
    {
        for(int j = degree ; j>r-1 ; j--)
        {
            double alpha = (t-knots[j+indexOfIntervalOfT-degree]) / (knots[j+1+indexOfIntervalOfT-r] - knots[j+indexOfIntervalOfT-degree]);
            d[j] = d[j-1] * (1-alpha) + d[j] * alpha;
        }
    }
    return d[degree];
}

void Spline::saveCTRLinFile(const std::string &fileName)
{
    //enregistrer les points de controle et les noeuds dans un fichier
    std::ofstream out(fileName+".txt");
    out << '0' << std::endl;
    out << degree << std::endl;
    out << numberOfKnots << std::endl;
    out << dimension << std::endl;

    for(int i = 0 ; i<numberOfKnots ; ++i)
    {
        out << knots[i] << " ";
    }
    out<<std::endl;

    for(int i = 0 ; i<numberOfControlPoints ; ++i)
    {
        for(int j = 0 ; j<dimension ; j++)
            out << controlPoints[i].getValueAtDimension(j)<<" ";
        out << std::endl;
    }
    out.close();
}

void Spline::savePASSinFile(const std::string &fileName)
{
    //enregistrer les points de passage dans un fichier
    std::ofstream out(fileName+".txt");
    out << '1' << std::endl;
    out << passingPoints.length() << std::endl;
    out << dimension << std::endl;

    for(int i = 0 ; i<passingPoints.length() ; ++i)
    {
        for(int j = 0 ; j<dimension ; j++)
            out << passingPoints[i].getValueAtDimension(j)<<" ";
        out << std::endl;
    }
    out.close();
}

void Spline::saveSPLNinFileAsPASS(const std::string &fileName)
{
    //enregistrer les points de la spline dans un fichier pour le souvrir en tant que points de passage
    std::ofstream out(fileName+".txt");
    out << '1' << std::endl;
    out << splinePoints.length() << std::endl;
    out << dimension << std::endl;

    for(int i = 0 ; i<splinePoints.length() ; ++i)
    {
        for(int j = 0 ; j<dimension ; j++)
            out << splinePoints[i].getValueAtDimension(j)<<" ";
        out << std::endl;
    }
    out.close();
}

void Spline::saveKnotsInFile(const std::string &fileName)
{
    //enregistre les noeuds dans un fichier
    std::ofstream out(fileName+".txt");
    out << numberOfKnots << std::endl;
    for(int i = 0 ; i<knots.length() ; i++)
    {
        out << knots[i] << " ";
    }
}

double Spline::getKnotAt(int index) const
{
    if(index>=0 && index<numberOfKnots)
    {
        return knots[index];
    }
    return INT_MAX;
}

int Spline::getNumberOfKnots() const
{
    return numberOfKnots;
}

Point Spline::getControlPointAt(int index) const
{
    if(index >= 0 && index < numberOfControlPoints)
    {
        return controlPoints[index];
    }
    return Point({0,0});
}

void Spline::setControlPointAt(Point p, int index)
{
    if(index < numberOfControlPoints && index >=0)
    {
        controlPoints[index] = p;
    }
}

int Spline::getNumberOfControlPoints() const
{
    return numberOfControlPoints;
}

int Spline::getDimension() const
{
    return dimension;
}

QVector<QVector<double>> Spline::getBSplineY() const
{
    return bSplineY;
}

QVector<QVector<double>> Spline::getBSplineX() const
{
    return bSplineX;
}

QVector<double> Spline::getKnots() const
{
    return knots;
}

QVector<Point> Spline::getControlPoints() const
{
    return controlPoints;
}

void Spline::addControlPoint(Point p)
{
    controlPoints.push_back(p);
    numberOfControlPoints++;
}

void Spline::insertControlPoint(int index, Point p)
{
    if(index<controlPoints.length() && index >= 0){
        controlPoints.insert(index, p);
        numberOfControlPoints++;
    }
}

void Spline::removeControlPointAt(int index)
{
    if(index >= 0 && index < numberOfControlPoints){
        controlPoints.remove(index);
        numberOfControlPoints--;
    }
}

void Spline::setControlPoints(const QVector<Point> &value)
{
    controlPoints = value;
    numberOfControlPoints = controlPoints.length();
}

int Spline::getDegree()
{
    return degree;
}

void Spline::setKnots(const QVector<double> &value)
{
    knots = value;
    numberOfKnots = knots.length();
}

void Spline::setKnots(QString fileName)
{
    std::ifstream ist(fileName.toStdString());
    ist>>numberOfKnots;
    knots.resize(numberOfKnots);
    for(int i = 0 ; i<numberOfKnots ; i++)
    {
        ist>>knots[i];
    }
}

void Spline::setDegree(int value)
{
    degree = value;
}


void Spline::calculateSplinePoints(int detail)
{
    //calcule les points de la spline pou rl'affichage

    splinePoints.clear();
//    std::ofstream out("exempleTemps_test.txt"); //enregistrer un temps pour chaque point de la spline
//    out<<"tp = {";
    double t;
    int i = degree;

    int k = std::min(knots.length()-degree-1, controlPoints.length());

    while(i<k)
    {
        t=knots[i];
        double pas = (knots[i+1]-knots[i])/(1.0*detail);
        while (t<knots[i+1])
        {
            splinePoints.push_back(deBoor(i,t));
//            out<<t<<", ";
            t += pas;
        }
        if(t-pas < knots[i+1]-EPS*pas){
            splinePoints.push_back(deBoor(i,knots[i+1]-EPS*pas));
//            out<<knots[i+1]-EPS*pas<<", ";
        }
        i++;
    }

//    out<<"};";
}

QVector<Point> Spline::getSplinePoints() const
{
    return splinePoints;
}

Point Spline::getSplinePointsAt(int index) const
{
    if(index>=0 && index < splinePoints.length())
        return splinePoints[index];
    return Point();
}

void Spline::setPassingPoints(const QVector<Point> &pts)
{
    passingPoints = pts;
}

QVector<Point> Spline::getPassingPoints() const
{
    return passingPoints;
}

Point Spline::getPassingPointAt(int index) const
{
    if(index >= 0 && index < passingPoints.length())
        return passingPoints[index];
    return Point();
}

void Spline::insertPassingPoint(int index, Point p)
{
    if(index >= 0 && index < passingPoints.length())
        passingPoints.insert(index,p);
}

void Spline::addPassingPoint(Point p)
{
    passingPoints.push_back(p);
}

void Spline::setPassingPointsAt(int index, Point p)
{
    if(index < passingPoints.length() && index >= 0)
        passingPoints[index] = p;
}

void Spline::removePassingPointAt(int index)
{
    if(index >= 0 && index < passingPoints.length()){
        passingPoints.remove(index);
    }
}

void Spline::clear()
{
    degree = 0;
    dimension = 0;
    numberOfKnots = 0;
    numberOfControlPoints = 0;
    knots.clear();
    controlPoints.clear();
    splinePoints.clear();
    bSplineY.clear();
    bSplineX.clear();
    passingPoints.clear();
}

