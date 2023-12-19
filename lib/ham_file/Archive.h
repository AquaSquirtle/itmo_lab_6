#pragma once
#include "cstdint"
#include "string"
#include "fstream"
#include "iostream"
#include "vector"
#include "bitset"



class Archive {
public:
    explicit Archive(const std::string& path_);
    void Create();
    void AppendFile(const std::string& name);
    void DeleteFile(const std::string &name);
    void ExtractFile(const std::string& name);
    void Concatenate(const std::string& first_archive, const std::string& second_archive);
    void ExtractAllFiles();
    void GetListOfFiles();
    std::vector<char> EncodeBuffer(std::vector<char>& message);
    std::vector<char> DecodeBuffer(std::vector<char>& message);
private:
    std::string path;
    static const int kSizeOfBuffer = 30;
    static const int kSizeOfByte = 8;
};
