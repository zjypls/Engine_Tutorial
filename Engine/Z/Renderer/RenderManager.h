//
// Created by z on 24-1-16.
//

#ifndef ENGINEALL_RENDERMANAGER_H
#define ENGINEALL_RENDERMANAGER_H

#include "Z/Renderer/GraphicInterface.h"

namespace Z {

    class Z_API RenderManager final{
    public:


        static void Init();
        static auto GetInstance(){return m_Context;}
        static void Update(float deltaTime);

    private:
        static Ref<GraphicInterface> m_Context;
    };

} // Z

#endif //ENGINEALL_RENDERMANAGER_H
