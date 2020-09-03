#include "hcnse_portable.h"
#include "hcnse_core.h"

typedef struct {
    int data;
} hcnse_test1_cntx_t;

hcnse_err_t
test1_handler(hcnse_cmd_params_t *params, void *conf, int argc, char **argv)
{
    int i;

    printf("%s ", params->directive->name);

    for (i = 0; i < argc; ++i) {
        printf("%s ", argv[i]);
    }

    printf("\n");

    return HCNSE_OK;
}



void *
hcnse_test1_preinit(hcnse_server_t *server)
{
    hcnse_test1_cntx_t *cntx;

    cntx = malloc(sizeof(hcnse_test1_cntx_t));

    return cntx;
}


hcnse_err_t
hcnse_test1_init(hcnse_server_t *server, void *cntx)
{

    return HCNSE_OK;
}



hcnse_command_t hcnse_test1_cmd[] = {
    {"test1", HCNSE_TAKE1, test1_handler},
    HCNSE_NULL_COMMAND
};

hcnse_module_t hcnse_module = {
    "test1",
    0x00000003,
    hcnse_test1_cmd,
    hcnse_test1_preinit,
    hcnse_test1_init,
    NULL,
    HCNSE_MODULE_RUN_TIME_PART
};
