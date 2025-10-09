#pragma once
#include <json/json.h>
#include <string>
#include <memory>


//设计一个协议
//"len/r/n"{json}"/r/n

static const std::string kSep = "\r\n";

// len + CRLF + json + CRLF
inline std::string Encode(const std::string& json) {
    return std::to_string(json.size()) + kSep + json + kSep;
}

// 尝试从 packagestream 中解出一个完整包：
//  - 成功：返回 true，并把 json_out 写好，同时从 packagestream 中移除该包
//  - 数据不完整：返回 false，不修改 packagestream
//  - 数据畸形：返回 false，并可选择清理或保留（这里选择保留，交由上层处置）
inline bool Decode(std::string& packagestream, std::string& json_out) {
    // 1) 找到第一个 CRLF，确定长度字段
    std::size_t pos = packagestream.find(kSep);
    if (pos == std::string::npos) {
        return false; // 还没收到完整的长度行
    }

    // 2) 解析长度（只允许十进制数字）
    const std::string lenstr = packagestream.substr(0, pos);
    if (lenstr.empty()) return false;

    std::size_t i = 0;
    for (; i < lenstr.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(lenstr[i]))) {
            return false; // 非法长度
        }
    }

    // 使用 stoul + 边界检查
    std::size_t json_len = 0;
    try {
        json_len = static_cast<std::size_t>(std::stoul(lenstr));
    } catch (...) {
        return false; // 溢出或格式错
    }

    // 3) 计算包的总长度：len 行 + CRLF + 正文 + CRLF
    const std::size_t header_len = pos + kSep.size();
    const std::size_t total_len  = header_len + json_len + kSep.size();

    // 4) 判断缓冲是否已包含完整包
    if (packagestream.size() < total_len) {
        return false; // 数据还不够，继续收
    }

    // 5) 校验结尾 CRLF 是否存在
    if (packagestream.compare(header_len + json_len, kSep.size(), kSep) != 0) {
        return false; // 尾部分隔符缺失或损坏
    }

    // 6) 取出 JSON 正文，并从流里移除这个包
    json_out = packagestream.substr(header_len, json_len);
    packagestream.erase(0, total_len);
    return true;
}

class Request
{
public:
    Request(int x = 0, int y = 0, char op = '+')
        : _x(x), _y(y), _op(op)
    {}
    ~Request() {}

    void SetValue(int x, int y, char op)
    {
        _x = x;
        _y = y;
        _op = op;
    }

    bool Serialize(std::string* out)
    {
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["op"] = std::string(1, _op);  // 转为字符串
        Json::StreamWriterBuilder builder;
        *out = Json::writeString(builder, root);
        return true;
    }

    bool Deserialize(const std::string& in)
    {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        std::string errs;
        if (!reader->parse(in.c_str(), in.c_str() + in.size(), &root, &errs))
            return false;

        _x = root["x"].asInt();
        _y = root["y"].asInt();
        _op = root["op"].asString()[0];
        return true;
    }


    int _x;
    int _y;
    char _op;
};

class Response
{
public:
    Response(int result = 0, int code = 0)
        : _result(result), _code(code)
    {}
    ~Response() {}

    bool Serialize(std::string* out)
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;
        Json::StreamWriterBuilder builder;
        *out = Json::writeString(builder, root);
        return true;
    }

    bool Deserialize(const std::string& in)
    {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        std::string errs;
        if (!reader->parse(in.c_str(), in.c_str() + in.size(), &root, &errs))
            return false;

        _result = root["result"].asInt();
        _code = root["code"].asInt();
        return true;
    }

    void Print()
    {
        std::cout << "result: " << _result << std::endl;
        std::cout << "code: " << _code << std::endl;
    }


    int _result;
    int _code;
};

class Factory
{
public:
    static std::shared_ptr<Request> BuildRequest()
    {
        return std::make_shared<Request>();
    }
    static std::shared_ptr<Response> BuildResponse()
    {
        return std::make_shared<Response>();
    }
};
