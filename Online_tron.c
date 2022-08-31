#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int fb, a_big = 0, w_big = 0, s_big = 0, d_big = 0, a_small = 0, w_small = 0, s_small = 0, d_small = 0;
struct fb_var_screeninfo info;
size_t fb_size, map_size, page_size;
uint32_t *ptr; 
int x_window_gl, y_window_gl, y_center_gl, x_center_gl; 
int work_flag = 1;
struct sockaddr_in me;
struct sockaddr_in other;
int my_fd;
int other_fd;
char direction = '0',other_ch = '0';
int size = sizeof(me);
int my[4], not_my[4];
int sum1, sum2, crash_big, crash_small;
char IP[15]="";

void handler(int none) {
  work_flag = 0;
}

void clav() {
	int m;
  if(sum1 > sum2) {
    while(m < 2) {
      direction = getch();
      switch(direction) {
        case 'w':
	 if(s_big == 1) break;
          a_big = s_big = d_big = 0;
          w_big = 1;
	  sendto(my_fd,&direction,1,MSG_CONFIRM,(struct sockaddr*)&other,size);
          break;
        case 'a':
	  if(d_big == 1) break;
          w_big = s_big = d_big = 0;
          a_big = 1;
          sendto(my_fd,&direction,1,MSG_CONFIRM,(struct sockaddr*)&other,size);
          break;
        case 's':
	  if(w_big == 1) break;
          w_big = a_big = d_big = 0;
          s_big = 1;
          sendto(my_fd,&direction,1,MSG_CONFIRM,(struct sockaddr*)&other,size);
          break;
        case 'd':
	  if(a_big == 1) break;
          w_big = a_big = s_big = 0;
          d_big = 1;
          sendto(my_fd,&direction,1,MSG_CONFIRM,(struct sockaddr*)&other,size);
          break;      
      }
     if(crash_big != 0 || crash_small != 0) m++;   
    } 
  }
  if(sum1 < sum2) {
    while(m < 2) {
      direction = getch();
      switch(direction) {
        case 'w':
	  if(s_small == 1) break;
          a_small = s_small = d_small = 0;
          w_small = 1;
	  sendto(my_fd,&direction,1,MSG_CONFIRM,(struct sockaddr*)&other,size);
          break;
        case 'a':
	  if(d_small == 1) break;
          w_small = s_small = d_small = 0;
          a_small = 1;
          sendto(my_fd,&direction,1,MSG_CONFIRM,(struct sockaddr*)&other,size);
          break;
        case 's':
	  if(w_small == 1) break;
          w_small = a_small = d_small = 0;
          s_small = 1;
          sendto(my_fd,&direction,1,MSG_CONFIRM,(struct sockaddr*)&other,size);
          break;
        case 'd':
	  if(a_small == 1) break;
          w_small = a_small = s_small = 0;
          d_small = 1;
          sendto(my_fd,&direction,1,MSG_CONFIRM,(struct sockaddr*)&other,size);
          break;      
      }
    if(crash_big != 0 || crash_small != 0) m++;   
    } 
  }

}


void* recw(){
  int m = 0;
  if(sum1 < sum2) {
    while(m < 2){  
      recvfrom(my_fd,&other_ch,1,MSG_WAITALL,(struct sockaddr*)&me,&size);
      switch(other_ch) {
        case 'w':
          if(s_big == 1) break;
	  a_big = s_big = d_big = 0;
          w_big = 1;
	  break;
        case 'a':
	  if(d_big == 1) break;
          w_big = s_big = d_big = 0;
          a_big = 1;
          break;
        case 's':
	  if(w_big == 1) break;
          w_big = a_big = d_big = 0;
          s_big = 1;
          break;
        case 'd':
	  if(a_big == 1) break; 
          w_big = a_big = s_big = 0;
          d_big = 1;
          break;      
      }
     if(crash_big != 0 || crash_small != 0) m++;  
    }
  }
  if(sum1 > sum2) {
    while(m < 2){   
      recvfrom(my_fd,&other_ch,1,MSG_WAITALL,(struct sockaddr*)&me,&size);
      switch(other_ch) {
        case 'w':
	  if(s_small == 1) break;	
          a_small = s_small = d_small = 0;
          w_small = 1;
          break;
        case 'a':
	  if(d_small == 1) break;
          w_small = s_small = d_small = 0;
          a_small = 1;
          break;
        case 's':
	  if(w_small == 1) break;
          w_small = a_small = d_small = 0;
          s_small = 1;
          break;
        case 'd':
	  if(a_small == 1) break;
          w_small = a_small = s_small = 0;
          d_small = 1;
          break;      
      }
    if(crash_big != 0 || crash_small != 0) m++;   
    } 
  }
}

