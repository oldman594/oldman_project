#ifndef __MY_DATA__
#define __MY_DATA__

#include <unordered_map>
#include "util.hpp"
#include "cloud.hpp"

namespace cloud
{
    struct Backupinfo
    {
        bool pack_flag;        // 是否压缩标志
        size_t fsize;          // 文件大小
        std::string atime;     // 最后修改时间
        std::string mtime;     // 最后访问时间
        std::string real_path; // 文件实际存储路径名称
        std::string pack_path; // 压缩包存储路径名称
        std::string url;
        void NewBackupinfo(const std::string realpath)
        {
            FileUtil fu(realpath); // 传入文件实际路径
            if (fu.File_Exist() == false)
                return;
            this->fsize = fu.Obtain_File_Size();
            this->mtime = fu.Obtain_File_Modify();
            this->atime = fu.Obtain_File_Visit();
            this->real_path = realpath;
            std::string packpath = cloud::Config::GetInstance()->GetPackDir();
            std::string URL = cloud::Config::GetInstance()->GetDownloadPrefix();
            std::string suffix = cloud::Config::GetInstance()->GetPackFileSuffix();
            this->pack_flag = false;
            this->pack_path = packpath + fu.Obtain_File_Name() + suffix;
            this->url = URL + fu.Obtain_File_Name();
        }
    };
    class DataManager
    {
    private:
        std::string _backup_file;                           // 持久化存储文件
        std::unordered_map<std::string, Backupinfo> _table; // 内存中一hash表存储
        pthread_rwlock_t _rwlock;                           // 读写锁--读共享，写互斥
    public:
        DataManager()
        {
            _backup_file = cloud::Config::GetInstance()->GetBackUpFile();
            pthread_rwlock_init(&_rwlock, NULL);
            InitLoad();
        }
        ~DataManager() { pthread_rwlock_destroy(&_rwlock); }
        bool Storage() // 每次数据新增或修改都要重新持久化存储，避免数据丢失
        {
            std::vector<Backupinfo> arry;
            this->GetAll(&arry);
            std::string body;
            Json::Value root;
            for (int i = 0; i < arry.size(); i++)
            {
                Json::Value tmp;
                tmp["fsize"] = (Json::Int64)arry[i].fsize;
                tmp["atime"] = arry[i].atime;
                tmp["mtime"] = arry[i].mtime;
                tmp["pack_flag"] = arry[i].pack_flag;
                tmp["pack_path"] = arry[i].pack_path;
                tmp["real_path"] = arry[i].real_path;
                tmp["url"] = arry[i].url;
                root.append(tmp);
            }
            cloud::JsonUtil::Serialize(root, &body);
            FileUtil fu(_backup_file);
            fu.Direction_File_Write(body);
            return true;
        }
        bool InitLoad() // 初始化加载，在每次系统重启都要加载以前的数据
        {
            FileUtil fu(_backup_file);
            if (fu.File_Exist() == false)
                return true;
            std::string body;
            if (fu.Obtain_File_Content(&body) == false)
            {
                std::cout << "Obtain_File_Content fail ..." << std::endl;
                return false;
            }
            Json::Value root;
            if (cloud::JsonUtil::UnSerialize(&root, body) == false)
            {
                std::cout << "UnSerialize fail ..." << std::endl;
                return false;
            }
            for (int i = 0; i < root.size(); i++)
            {
                Backupinfo info;
                info.atime = root[i]["atime"].asString();
                info.pack_flag = root[i]["pack_flag"].asBool();
                info.fsize = root[i]["fsize"].asInt();
                info.mtime = root[i]["mtime"].asString();
                info.real_path = root[i]["real_path"].asString();
                info.pack_path = root[i]["pack_path"].asString();
                info.url = root[i]["url"].asString();
                Insert(info);
            }
        }
        bool Insert(const Backupinfo &info) // 新增
        {
            pthread_rwlock_wrlock(&_rwlock); // 写锁
            _table[info.url] = info;
            pthread_rwlock_unlock(&_rwlock);
            Storage();
            return true;
        }
        bool Update(const Backupinfo &info) // 修改
        {
            pthread_rwlock_wrlock(&_rwlock);
            _table[info.url] = info;
            pthread_rwlock_unlock(&_rwlock);
            Storage();
            return true;
        }
        bool GetOneByUrl(const std::string &url, Backupinfo *info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it = _table.find(url);
            if (it == _table.end())
            {
                pthread_rwlock_unlock(&_rwlock);
                return false;
            }
            *info = it->second;
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        bool GetOneByRealpath(const std::string &path, Backupinfo *info)
        {
            //std::cout << "-------------------------7--------------------" << std::endl;
            pthread_rwlock_wrlock(&_rwlock);
            //std::cout << "-------------------------5--------------------" << std::endl;
            auto it = _table.begin();
            for (; it != _table.end(); it++)
            {
                if (it->second.real_path == path)
                {
                    *info = it->second;
                    pthread_rwlock_unlock(&_rwlock);
                    return true;
                }
            }
            pthread_rwlock_unlock(&_rwlock);
            return false;
        }
        bool GetAll(std::vector<Backupinfo> *arry)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it = _table.begin();
            for (; it != _table.end(); it++)
                arry->push_back(it->second);
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
    };
}
// extern cloud::DataManager *Data;

#endif