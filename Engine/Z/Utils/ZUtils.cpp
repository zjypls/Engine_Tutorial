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

    std::pair<int, int> Utils::GetWindowSizeFromIniConfig(const std::string& configFileStr, const std::string& windowName)
    {
        auto pos=configFileStr.find(windowName);
        if(pos==std::string::npos){
            Z_CORE_ERROR("Window name not found in config file: {0}",windowName);
            return {0,0};
        }
        auto SizePos=configFileStr.find("Size=",pos);
        if(SizePos==std::string::npos){
            Z_CORE_ERROR("Size not found in config file: {0}",windowName);
            return {0,0};
        }
        auto endPos=configFileStr.find("\n",SizePos);
        if(endPos==std::string::npos){
            Z_CORE_ERROR("Size not found in config file: {0}",windowName);
            return {0,0};
        }
        auto sizeStr=configFileStr.substr(SizePos+5,endPos-SizePos-5);
        auto commaPos=sizeStr.find(",");
        if(commaPos==std::string::npos){
            Z_CORE_ERROR("Size not found in config file: {0}",windowName);
            return {0,0};
        }
        auto widthStr=sizeStr.substr(0,commaPos);
        auto heightStr=sizeStr.substr(commaPos+1);
        return {std::stoi(widthStr),std::stoi(heightStr)};
    }
}