//
// Created by z on 24-3-14.
//

#ifndef ENGINE_ANIMATION_H
#define ENGINE_ANIMATION_H
#include <vector>
#include <string>
#include <map>
#include "Include/glm/glm.hpp"
#include "Include/glm/gtx/quaternion.hpp"

#include "Include/assimp/include/assimp/scene.h"

namespace Z {

    inline glm::mat4 AssimpMatrixToGLM(const aiMatrix4x4& from)
    {
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }

    struct KeyPosition
    {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyRotation
    {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyScale
    {
        glm::vec3 scale;
        float timeStamp;
    };

    struct BoneInfo{
        int id;
        glm::mat4 offset;
    };

    class Bone{
    public:
        Bone(const std::string&name,int id,const aiNodeAnim* node);
        void Update(float time);
        int GetPositionKeyIndex(float animationTime);
        int GetRotationKeyIndex(float animationTime);
        int GetScaleKeyIndex(float animationTime);

        glm::mat4 GetLocalTransform(){return localTransform;}
        auto GetBoneName() const{return name;}
        auto GetBoneID() const {return id;}

    private:
        glm::mat4 InterpolatePosition(float time);
        glm::mat4 InterpolateRotation(float time);
        glm::mat4 InterpolateScale(float time);

        inline float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
        {
            float scaleFactor = 0.0f;
            float midWayLength = animationTime - lastTimeStamp;
            float framesDiff = nextTimeStamp - lastTimeStamp;
            scaleFactor = midWayLength / framesDiff;
            return scaleFactor;
        }

        std::vector<KeyPosition> keyPositions;
        std::vector<KeyRotation> keyRotations;
        std::vector<KeyScale> keyScales;
        int currentKey=0;
        glm::mat4 localTransform{1.f};
        std::string name;
        int id=0;
    };


    struct NodeData{
        glm::mat4 transform;
        std::string name;
        std::vector<NodeData> children;
    };

    class Animation {
    public:
        Animation() = default;
        Animation (const aiScene *scene,std::map<std::string,BoneInfo>& bonemap);

        Bone* FindBone(const std::string & name){
            auto res= std::find_if(bones.begin(),bones.end(),[&](auto& node){
                return node.GetBoneName()==name;
            });
            if(res==bones.end())return nullptr;
            return &(*res);
        }
        inline float GetTickPerSec(){return ticksPerSec;}
        inline float GetDuration(){return duration;}
        inline auto& GetRootNode(){return rootNode;};
        inline auto& GetBoneMap(){return boneMap;}
        inline int GetBoneCount(){return bones.size();}

    private:
        void ReadBones(const aiAnimation* animation);

        void ReadHierarchyData(NodeData& dst,const aiNode* src);

        float duration=0;
        int ticksPerSec=0;
        std::vector<Bone> bones;
        NodeData rootNode;
        std::map<std::string,BoneInfo> boneMap;
    };

    class Animator{
    public:
        Animator(const aiScene *scene,std::map<std::string,BoneInfo>& bonemap);

        void Update(float DeltaTime);

        void CalculateTransform(NodeData* data,glm::mat4 parentTrans);

        auto& GetMatrices(){return boneMatrices;}

        auto GetAnimation(){return animation;}

        void Clear(){delete animation;}

    private:
        float currentTime=0;
        float deltaTime=0;
        Animation* animation;
        std::vector<glm::mat4>boneMatrices;
    };




} // Z

#endif //ENGINE_ANIMATION_H
