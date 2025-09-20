#include "vcomp_backend.h"
#include "vcomp_context.h"
#include "vcomp_qnncontext.h"
#include "vcomp_graph.h"

#include "vqnn-protocol/vqnn_protocol_renderer_defines.h"
#include "vqnn-protocol/vqnn_protocol_renderer_graph.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <inttypes.h>

#include "vrend_renderer.h"
#include <math.h>

static void
vcomp_dispatch_clQnnGraphRetrieveMESA(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnGraphRetrieveMESA *args)
{
    struct vcomp_context *vctx = dispatch->data;
    struct vcomp_qnn_context *qnn_context_handle = vcomp_qnn_context_from_handle(args->contextHandle);
    if (!qnn_context_handle) {
        fprintf(stderr, "Invalid context handle in QnnGraphRetrieve \n");
        args->ret = QNN_GRAPH_ERROR_INVALID_HANDLE;
        return;
    }

    Qnn_GraphHandle_t graph;

    if (vqnn_functionPointers.qnnInterface.graphRetrieve)
    {
        args->ret = vqnn_functionPointers.qnnInterface.graphRetrieve(qnn_context_handle->base.handle.qnn_context, args->graphName, &graph);
    }
    else
    {
        fprintf(stderr, "Graph Retrieve function not found!\n");
        args->ret = -1;
        return;
    }

    if (!graph)
    {
        fprintf(stderr, "Graph handle not created in graph retrieve! \n");
        return;
    }

    const vcomp_object_id id = vcomp_cs_handle_load_id((const void **)args->graphHandle);
    if (!vcomp_context_validate_object_id(vctx, id))
    {
        args->ret = QNN_GRAPH_ERROR_INVALID_HANDLE;
        return;
    }

    struct vcomp_graph *graph_handle = vcomp_object_alloc(sizeof(*graph_handle), id);

    if(!graph_handle) {
        args->ret = QNN_COMMON_ERROR_MEM_ALLOC;
        fprintf(stderr, "Unable to allocate spacce for graph handle in QnnGraphRetrieve \n");
        return;
    }

    graph_handle->base.handle.graph = graph;
    vcomp_context_add_object(vctx, &graph_handle->base);
}


