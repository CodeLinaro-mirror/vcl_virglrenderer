#############################################################################
#
# Copyright (C) 2019 Collabora Ltd
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#

subdir('gallium')

virgl_sources = [
   'iov.c',
   'virgl_context.c',
   'virgl_context.h',
   'virgl_hw.h',
   'virgl_protocol.h',
   'virgl_resource.c',
   'virgl_resource.h',
   'virgl_util.c',
   'virgl_util.h',
]

vrend_sources = [
   'vrend_blitter.c',
   'vrend_blitter.h',
   'vrend_debug.c',
   'vrend_debug.h',
   'vrend_decode.c',
   'vrend_formats.c',
   'vrend_iov.h',
   'vrend_object.c',
   'vrend_object.h',
   'vrend_renderer.c',
   'vrend_renderer.h',
   'vrend_shader.c',
   'vrend_shader.h',
   'vrend_strbuf.h',
   'vrend_tweaks.c',
   'vrend_tweaks.h',
   'vrend_winsys.c',
   'vrend_winsys.h',
]

virglrenderer_sources = [
   'virglrenderer.c',
   'virglrenderer.h',
   'virglrenderer_hw.h',
]

vrend_winsys_egl_sources = [
   'vrend_winsys_gbm.c',
   'vrend_winsys_gbm.h',
   'vrend_winsys_egl.c',
   'vrend_winsys_egl.h',
]

vrend_winsys_glx_sources = [
   'vrend_winsys_glx.c',
   'vrend_winsys_glx.h',
]

venus_sources = [
   'venus-protocol/vn_protocol_renderer.h',
   'venus_hw.h',
   'vkr_cs.c',
   'vkr_cs.h',
   'vkr_object.h',
   'vkr_renderer.c',
   'vkr_renderer.h',
   'vkr_ring.c',
   'vkr_ring.h',
]

vcl_sources = [
   'vcl/vcomp_common.c',
   'vcl/vcomp_common.h',
   'vcl/vcomp_cl_context.c',
   'vcl/vcomp_cl_context.h',
   'vcl/vcomp_context.c',
   'vcl/vcomp_context.h',
   'vcl/vcomp_cs.c',
   'vcl/vcomp_cs.h',
   'vcl/vcomp_device.c',
   'vcl/vcomp_device.h',
   'vcl/vcomp_event.c',
   'vcl/vcomp_event.h',
   'vcl/vcomp_memory.c',
   'vcl/vcomp_memory.h',
   'vcl/vcomp_renderer.c',
   'vcl/vcomp_renderer.h',
   'vcl/vcomp_platform.c',
   'vcl/vcomp_platform.h',
   'vcl/vcomp_queue.c',
   'vcl/vcomp_queue.h',
   'vcl/vcomp_transport.c',
   'vcl/vcomp_transport.h',
   'vcl/vcomp_program.c',
   'vcl/vcomp_program.h'
]

virgl_depends = [
   gallium_dep,
   epoxy_dep,
   libdrm_dep,
   thread_dep,
   m_dep,
]

if with_tracing == 'perfetto'
   virgl_depends += [vperfetto_min_dep]
endif

if with_tracing == 'percetto'
   virgl_depends += [percetto_dep]
endif

virgl_sources += vrend_sources

if have_egl
   virgl_sources += vrend_winsys_egl_sources
   virgl_depends += [gbm_dep]
endif

if have_glx
   virgl_sources += vrend_winsys_glx_sources
   virgl_depends += [glx_dep]
endif

if with_venus
   virgl_sources += venus_sources
   virgl_depends += [venus_dep]
endif

if with_vcl
   virgl_sources += vcl_sources
   virgl_depends += [vcl_dep]
endif

libvirgl = static_library(
   'virgl',
   virgl_sources,
   include_directories: [inc_gallium, inc_configuration, 'vcl', 'drm', 'drm/drm-uapi'],
   dependencies : virgl_depends,
)

libvirgl_inc = [
   inc_gallium,
   inc_configuration,
   include_directories('.')
]

libvirgl_dep = declare_dependency(
   link_with: libvirgl,
   include_directories: libvirgl_inc
)

libvirglrenderer = shared_library(
   'virglrenderer',
   virglrenderer_sources,
   include_directories: [inc_gallium, inc_configuration],
   dependencies : [virgl_depends, libvirgl_dep],
   version : binary_age.to_string() + '.'
             + interface_age.to_string() + '.'
             + revision.to_string(),
   install : true
)

libvirglrenderer_dep = declare_dependency(
   link_with: libvirglrenderer,
   include_directories: libvirgl_inc,
   dependencies : [libvirgl_dep, gallium_dep]
)

install_headers('virglrenderer.h', subdir : 'virgl')
