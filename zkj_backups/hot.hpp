#ifndef __MY_HOT__
#define __MY_HOT__
#include <unistd.h>
#include "data.hpp"

extern cloud::DataManager *Data;

#define YEAR_SEG "-"
#define SUN_SEG ":"
#define YEAR_SUN " "

namespace cloud
{

    class HotManager
    {
    private:
        std::string _back_dir;    // 备份文件路径
        std::string _pack_dir;    // 压缩文件路径
        std::string _pack_suffix; // 压缩包后缀名
        int _hot_time;            // 热点判断时间
    private:
        // bool Hotspot_Time_Judge(const std::string &filename)
        // {
        //     cloud::FileUtil fu(filename);
        //     std::string atime = fu.Obtain_File_Visit();
        //     auto pos1 = atime.find(YEAR_SEG);
        //     std::string year = atime.substr(0,pos1);
        //     auto pos2 = atime.find(YEAR_SEG,pos1+1);
        //     std::string moon = atime.substr(pos1+1,pos2-pos1);
        //     auto pos = atime.find(YEAR_SUN);
        //     std::string sun = atime.substr(pos2+1,pos-pos2);
        //     auto pos3 = atime.find(SUN_SEG);
        //     auto pos4 = atime.find(SUN_SEG,pos3+1);
        //     std::string time = atime.substr(pos+1,pos3-pos);
        //     std::string minute = atime.substr(pos3+1,pos4-pos3);
        //     std::string second = atime.substr(pos4);
        // }
        bool Hotspot_Time_Judge(const std::string &filename)
        {
            struct stat st;
            if (stat(filename.c_str(), &st) < 0)
            {
                std::cout << "stat fail..." << std::endl;
                return NULL;
            }
            //std::cout << "-----------------------2----------------------" << std::endl;
            time_t atime = st.st_atime;
            time_t Difference = time(NULL) - atime;
            if (Difference > _hot_time)
                return false;
            return true; // 表示不用压缩
        }

    public:
        HotManager()
        {
            cloud::Config *config = cloud::Config::GetInstance();
            _back_dir = config->GetBackDir();
            _pack_dir = config->GetPackDir();
            _pack_suffix = config->GetPackFileSuffix();
            _hot_time = config->GetHotTime();
            cloud::FileUtil tmp1(_back_dir); // 创建目录
            cloud::FileUtil tmp2(_pack_dir);
            tmp1.File_Create();
            tmp2.File_Create();
            //std::cout<<"--------------6---------------"<<std::endl;
        }
        bool RunModule()
        {
            while (1)
            {
                std::vector<std::string> arry;
                cloud::FileUtil fu(_back_dir);
                fu.Obtain_File(&arry); // 获取文件夹下文件没问题
                //std::cout << "--------------------1--------------------------------" << std::endl;
                for (int i = 0; i < arry.size(); i++)
                {
                    // 热点文件判断
                    if (Hotspot_Time_Judge(arry[i]))
                        continue;
                    // 到这里表明文件需要压缩并修改备份信息
                    cloud::Backupinfo info;
                    //std::cout<<arry[i]<<std::endl;
                    if (Data->GetOneByRealpath(arry[i], &info) == false)
                    {
                        //std::cout<<"-----------------------------4------------------"<<std::endl;
                        info.NewBackupinfo(arry[i]); // 如果该文件没有备份先获取文件的信息
                    }
                    //std::cout << "--------------------3--------------------------------" << std::endl;

                    cloud::FileUtil tmp(arry[i]);
                    if (tmp.Compress(info.pack_path) == false)
                    {
                        std::cout << "RunModule fail ..." << std::endl;
                        return false;
                    }
                    tmp.Remove();
                    info.pack_flag = true;
                    Data->Update(info);
                }
                usleep(1000);
            }
        }
    };
}

#endif
