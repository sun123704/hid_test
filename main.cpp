// #include <pthread.h>
// #include <ctype.h>
// #include <errno.h>

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/ioctl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <linux/hid.h>
// #include <linux/hidraw.h>


#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/hid.h>
#include <linux/hidraw.h>

#define HID_MAX_PACKET_SIZE 64
#define HID_BUFFER_SIZE (10*1024*1024)// 10M bytes

typedef struct {
    char null_array[HID_MAX_PACKET_SIZE];
} buffer_offset_size_t;


static char hid_read_buffer[HID_BUFFER_SIZE];
static char hid_write_buffer[HID_BUFFER_SIZE];

static int hid_file_handle = 0;
static const char* hid_file_name = "/dev/hidraw1";

static int hid_init(void);
static int hid_exit(void);

static int hid_read(void* buffer, int buffer_size);
static int hid_write(void* buffer, int buffer_size);

/*****************************************************************
* Function name:             main
* Author:                DriverMonkey
* Function Description:     main function just for hid temp testing
*****************************************************************/
int main(void)
{
  int read_size = 0;
  int test_count = 10;
  struct hidraw_devinfo raw_info;
  hid_init();
  if (-1 == ioctl(hid_file_handle, HIDIOCGRAWINFO, &raw_info)) {
    printf("1");
  }
  printf("pid:%d,  vid:%d\n", raw_info.product, raw_info.vendor);

// #if 0
  while (test_count--) {
    memset(hid_read_buffer, 0x00, HID_BUFFER_SIZE);
    read_size = hid_read(hid_read_buffer, HID_BUFFER_SIZE);
    printf("getting report::%s \n", hid_read_buffer);
    printf("read size::%d \n", read_size);

    memset(hid_write_buffer, 0x00, HID_BUFFER_SIZE);
    strcpy(hid_write_buffer, hid_read_buffer);
    hid_write(hid_write_buffer, read_size);
  }
// #endif
  hid_exit();
  return 0;
}

static int hid_init(void) {
  if ((hid_file_handle = open(hid_file_name, O_RDWR, 0666)) < 0) {
    perror(hid_file_name);
    return hid_file_handle;
  } else {
    return hid_file_handle;
  }
}

static int hid_exit(void) {
  close(hid_file_handle);
  return 0;
}

static int hid_read(void* buffer, int buffer_size) {
  if (buffer == NULL) {
    perror("hid_read::pointer error!");
    return -1;
  }
  return read(hid_file_handle, buffer, buffer_size);
}

static int hid_write(void* buffer, int buffer_size) {
  int return_v = 0;
  int writting_count = buffer_size / HID_MAX_PACKET_SIZE;
  int remainding_size = buffer_size % HID_MAX_PACKET_SIZE;
  buffer_offset_size_t* buffer_offset = (buffer_offset_size_t*)buffer;
  if (buffer == NULL)  {
    perror("hid_write::pointer error!");
    return -1;
  }
  while (writting_count--) {
    return_v = write(hid_file_handle, buffer_offset, HID_MAX_PACKET_SIZE);
    if (return_v < 0) {
      perror("hid_write::writting error!");
      return return_v;
    }
    buffer_offset++;
  }
  return_v = write(hid_file_handle, buffer_offset, remainding_size);
  return return_v;
}