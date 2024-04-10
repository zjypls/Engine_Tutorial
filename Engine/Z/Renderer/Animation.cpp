//
// Created by z on 24-3-14.
//

#include <iostream>
#include "Z/Renderer/Animation.h"
#include "Include/glm/gtc/type_ptr.hpp"

namespace Z {

    Bone::Bone(const std::string &name, int id, const aiNodeAnim *node):name(name),id(id) {
        localTransform=glm::mat4{1};
        keyPositions.resize(node->mNumPositionKeys);
        for(int i=0;i<node->mNumPositionKeys;++i){
            auto position=node->mPositionKeys[i].mValue;
            keyPositions[i].position={position.x,position.y,position.z};
            keyPositions[i].timeStamp=node->mPositionKeys[i].mTime;
        }
        keyRotations.resize(node->mNumRotationKeys);
        for(int i=0;i<node->mNumRotationKeys;++i){
            auto rotate=node->mRotationKeys[i].mValue;
            keyRotations[i].timeStamp=node->mRotationKeys[i].mTime;
            keyRotations[i].orientation={rotate.w,rotate.x,rotate.y,rotate.z};
        }
        keyScales.resize(node->mNumScalingKeys);
        for(int i=0;i<node->mNumScalingKeys;++i){
            auto scale=node->mScalingKeys[i].mValue;
            keyScales[i].timeStamp=node->mScalingKeys[i].mTime;
            keyScales[i].scale={scale.x,scale.y,scale.z};
        }
    }

    int Bone::GetPositionKeyIndex(float animationTime) {
        for(int index=0;index<keyPositions.size()-1;++index){
            if(animationTime<keyPositions[index+1].timeStamp)
                return index;
        }
        std::cerr<<"Get Error animation time : "<<animationTime<<std::endl;
        assert(false);
    }

    int Bone::GetRotationKeyIndex(float animationTime) {
        for(int i=0;i<keyRotations.size()-1;++i){
            if(animationTime<keyRotations[i+1].timeStamp){
                return i;
            }
        }
        std::cerr<<"Get Error animation time : "<<animationTime<<std::endl;
        assert(false);
    }

    int Bone::GetScaleKeyIndex(float animationTime) {
        for(int i=0;i<keyScales.size()-1;++i){
            if(animationTime<keyScales[i+1].timeStamp){
                return i;
            }
        }
        std::cerr<<"Get Error animation time : "<<animationTime<<std::endl;
        assert(false);
    }

    glm::mat4 Bone::InterpolatePosition(float time) {
        if(keyPositions.size()<=1){
            return glm::translate(glm::mat4{1.f},keyPositions[0].position);
        }
        auto index= GetPositionKeyIndex(time);
        auto next=index+1;
        auto scale= GetScaleFactor(keyPositions[index].timeStamp,keyPositions[next].timeStamp,time);
        auto finalPos=glm::mix(keyPositions[index].position,keyPositions[next].position,scale);
        return glm::translate(glm::mat4{1.f},finalPos);
    }

    glm::mat4 Bone::InterpolateRotation(float time) {
        if(keyRotations.size()<=1){
            return glm::toMat4(glm::normalize(keyRotations[0].orientation));
        }
        auto index= GetRotationKeyIndex(time);
        auto next=index+1;
        auto scale= GetScaleFactor(keyRotations[index].timeStamp,keyRotations[next].timeStamp,time);
        auto finalRotate=glm::slerp(keyRotations[index].orientation,keyRotations[next].orientation,scale);
        return glm::toMat4(glm::normalize(finalRotate));
    }

    glm::mat4 Bone::InterpolateScale(float time) {
        if(keyScales.size()<=1){
            return glm::scale(glm::mat4(1.f),keyScales[0].scale);
        }
        auto index= GetScaleKeyIndex(time);
        int next=index+1;
        auto scale= GetScaleFactor(keyScales[index].timeStamp,keyScales[next].timeStamp,time);
        auto finalScale=glm::mix(keyScales[index].scale,keyScales[next].scale,scale);
        return glm::scale(glm::mat4{1.f},finalScale);
    }

    void Bone::Update(float time) {
        auto transPos= InterpolatePosition(time);
        auto transRotate= InterpolateRotation(time);
        auto transScale= InterpolateScale(time);
        localTransform=transPos*transRotate*transScale;
    }

    Animation::Animation(const aiScene *scene,std::map<std::string,BoneInfo>&bonemap) {
        if(!scene->mAnimations)return;
        auto animation=scene->mAnimations[0];
        duration=animation->mDuration;
        ticksPerSec=animation->mTicksPerSecond;
        boneMap=bonemap;
        ReadHierarchyData(rootNode,scene->mRootNode);
        ReadBones(animation);
    }

    void Animation::ReadBones(const aiAnimation *animation) {
        for(int i=0;i<animation->mNumChannels;++i){
            auto channel=animation->mChannels[i];
            auto name=channel->mNodeName.data;
            if(boneMap.find(name)==boneMap.end()){
                boneMap[name].id=i;
            }
            bones.emplace_back(name,boneMap[name].id,channel);
        }
    }

    void Animation::ReadHierarchyData(NodeData &dst, const aiNode *src) {
        dst.name=src->mName.data;
        dst.transform= AssimpMatrixToGLM(src->mTransformation);
        dst.children.resize(src->mNumChildren);
        for(int i=0;i< src->mNumChildren;++i){
            NodeData nodeData{};
            ReadHierarchyData(nodeData,src->mChildren[i]);
            dst.children[i]=nodeData;
        }
    }

    Animator::Animator(const aiScene *scene, std::map<std::string, BoneInfo> &bonemap) {
        currentTime=0;
        deltaTime=0;
        animation=new Animation(scene,bonemap);
        boneMatrices.resize(bonemap.size());
        for(auto&mat:boneMatrices){
            mat=glm::mat4 {1.f};
        }
    }

    void Animator::Update(float DeltaTime) {
        deltaTime=DeltaTime;
        if(animation){
            currentTime+=animation->GetTickPerSec()*DeltaTime;
            currentTime=fmod(currentTime,animation->GetDuration());
            CalculateTransform(&animation->GetRootNode(),glm::mat4{1.f});
        }

    }

    void Animator::CalculateTransform(NodeData *data, glm::mat4 parentTrans) {
        auto name=data->name;
        auto nodeTransform=data->transform;
        auto bone=animation->FindBone(name);
        if(bone){
            bone->Update(currentTime);
            nodeTransform=bone->GetLocalTransform();
        }
        auto globalTransform=parentTrans*nodeTransform;
        auto boneInfoMap=animation->GetBoneMap();
        if(boneInfoMap.find(name)!=boneInfoMap.end()){
            boneMatrices[boneInfoMap[name].id]=globalTransform*boneInfoMap[name].offset;
        }
        for(auto&node:data->children){
            CalculateTransform(&node,globalTransform);
        }
    }

} // Z