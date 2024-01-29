//
// Created by z on 24-1-29.
//

#ifndef IMGUICONTENT_H
#define IMGUICONTENT_H
#include "Z/Core/Core.h"
namespace Z {

class Z_API ImGuiContent {
public:
    virtual void OnImGuiRender()=0;
};

} // Z

#endif //IMGUICONTENT_H
