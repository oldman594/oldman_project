#include<pthread.h>
#include "hot.hpp"
#include"service.hpp"

// int main(int argc,char *argv[])
// {
//     if(argc != 2) return -1;
//     cloud::FileUtil fu(argv[1]);
//     std::cout<<fu.Obtain_File_Size()<<std::endl;
//     std::string name = fu.Obtain_File_Name();
//     std::cout<<name<<std::endl;
//     std::cout<<fu.Obtain_File_Modify()<<std::endl;
//     std::cout<<fu.Obtain_File_Visit()<<std::endl;
//     // std::string body;
//     // fu.Obtain_File_Content(&body);
//     // std::cout<<body<<std::endl;
//     // cloud::FileUtil fu2(argv[2]);
//     // fu2.Direction_File_Write(body);
//     return 0;
// }

// void text()
// {
//     cloud::FileUtil fu("text.txt");
//     //fu.Compress("text.txt.lz");
//     //cloud::FileUtil fu2("text.txt.lz");
//     //fu2.UnCompress("text2.txt");
//     std::cout<<fu.File_Exist()<<std::endl;
//     cloud::FileUtil fu2("pack");
//     fu2.File_Create();
//     cloud::FileUtil fu3("zkj");
//     std::vector<std::string> arry;
//     fu3.Obtain_File(&arry);
//     for(auto &a:arry)
//         std::cout<<a<<" ";

// }

// void text()
// {
//     int age = 12;
//     std::string name = "zkj";
//     int Class = 231;
//     int studentid = 2023;
//     int score[3] = {59,59,52};
//     Json::Value val;
//     val["age"] = age;
//     val["Class"] = Class;
//     val["name"] = name;
//     val["studentid"] = studentid;
//     val["score"].append(score[0]);
//     val["score"].append(score[1]);
//     val["score"].append(score[2]);
//     std::string str;
//     cloud::JsonUtil::Serialize(val,&str);
//     //std::cout<<str<<std::endl;
//     Json::Value val2;
//     cloud::JsonUtil::UnSerialize(&val2,str);
//     std::cout<<val2["age"]<<std::endl;
// }

// 到这里文件接口类完成

// 系统配置类的测试
// void text()
// {
//     cloud::Config* config = cloud::Config::GetInstance();
//     std::cout<<config->GetBackDir()<<std::endl;
//     std::cout<<config->GetBackUpFile()<<std::endl;
//     std::cout<<config->GetDownloadPrefix()<<std::endl;
//     std::cout<<config->GetHotTime()<<std::endl;
//     std::cout<<config->GetPackDir()<<std::endl;
//     std::cout<<config->GetPackFileSuffix()<<std::endl;
//     std::cout<<config->GetServerip()<<std::endl;
//     std::cout<<config->GetServerPort()<<std::endl;

// }

// // 测试文件持久化存储接口
// void text(const std::string realpath)
// {
//     cloud::Backupinfo info;
//     info.NewBackupinfo(realpath);
//     cloud::DataManager data;
//     data.Insert(info);
//     cloud::FileUtil fu("./backdir");
//     std::vector<std::string> arry;
//     fu.Obtain_File(&arry);
//     for (int i = 0; i < arry.size(); i++)
//     {
//         cloud::Backupinfo tmp;
//         tmp.NewBackupinfo(realpath);
//         data.Insert(tmp);
//     }
//     cloud::Backupinfo text;
//     data.GetOneByRealpath("text.txt",&text);
//     std::cout<<text.atime<<std::endl;
//     std::cout<<text.mtime<<std::endl;
//     std::cout<<text.fsize<<std::endl;
//     std::cout<<text.pack_flag<<std::endl;
//     std::cout<<text.pack_path<<std::endl;
//     std::cout<<text.real_path<<std::endl;
//     std::cout<<text.url<<std::endl;
// }

// int main(int argc, char *argv[])
// {
//     text(argv[1]);
//     return 0;
// }

// 开始测试热点判断接口

cloud::DataManager *Data;

void DataRun()
{
    cloud::HotManager hot;
    hot.RunModule();
}


// void text()
// {
//     cloud::FileUtil fu("./backdir");
//     std::vector<std::string> arry;
//     fu.Obtain_File(&arry);
//     for(auto a:arry)
//         std::cout<<a<<std::endl;
// }

void service()
{
    cloud::Service sic;
    //std::cout<<"service -----------------------------------------"<<std::endl;
    sic.RunModule();
}


int main()
{
    Data = new cloud::DataManager();
    std::thread thread_hot_manager(DataRun);
    std::thread thread_service(service);
    thread_hot_manager.join();
    thread_service.join();
    return 0;
}

