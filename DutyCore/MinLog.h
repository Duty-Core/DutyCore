#pragma once
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

class MinLog
{
private:
    std::ofstream OutputStream;
    bool _isOpen = false;

public:
    MinLog(const char* filename, const char* dir = "")
    {
        Open(filename, dir);
    }

    void Open(const char* filename, const char* dir = "")
    {
        if (_isOpen) return;
        std::filesystem::path const directory = std::filesystem::current_path() / dir;
        std::filesystem::create_directories(directory);
        MinLog::OutputStream = std::ofstream(directory / filename);
        _isOpen = true;
    }

    void Write(const char* value)
    {
        MinLog::OutputStream << value;
        std::cout << value;
    }

    void Write(std::string value)
    {
        MinLog::OutputStream << value;
        std::cout << value;
    }

    void WriteLine(const char *value)
    {
        MinLog::OutputStream << value << std::endl;
        std::cout << value << std::endl;
    }

    void WriteLine(std::string value)
    {
        MinLog::OutputStream << value << std::endl;
        std::cout << value << std::endl;
    }

    MinLog& operator<<(auto const& value) {
        MinLog::OutputStream << value;
        std::cout << value;
        return *this;
    }

    void Flush() {
        MinLog::OutputStream.flush();
        std::cout.flush();
    }
};
