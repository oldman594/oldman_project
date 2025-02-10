#ifndef __MY_UTIL__
#define __MY_UTIL__

#include <iostream>
#include "bundle.h"
#include <fstream>
#include <string>
#include <vector>
#include <jsoncpp/json/json.h>
#include <experimental/filesystem>
#include <sys/stat.h>

namespace cloud
{
    namespace fs = std::experimental::filesystem;
    class FileUtil
    {
    private:
        std::string _filename;

    public:
        FileUtil(const std::string &filename) : _filename(filename) {}
        size_t Obtain_File_Size()
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "stat fail...." << std::endl; // error
                return -1;
            }
            return st.st_size;
        }

        bool Remove()
        {
            remove(_filename.c_str());
            return true;
        }

        std::string print_normal_time(time_t timestamp)
        {
            // 将时间戳转换为本地时间
            struct tm *timeInfo = localtime(&timestamp);
            if (timeInfo == nullptr)
            {
                std::cout << "Error converting timestamp to local time." << std::endl;
                return NULL;
            }

            // 格式化输出时间
            char timeStr[100];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);
            return timeStr;
        }

        // 文件名、从某个位置开始获取文件内容，获取文件全部内容
        std::string Obtain_File_Modify() // 获取文件最后修改时间
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "stat fail..." << std::endl;
                return NULL;
            }
            return print_normal_time(st.st_mtim.tv_sec); // timespec比time_t的精度更高通过.tv_sec可以忽略timespec的纳秒部分
        }
        std::string Obtain_File_Visit() // 获取文件最后访问时间
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "stat fail..." << std::endl;
                return NULL;
            }
            return print_normal_time(st.st_atim.tv_sec);
        }
        std::string Obtain_File_Name()
        {
            auto pos = _filename.rfind('/'); // find的返回值是一个位置
            // std::cout<<pos<<_filename<<_filename.size();
            if (pos == _filename.size() - 1)
            {
                return _filename;
            }
            std::string filename = _filename.substr(pos+1);
            return filename;
        }
        bool Obtain_File_pos_Content(std::string *body, size_t pos, size_t len)
        {
            size_t fsize = this->Obtain_File_Size();
            if (pos + len > fsize)
            {
                std::cout << "Content exceeds the limit" << std::endl;
                return false;
            }
            std::ifstream ifs;
            ifs.open(_filename, std::ios::binary); // 二进制打开文件
            if (ifs.is_open() == false)
            {
                std::cout << "open fail ..." << std::endl;
                return false;
            }
            ifs.seekg(pos, std::ios::beg);
            body->resize(len + 2);
            ifs.read(&(*body)[0], len);
            if (ifs.good() == false)
            {
                std::cout << "read fail ..." << std::endl;
                ifs.close();
                return false;
            }
            ifs.close();
            return true;
        }
        bool Obtain_File_Content(std::string *body)
        {
            return Obtain_File_pos_Content(body, 0, this->Obtain_File_Size());
        }

        bool Direction_File_Write(const std::string &body)
        {
            std::ofstream ofs;
            ofs.open(_filename.c_str(), std::ios::binary/* | std::ios::app*/); // 以二进制打开文件并以追加的方式来写
            std::cout<<_filename.c_str()<<std::endl;
            if (ofs.is_open() == false)
            {
                std::cout << "open fail ..." << std::endl;
                return false;
            }
            ofs.write(&body[0], body.size());
            if (ofs.good() == false)
            {
                std::cout << "write fail ..." << std::endl;
                ofs.close();
                return false;
            }
            ofs.close();
            return true;
        }
        bool Compress(const std::string &packname) // 压缩文件
        {
            std::string body;
            if (this->Obtain_File_Content(&body) == false)
            {
                std::cout << "Obtain_File_Content fail ..." << std::endl;
                return false;
            }
            std::string packstr = bundle::pack(bundle::LZIP, body);
            FileUtil fu(packname);
            if (fu.Direction_File_Write(packstr) == false)
            {
                std::cout << "Direction_File_Write fail ... " << std::endl;
                return false;
            }
            return true;
        }
        bool UnCompress(const std::string &filename) // 解压缩文件
        {
            std::string body;
            if (this->Obtain_File_Content(&body) == false)
            {
                std::cout << "Obtain_File_Content fail ..." << std::endl;
                return false;
            }
            std::string packstr = bundle::unpack(body);
            FileUtil fu(filename);
            if (fu.Direction_File_Write(packstr) == false)
            {
                std::cout << "Direction_File_Write fail ... " << std::endl;
                return false;
            }
            return true;
        }
        bool File_Exist() // 判断文件是否存在
        {
            return std::experimental::filesystem::exists(_filename);
        }
        bool File_Create() // 创建目录文件
        {
            if (this->File_Exist())
                return true;
            return std::experimental::filesystem::create_directories(_filename);
        }
        bool Obtain_File(std::vector<std::string> *arry) // 获取目录内容
        {
            //std::cout<<"进入Obtain_File"<<std::endl;
            for (auto &a : std::experimental::filesystem::directory_iterator(_filename))
            {
                if (std::experimental::filesystem::is_directory(a) == true)
                    continue;
                //std::cout<<std::experimental::filesystem::path(a).relative_path().string()<<std::endl;
                arry->push_back(std::experimental::filesystem::path(a).relative_path().string());
            }
            //std::cout<<"退出Obtain_File"<<std::endl;
            return true;
        }
    };
    class JsonUtil{
    public:
        static bool Serialize(const Json::Value &root, std::string * str)
        {
            Json::StreamWriterBuilder swb;
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
            std::stringstream strs;
            sw->write(root,&strs);
            *str = strs.str();
            return true;
        }
        static bool UnSerialize(Json::Value *root,const std::string &str)
        {
            Json::CharReaderBuilder crb;
            std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
            std::string err;
            bool ret = cr->parse(str.c_str(),str.c_str()+str.size(),root,&err);
            if(ret == false)
            {
                std::cout<<"pares error :"<<err<<std::endl;
                return false;
            }
            return true;
        }
    };  
}

#endif