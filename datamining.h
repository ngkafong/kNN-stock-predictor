#ifndef DATAMINING_H
#define DATAMINING_H

#include <QVector>
#include <math.h>

class Dataminer{

public:

    Dataminer();
    QVector<QVector<double>> generateAttributes (QVector<double> arr);
    QVector<QVector<double>> generateTargets (QVector<double> arr);

    void input(QVector<double> arr)
    {
        input_data = arr;
    }
    void trainSet(QVector<QVector<double>> input, QVector<QVector<double>> target)
    {
        trainSet_inputdata = input; trainSet_targetdata = target;
    }

    QVector<QVector<double>> kNN(int k);
    QVector<double> getResultfromKNN(QVector<QVector<double>> kNN);

    double distance (QVector<double> a, QVector<double> b);

    void clear();

private:

    QVector<double> input_data;
    QVector<QVector<double>> trainSet_inputdata;
    QVector<QVector<double>> trainSet_targetdata;
    QVector<double> output;

};



#endif // DATAMINING_H
