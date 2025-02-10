#ifndef __MY_CLOUD__
#define __MY_CLOUD__

#include <pthread.h>
#include <mutex>
#include "util.hpp"

namespace cloud
{
#define CONFIG_FILE "./cloud.conf"
    class Config
    {
    private:
        Config() {ReadConfigFile();}
        static std::mutex _mutex;
        static Config *_instance;

    private:
        int _hot_time;                // 热点时间
        int _server_port;             // 服务器监听端口
        std::string _download_prefix; // 下载的url前缀路径
        std::string _packfile_suffix; // 压缩包后缀名称
        std::string _back_dir;        // 备份文件存放目录
        std::string _pack_dir;        // 压缩包存放目录
        std::string _server_ip;       // 服务器ip地址
        std::string _backup_file;     // 数据存放文件
        bool ReadConfigFile()
        {
            FileUtil fu(CONFIG_FILE);
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
            _hot_time = root["hot_time"].asInt();
            _server_port = root["server_port"].asInt();
            _download_prefix = root["download_prefix"].asString();
            _packfile_suffix = root["packfile_suffix"].asString();
            _back_dir = root["back_dir"].asString();
            _pack_dir = root["pack_dir"].asString();
            _server_ip = root["server_ip"].asString();
            _backup_file = root["backup_file"].asString();
            return true;
        }

    public:
        static Config *GetInstance()
        {
            if (_instance == NULL)
            {
                _mutex.lock();
                if (_instance == NULL)
                {
                    _instance = new Config();
                }
                _mutex.unlock();
            }
            return _instance;
        }
        int GetHotTime()
        {
            return _hot_time;
        }
        int GetServerPort()
        {
            return _server_port;
        }
        std::string GetDownloadPrefix()
        {
            return _download_prefix;
        }
        std::string GetPackFileSuffix()
        {
            return _packfile_suffix;
        }
        std::string GetBackDir()
        {
            return _back_dir;
        }
        std::string GetPackDir()
        {
            return _pack_dir;
        }
        std::string GetServerip()
        {
            return _server_ip;
        }
        std::string GetBackUpFile()
        {
            return _backup_file;
        }
    };
    std::mutex Config::_mutex;
    Config *Config::_instance = NULL;
}

#endif