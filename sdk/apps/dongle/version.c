#include "system/includes.h"
#include "generic/log.h"

int app_version_check()
{
    lib_version_check();

#ifdef CONFIG_FATFS_ENBALE
    VERSION_CHECK(fatfs, FATFS_VERSION);
#endif

#ifdef SDFILE_VERSION
    VERSION_CHECK(sdfile, SDFILE_VERSION);
#endif
    return 0;
}

