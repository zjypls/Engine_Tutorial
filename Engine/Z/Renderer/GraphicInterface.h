//
// Created by z on 24-1-15.
//

#ifndef ENGINEALL_GRAPHICINTERFACE_H
#define ENGINEALL_GRAPHICINTERFACE_H

namespace Z {

    struct GraphicSpec{

    };

    class GraphicInterface {
    public:
        virtual void Init(const GraphicSpec&initInfo)=0;
        virtual void prepare()=0;


    };

} // Z

#endif //ENGINEALL_GRAPHICINTERFACE_H
