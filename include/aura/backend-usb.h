#ifndef AURA_BACKEND_USB
#define AURA_BACKEND_USB

int aura_usb_control_write(void *data, size_t length);
int aura_usb_control_read(void *data, size_t length);
int aura_usb_parse_setup(unsigned char data[8]);


#endif /* end of include guard: AURA_BACKEND_USB */
