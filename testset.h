#ifndef TESTSET_H
#define TESTSET_H
#include <vector>
#include <string>
#include <chrono>
#include <QtCharts/QBarSet>

typedef std::chrono::time_point<std::chrono::system_clock> timepnt;
typedef std::chrono::microseconds timedur;

class TestSet
{
public:
    /**
    * Function:getTestSets
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     static TestSet *getTestSets();
    *
    * Return:
    *
    * Notes: gets the instance of TestSet or initializes one if there is none currently saved.
    * This allows the class to act as a singleton
    */
    static TestSet *getTestSets();
    /**
    * Function:destroyTestSets
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     static void destroyTestSets();
    *
    * Return:
    *
    * Notes: destroys the TestSet instance, functions as the means for cleanup
    */
    static void destroyTestSets();
    //instance variable
    static TestSet *instance;
    /**
    * Function:inputCode
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface
    *     void inputCode(std::string code);
    *
    * Return:
    *
    * Notes: applys the compressed stats data to the server test set
    */
    void inputCode(const std::string& code);
    /**
    * Function:outputCode
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     std::string outputCode();
    *
    * Return:
    *
    * Notes: gets the compresed stats data for the server test set
    */
    std::string outputCode();
    /**
    * Function:addToTest
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void addToTest(double sent, double lost, double size);
    *
    * Return:
    *
    * Notes: adds the packet data to the test case and internally manages the time taken
    */
    void addToTest(double sent, double lost, double size);
    /**
    * Function:newTest
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void newTest(std::string protocol);
    *
    * Return:
    *
    * Notes: move to a new test set, archives the last one or creates a first one if internally there are no tests
    */
    void newTest(std::string protocol);
    /**
    * Function:extractSets
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void extractSets(QtCharts::QBarSet *outTotal, QtCharts::QBarSet *outLost, QtCharts::QBarSet *outSize);
    *
    * Return:
    *
    * Notes: writes the integer data from the test cases into the QBarSets for displaying on the stats window
    */
    void extractSets(QtCharts::QBarSet *outTotal, QtCharts::QBarSet *outLost, QtCharts::QBarSet *outSize);
    /**
    * Function:size
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     int size();
    *
    * Return:
    *
    * Notes:
    */
    int size();
    /**
    * Function:clear
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void clear();
    *
    * Return:
    *
    * Notes: clears internal test sets, newTest must be called before more data is added after this call
    */
    void clear();
    /**
    * Function:at
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     std::tuple<int,int,int,std::string,timedur> at(int index);
    *
    * Return:
    *
    * Notes: returns the test set at the specified index
    */
    std::tuple<int,int,int,std::string,timedur> at(int index);
private:
    //total sent, total lost, total size
    std::vector<std::tuple<int,int,int,std::string, timedur>> tests;
    timepnt start, newest;
    /**
    * Function:TestSet
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     TestSet();
    *
    * Return:
    *
    * Notes: private constructor to allow class to be a singleton as there should only ever be one set of tests
    * called in getTestSet if there is no current instance.
    */
    TestSet();
};

#endif // TESTSET_H
