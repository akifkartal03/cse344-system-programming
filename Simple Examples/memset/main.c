#include <stdio.h>
#include <string.h>
#define SIZE 10

void printArray(int arr[],int size);

int main(){
    int arr[SIZE];

    memset(arr,-1,SIZE*sizeof(int));
    printf("Array after memset\n");
    printArray(arr,SIZE);
    return 0;
}
void printArray(int arr[],int size){
    for (int i = 0; i < size; i++)
    {
        printf("%d\n",arr[i]);
    }

}