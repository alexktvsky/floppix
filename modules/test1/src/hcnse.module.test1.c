#include "fpx_portable.h"
#include "fpx_core.h"

typedef struct {
    int data;
} fpx_test1_cntx_t;

fpx_err_t
test1_handler(fpx_cmd_params_t *params, void *conf, int argc, char **argv)
{
    int i;

    printf("%s ", params->directive->name);

    for (i = 0; i < argc; ++i) {
        printf("%s ", argv[i]);
    }

    printf("\n");

    return FPX_OK;
}



void *
fpx_test1_preinit(fpx_server_t *server)
{
    fpx_test1_cntx_t *cntx;

    cntx = malloc(sizeof(fpx_test1_cntx_t));

    return cntx;
}


fpx_err_t
fpx_test1_init(fpx_server_t *server, void *cntx)
{

    return FPX_OK;
}



fpx_command_t fpx_test1_cmd[] = {
    {"test1", FPX_TAKE1, test1_handler},
    FPX_NULL_COMMAND
};

fpx_module_t fpx_module = {
    "test1",
    0x00000003,
    fpx_test1_cmd,
    fpx_test1_preinit,
    fpx_test1_init,
    NULL,
    FPX_MODULE_RUN_TIME_PART
};
