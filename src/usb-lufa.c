#include <LUFA/Drivers/USB/USB.h>
#include <aura/backend-usb.h>

int aura_usb_control_write(void *data, size_t length)
{
	Endpoint_ClearSETUP();
	Endpoint_Write_Control_Stream_LE(data, length);
	Endpoint_ClearOUT();
	return 0;
}

void aura_usb_config_changed()
{
	#ifdef AURA_TX_EPADDR
		Endpoint_ConfigureEndpoint(AURA_TX_EPADDR, EP_TYPE_INTERRUPT, AURA_INTERRUPT_EPSIZE, 1);
	#endif
}

void aura_task()
{
}

int aura_usb_control_read(void *data, size_t length)
{

	Endpoint_ClearSETUP();
	Endpoint_Read_Control_Stream_LE(data, length);
	Endpoint_ClearIN();
	return 0;
}


int aura_usb_interrupt_write(char *buf, int len)
{
	return Endpoint_Write_Stream_LE(buf, len, NULL);
}

int aura_usb_interrupt_finish()
{
	Endpoint_ClearIN();
	return 0;
}
