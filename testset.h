#ifndef TESTSET_H
#define TESTSET_H
#include <vector>
#include <QtCharts/QBarSet>

class TestSet
{
public:
    static TestSet *getTestSets();
    static void destroyTestSets();
    static TestSet *instance;

    void addToTest(double sent, double lost, double total);
    void newTest();
    void extractSets(QtCharts::QBarSet *outTotal, QtCharts::QBarSet *outLost, QtCharts::QBarSet *outSize);
    int size();
    void clear();
private:
    std::vector<double> testsSent;
    std::vector<double> testsLost;
    std::vector<double> testsSize;
    TestSet();
};

#endif // TESTSET_H
