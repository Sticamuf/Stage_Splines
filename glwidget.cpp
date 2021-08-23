#include "glwidget.h"
#include <QMouseEvent>
#include <qdebug.h>
#include <math.h>

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), spl(0,2,{})
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(format);

}

GLWidget::~GLWidget()
{

}

QVector<Point> GLWidget::getPPoints() const
{
    return spl.getPassingPoints();
}

QVector<Point> GLWidget::getCPoints() const
{
    return spl.getControlPoints();
}

void GLWidget::setSpl(Spline value)
{
    spl = value;
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    left = 0;
    right = width();
    bottom = 0;
    top = height();

    double mult = EPS*pointSizeMultiplier;
    dWid = (right-left)/static_cast<double>(width());
    dHei = (top-bottom)/static_cast<double>(height());
    xMult = mult * dWid;
    yMult = mult * dHei;

    initialisePreviousBorders();
    update();
}


void GLWidget::paintGL()
{

    glLoadIdentity();
    glOrtho(left,right,bottom,top,0.0f,1.0f);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    double x,y;
    double mult = EPS*pointSizeMultiplier;
    dWid = (right-left)/static_cast<double>(width());
    dHei = (top-bottom)/static_cast<double>(height());
    xMult = mult * dWid;
    yMult = mult * dHei;

    //pour tout point de passage
    glColor3f(0.0f, 0.7f,0.0f);
    for(int i = 0 ; i<spl.getPassingPoints().length() ; i++){
        x = spl.getPassingPointAt(i).getValueAtDimension(0);
        y = spl.getPassingPointAt(i).getValueAtDimension(1);
        //afficher ce point sous la forme d'un rectangle
        drawSquareAtCenter(x,y,xMult,yMult);
    }

    //pour tout point de contrôle
    glColor3f(0.0f, 0.0f, 0.0f);
    for(int i = 0 ; i<spl.getNumberOfControlPoints() ; i++)
    {
        x = spl.getControlPointAt(i).getValueAtDimension(0);
        y = spl.getControlPointAt(i).getValueAtDimension(1);
        //afficher ce point sous la forme d'un rectangle
        drawSquareAtCenter(x,y,xMult,yMult);
    }


    if(showLinesPASS){
        //pour tout point de passage sauf le dernier
        glColor3f(0.0f,0.7f,0.0f);
        for(int i = 0 ; i<spl.getPassingPoints().length()-1 ; i++)
        {
            //tracer un trait entre ce point et son successeur
            x = spl.getPassingPointAt(i).getValueAtDimension(0);
            y = spl.getPassingPointAt(i).getValueAtDimension(1);
            glBegin(GL_LINES);
            glVertex2d(x,y);
            glVertex2d(spl.getPassingPointAt(i+1).getValueAtDimension(0),spl.getPassingPointAt(i+1).getValueAtDimension(1));
            glEnd();
        }
    }

    if(showLinesCTRL){
        //pour tout point de contrôle sauf le dernier
        glColor3f(0.0f, 0.0f, 0.0f);
        for(int i = 0 ; i<spl.getNumberOfControlPoints()-1 ; i++)
        {
            //tracer un trait entre ce point et son successeur
            x = spl.getControlPointAt(i).getValueAtDimension(0);
            y = spl.getControlPointAt(i).getValueAtDimension(1);
            glBegin(GL_LINES);
            glVertex2d(x,y);
            glVertex2d(spl.getControlPointAt(i+1).getValueAtDimension(0),spl.getControlPointAt(i+1).getValueAtDimension(1));
            glEnd();
        }
    }

    if(controlP){
        //afficher le point de contrôle selectionné en rouge
        if(setRedPointIndex>=0 && setRedPointIndex<spl.getNumberOfControlPoints()){

            x = spl.getControlPointAt(setRedPointIndex).getValueAtDimension(0);
            y = spl.getControlPointAt(setRedPointIndex).getValueAtDimension(1);

            glColor3f(1.0f, 0.0f, 0.0f);
            drawSquareAtCenter(x,y,xMult,yMult);
        }

        //afficher la ligne selectionnée entre deux points de contrôle en rouge
        if(setRedLineIndex >= 0 && setRedLineIndex<spl.getNumberOfControlPoints()-1){
            x = spl.getControlPointAt(setRedLineIndex).getValueAtDimension(0);
            y = spl.getControlPointAt(setRedLineIndex).getValueAtDimension(1);

            glBegin(GL_LINES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2d(x,y);
            glVertex2d(spl.getControlPointAt(setRedLineIndex+1).getValueAtDimension(0),spl.getControlPointAt(setRedLineIndex+1).getValueAtDimension(1));
            glEnd();
        }
    }else{
        //afficher le point de passage selectionné en rouge
        if(setRedPointIndex>=0 && setRedPointIndex<spl.getPassingPoints().length()){

            x = spl.getPassingPointAt(setRedPointIndex).getValueAtDimension(0);
            y = spl.getPassingPointAt(setRedPointIndex).getValueAtDimension(1);

            glColor3f(1.0f, 0.0f, 0.0f);
            drawSquareAtCenter(x,y,xMult,yMult);
        }

        //afficher la ligne selectionnée entre deux points de passage en rouge
        if(setRedLineIndex >= 0 && setRedLineIndex<spl.getPassingPoints().length()-1){
            x = spl.getPassingPointAt(setRedLineIndex).getValueAtDimension(0);
            y = spl.getPassingPointAt(setRedLineIndex).getValueAtDimension(1);

            glBegin(GL_LINES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2d(x,y);
            glVertex2d(spl.getPassingPointAt(setRedLineIndex+1).getValueAtDimension(0),spl.getPassingPointAt(setRedLineIndex+1).getValueAtDimension(1));
            glEnd();
        }
    }

    double x1, y1, x2, y2;

    //afficher la spline
    glColor3f(0.0f,0.0f,1.0f);
    bool draw;
    //pour chaque point sauf le dernier, si les conditions sont bonnes, afficher un segment entre lui et son successeur
    for(int i = 0 ; i<spl.getSplinePoints().length()-1 ; i++)
    {
        draw = true;
        //pour chaque point de controle
        for(int j = 0 ; j<spl.getControlPoints().length() && draw==true ; j++)
        {
            //si le point de la spline est suffisamment proche du point de controle
            if(Point::abs(spl.getControlPointAt(j)-spl.getSplinePointsAt(i)) < EPS4)
            {
                //pour chaque point de controle restant
                for(int k = j+1 ; k<spl.getControlPoints().length() && draw == true ; k++)
                {
                    //si le prochain point de la spline est suffisamment proche du point de controle
                    if(Point::abs(spl.getControlPointAt(k)-spl.getSplinePointsAt(i+1)) < EPS4)
                    {
                        //ne pas afficher le segment entre les deux points de la spline
                        draw = false;
                    }
                }
            }
        }
        if(draw)
        {
            x1 = spl.getSplinePointsAt(i).getValueAtDimension(0);
            y1 = spl.getSplinePointsAt(i).getValueAtDimension(1);
            x2 = spl.getSplinePointsAt(i+1).getValueAtDimension(0);
            y2 = spl.getSplinePointsAt(i+1).getValueAtDimension(1);

            glBegin(GL_LINES);
            glVertex2d(x1,y1);
            glVertex2d(x2,y2);
            glEnd();
        }
    }

    glFlush();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    //mettre les bordures précédentes sur les bordures courantes
    initialisePreviousBorders();

    selectedPointIndex = -1;
    Point p({ getShifted(event->x(),0,width(),left,right), getShifted(event->y(),0,height(),top, bottom) });


    double xDelta, yDelta, smallestXDelta = 0, smallestYDelta = 0;

    if(controlP){
        //pour chaque point de controle
        for(int i = 0 ; i<spl.getNumberOfControlPoints() ; i++)
        {
            xDelta = abs(p.getValueAtDimension(0)-spl.getControlPointAt(i).getValueAtDimension(0));
            yDelta = abs(p.getValueAtDimension(1)-spl.getControlPointAt(i).getValueAtDimension(1));
            //verifier si l'on a cliqué proche d'un point, marquer le plus proche comme selectionné s'il est assez proche
            if( xDelta < xMult &&  yDelta < yMult)
            {
                if(selectedPointIndex == -1){
                    selectedPointIndex = i;
                    smallestXDelta = abs(p.getValueAtDimension(0)-spl.getControlPointAt(selectedPointIndex).getValueAtDimension(0));
                    smallestYDelta = abs(p.getValueAtDimension(1)-spl.getControlPointAt(selectedPointIndex).getValueAtDimension(1));
                }else if(xDelta < smallestXDelta && yDelta < smallestYDelta ){
                    selectedPointIndex = i;
                }
            }
        }

        //s'il n'y a pas de point selectionné, translation en cours
        if(selectedPointIndex == -1)
        {
            translateOriginX = event->x();
            translateOriginY = event->y();
        }//s'il y a un point selectionné et bouton droit de la souris a déclenché la fonction, supprimer ce point
        else if(event->buttons() == Qt::RightButton)
        {
            spl.removeControlPointAt(selectedPointIndex);
            selectedPointIndex = -1;
            emit pointsChanged(spl.getControlPoints());
        }
    }else{
        //pour chaque point de passage
        for(int i = 0 ; i<spl.getPassingPoints().length() ; i++)
        {
            xDelta = abs(p.getValueAtDimension(0)-spl.getPassingPointAt(i).getValueAtDimension(0));
            yDelta = abs(p.getValueAtDimension(1)-spl.getPassingPointAt(i).getValueAtDimension(1));
            //verifier si l'on a cliqué proche d'un point, marquer le plus proche comme selectionné s'il est assez proche
            if( xDelta < xMult &&  yDelta < yMult)
            {
                if(selectedPointIndex == -1){
                    selectedPointIndex = i;
                    smallestXDelta = abs(p.getValueAtDimension(0)-spl.getPassingPointAt(selectedPointIndex).getValueAtDimension(0));
                    smallestYDelta = abs(p.getValueAtDimension(1)-spl.getPassingPointAt(selectedPointIndex).getValueAtDimension(1));
                }else if(xDelta < smallestXDelta && yDelta < smallestYDelta ){
                    selectedPointIndex = i;
                }
            }
        }

        //s'il n'y a pas de point selectionné, translation en cours
        if(selectedPointIndex == -1)
        {
            translateOriginX = event->x();
            translateOriginY = event->y();
        }//s'il y a un point selectionné et bouton droit de la souris a déclenché la fonction, supprimer ce point
        else if(event->buttons() == Qt::RightButton)
        {
            spl.removePassingPointAt(selectedPointIndex);
            selectedPointIndex = -1;
            emit pointsChanged(spl.getPassingPoints());
        }
    }


}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //si un point était selectionné
    if(selectedPointIndex != -1)
    {
        //récupérer la position de la souris
        Point p({ getShifted(event->x(),0,width(),left,right), getShifted(event->y(),0,height(),top, bottom) });
        if(controlP){
            //mettre le point de contrôle selectionné à la position de la souris
            spl.setControlPointAt(p,selectedPointIndex);
            emit pointsChanged(spl.getControlPoints());
        }else{
            //mettre le point de passage selectionné à la position de la souris
            spl.setPassingPointsAt(selectedPointIndex, p);
            emit pointsChanged(spl.getPassingPoints());
        }
        //déselectionner le point
        selectedPointIndex = -1;
    }

    initialisePreviousBorders();
    translateEndX = 0;
    translateEndY = 0;
    translateOriginX = 0;
    translateOriginY = 0;

    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    //si le bouton gauche de la souris est maintenu
    if(event->buttons() == Qt::LeftButton){
        //si on a selectionné un point, bouger ce point
        if(selectedPointIndex != -1){
            Point p({ getShifted(event->x(),0,width(),left,right), getShifted(event->y(),0,height(),top, bottom) });
            if(controlP){
                spl.setControlPointAt(p, selectedPointIndex);
                emit pointsChanged(spl.getControlPoints());
            }else{
                spl.setPassingPointsAt(selectedPointIndex, p);
                emit pointsChanged(spl.getPassingPoints());
            }


        }//aucun point selectionné : translater l'image
        else{
            translateEndX = event->x();
            translateEndY = event->y();

            calculateTotalTranslate();
        }
    }//bouton non maintenu : afficher en rouge le point au dessus duquel on se situe
    else{
        Point p({ getShifted(event->x(),0,width(),left,right), getShifted(event->y(),0,height(),top, bottom) });
        setRedPointIndex = -1;
        setRedLineIndex = -1;
        double xDelta, yDelta, smallestXDelta = 0, smallestYDelta = 0;

        //manipuler les points de controle
        if(controlP){
            //pour chaque point
            for(int i = 0 ; i<spl.getNumberOfControlPoints() ; i++)
            {
                xDelta = abs(p.getValueAtDimension(0)-spl.getControlPointAt(i).getValueAtDimension(0));
                yDelta = abs(p.getValueAtDimension(1)-spl.getControlPointAt(i).getValueAtDimension(1));
                //verifier si l'on est proche d'un point
                if( xDelta < xMult &&  yDelta < yMult)
                {
                    if(setRedPointIndex == -1){
                        setRedPointIndex = i;
                        smallestXDelta = abs(p.getValueAtDimension(0)-spl.getControlPointAt(setRedPointIndex).getValueAtDimension(0));
                        smallestYDelta = abs(p.getValueAtDimension(1)-spl.getControlPointAt(setRedPointIndex).getValueAtDimension(1));
                    }else if(xDelta < smallestXDelta && yDelta < smallestYDelta ){
                        setRedPointIndex = i;
                    }
                }
            }
            //si aucun point n'est marqué en rouge, verifier si on se situe sur un segment entre deux points de contrôle
            if(setRedPointIndex == -1)
            {
                double xInf, xSup, yInf, ySup;
                double mouseX = p.getValueAtDimension(0);
                double mouseY = p.getValueAtDimension(1);
                for(int i = 0 ; i<spl.getNumberOfControlPoints()-1 ; i++)
                {
                    xInf = spl.getControlPointAt(i).getValueAtDimension(0);
                    yInf = spl.getControlPointAt(i).getValueAtDimension(1);
                    xSup = spl.getControlPointAt(i+1).getValueAtDimension(0);
                    ySup = spl.getControlPointAt(i+1).getValueAtDimension(1);

                    double pVect = (xSup-xInf)*(mouseY-yInf)-(ySup-yInf)*(mouseX-xInf);

                    if(std::abs(pVect) <= EPS3) //veifier alignement par produit vectoriel
                    {
                        if(xInf > xSup) std::swap(xInf, xSup);
                        if(yInf > ySup) std::swap(yInf, ySup);
                        if(mouseX >= xInf - EPS2*dWid && mouseX <= xSup + EPS2*dWid && mouseY >= yInf - EPS2*dHei && mouseY <= ySup + EPS2*dHei ) //verifier appartenance au rectangle
                            setRedLineIndex = i;
                    }

                }
            }
        }
        //manipuler les points de passage
        else{
            //pour chaque point
            for(int i = 0 ; i<spl.getPassingPoints().length() ; i++)
            {
                xDelta = abs(p.getValueAtDimension(0)-spl.getPassingPointAt(i).getValueAtDimension(0));
                yDelta = abs(p.getValueAtDimension(1)-spl.getPassingPointAt(i).getValueAtDimension(1));
                //verifier si l'on est proche d'un point
                if( xDelta < xMult &&  yDelta < yMult)
                {
                    if(setRedPointIndex == -1){
                        setRedPointIndex = i;
                        smallestXDelta = abs(p.getValueAtDimension(0)-spl.getPassingPointAt(setRedPointIndex).getValueAtDimension(0));
                        smallestYDelta = abs(p.getValueAtDimension(1)-spl.getPassingPointAt(setRedPointIndex).getValueAtDimension(1));
                    }else if(xDelta < smallestXDelta && yDelta < smallestYDelta ){
                        setRedPointIndex = i;
                    }
                }
            }
            //si aucun point n'est marqué en rouge, verifier si on se situe sur un segment entre deux points de contrôle
            if(setRedPointIndex == -1)
            {
                double xInf, xSup, yInf, ySup;
                double mouseX = p.getValueAtDimension(0);
                double mouseY = p.getValueAtDimension(1);
                for(int i = 0 ; i<spl.getPassingPoints().length()-1 ; i++)
                {
                    xInf = spl.getPassingPointAt(i).getValueAtDimension(0);
                    yInf = spl.getPassingPointAt(i).getValueAtDimension(1);
                    xSup = spl.getPassingPointAt(i+1).getValueAtDimension(0);
                    ySup = spl.getPassingPointAt(i+1).getValueAtDimension(1);

                    double pVect = (xSup-xInf)*(mouseY-yInf)-(ySup-yInf)*(mouseX-xInf);

                    if(std::abs(pVect) <= EPS3) //veifier alignement par produit vectoriel
                    {
                        if(xInf > xSup) std::swap(xInf, xSup);
                        if(yInf > ySup) std::swap(yInf, ySup);
                        if(mouseX >= xInf - EPS2*dWid && mouseX <= xSup + EPS2*dWid && mouseY >= yInf - EPS2*dHei && mouseY <= ySup + EPS2*dHei) //verifier appartenance au rectangle
                            setRedLineIndex = i;
                    }

                }
            }
        }
    }
    update();

}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Point p({ getShifted(event->x(),0,width(),left,right), getShifted(event->y(),0,height(),top, bottom) });
    if(setRedLineIndex != -1)
    {
        if(controlP){
            //ajouter un point de contrôle
            spl.insertControlPoint(setRedLineIndex + 1, p);
            emit pointsChanged(spl.getControlPoints());
        }else{
            //ajouter un point de passage
            spl.insertPassingPoint(setRedLineIndex + 1, p);
            emit pointsChanged(spl.getPassingPoints());
        }
    }else{
        if(controlP){
            //ajouter un point de contrôle entre deux points
            spl.addControlPoint(p);
            emit pointsChanged(spl.getControlPoints());
        }else{
            //ajouter un point de passage entre deux points
            spl.addPassingPoint(p);
            emit pointsChanged(spl.getPassingPoints());
        }
    }
    update();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    double x = event->position().x()/(1.0*width());
    double y = event->position().y()/(1.0*height());

    QPoint numDegrees = event->angleDelta();
    if(numDegrees.y() < 0)//dezoom
    {
        left -= (right-left)*0.1*x;
        right += (right-left)*0.1*(1-x);
        top += (top-bottom)*0.1*y;
        bottom -= (top-bottom)*0.1*(1-y);
    }else if(numDegrees.y() > 0)//zoom
    {
        left += (right-left)*0.1*x;
        right -= (right-left)*0.1*(1-x);
        top -= (top-bottom)*0.1*y;
        bottom += (top-bottom)*0.1*(1-y);
    }
    initialisePreviousBorders();
    update();
}

