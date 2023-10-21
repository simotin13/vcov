#ifndef _COMMON_H_
#define _COMMON_H_

#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <algorithm>

class StringHelper
{
private:
    template<typename T, typename std::enable_if<std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string>::value>::type* = nullptr>
    static auto convert(T value)
    {
        return std::forward<T>(value).c_str();
    }

    template<typename T, typename std::enable_if<!std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string>::value>::type* = nullptr>
    static auto convert(T value)
    {
        return std::forward<T>(value);
    }

    template<typename ... Args>
    static std::string strformat(const std::string &fmt, Args ... args)
    {
        int len = std::snprintf(nullptr, 0, fmt.c_str(), std::forward<Args>(args) ...);
        if (len < 0)
        {
            return "";
        }
        size_t bufSize = len + sizeof(char);
        std::vector<char> buf(bufSize);
        std::snprintf(&buf[0], bufSize, fmt.c_str(), args ...);
        return std::string(&buf[0], &buf[0] + len);
    }

public:
    template<typename ... Args>
    static std::string strprintf(const std::string &fmt, Args ... args)
    {
        return strformat(fmt, convert(std::forward<Args>(args)) ...);
    }

    static bool GetHostFromUrl(const std::string url, std::string &host)
    {
        std::string protocol = "http://";
        auto pos = url.find(protocol);
        if (pos != 0)
        {
            // HTTPS
            protocol = "https://";
            pos = url.find(protocol);
        }

        // URL文字列でない
        if (pos != 0)
        {
            return false;
        }

        host = url.substr(protocol.size());

        // :<ポート番号>があればそこまでをホスト名とする
        pos = host.find(":");
        if (pos != std::string::npos)
        {
            host = host.substr(0, pos);
            return true;
        }

        // ポート番号の指定がない場合は最初に登場する/までをホスト名とする
        pos = host.find("/");
        if (pos != std::string::npos)
        {
            host = host.substr(0, pos);
        }

        return true;
    }

    static std::vector<std::string> Split(const std::string &str, char delimiter)
    {
        std::vector<std::string> entries;
        std::stringstream ss(str);
        std::string buffer;

        while(std::getline(ss, buffer, delimiter))
        {
            entries.push_back(buffer);
        }

        return entries;
    }

    static std::string ToUpper(const std::string &str)
    {
        std::string s = str;
        transform (s.begin(), s.end(), s.begin(), toupper);
        return s;
    }

    static std::string ToLower(const std::string &str)
    {
        std::string s = str;
        transform (s.begin(), s.end(), s.begin(), tolower);
        return s;
    }

    static std::string Trim(const std::string& string, const char* trimCharacterList = " \t\v\r\n")
    {
        std::string s;
        std::string::size_type left = string.find_first_not_of(trimCharacterList);
        if (left != std::string::npos)
        {
            std::string::size_type right = string.find_last_not_of(trimCharacterList);
            s = string.substr(left, right - left + 1);
        }
        return s;
    }
};

class TimeHelper
{
public:
    static std::string GetLocalTimestamp()
    {
        struct timeval now;
        struct tm local;

        gettimeofday(&now, NULL);
        localtime_r(&now.tv_sec, &local);
        return StringHelper::strprintf("%04d-%02d-%02d %02d:%02d:%02d.%03d",
            local.tm_year + 1900,
            local.tm_mon + 1,
            local.tm_mday,
            local.tm_hour,
            local.tm_min,
            local.tm_sec,
            now.tv_usec / 1000
        );
    }
};

#endif  // _COMMON_H_
