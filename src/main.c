#include <stdio.h>
#include <libgen.h>
#include "malloc.h"
#include "logger.h"

int main(int argc, const char* argv[])
{
    if (argc <= 0 || 0 != init_logger(stderr, LOG_TRACE, true))
    {
        fprintf(stderr, "FAIL: error initializing logger.\n");
        return 1;
    }

    log_info("starting program '%s'", basename((char*)argv[0]));
    
    void *blk1 = sma_malloc(128);
    void *blk2 = sma_malloc(256);
    void *blk3 = sma_malloc(64);

    sma_free(blk1);
    sma_free(blk2);
    sma_free(blk3);
   
    return 0;
}
