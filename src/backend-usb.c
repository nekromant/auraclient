#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <alloca.h>

#include <aura/client.h>
#include <aura/backend-usb.h>




enum usb_requests {
        RQ_GET_DEV_INFO,
        RQ_GET_OBJ_INFO,
        RQ_GET_EVENT,
        RQ_PUT_CALL
};


#define usbdev_is_be(pck) (pck & (1<<0))

struct usb_info_packet {
        uint8_t  flags;
        uint8_t  id_bits;
        uint8_t  len_bits;
        uint16_t num_objs;
        uint16_t io_buf_size;
        uint8_t  dev_to_host_epnum;
        uint8_t  host_to_dev_epnum;
        uint16_t poll_ms;
} __attribute__((packed));

unsigned char *bpos;
int rq_len;

struct usb_setup_packet {
        uint8_t bmRequestType;
        uint8_t bRequest;
        uint16_t wValue;
        uint16_t wIndex;
        uint16_t wLength;
};


struct aura_ipacket_header {
        uint16_t magic;
        uint16_t id;
        uint16_t datalen;
        uint8_t data[];
};


static int send_object(struct aura_object *o)
{
        int namelen=0;
        int arglen=0;
        int retlen=0;
        int destlen;
        char *dest;

        namelen = strlen(o->name);

        if (o->arg)
                arglen = strlen(o->arg);
        if (o->ret)
                retlen = strlen(o->ret);

        destlen = namelen + arglen + retlen + 4;
        /* FixMe: This will suck for 8051 with all their small stacks */
        dest = alloca(destlen);
        int ptr = 1;

        dest[0] = aura_object_is_method(o) ? 1 : 0;

        memcpy(&dest[ptr], o->name, namelen);
        ptr += namelen;

        dest[ptr++]=0;

        memcpy(&dest[ptr], o->arg, arglen);
        ptr += arglen;

        dest[ptr++]=0;

        memcpy(&dest[ptr], o->ret, retlen);
        ptr += retlen;

        dest[ptr++]=0;
        return aura_usb_control_write(dest, destlen);
}

int aura_usb_parse_setup(unsigned char data[8])
{
        struct usb_setup_packet *pck = (struct usb_setup_packet *) data;
        struct aura_object **reg;
        int ocount = aura_get_registry(&reg);
        struct aura_object *o = NULL;

        if (pck->wIndex < ocount)
                o = reg[pck->wIndex];
        else
                return -1;

        switch(pck->bRequest) //bRequest
        {
        case RQ_GET_DEV_INFO: {
                struct usb_info_packet p;
                memset(&p, 0x0, sizeof(p));
                p.flags = 0;
                p.num_objs = ocount;
                p.io_buf_size = 512;
                p.id_bits = sizeof(aura_id_t) * 8;
                p.len_bits = sizeof(aura_length_t) * 8;
                p.poll_ms = 100;
                return aura_usb_control_write(&p, sizeof(p));
        };

        case RQ_GET_OBJ_INFO:
                return send_object(o);

        case RQ_GET_EVENT:
        {
                aura_id_t id;
                char *tmp = NULL;
                aura_length_t datalen = aura_eventqueue_next(&id);
                if (datalen) {
                        tmp = alloca(datalen);
                        aura_eventqueue_read(tmp, datalen);
                        return aura_usb_control_write(tmp, datalen);
                }
                break;
        }
        case RQ_PUT_CALL:
        {
                struct aura_object *o = aura_registry_lookup(pck->wIndex);
                /* TODO: Do we really need length checking here? */
                if (!o)
                        return -1;
                int len = pck->wLength;
                char *args = alloca(len);
                aura_usb_control_read(args, len);
                aura_call(pck->wIndex, args);
                return 0;
        }
        }

        return -1;

}
