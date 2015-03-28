Nvidia module for 311c.

Two parts, module and libraries.

Module requirements
nvidia.ko
nvidia-frontend.ko
nvidia[0-7].ko
nvidia-uvm.ko


nvidia_drv.so used by X11
libglx.so.x.y.z
libwfb.so
libGL.so.x.y.z
libnvidia-glcore.so.x.y.z
libvdpau.so.x.y.z
libcuda.so.x.y.z, libcuda.la
/usr/lib/libOpenCL.so.1.0.0, /usr/lib/libnvidia-opencl.so.x.y.z


/usr/lib/libnvidia-tls.so.x.y.z and /usr/lib/tls/libnvidia-tls.so.x.y.z
/usr/lib/libnvidia-ml.so.x.y.z
