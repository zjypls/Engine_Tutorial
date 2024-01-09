//
// Created by z on 24-1-9.
//
#include "Z/Core/Log.h"
#include "Z/Utils/ZUtils.h"

// Todo:improve
// temporary solution
namespace Z {
    std::string Utils::FileOpen(const char *filter, const char *defaultOpen, const char *initialDir){
        Z_CORE_WARN("Utils::FileOpen On Linux Not Complete Yet ! Just Return Default Project !");
        return "Projects/Test001/Test001.zPrj";
    }
    std::string Utils::FileSave(const char *filter, const char *defaultOpen, const char *initialDir){
        Z_CORE_ERROR("Utils::FileSave On Linux Not Complete Yet ! Just Return Empty String !");
        return "";
    }
}