void bort() {
  uint32_t color_0 = 0xffffffff;
  int i,k,j;
 mvprintw(10,10,"Press any key to start the game)");
 refresh(); 
 scanw("%d", &k);
mvprintw(10,10,"                                                             ");
  refresh();
  move(1,COLS-10);
  curs_set(0);
  refresh();
  
  ptr[(y_center_gl - (y_window_gl/2)) * info.xres_virtual + (x_center_gl - (x_window_gl/2))] = color_0;
  for(i = 1; i < x_window_gl; i++) {
    ptr[(y_center_gl - y_window_gl/2) * info.xres_virtual + (x_center_gl - x_window_gl/2) + i] = color_0;
  }
  for(i = 1; i < y_window_gl; i++) {
    ptr[(y_center_gl - (y_window_gl/2) + i) * info.xres_virtual + (x_center_gl + (x_window_gl/2))] = color_0;
  }
  for(i = 1; i < x_window_gl; i++) {
    ptr[(y_center_gl + y_window_gl/2) * info.xres_virtual + (x_center_gl + x_window_gl/2) - i] = color_0;
  }
  for(i = 1; i < y_window_gl-1; i++) {
    ptr[(y_center_gl + y_window_gl/2 - i) * info.xres_virtual + (x_center_gl - x_window_gl/2)] = color_0;
  }
  i = 0;
  sendto(my_fd,&k,1,MSG_CONFIRM,(struct sockaddr*)&other,size);  
  while(i == 0) {
    if(other_ch != '0') i++;
  } 
  d_big = 1, a_small = 1;
}

