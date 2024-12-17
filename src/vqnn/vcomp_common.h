/*
 * Copyright 2024 Qualcomm Innovation Center, Inc. All Rights Reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VCOMP_COMMON_H
#define VCOMP_COMMON_H

#include <assert.h>
#include <stdbool.h>

#include "vqnn-protocol/vcl_cl.h"

typedef uint64_t vcomp_object_id;

typedef union
{
   uint64_t u64;
   Qnn_LogHandle_t logger;
   Qnn_BackendHandle_t backend;
   Qnn_ContextHandle_t qnn_context;
   Qnn_DeviceHandle_t device;
   Qnn_GraphHandle_t graph;
} vcomp_handle;

struct vcomp_object
{
   vcomp_object_id id;
   vcomp_handle handle;
};

/* define a type-safe cast function */
#define VCOMP_DEFINE_OBJECT_CAST(vcomp_type, qnn_type)                                        \
   static inline struct vcomp_##vcomp_type *vcomp_##vcomp_type##_from_handle(qnn_type handle) \
   {                                                                                         \
      struct vcomp_##vcomp_type *obj = (struct vcomp_##vcomp_type *)(uintptr_t)handle;       \
      if (obj)                                                                               \
      {                                                                                      \
         assert(obj->base.id);                                                               \
         assert(obj->base.handle.vcomp_type);                                                \
         assert((uintptr_t)obj->base.handle.vcomp_type == obj->base.handle.u64);             \
      }                                                                                      \
      return obj;                                                                            \
   }

void vcomp_log(const char *fmt, ...);

static inline void *
vcomp_object_alloc(size_t size, vcomp_object_id id)
{
   assert(size >= sizeof(struct vcomp_object));

   struct vcomp_object *obj = calloc(1, size);
   if (!obj)
      return NULL;

   /* obj is only half-initialized */
   obj->id = id;

   return obj;
}

#endif /* VCOMP_COMMON_H */
