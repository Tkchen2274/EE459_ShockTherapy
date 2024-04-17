void uart_init(void);
void uart_transmit(unsigned char);
unsigned char uart_receive(void);

extern volatile unsigned char byte_count;	// keeps track of bytes from rx
extern volatile unsigned char face_flag;	// flag for telling us we are receiving a name result
extern volatile unsigned char facebuf[21];		// buffer for storing name of person
extern volatile unsigned char name_done;	// flag for notifying main loop that face detection is done
extern volatile unsigned char finger_flag;	// flag for telling us we are receiving a finger result
extern volatile unsigned char finger_done;	// flag for notifying main loop that face detection is done
extern volatile unsigned char rfid_flag;	// flag for telling us we are receiving a finger result
extern volatile unsigned char rfid_done;	// flag for notifying main loop that face detection is done
extern volatile unsigned char touch_main_flag;	// flag for notifying main loop that touch succeded
extern volatile unsigned char face_main_flag;	// flag for notifying main loop that face succeded
extern volatile unsigned char rfid_main_flag;	// flag for notifying main loop that rfid succeded
extern volatile unsigned char enable_auth_flag;
extern volatile unsigned int total_auth_flag;