void go_big() { 
  int x_window = x_window_gl, y_window = y_window_gl, y_center = y_center_gl, x_center = x_center_gl; 
  int i,j,m,gg = 0;
  int x = 9, y = 5, xstep = 1, ystep = 1;
  int flag_w, flag_a, flag_s, flag_d;
  uint32_t color_0 = 0xfff00000;
  while(m < 2) {
    int a = a_big, w = w_big, s = s_big, d = d_big; 
    if(d == 1) {
      if(flag_d == 0){
	if(flag_w == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) { 
      	      if(j == 0) continue;
	      ptr[(y_center - (y_window/2) + y + i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = 0;
	    }
	  }  
	}
	if(flag_s == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {	  
      	      if(j == 0) continue;
	      ptr[(y_center - (y_window/2) + y - i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = 0;
	    }
	  }  
	}
	x += 9;
	ptr[(y_center - (y_window/2) + y) * info.xres_virtual + (x_center - (x_window/2)) + x - 8] = color_0;
	for(i = 0; i < 8; i++) {
	  for(j = -2; j < 3; j++) {
	    if(ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - 7 + i] != 0) {
	  gg = 1;
	  break;
	}
	    ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - 7 + i] = color_0;
	  }
	  if(gg != 0) break;
	}
	if(gg != 0) {
	  crash_big++;
	  break;
        }
	flag_d = 1, flag_w = 0, flag_a = 0, flag_s = 0;
      }
      for(j = -2; j < 3; j++) {
	if(ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + xstep] != 0) {
	  gg = 1;
	  break;
	}
        ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + xstep] = color_0;
        if(j == 0) continue;
	ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - 7] = 0; 
      }
      if(gg != 0) {
	crash_big++;
	break;
      }
      x++; 
    }
    if(s == 1) {
      if(flag_s == 0){
	if(flag_a == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {
	      if(j == 0) continue;	    
      	      ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + i] = 0;
	    }
	  }
	}
	if(flag_d == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {
	      if(j == 0) continue;	    
      	      ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - i] = 0;
      	    }
	  }
	}
	y += 9;
	ptr[(y_center - (y_window/2) + y - 8) * info.xres_virtual + (x_center - (x_window/2)) + x] = color_0;
      	for(i = 0; i < 8; i++) {
	  for(j = -2; j < 3; j++) {
	    if(ptr[(y_center - (y_window/2) + y - 7 + i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] != 0) {
	  gg = 1;
	  break;
	}
	    ptr[(y_center - (y_window/2) + y - 7 + i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = color_0;
	  }
	  if(gg != 0) break;
	}
	if(gg != 0 ) {
	  crash_big++;
	  break;
        }
	flag_d = 0, flag_w = 0, flag_a = 0, flag_s = 1;
      }
      for(j = -2; j < 3; j++) {
	if(ptr[(y_center - (y_window/2) + y + ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] != 0) {
	  gg = 1;
	  break;
	}
        ptr[(y_center - (y_window/2) + y + ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = color_0;
        if(j == 0) continue;
        ptr[(y_center - (y_window/2) + y - 7) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = 0;
      }
      	if(gg != 0 ) {
	  crash_big++;
	  break;
        }
      y++;  
    }
    if(a == 1) {
      if(flag_a == 0){
        if(flag_w == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {
	      if(j == 0) continue;	      
	      ptr[(y_center - (y_window/2) + y + i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = 0;
	    }	
	  }
	}
	if(flag_s == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {
	      if(j == 0) continue;
      	      ptr[(y_center - (y_window/2) + y - i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = 0;
            }
	  }
	}
	x -= 9;
	ptr[(y_center - (y_window/2) + y) * info.xres_virtual + (x_center - (x_window/2)) + x + 8] = color_0;
      	for(i = 0; i < 8; i++) {
	  for(j = -2; j < 3; j++) {
	    if(ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + 7 - i] != 0) {
	  gg = 1;
	  break;
	}
	    ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + 7 - i] = color_0;
	  }
	  if(gg != 0) break;
	}
	if(gg != 0 ) {
	  crash_big++;
	  break;
	}
	flag_d = 0, flag_w = 0, flag_a = 1, flag_s = 0;
      }
      for(j = -2; j < 3; j++) {
        if(ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - xstep] != 0) {
	  gg = 1;
	  break;
	}

      	ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - xstep] = color_0;
	if(j == 0) continue;
        ptr[(y_center - (y_window/2) + y - j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + 7] = 0;
      }
      if(gg != 0 ) {
	crash_big++;
	break;
      }
      x--;
    }
    if(w == 1) {
      if(flag_w == 0){
	if(flag_a == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {	  
	      if(j == 0) continue;
	      ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + i] = 0;
	    }
	  }
	}
	if(flag_d == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {
	      if(j == 0) continue;	    
      	      ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - i] = 0; 
	    }
	  }
	}
      	y -= 9;
	ptr[(y_center - (y_window/2) + y + 8) * info.xres_virtual + (x_center - (x_window/2)) + x] = color_0;
      	for(i = 0; i < 8; i++) {
	  for(j = -2; j < 3; j++) {
	    if(ptr[(y_center - (y_window/2) + y + 7 - i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] != 0) {
	  gg = 1;
	  break;
	}
	    ptr[(y_center - (y_window/2) + y + 7 - i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = color_0;
	  }
	  if(gg != 0) break;
	}        
	if(gg != 0 ) {
	  crash_big++;
	  break;
        }
        flag_d = 0, flag_w = 1, flag_a = 0, flag_s = 0;	
      }
      for(j = -2; j < 3; j++) {
        if(ptr[(y_center - (y_window/2) + y - ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] != 0) {
	  gg = 1;
	  break;
	}

        ptr[(y_center - (y_window/2) + y - ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = color_0;
	if(j == 0) continue;
        ptr[(y_center - (y_window/2) + y + 7) * info.xres_virtual + (x_center - (x_window/2)) + x - j*xstep] = 0;
      }
      if(gg != 0 ) {
	crash_big++;
	break;
      }
    y--;  
  }
  usleep(64500);
  if(crash_small != 0) m++; 
 }
}

void go_small() {
  int i,j,m,gg = 0;
  int xstep = 1, ystep = 1;
  int flag_w, flag_a, flag_s, flag_d;
  uint32_t color_0 = 0x00000fff;
  int x_window = x_window_gl, y_window = y_window_gl, y_center = y_center_gl, x_center = x_center_gl; 
  int x = x_window - 9;
  int y = y_window - 6;
  while(m < 2) {
    int a = a_small, w = w_small, s = s_small, d = d_small;	  
    if(d == 1) {
      if(flag_d == 0){
	if(flag_w == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) { 
      	      if(j == 0) continue;
	      ptr[(y_center - (y_window/2) + y + i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = 0;
	    }
	  }  
	}
	if(flag_s == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {	  
      	      if(j == 0) continue;
	      ptr[(y_center - (y_window/2) + y - i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = 0;
	    }
	  }  
	}
	x += 9;
	ptr[(y_center - (y_window/2) + y) * info.xres_virtual + (x_center - (x_window/2)) + x - 8] = color_0;
	for(i = 0; i < 8; i++) {
	  for(j = -2; j < 3; j++) {
	    if(ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - 7 + i] != 0) {
	  gg = 1;
	  break;
	}
	    ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - 7 + i] = color_0;
	  }
	  if(gg != 0) break;
	}
	if(gg != 0 ) {
	  crash_small++;
	  break;
        } 	
	flag_d = 1, flag_w = 0, flag_a = 0, flag_s = 0;
      }
      for(j = -2; j < 3; j++) {
	if(ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + xstep] != 0) {
	  gg = 1;
	  break;
	}

        ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + xstep] = color_0;
        if(j == 0) continue;
	ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - 7] = 0; 
      }
      if(gg != 0 ) {
	crash_small++;
	break;
      }	
      x++; 
    }
    if(s == 1) {
      if(flag_s == 0){
	if(flag_a == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {
	      if(j == 0) continue;	    
      	      ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + i] = 0;
	    }
	  }
	}
	if(flag_d == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {
	      if(j == 0) continue;	    
      	      ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - i] = 0;
      	    }
	  }
	}
	y += 9;
	ptr[(y_center - (y_window/2) + y - 8) * info.xres_virtual + (x_center - (x_window/2)) + x] = color_0;
      	for(i = 0; i < 8; i++) {
	  for(j = -2; j < 3; j++) {
	    if(ptr[(y_center - (y_window/2) + y - 7 + i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] != 0) {
	  gg = 1;
	  break;
	}
	    ptr[(y_center - (y_window/2) + y - 7 + i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = color_0;
	  }
	  if(gg != 0) break;
	}
	if(gg != 0 ) {
	  crash_small++;
	  break;
        }
	flag_d = 0, flag_w = 0, flag_a = 0, flag_s = 1;
      }
      for(j = -2; j < 3; j++) {
	if(ptr[(y_center - (y_window/2) + y + ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] != 0) {
	  gg = 1;
	  break;
	}

        ptr[(y_center - (y_window/2) + y + ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = color_0;
        if(j == 0) continue;
        ptr[(y_center - (y_window/2) + y - 7) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = 0;
      }
      if(gg != 0 ) {
	crash_small++;
	break;
      }
      y++;  
    }
    if(a == 1) {
      if(flag_a == 0){
        if(flag_w == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {
	      if(j == 0) continue;	      
	      ptr[(y_center - (y_window/2) + y + i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = 0;
	    }	
	  }
	}
	if(flag_s == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {
	      if(j == 0) continue;
      	      ptr[(y_center - (y_window/2) + y - i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = 0;
            }
	  }
	}
       	x -= 9;
	ptr[(y_center - (y_window/2) + y) * info.xres_virtual + (x_center - (x_window/2)) + x + 8] = color_0;
      	for(i = 0; i < 8; i++) {
	  for(j = -2; j < 3; j++) {
	    if(ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + 7 - i] != 0) {
	  gg = 1;
	  break;
	}
	    ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + 7 - i] = color_0;
	  }
	  if(gg != 0) break;
	}
	if(gg != 0 ) {
	  crash_small++;
	  break;
        }
	flag_d = 0, flag_w = 0, flag_a = 1, flag_s = 0;
      }      
      for(j = -2; j < 3; j++) {
        if(ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - xstep] != 0) {
	  gg = 1;
	  break;
	}

      	ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - xstep] = color_0;
	if(j == 0) continue;
        ptr[(y_center - (y_window/2) + y - j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + 7] = 0;
      }
      if(gg != 0 ) {
	crash_small++;
	break;
      }
      x--;
    }
    if(w == 1) {
      if(flag_w == 0){
	if(flag_a == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {	  
	      if(j == 0) continue;
	      ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + i] = 0;
	    }
	  }
	}
	if(flag_d == 1) {
	  for(i = 0; i < 8; i++) {
	    for(j = -2; j < 3; j++) {
	      if(j == 0) continue;	    
      	      ptr[(y_center - (y_window/2) + y + j*ystep) * info.xres_virtual + (x_center - (x_window/2)) + x - i] = 0; 
	    }
	  }
	}
      	y -= 9;
	ptr[(y_center - (y_window/2) + y + 8) * info.xres_virtual + (x_center - (x_window/2)) + x] = color_0;
      	for(i = 0; i < 8; i++) {
	  for(j = -2; j < 3; j++) {
	    if(ptr[(y_center - (y_window/2) + y + 7 - i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] != 0) {
	  gg = 1;
	  break;
	}
	    ptr[(y_center - (y_window/2) + y + 7 - i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = color_0;
	  }
	  if(gg != 0) break;
	}	  
        if(gg != 0 ) {
	  crash_small++;
	  break;
        }
      	ptr[(y_center - (y_window/2) + y + i) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = color_0;
        flag_d = 0, flag_w = 1, flag_a = 0, flag_s = 0;	
      }
      for(j = -2; j < 3; j++) {
        if(ptr[(y_center - (y_window/2) + y - ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] != 0) {
	  gg = 1;
	  break;
	}

        ptr[(y_center - (y_window/2) + y - ystep) * info.xres_virtual + (x_center - (x_window/2)) + x + j*xstep] = color_0;
	if(j == 0) continue;
        ptr[(y_center - (y_window/2) + y + 7) * info.xres_virtual + (x_center - (x_window/2)) + x - j*xstep] = 0;
    }
    if(gg != 0) {
	crash_small++;
	break;
    }
    y--;  
  }
  usleep(64500);
  if(crash_big != 0) m++;
  }
}

int main(int argc, char *argv[]) {
  uint32_t color_0 = 0xffffffff;
  int i;
  FILE* stream = popen("ip route get 1.2.3.4 | awk '{print$7}'", "r");
  fgets(IP, 15, stream);
  pclose(stream);

  signal(SIGINT, handler); 
  page_size = sysconf(_SC_PAGESIZE);
  
  if ( 0 > (fb = open("/dev/fb0", O_RDWR))) {
    perror("open");
    return __LINE__;
  }

  if ( (-1) == ioctl(fb, FBIOGET_VSCREENINFO, &info)) {
    perror("ioctl");
    close(fb);
    return __LINE__;
  }

  fb_size = sizeof(uint32_t) * info.xres_virtual * info.yres_virtual;
  map_size = (fb_size + (page_size - 1 )) & (~(page_size-1));

  ptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
  
  if ( MAP_FAILED == ptr ) {
    perror("mmap");
    close(fb);
    return __LINE__;
  }

  if(NULL == initscr())
    return __LINE__;
  
  if(argc<3){
    printf("Usage: %s Your_ip enemy_ip\n",argv[0]);
    exit(EXIT_FAILURE);
  }
  if ( (my_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  me.sin_family    = AF_INET;
  me.sin_addr.s_addr = inet_addr(IP);
  me.sin_port = htons(12345);
  if ( bind(my_fd, (const struct sockaddr *)&me,sizeof(me)) < 0 ) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if ( (other_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  other.sin_family = AF_INET;
  other.sin_addr.s_addr = inet_addr((argv[3]));
  other.sin_port = htons(12345);
  
  sscanf(IP,"%d.%d.%d.%d",&my[0],&my[1],&my[2],&my[3]);
  sscanf(argv[3],"%d.%d.%d.%d",&not_my[0],&not_my[1],&not_my[2],&not_my[3]);
  sum1 = my[0] + my[1] + my[2] + my[3];
  sum2 = not_my[0] + not_my[1] + not_my[2] + not_my[3];

  pthread_t sock;
  pthread_create(&sock, NULL, recw, NULL); 

  noecho();
  keypad(stdscr, TRUE);
  curs_set(2);
  refresh();
  x_window_gl = atoi(argv[1])-1;
  if(x_window_gl > 1919) x_window_gl = 1919;
  y_window_gl = atoi(argv[2])-1;
  if(y_window_gl > 1079) y_window_gl = 1079;
  y_center_gl = info.yres_virtual/2;
  x_center_gl = info.xres_virtual/2;
  bort();
  pthread_t claw;
  pthread_create(&claw, NULL, clav, NULL);
  pthread_t big;
  pthread_create(&big, NULL, go_big, NULL);
  pthread_t small;
  pthread_create(&small, NULL, go_small, NULL);
  while(crash_big == 0 && crash_small == 0) sleep(1);
  a_big = 0, w_big = 0, s_big = 0, d_big = 0, a_small = 0, s_small = 0, d_small = 0, w_small = 0;

  if(crash_big == 1 && crash_small == 0) {
    printw("Blue win!");
    refresh();
    sleep(3);
  }
  if(crash_big == 0 && crash_small == 1) {
    printw("Red win!");
    refresh();
    sleep(3);
  }
  if(crash_big == 1 && crash_small == 1) {
    printw("Both lost!");
    refresh();
    sleep(3);
  }

  munmap(ptr, map_size);
  close(fb);
  endwin();
  system("reset");
  system("clear");
  exit(1);
  }