void GLWidget::drawSquareAtCenter(double x, double y, double xMult, double yMult)
{

    glBegin(GL_QUADS);
    glVertex2d(x-xMult, y-yMult);
    glVertex2d(x+xMult, y-yMult);
    glVertex2d(x+xMult, y+yMult);
    glVertex2d(x-xMult, y+yMult);
    glEnd();
}

Spline GLWidget::getSpl() const
{
    return spl;
}

void GLWidget::setPassingPointAt(int index, Point p)
{
    spl.setPassingPointsAt(index, p);
    update();
}

void GLWidget::setControlPointAt(int index, Point p)
{
    spl.setControlPointAt(p, index);
    update();
}

double GLWidget::getShifted(double t, double a, double b, double c, double d)
{
    //https://math.stackexchange.com/questions/914823/shift-numbers-into-a-different-range/914843
    return c + ((d-c) / (b-a)) * (t-a);
}

void GLWidget::calculateTotalTranslate()
{
    double lr = (right-left)/(1.0*width());
    double bt = (top-bottom)/(1.0*height());

    double tLeft = previousLeft - (translateEndX-translateOriginX) * lr;
    double tRight = previousRight - (translateEndX-translateOriginX) * lr;
    double tBottom = previousBottom + (translateEndY-translateOriginY) * bt;
    double tTop = previousTop + (translateEndY-translateOriginY) * bt;

    left = tLeft;
    right = tRight;
    bottom = tBottom;
    top = tTop;
}