static void deep_copy_tensor(Qnn_Tensor_t* tensor, int numTensors, char* mem, size_t* cur_size) {

    for(int i = 0; i < numTensors; i++){
        if(tensor[i].version == QNN_TENSOR_VERSION_1){
            if(tensor[i].v1.name != NULL) {
                tensor[i].v1.name = mem+ *cur_size;
                size_t len = strlen(tensor[i].v1.name);
                *cur_size += len;
            }

            if(tensor[i].v1.quantizeParams.quantizationEncoding == QNN_QUANTIZATION_ENCODING_AXIS_SCALE_OFFSET) {
                if(tensor[i].v1.quantizeParams.axisScaleOffsetEncoding.numScaleOffsets > 0){
                    size_t sc_size = tensor[i].v1.quantizeParams.axisScaleOffsetEncoding.numScaleOffsets * sizeof(Qnn_ScaleOffset_t);
                    tensor[i].v1.quantizeParams.axisScaleOffsetEncoding.scaleOffset = (Qnn_ScaleOffset_t*)(mem + *cur_size);
                    *cur_size += sc_size;
                }
            }


            if(tensor[i].v1.rank > 0) {
                // Ensure the current size is aligned to the alignment of uint32_t
                size_t alignment = sizeof(uint32_t);
                size_t offset = (*cur_size % alignment == 0) ? 0 : (alignment - (*cur_size % alignment));
                tensor[i].v1.dimensions = (uint32_t*)((char*)mem + *cur_size + offset);
                *cur_size += (tensor[i].v1.rank * sizeof(uint32_t));
            }


            if(tensor[i].v1.memType == QNN_TENSORMEMTYPE_MEMHANDLE) {
                //HANDLE MEM HANDLE
                fprintf(stderr, "Mem Handle for tensors not implemented yet!\n");
            } else if(tensor[i].v1.memType == QNN_TENSORMEMTYPE_RAW) {
                if(tensor[i].v1.clientBuf.dataSize > 0) {
                    size_t alignment = sizeof(uint32_t);
                    *cur_size = (*cur_size + alignment - 1) & ~(alignment - 1);
                    tensor[i].v1.clientBuf.data = (uint8_t*)(mem + *cur_size);
                    *cur_size += tensor[i].v1.clientBuf.dataSize;
                }
            } else {
                fprintf(stderr, "Invalid mem_type of QnnTensor_t \n");
            }
        } else if (tensor[i].version == QNN_TENSOR_VERSION_2) {
            if(tensor[i].v2.name != NULL) {
                tensor[i].v2.name = mem+ *cur_size;
                size_t len = strlen(tensor[i].v2.name);
                *cur_size += len;
            }
            if(tensor[i].v2.quantizeParams.quantizationEncoding == QNN_QUANTIZATION_ENCODING_AXIS_SCALE_OFFSET) {
                if(tensor[i].v2.quantizeParams.axisScaleOffsetEncoding.numScaleOffsets > 0){
                    size_t sc_size = tensor[i].v2.quantizeParams.axisScaleOffsetEncoding.numScaleOffsets * sizeof(Qnn_ScaleOffset_t);
                    tensor[i].v2.quantizeParams.axisScaleOffsetEncoding.scaleOffset = (Qnn_ScaleOffset_t*)(mem + *cur_size);
                    *cur_size += sc_size;
                }
            }


            if(tensor[i].v2.rank > 0) {
                // Ensure the current size is aligned to the alignment of uint32_t
                size_t alignment = sizeof(uint32_t);
                size_t offset = (*cur_size % alignment == 0) ? 0 : (alignment - (*cur_size % alignment));
                tensor[i].v2.dimensions = (uint32_t*)((char*)mem + *cur_size + offset);
                *cur_size += (tensor[i].v2.rank * sizeof(uint32_t));
            }


            if(tensor[i].v2.memType == QNN_TENSORMEMTYPE_MEMHANDLE) {
                //HANDLE MEM HANDLE
                fprintf(stderr, "Mem Handle for tensors not implemented yet!\n");
            } else if(tensor[i].v2.memType == QNN_TENSORMEMTYPE_RAW) {
                if(tensor[i].v2.clientBuf.dataSize > 0) {
                    size_t alignment = sizeof(uint32_t);
                    *cur_size = (*cur_size + alignment - 1) & ~(alignment - 1);
                    tensor[i].v2.clientBuf.data = (uint8_t*)(mem + *cur_size);
                    *cur_size += tensor[i].v2.clientBuf.dataSize;
                }
            } else if(tensor[i].v2.memType == QNN_TENSORMEMTYPE_RETRIEVE_RAW) {
                fprintf(stderr, "QNN_TENSORMEMTYPE_RETRIEVE_RAW for tensors not implemented yet!\n");
            } else {
                fprintf(stderr, "Invalid mem_type of QnnTensor_t \n");
            }
            tensor[i].v2.isDynamicDimensions = NULL;

        } else {
            fprintf(stderr, "Incorrect version of QNN tensor \n");
        }
    }
}


static void
vcomp_dispatch_clQnnGraphExecute(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnGraphExecute *args)
{

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

    char *mem = vres->ptr;
    if (!vres->ptr) fprintf(stderr, "\n Shared memory pointer does not exist! \n");

    size_t cur_size = sizeof(Qnn_Tensor_t) * args->numInputs;
    Qnn_Tensor_t* inputs = (Qnn_Tensor_t*)mem;

    deep_copy_tensor(inputs, args->numInputs, mem, &cur_size);

    size_t align_size = (ceil((double)cur_size/(double)sizeof(Qnn_Tensor_t)) * sizeof(Qnn_Tensor_t)) - cur_size;
    Qnn_Tensor_t *outputs = (Qnn_Tensor_t*)(mem + cur_size + align_size);
    cur_size += args->numOutputs * sizeof(Qnn_Tensor_t) + align_size;

    deep_copy_tensor(outputs, args->numOutputs, mem, &cur_size);

    struct vcomp_context *vctx = dispatch->data;
    struct vcomp_graph *graph = vcomp_graph_from_handle(args->graphHandle);
    if (!graph) {
        fprintf(stderr, "Invalid graph handle in QnnGraphExecute \n");
        args->ret = QNN_GRAPH_ERROR_INVALID_HANDLE;
        return;
    }
    if (vqnn_functionPointers.qnnInterface.graphExecute)
    {
        args->ret = vqnn_functionPointers.qnnInterface.graphExecute(graph->base.handle.graph, inputs, args->numInputs, outputs, args->numOutputs, args->profileHandle, args->signalHandle);

    }
    else
    {
        fprintf(stderr, "Graph Execute function not found!\n");
        args->ret = -1;
        return;
    }
}

void vcomp_context_init_graph_dispatch(struct vcomp_context *vctx)
{
    struct vcl_dispatch_context *dispatch = &vctx->dispatch;

    dispatch->dispatch_clQnnGraphRetrieveMESA = vcomp_dispatch_clQnnGraphRetrieveMESA;
    dispatch->dispatch_clQnnGraphExecute = vcomp_dispatch_clQnnGraphExecute;
}
