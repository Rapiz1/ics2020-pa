#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t raw_value = inl(VGACTL_ADDR);
  uint32_t w = raw_value>>16, h = raw_value&((1<<16)-1);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = w*h 
  };
}

void __am_gpu_init() {
  int i;
  AM_GPU_CONFIG_T cfg;
  __am_gpu_config(&cfg);
  int w = cfg.width;  // TODO: get the correct width
  int h = cfg.height;  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  /*
  for (int i = 0; i < w; i++) for (int j = 0; j < h; j++) {
    fb[i*w + j] = i^j;
  }
  */
  outl(SYNC_ADDR, 1);
}


void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