void GLWidget::setPointSizeMultiplier(double value)
{
    pointSizeMultiplier = value;
    update();
}

void GLWidget::setShowLinesCTRL(bool value)
{
    showLinesCTRL = value;
    update();
}

void GLWidget::setShowLinesPASS(bool value)
{
    showLinesPASS = value;
    update();
}

void GLWidget::redimension()
{
    left = 0;
    right = width();
    bottom = 0;
    top = height();

    initialisePreviousBorders();

    update();
}

void GLWidget::adapt()
{
    //adapter l'affichage aux extremes parmi les deux types de points
    if(spl.getNumberOfControlPoints() > 0){
        left = spl.getControlPointAt(0).getValueAtDimension(0);
        right = spl.getControlPointAt(0).getValueAtDimension(0);
        bottom = spl.getControlPointAt(0).getValueAtDimension(1);
        top = spl.getControlPointAt(0).getValueAtDimension(1);
    }else if(spl.getPassingPoints().length() > 0){
        left = spl.getPassingPointAt(0).getValueAtDimension(0);
        right = spl.getPassingPointAt(0).getValueAtDimension(0);
        top = spl.getPassingPointAt(0).getValueAtDimension(1);
        bottom = spl.getPassingPointAt(0).getValueAtDimension(1);
    }

    double x,y;
    for(int i = 1 ; i<spl.getNumberOfControlPoints() ; i++)
    {
        x = spl.getControlPointAt(i).getValueAtDimension(0);
        y = spl.getControlPointAt(i).getValueAtDimension(1);
        if(x<left){
            left = x;
        }else if(x>right){
            right = x;
        }
        if(y<bottom){
            bottom = y;
        }else if(y>top){
            top = y;
        }
    }

    for(int i = 0 ; i<spl.getPassingPoints().length() ; i++)
    {
        x = spl.getPassingPointAt(i).getValueAtDimension(0);
        y = spl.getPassingPointAt(i).getValueAtDimension(1);
        if(x<left){
            left = x;
        }else if(x>right){
            right = x;
        }
        if(y<bottom){
            bottom = y;
        }else if(y>top){
            top = y;
        }
    }

    //si les min et max sont égaux, éviter de faire une hauteur ou largeur nulle
    if(bottom == top){
        bottom = 0;
        top = height();
    }
    if(left == right){
        left = 0;
        right = width();
    }
    initialisePreviousBorders();

    update();
}

void GLWidget::changeTypeOfPoints()
{
    controlP = !controlP;
    if(controlP){
        emit pointsChanged(spl.getControlPoints());
    }else{
        emit pointsChanged(spl.getPassingPoints());
    }
}

void GLWidget::resizeEvent(QResizeEvent *e)
{
    QOpenGLWidget::resizeEvent(e);
    redimension();
}


void GLWidget::initialisePreviousBorders()
{
    previousLeft = left;
    previousRight = right;
    previousBottom = bottom;
    previousTop = top;
}

