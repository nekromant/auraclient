#include <aura/client.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


static char iobuf[AURA_EVENT_BUFFER_SIZE];
static int head;
static int tail;
struct aura_object *cur_obj;
static aura_length_t last_obj_len;


#define CIRC_CNT(head,tail,size) (((head) - (tail)) & ((size)-1))
#define CIRC_SPACE(head,tail,size) CIRC_CNT((tail),((head)+1),(size))
#define CIRC_CNT_TO_END(head,tail,size) \
        ({int end = (size) - (tail); \
          int n = ((head) + end) & ((size)-1); \
          n < end ? n : end;})
#define CIRC_SPACE_TO_END(head,tail,size) \
        ({int end = (size) - 1 - (head); \
          int n = (end + (tail)) & ((size)-1); \
          n <= end ? n : end+1;})

struct aura_serialized_object {
	aura_length_t	length;
	aura_id_t		id;
	char			data[];
};

/**
 * Start start writing an event/call result to the backend.
 *
 * Every data chunk is preceeded by object id
 *
 * @param  o object pointer
 * @return   0 if it can be started, -1 if there is not enough buffer space
 */
int aura_evt_start(struct aura_object *o)
{
	if (CIRC_SPACE(head, tail, AURA_EVENT_BUFFER_SIZE) < o->retlen)
		return -1; /* Not enough free space in event buffer */
	/* Now, let's serialize object id */
	aura_evt_write((void *)&o->id, sizeof(aura_id_t));
	last_obj_len=0;
    cur_obj = o;
	return 0;
}

/**
 * Write the next bunch of data to the event buffer
 * @param buf buffer pointer
 * @param len buffer length
 */
void aura_evt_write(const void *buf, aura_length_t len)
{
	last_obj_len += len;
	while (len) {
		int tocopy = min_t(int, len,
			CIRC_SPACE_TO_END(head, tail, AURA_EVENT_BUFFER_SIZE));
			memcpy(&iobuf[head], buf, tocopy);
			head += tocopy;
			head &= (AURA_EVENT_BUFFER_SIZE - 1);
			buf+=tocopy;
			len-=tocopy;
	}
}

void aura_evt_finish()
{
	if (cur_obj->retlen != last_obj_len)
		aura_panic("Object doesn't follow format");
}

void aura_eventqueue_peek(void *buf, aura_length_t len)
{
	aura_length_t pos = tail;
	while (len) {
		int tocopy = min_t(int, len,
			CIRC_CNT_TO_END(head, pos, AURA_EVENT_BUFFER_SIZE));
			memcpy(buf, &iobuf[pos], tocopy);
			pos += tocopy;
			pos &= (AURA_EVENT_BUFFER_SIZE - 1);
			buf+=tocopy;
			len-=tocopy;
	}
}

void aura_eventqueue_read(void *buf, aura_length_t len)
{
	aura_eventqueue_peek(buf, len);
	tail += len;
	tail &= (AURA_EVENT_BUFFER_SIZE - 1);
}

/**
 * Returns the length of the next event/call result in queue to be read
 *
 * @return The length is the actual data plus the length of aura_id_t byte
 */
aura_length_t aura_eventqueue_next()
{
	aura_id_t id;
	if (!CIRC_CNT(head, tail, AURA_EVENT_BUFFER_SIZE))
		return AURA_ID_INVALID;
	aura_eventqueue_peek(&id, sizeof(aura_id_t));
	struct aura_object *o = aura_registry_lookup(id);
	if (!o)
		return 0;
	return o->retlen + sizeof(aura_id_t);
}

aura_length_t aura_eventqueue_size()
{
	return CIRC_CNT(head, tail, AURA_EVENT_BUFFER_SIZE);
}
