#ifndef ESP8266_ADAPTER_H
#define ESP8266_ADAPTER_H

#if defined(__cplusplus) 
extern "C" {
#endif


/* The following functions must be defined for this platform:
 * i2c_write(unsigned char slave_addr, unsigned char reg_addr,
 *      unsigned char length, unsigned char const *data)
 * i2c_read(unsigned char slave_addr, unsigned char reg_addr,
 *      unsigned char length, unsigned char *data)
 * delay_ms(unsigned long num_ms)
 * get_ms(unsigned long *count)
 * reg_int_cb(void (*cb)(void), unsigned char port, unsigned char pin)
 * labs(long x)
 * fabsf(float x)
 * min(int a, int b)
 */

void esp8266_delay_ms( unsigned long num_ms );
void esp8266_get_ms( unsigned long *count );
int esp8266_i2c_read(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data);
int esp8266_i2c_write(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data);

void esp8266_trace( const char * format, ... );

#define i2c_write(a, b, c, d) 	esp8266_i2c_write(a, b, c, d)
#define i2c_read(a, b, c, d)  	esp8266_i2c_read(a, b, c, d)
#define delay_ms(ms)  			esp8266_delay_ms(ms)
#define get_ms(ms_addr)    		esp8266_get_ms(ms_addr)
#define log_i    				esp8266_trace
#define log_e    				esp8266_trace
#ifndef min
#define min(a,b) 				((a<b)?a:b)
#endif
static inline int reg_int_cb(struct int_param_s* dummy)
{
	return -1;
}

#if defined(__cplusplus) 
}
#endif


#endif // _ARDUINO_MPU9250_I2C_H_
