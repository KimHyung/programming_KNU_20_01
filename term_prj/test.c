#include <stdio.h>
#include <stdlib.h>

void reset () {
  printf("\033[0m");
}

void color_switch (int i) {
	switch(i){
		case 0: printf("\033[7;31m");break;
		case 1: printf("\033[7;32m");break;
		case 2: printf("\033[6;33m");break;
		case 3: printf("\033[6;34m");break;
		case 4: printf("\033[6;35m");break;
		case 5: printf("\033[6;36m");break;
        case 6: printf("\033[6;37m");break;
        case 7: printf("\033[6;38m");break;
        case 8: printf("\033[6;39m");break;
        case 9: printf("\033[6;80m");break;
		default : reset();
	}
}
int main(){
    for(int i=0; i<10; i++){
        printf("%d : ",i);
        color_switch(i);
        printf("Hello world!\n");
        reset(0);
    }

    // int i, j, n;
    
    // for (i = 0; i < 11; i++) {
    //     for (j = 0; j < 10; j++) {
    //     n = 10*i + j;
    //     if (n > 108) break;
    //     printf("\033[%dm %3d\033[m", n, n);
    //     }
    //     printf("\n");
    // }
   
    exit(1);
}