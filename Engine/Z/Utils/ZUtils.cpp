//
// Created by z on 2024/1/4.
//
#include <fstream>
#include "Z/Core/Log.h"
#include "Z/Utils/ZUtils.h"


namespace Z{
    std::string Utils::ReadFile(const std::string& path){
        std::ifstream file(path);
        if(!file.is_open()){
            Z_CORE_ERROR("File not found: {0}",path);
            return "";
        }
        std::string content((std::istreambuf_iterator<char>(file)),(std::istreambuf_iterator<char>()));
        file.close();
        return content;
    }
}