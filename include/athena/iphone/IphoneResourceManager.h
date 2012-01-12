#ifndef _IPHONERESOURCEMANAGER_H_
#define _IPHONERESOURCEMANAGER_H_

#include "athena/ResourceManager.h"

namespace Athena
{
    class CIphoneResourceManager : public CResourceManager
    {
    public:
        static void                 CreateInstance();
        static void                 DestroyInstance();
        
        std::string                 MakeResourcePath(const char*) const;
        
    protected:
                                    CIphoneResourceManager();
        virtual						~CIphoneResourceManager();
    };
}

#endif
