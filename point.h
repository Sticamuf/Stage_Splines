#ifndef POINT_H
#define POINT_H

#include <QVector>

class Point
{
public:
    Point(QVector<double> z);
    Point();

    QVector<double> getPoint() const;
    void setPoint(QVector<double> &p);

    /**
     * @brief getNumberOfDimensions
     * @return nombre de dimensions du point
     */
    int getNumberOfDimensions() const;
    /**
     * @brief setNumberOfDimensions
     * @param number nouveau nombre de dimensions du point
     */
    void setNumberOfDimensions(int number);
    /**
     * @brief getValueAtDimension
     * retourne la valeur du point dans la dimension du paramètre "dimension"
     * @param dimension
     * @return la valeur du point dans la dimension du paramètre "dimension"
     */
    double getValueAtDimension(int dimension) const;
    /**
     * @brief setValueAtDimension
     * met la valeur du point dans la dimension "dimension" à la valeur "value"
     * @param dimension
     * @param value
     */
    void setValueAtDimension(int dimension, double value);

    Point operator*(const double& val);
    Point operator+(const Point& p);
    bool operator==(const Point& p);
    Point operator-(const Point& p);
    bool operator<(const double& val);

    static Point abs(const Point& p);

    std::string toString();

private:
    QVector<double> point;
};

#endif // POINT_H
