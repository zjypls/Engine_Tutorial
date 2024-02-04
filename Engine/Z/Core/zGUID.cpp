//
// Created by 32725 on 2023/4/4.
//

#include "Z/Core/zGUID.h"


namespace Z {
	static std::random_device rd;
	static std::mt19937_64 randomEngine(rd());
	static std::uniform_int_distribution<uint64> distribution;
    std::unordered_set<zGUID::Type> zGUID::registerMaps{};

	zGUID::zGUID(){
        auto id=distribution(randomEngine);
#ifdef Z_SECUREGUID
        while(!IsGuidPresent(id)){
            id=distribution(randomEngine);
        }
        Register(id);
#endif
        this->value=id;
	}

    void zGUID::RemoveID(const zGUID::Type &id) {
#ifdef Z_SECUREGUID
        registerMaps.erase(id);
#else
        return;
#endif
    }

    bool zGUID::IsGuidPresent(const zGUID::Type &id) {
#ifdef Z_SECUREGUID
#if __cplusplus>=202002L
        return registerMaps.contains(id);
#else
        return registerMaps.find(id)!=registerMaps.end();
#endif
#else
        return false;
#endif
    }

    void zGUID::Register(const zGUID::Type &id) {
#ifdef Z_SECUREGUID
        registerMaps.insert(id);
#endif
    }

}