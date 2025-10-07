#include<unordered_map>
#include<string>
#include<fstream>
#include<iostream>
#include<sstream>

const char sep = ':';
class Dict
{
public:
    Dict() = default;
    Dict(const char*path)
    {
        load(path);
    }

    bool load(const char* path)
    {
        std::ifstream fin(path);
        if(!fin.is_open())
        {
            std::cerr << "open " << path << " error" << std::endl;
            return false;
        }

        std::string line;
        while(std::getline(fin,line))
        {
            if(line.empty())
                continue;
            size_t pos = line.find(sep);
            if(pos == std::string::npos)
                continue;
            std::string zh = line.substr(0,pos);
            std::string en = line.substr(pos+1);
            trim(zh);
            trim(en);

            if(!zh.empty() && !en.empty())
                _dict[zh] = en;
            
        }

        fin.close();
        return true;
    }

    std::string translate(const std::string& word) const
    {
        auto it = _dict.find(word);
        if(it != _dict.end())
            return it->second;
        return "未找到该单词";
    }
private:
    std::unordered_map<std::string,std::string> _dict;

    static void trim(std::string& s)
    {
        // 去掉首尾空白
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        if (start == std::string::npos)
            s.clear();
        else
            s = s.substr(start, end - start + 1);
    }
};