#include "vcomp_backend.h"
#include "vcomp_context.h"
#include "vcomp_logger.h"

#include "vqnn-protocol/vqnn_protocol_renderer_defines.h"
#include "vqnn-protocol/vqnn_protocol_renderer_backend.h"
#include <string.h>

static void
vcomp_dispatch_clQnnBackendCreateMESA(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnBackendCreateMESA *args)
{
    struct vcomp_context *vctx = dispatch->data;
    struct vcomp_logger *logger = vcomp_logger_from_handle(args->logger);
    if (!logger) {
        fprintf(stderr, "Invalid logger handle in QnnBackendCreate. Exiting.");
        args->ret = QNN_BACKEND_ERROR_INVALID_HANDLE;
        return;
    }

    Qnn_BackendHandle_t backend;

    if (vqnn_functionPointers.qnnInterface.backendCreate)
    {
        args->ret = vqnn_functionPointers.qnnInterface.backendCreate(logger->base.handle.logger, NULL, &backend);
    }
    else
    {
        fprintf(stderr, "QnnBackendCreate function not found in QnnInterface!\n");
        args->ret = -1;
        return;
    }

    if (!backend)
    {
        fprintf(stderr, "Backend not created in QnnBackendCreate.\n");
        return;
    }

    const vcomp_object_id id = vcomp_cs_handle_load_id((const void **)args->backend);
    if (!vcomp_context_validate_object_id(vctx, id))
    {
        args->ret = QNN_BACKEND_ERROR_INVALID_HANDLE;
        return;
    }

    struct vcomp_backend *backend_handle = vcomp_object_alloc(sizeof(*backend_handle), id);

    if(!backend_handle) {
        args->ret = QNN_COMMON_ERROR_MEM_ALLOC;
        fprintf(stderr, "Unable to allocate space for backend handle in QnnBackendCreate.\n");
        return;
    }

    backend_handle->base.handle.backend = backend;
    vcomp_context_add_object(vctx, &backend_handle->base);
}

static void
vcomp_dispatch_clQnnBackendFree(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnBackendFree *args
) {
    
    struct vcomp_context *vctx = dispatch->data;
    struct vcomp_backend *backend = vcomp_backend_from_handle(args->backend);

    if (!backend) {
        args->ret=QNN_BACKEND_ERROR_INVALID_HANDLE;
        fprintf(stderr, "Invalid backend handle in QnnBackendFree.\n");
        return;
    }

    if (vqnn_functionPointers.qnnInterface.backendFree)
    {
        args->ret = vqnn_functionPointers.qnnInterface.backendFree(backend->base.handle.backend);

    }
    else
    {
        fprintf(stderr, "QnnBackendFree function not found in QnnInterface!\n");
        args->ret = -1;
        return;
    }
    vcomp_context_remove_object(vctx, &backend->base);

}

static void
vcomp_dispatch_clQnnBackendGetBuildId(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnBackendGetBuildId *args
)
{
    if (!args->id) {
        fprintf(stderr, "NULL Build ID in QnnBackendGetBuildID \n");
        args->ret = QNN_BACKEND_ERROR_INVALID_ARGUMENT;
        return;
    }
    if (vqnn_functionPointers.qnnInterface.backendGetBuildId){
        args->ret = vqnn_functionPointers.qnnInterface.backendGetBuildId((const char **)&args->id);
        *args->ret_id_size = strlen(args->id);
    } else {
        fprintf(stderr, "backendGetBuildId function not found!\n");
        args->ret = -1;
    }
}

void vcomp_context_init_backend_dispatch(struct vcomp_context *vctx)
{
    struct vcl_dispatch_context *dispatch = &vctx->dispatch;

    dispatch->dispatch_clQnnBackendCreateMESA = vcomp_dispatch_clQnnBackendCreateMESA;
    dispatch->dispatch_clQnnBackendFree = vcomp_dispatch_clQnnBackendFree;
    dispatch->dispatch_clQnnBackendGetBuildId = vcomp_dispatch_clQnnBackendGetBuildId;
}
