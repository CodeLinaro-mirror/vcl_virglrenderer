#include "vcomp_context.h"
#include "vcomp_logger.h"
#include "vqnn-protocol/vqnn_protocol_renderer_defines.h"
#include "vqnn-protocol/vqnn_protocol_renderer_logger.h"

//#include "vqnn-protocol/vqnn_protocol_renderer_dummy.h"

static void
vcomp_dispatch_clQnnLogCreate(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnLogCreate *args
) {

    struct vcomp_context *vctx = dispatch->data;
    Qnn_LogHandle_t logger;

    if (vqnn_functionPointers.qnnInterface.logCreate)
    {
        // args->callback replace the NULL if required
        args->ret = vqnn_functionPointers.qnnInterface.logCreate(NULL, args->maxLogLevel, &logger);
    }
    else
    {
        fprintf(stderr, "Log create function not found!!!\n");
        args->ret = -1;
        return;
    }

    if (!logger)
    {
        fprintf(stderr, "Log handle not created in QnnLogCreate! \n");
        return;
    }

    const vcomp_object_id id = vcomp_cs_handle_load_id((const void **)args->logger);

    if (!vcomp_context_validate_object_id(vctx, id))
    {
        args->ret = QNN_LOG_ERROR_INVALID_HANDLE;
        return;
    }

    struct vcomp_logger *log_handle = vcomp_object_alloc(sizeof(*log_handle), id);

    if(!log_handle) {
        args->ret = QNN_COMMON_ERROR_MEM_ALLOC;
        fprintf(stderr, "Unable to allocate space for log handle \n");
        return;
    }

    log_handle->base.handle.logger = logger;
    vcomp_context_add_object(vctx, &log_handle->base);
}

static void
vcomp_dispatch_clQnnLogSetLogLevel(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnLogSetLogLevel *args
) {

    struct vcomp_logger *logger = vcomp_logger_from_handle(args->logger);
    if (!logger) {
        args->ret=QNN_LOG_ERROR_INVALID_HANDLE;
        fprintf(stderr, "Logger handle not found in QnnLogSetLogLevel \n");
        return;
    }

    if (vqnn_functionPointers.qnnInterface.logSetLogLevel)
    {
        // args->callback replace the NULL if required
        args->ret = vqnn_functionPointers.qnnInterface.logSetLogLevel(logger, args->maxLogLevel);
    }
    else
    {
        fprintf(stderr, "logSetLogLevel function not found!\n");
        args->ret = -1;
        return;
    }
}

static void
vcomp_dispatch_clQnnLogFree(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnLogFree *args
) {
    struct vcomp_context *vctx = dispatch->data;
    struct vcomp_logger *logger = vcomp_logger_from_handle(args->logger);
    if (!logger) {
        args->ret=QNN_LOG_ERROR_INVALID_HANDLE;
        fprintf(stderr, "Invalid logger handle in QnnLogFree \n");
        return;
    }

    if (vqnn_functionPointers.qnnInterface.logFree)
    {
        // args->callback replace the NULL if required
        args->ret = vqnn_functionPointers.qnnInterface.logFree(logger);
    }
    else
    {
        fprintf(stderr, "logFree function not found!\n");
        args->ret = -1;
        return;
    }
    vcomp_context_remove_object(vctx, &logger->base);
}

void vcomp_context_init_logger_dispatch(struct vcomp_context *vctx)
{
    struct vcl_dispatch_context *dispatch = &vctx->dispatch;

    dispatch->dispatch_clQnnLogCreate = vcomp_dispatch_clQnnLogCreate;
    dispatch->dispatch_clQnnLogSetLogLevel = vcomp_dispatch_clQnnLogSetLogLevel;
    dispatch->dispatch_clQnnLogFree = vcomp_dispatch_clQnnLogFree;
}
