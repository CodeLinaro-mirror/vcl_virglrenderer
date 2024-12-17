#include "vcomp_backend.h"
#include "vcomp_context.h"
#include "vcomp_qnncontext.h"

#include "vqnn-protocol/vqnn_protocol_renderer_defines.h"
#include "vqnn-protocol/vqnn_protocol_renderer_context.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "vrend_renderer.h"

static uint8_t* data = NULL;
static uint64_t offset = 0;

static void fill_data(uint64_t initial_data_size, uint64_t curr_size, struct vrend_resource *vres)
{
    if (initial_data_size!=0) {
        data = (uint8_t *)malloc(initial_data_size);
        offset = 0;
    }
    memcpy(data + offset, (uint8_t*)vres->ptr, curr_size);
    offset += curr_size; 
}

static void vcomp_dispatch_clQnnBufferDataTransfer(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnBufferDataTransfer *args)
{
    size_t offset = 0;
    struct vcomp_context *vctx = dispatch->data;

    struct virgl_resource *res = virgl_resource_lookup(args->mem_ptr);
    if (!res)
    {
        vcomp_log("Failed to find virgl resource %u", args->mem_ptr);
        return;
    }

    struct vrend_resource *vres = (struct vrend_resource *)res->pipe_resource;
    if (!vres)
    {
        vcomp_log("No pipe resource attached to virgl resource %u", args->mem_ptr);
        return;
    } 
    fill_data(args->initial_data_size, args->curr_size, vres);
    args->ret = 0; 
}

static void
vcomp_dispatch_clQnnContextCreateFromBinaryMESA(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnContextCreateFromBinaryMESA *args)
{

    struct vcomp_context *vctx = dispatch->data;

    struct vcomp_backend *backend = vcomp_backend_from_handle(args->backend);
    if (!backend) {
        fprintf(stderr, "Invalid backend handle in QnnCreateContextFromBinary \n");
        args->ret = QNN_CONTEXT_ERROR_INVALID_ARGUMENT;
        return;
    }
    Qnn_ContextHandle_t qnn_context;

    if (vqnn_functionPointers.qnnInterface.contextCreateFromBinary)
    {
        args->ret = vqnn_functionPointers.qnnInterface.contextCreateFromBinary(backend->base.handle.backend, args->device, NULL, (const void*)data, args->binaryBufferSize, &qnn_context, args->profile);
    }
    else
    {
        fprintf(stderr, "Context create function not found!\n");
        args->ret = -1;
    }

    if (!qnn_context)
    {
        fprintf(stderr, "Context not created in QnnCreateContextFromBinary \n");
        return;
    }

    const vcomp_object_id id = vcomp_cs_handle_load_id((const void **)args->context);
    if (!vcomp_context_validate_object_id(vctx, id))
    {
        args->ret = QNN_CONTEXT_ERROR_INVALID_HANDLE;
        return;
    }

    struct vcomp_qnn_context *qnn_context_handle = vcomp_object_alloc(sizeof(*qnn_context_handle), id);

    if (!qnn_context_handle)
    {
        args->ret = QNN_COMMON_ERROR_MEM_ALLOC;
        fprintf(stderr, "Unable to allocate space for context handle \n");
        return;
    }


    qnn_context_handle->base.handle.qnn_context = qnn_context;
    vcomp_context_add_object(vctx, &qnn_context_handle->base);
}

static void
vcomp_dispatch_clQnnContextFree(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnContextFree *args
) {
    
    struct vcomp_context *vctx = dispatch->data;
    struct vcomp_qnn_context *qnn_context_handle = vcomp_qnn_context_from_handle(args->context);

    if (!qnn_context_handle) {
        fprintf(stderr, "Invalid context handle in QnnCreateFree \n");
        args->ret=QNN_CONTEXT_ERROR_INVALID_HANDLE;
        return;
    }

    if (vqnn_functionPointers.qnnInterface.contextFree)
    {
        // args->callback replace the NULL if required
        args->ret = vqnn_functionPointers.qnnInterface.contextFree(qnn_context_handle, args->profile);
    }
    else
    {
        fprintf(stderr, "QnnContextFree function not found!\n");
        args->ret = -1;
        return;
    }
    vcomp_context_remove_object(vctx, &qnn_context_handle->base);

}

void vcomp_context_init_context_dispatch(struct vcomp_context *vctx)
{
    struct vcl_dispatch_context *dispatch = &vctx->dispatch;

    dispatch->dispatch_clQnnContextCreateFromBinaryMESA = vcomp_dispatch_clQnnContextCreateFromBinaryMESA;
    dispatch->dispatch_clQnnContextFree = vcomp_dispatch_clQnnContextFree;
    dispatch->dispatch_clQnnBufferDataTransfer = vcomp_dispatch_clQnnBufferDataTransfer;
}
