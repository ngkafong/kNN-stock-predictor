#include "datamining.h"

Dataminer::Dataminer()
{
    // Do nothing
}


QVector<QVector<double>> Dataminer::generateAttributes (QVector<double> arr)
{
    // Generate Attributes - Percentage change compare to previous days' recorded price
    QVector<QVector<double>> table;
    for (int i=0; i < arr.size()-30; i++)
    {
        double todayclose = arr[i];
        double diff1day = (arr[i+1]-todayclose)/todayclose;
        double diff2day = (arr[i+2]-todayclose)/todayclose;
        double diff3day = (arr[i+3]-todayclose)/todayclose;
        double diff5day = (arr[i+5]-todayclose)/todayclose;
        double diff10day = (arr[i+10]-todayclose)/todayclose;
        double diff20day = (arr[i+20]-todayclose)/todayclose;
        double diff30day = (arr[i+30]-todayclose)/todayclose;
        table.append({diff1day,diff2day,diff3day,diff5day,diff10day,diff20day,diff30day});
    }
    return table;
}

QVector<QVector<double>> Dataminer::generateTargets (QVector<double> arr)
{
    // Generate Targets - Percentage change compare to future days' recorded price
    // Start from 8th record in the array because first 7 days won't have records of their future 7 days stock
    QVector<QVector<double>> table;
    for (int i=7; i < arr.size()-30; i++)
    {
        double todayclose = arr[i];
        double diff1day = (arr[i-1]-todayclose)/todayclose;
        double diff2day = (arr[i-2]-todayclose)/todayclose;
        double diff3day = (arr[i-3]-todayclose)/todayclose;
        double diff4day = (arr[i-4]-todayclose)/todayclose;
        double diff5day = (arr[i-5]-todayclose)/todayclose;
        double diff6day = (arr[i-6]-todayclose)/todayclose;
        double diff7day = (arr[i-7]-todayclose)/todayclose;
        table.append({diff1day,diff2day,diff3day,diff4day,diff5day,diff6day,diff7day});
    }
    return table;
}

double Dataminer::distance(QVector<double> a, QVector<double> b)
{
    // Calculate distance using values from the two arrays
    double d = 0;
    for (int i = 0; i< a.size(); i++)
    {
        d += pow((a[i] - b[i]),2);
    }
    return pow(d, 0.5);
}

QVector<QVector<double>> Dataminer::kNN(int k)
{
    // Generate kNN results with the requested value of k
    QVector<QVector<double>> table;
    QVector<double> sq_diff;

    for(int i =0; i< trainSet_inputdata.size(); i++)
    {
        double curr_diff = distance(trainSet_inputdata[i], input_data);
        for(int j=0; j<k; j++)
        {
            // Result store when list not yet full or shorter distance calculated
            if (j == sq_diff.size() || curr_diff < sq_diff[j])
            {
                // Store the record
                sq_diff.insert(j, curr_diff);
                table.insert(j,trainSet_targetdata[i]);
                // if out of k size
                if (sq_diff.size() > k)
                {
                    sq_diff.removeLast();
                    table.removeLast();
                }
                break;
            }
        }
    }
    return table;
}

QVector<double> Dataminer::getResultfromKNN(QVector<QVector<double>> kNN)
{
    // Collect data needed from kNN function
    // Calculate the mean value of future percentage change of nearest neighbors' records
    QVector<double> result;
    for (int i =  0; i < kNN[0].size() ; ++i)
    {
        double j = 0;
        for (int k = 0; k < kNN.size(); ++k) { j += kNN[k][i]; }
        result.append(j/kNN.size());
    }

    return result;

}

void Dataminer::clear()
{
    input_data = {};
    trainSet_inputdata = {{}};
    trainSet_targetdata = {{}};
    output = {};
}
