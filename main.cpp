#include <iostream>
#include "util.h"
#include "data.h"
#include "query.h"
#include "record.h"
int main()
{
    //char database[] = "kosarak";
    //char database[] = "retail";
    char database[] = "lastfm";
    //char database[] = "tweets";

    char* datafile = generateDatafile(database);
    char* queryfile = generateQueryfile(database);
    Data* data = new Data(datafile, queryfile);
    data->readData();

    LMBQ lmbq(*data);
    //double delta = 0.5;
    //double delta = 0.6;
    double delta = 0.7;
    //double delta = 0.8;
    //double delta = 0.9;


    //lmbq.LMBQ_SD(delta);//对称差
    //lmbq.LMBQ_SD_PV(delta);
    lmbq.LMBQ_SD_PV_NONE(delta);
    //lmbq.LMBQ_Test(delta);//单向差
    //lmbq.LMBQ_Test_PV(delta);
    //lmbq.LMBQ_Test_PV_NONE(delta);


    //lmbq.LMBQ_UD(delta);//SIGIR的非对称差
    //lmbq.LMBQ_UD_PV(delta);

    //更新
    //lebq.searchAfterInsert(delta);
    //lebq.searchAfterDelete(delta);

    return 0;
}
