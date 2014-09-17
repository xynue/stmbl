//
//  scanf.c
//  test
//
//  Created by crinq on 05.12.13.
//  Copyright (c) 2013 Rene Hopf. All rights reserved.
//

#include "scanf.h"

int scanf_(const char *format, ...){
    int ret = 0;
    #ifdef USBTERM
    char rx_buf[APP_TX_BUF_SIZE];
    va_list arg;
    if(UB_USB_CDC_GetStatus()==USB_CDC_CONNECTED){
      if(UB_USB_CDC_ReceiveString(rx_buf)==RX_READY){
        va_start(arg, format);
        ret = vfsscanf_(rx_buf, format, arg);
        va_end(arg);
      }else{
        ret = -1;
      }
    }
    #endif
    return(ret);
}

int sscanf_(const char* buf, const char *format, ...){
  int ret = 0;

  va_list arg;
  va_start(arg, format);
  ret = vfsscanf_(buf, format, arg);
  va_end(arg);
  return(ret);
}


int isBinDigit(char c){
    return(c == '0' || c == '1');
}

int isChar(char c){
	return((c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a') || isDecDigit(c));
}

int isDecDigit(char c){
    return(c >= '0' && c <= '9');
}

int isHexDigit(char c){
    return((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')  || (c >= 'a' && c <= 'f'));
}

int isWhitespace(char c){
    return(c == ' ' || c == '\t');
}

int vfsscanf_(const char *buf, const char *format, va_list arg){
    int buffer_pos = 0;
    int format_pos = 0;
		int string_pos = 0;

    int *i;
    float *f;
    char *c;
    int invert;
    float tf;

    int state = 0;  // 0 = last char = normal char
    // 1 = last char = '%'
    // 2 = last char = '\'

    while(format[format_pos]){
        switch(state){
            case 0:
                switch(format[format_pos]){
                    case '%':
                        state = 1;
                    break;

                    case '\'':
                        state = 2;
                    break;

										case ' ': // parse whithespaces
                    	while(isWhitespace(buf[buffer_pos])){
												buffer_pos++;
                      }
                    break;

                    default:
                        if(format[format_pos] != buf[buffer_pos++]){
                            return(format_pos);
                        }
                }
            break;

            case 1:
                switch(format[format_pos]){
                    case '%':
                        if(buf[buffer_pos++] != '%'){
                            return(format_pos);
                        }
                    break;

                    case 'c':
                        c = va_arg(arg, char *);
                        *c = buf[buffer_pos++];
                    break;

					case 's':
						string_pos = 0;
                        c = va_arg(arg, char *);
						while(isChar(buf[buffer_pos])){
							c[string_pos] = buf[buffer_pos++];
							string_pos++;
              c[string_pos] = '\0';
                      	}
                        
                    break;

                    case 'i':
                        i = va_arg(arg, int *);
                        *i = 0;
                        invert = 1;
                        if(buf[buffer_pos] == '-'){
                            invert = -1;
                            buffer_pos++;
                        }
                        else if(buf[buffer_pos] == '+'){
                            buffer_pos++;
                        }
                        while(isDecDigit(buf[buffer_pos])){
                            *i *= 10;
                            *i += buf[buffer_pos++] - '0';
                        }
                        *i *= invert;
                    break;

                    case 'b':
                        i = va_arg(arg, int *);
                        *i = 0;
                        invert = 1;
                        if(buf[buffer_pos] == '-'){
                            invert = -1;
                            buffer_pos++;
                        }
                        else if(buf[buffer_pos] == '+'){
                            buffer_pos++;
                        }
                        if(!(buf[buffer_pos] == 'b' || buf[buffer_pos] == 'B')){
                            return(format_pos);
                        }
                        buffer_pos++;
                        while(isBinDigit(buf[buffer_pos])){
                            *i *= 2;
                            *i += buf[buffer_pos++] - '0';
                        }
                        *i *= invert;
                    break;

                    case 'h':
                        i = va_arg(arg, int *);
                        *i = 0;
                        invert = 1;
                        if(buf[buffer_pos] == '-'){
                            invert = -1;
                            buffer_pos++;
                        }
                        else if(buf[buffer_pos] == '+'){
                            buffer_pos++;
                        }
                        if(buf[buffer_pos++] != '0'){
                            return(format_pos);
                        }
                        if(buf[buffer_pos++] != 'x'){
                            return(format_pos);
                        }
                        while(isHexDigit(buf[buffer_pos])){
                            *i *= 16;
                            if(buf[buffer_pos] < 'A'){
                                *i += buf[buffer_pos++] - '0';
                            }
                            else if(buf[buffer_pos] < 'a'){
                                *i += buf[buffer_pos++] - 'A' + 10;
                            }
                            else{
                                *i += buf[buffer_pos++] - 'a' + 10;
                            }
                        }
                        *i *= invert;
                    break;

                    case 'f':
                        f = va_arg(arg, float *);
                        *f = 0;
												tf = 10;
                        invert = 1;
                        if(buf[buffer_pos] == '-'){
                            invert = -1;
                            buffer_pos++;
                        }
                        else if(buf[buffer_pos] == '+'){
                            buffer_pos++;
                        }
                        while(isDecDigit(buf[buffer_pos])){
                            *f *= 10;
                            *f += buf[buffer_pos++] - '0';
                        }
                        if(buf[buffer_pos] == '.'){
                            buffer_pos++;
                            while(isDecDigit(buf[buffer_pos])){
                                *f += (buf[buffer_pos++] - '0') / tf;
																tf *= 10;
                            }
                        }
                        *f *= invert;
                    break;

                    default:
                        return(format_pos);
                    break;
                }
							state = 0;
            break;

            case 2:
                switch(format[format_pos]){
                    case '\'':
                        if(buf[buffer_pos++] != '\''){
                            return(format_pos);
                        }
                    break;

                    case 'n':
                        if(buf[buffer_pos++] != '\n'){
                            return(format_pos);
                        }
                    break;

                    default:
                        return(format_pos);
                    break;
                }
							state = 0;
            break;

            default:
            break;
        }

        format_pos++;
    }
    return(format_pos);
}
