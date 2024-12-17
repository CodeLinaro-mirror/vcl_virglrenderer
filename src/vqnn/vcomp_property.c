#include "vcomp_context.h"
#include "vcomp_property.h"
#include "vqnn-protocol/vqnn_protocol_renderer_defines.h"
#include "vqnn-protocol/vqnn_protocol_renderer_context.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

static void
vcomp_dispatch_clQnnPropertyHasCapability(
    struct vcl_dispatch_context *dispatch,
    struct vcl_command_clQnnPropertyHasCapability *args
)
{
    if (vqnn_functionPointers.qnnInterface.propertyHasCapability){
        args->ret = vqnn_functionPointers.qnnInterface.propertyHasCapability(args->key);
    } else {
        fprintf(stderr, "PropertyHasCapability function not found!\n");
        args->ret = -1;
    }
}

void vcomp_context_init_property_dispatch(struct vcomp_context *vctx)
{
    struct vcl_dispatch_context *dispatch = &vctx->dispatch;
    dispatch->dispatch_clQnnPropertyHasCapability = vcomp_dispatch_clQnnPropertyHasCapability;
}