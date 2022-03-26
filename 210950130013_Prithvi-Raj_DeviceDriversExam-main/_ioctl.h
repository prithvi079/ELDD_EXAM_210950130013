#define serial_magic '{'


//setting up ioctl command numbers

struct stats{
    int size;
    char k_buff[512];
    int r_w;
};

#define string_reset _IO(serial_magic,0)
#define string_getlen _IOR(serial_magic,1,int)

#define get_kbuff_status _IOR(serial_magic,2,struct stats)

#define serial_max_nr 8