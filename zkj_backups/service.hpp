#ifndef __MY_SERVICE__
#define __MYSERVICE__

#include "httplib.h"
#include "data.hpp"
#include "cloud.hpp"

extern cloud::DataManager *Data;

namespace cloud
{
    class Service
    {
    private:
        int _server_port;             // 服务器端口号
        std::string _server_ip;       // 服务器ip地址
        std::string _download_prefix; // 下载前缀
        httplib::Server _server;

    private:
        static void Upload(const httplib::Request &req, httplib::Response &rsp) // 上传请求处理函数
        {
            std::cout << "Upload" << std::endl;
            //std::cout << "---------------------------------------------" << std::endl;
            auto ret = req.has_file("file");
            if (ret == false)
            {
                rsp.status = 400;
                return;
            }
            const auto &file = req.get_file_value("file");
            std::string realpath = cloud::Config::GetInstance()->GetBackDir() + cloud::FileUtil(file.filename).Obtain_File_Name(); // 构建realpath
            cloud::FileUtil fu(realpath);
            fu.Direction_File_Write(file.content);
            cloud::Backupinfo info;
            info.NewBackupinfo(realpath);
            Data->Insert(info);
            return;
        }
        static void ListShow(const httplib::Request &req, httplib::Response &rsp) // 展示页面获取请求
        {
            // 获取文件的备份信息
            std::vector<cloud::Backupinfo> arry;
            Data->GetAll(&arry);
            std::stringstream ss;
            ss << "<html><head><title>Download</title></head>";
            ss << "<body><h1>Download</h1><table>";
            for (auto &a : arry)
            {
                ss << "<tr>";
                std::string filename = cloud::FileUtil(a.real_path).Obtain_File_Name();
                ss << "<td><a href='" << a.url << "'>" << filename << "</a></td>";
                ss << "<td align='right'>" << a.atime << "</td>";
                ss << "<td align='right'>" << a.fsize / 1024 << "</td>";
                ss << "</tr>";
            }
            ss << "</table></body></html>";
            rsp.body = ss.str();
            rsp.set_header("Content-Type", "text/html"); // Content-Type数据文件类型，决定了前端如何处理这个数据
            rsp.status = 200;
            return;
        }
        static std::string GETETag(const cloud::Backupinfo &info)
        {
            cloud::FileUtil fu(info.real_path);
            std::string etag = fu.Obtain_File_Name();
            etag += "-";
            etag += std::to_string(fu.Obtain_File_Size());
            etag += "-";
            etag += fu.Obtain_File_Modify();
            return etag;
        }

        static void Download(const httplib::Request &req, httplib::Response &rsp) // 文件下载处理函数
        {
            // 通过req里的文件路径找到要访问文件的信息
            cloud::Backupinfo info;
            auto pos = req.path.rfind("/");
            std::string filename = cloud::Config::GetInstance()->GetBackDir() + req.path.substr(pos + 1);
            Data->GetOneByRealpath(filename, &info);
            // std::cout<<"req.path"<<":"<<req.path<<std::endl;
            if (info.pack_flag == true)
            {
                // 说明文件已经被压缩
                // 需要将文件解压缩，并修改相应的信息
                cloud::FileUtil fu(info.pack_path);
                fu.UnCompress(info.real_path);
                fu.Remove();
                info.pack_flag = false;
                Data->Update(info);
            }
            // 将info.real_path里的数据写到rsp的body里
            cloud::FileUtil tmp(info.real_path);
            // std::cout << info.pack_path << std::endl;
            bool resume = false;
            std::string oldetag;
            // 先判断有没有断点续传标识符
            if (req.has_header("If-Range"))
            {
                // 将ETag与最新的ETag字段比较看是否相同
                oldetag = req.get_header_value("If-Range");
                // 如果相同说明需要断点续传
                if (oldetag == GETETag(info))
                    resume = true;
            }
            if (resume == false)
            {
                tmp.Obtain_File_Content(&rsp.body);
                rsp.set_header("Accept-Ranges", "bytes"); // 支持断点续传
                rsp.set_header("ETag", GETETag(info));  //ETag为文件唯一标识，有我们自己定义客户端并不关心
                rsp.set_header("Content-Type", "application/octet-stream"); // 设置Content-Type为application/octet-stream表明数据是一个二进制数据流
                rsp.status = 200;
            }
            else
            {
                tmp.Obtain_File_Content(&rsp.body);//httplib这个库里在向rsp.body写入时会判断要不要断点续传，如果需要它会帮我们完成截取字符的任务
                rsp.set_header("Accept-Ranges", "bytes"); // 支持断点续传
                rsp.set_header("ETag", GETETag(info));
                rsp.set_header("Content-Type", "application/octet-stream"); // 设置Content-Type为application/octet-stream表明数据是一个二进制数据流
                rsp.status = 206;//响应状态码为206表示断点续传
            }
        }

    public:
        Service()
        {
            _server_port = cloud::Config::GetInstance()->GetServerPort();
            _server_ip = cloud::Config::GetInstance()->GetServerip();
            _download_prefix = cloud::Config::GetInstance()->GetDownloadPrefix();
        }
        bool RunModule()
        {
            _server.Post("/upload", Upload); // 如果有/upload这个字段那么调用Upload这个函数
            _server.Get("/listshow", ListShow);
            _server.Get("/", ListShow);
            std::string download_url = _download_prefix + "(.*)"; // 正则表达式
            _server.Get(download_url, Download);
            _server.listen(_server_ip.c_str(), _server_port);
        }
    };
}

#endif