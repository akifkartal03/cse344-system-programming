#include <stdio.h>

int cozum (int N){

    int arr[32];
    int i = 0;
    while (N > 0) {
        arr[i] = N % 2;
        N = N / 2;
        i++;
    }
    int start = 0,counter = 0,stop = 0,temp = 0;
    for (int j = i - 1; j >= 0; j--){
        if (arr[j] == 1  && start && !stop)
        {
            stop = 1;
            start = 0;
            if (counter > temp)
            {
                temp = counter;
            }
            counter = 0;
        }
        if (arr[j] == 1 && !start)
        {
            start = 1;
            stop = 0;
        }
        if (arr[j] == 0  && start && !stop)
        {
            counter++;
        }
        
    }
        
    return temp;  
}
int main(){
    printf("cozum(1041): %d\n",cozum(1041));
    return 0;
}