#include<iostream>
#include<vector>
#include<string>
#include<pthread.h>



class Pthread{
private:
    pthread_t _tid;
    std::string _pname;
private:
    Pthread(pthread_t tid,std::string pname)
    :_tid(tid),_pname(pname)
    {
    }
};

//单例模式线程池
class Pthread_Pool{
private:
    std::vector<Pthread> pthreads;
public:
    void *Pthread_Run(void *arg)
    {
        Pthread* pthread = static_cast<Pthread*>(arg);
        std::string pthid;

    }
private:
    //构造函数私有化
    Pthread_Pool()
    {
        pthreads.resize(5);
        for(int i = 0;i<pthreads.size();i++)
        {
            pthread_t tid;
            
        }
    }    
};