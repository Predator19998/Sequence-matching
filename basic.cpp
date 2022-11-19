#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

extern int errno;
int miss_arr[4][4] = {0,110,48,94,
                    110,0,118,48,
                    48,118,0,110,
                    94,48,110,0};

// getrusage() is available in linux. Your code will be evaluated in Linux OS.
long getTotalMemory() {
    struct rusage usage;
    int returnCode = getrusage(RUSAGE_SELF, &usage);
    if(returnCode == 0) {
    return usage.ru_maxrss;
} else {
    //It should never occur. Check man getrusage for more info to debug.
    // printf("error %d", errno);
    return -1;
    }
}

int findMin(int a, int b, int c){
    if(a<b && a<c) return a;
    else if(b<c) return b;
    return c;
}

int findPenmiss(char a, char b) {
    int i,j;
    switch(a) {
        case 'A' : i=0;break;
        case 'C' : i=1;break;
        case 'G' : i=2;break;
        case 'T' : i=3;break;
        default:break;
    }
    switch(b) {
        case 'A' : j=0;break;
        case 'C' : j=1;break;
        case 'G' : j=2;break;
        case 'T' : j=3;break;
        default:break;
    }
    return miss_arr[i][j];
}

void sequenceAlignment(char* x, char* y,int pen_gap){
    int i,j;

    int m = strlen(x);
    int n = strlen(y);

    int dp[n+m+1][n+m+1];

    for(i=0;i<=(n+m);i++) {
        dp[i][0] = i*pen_gap;
        dp[0][i] = i*pen_gap;
    }

    for(i=1;i<=m;i++) {
        for(j=1;j<=n;j++) {
            if(x[i-1] == y[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = findMin(dp[i-1][j-1] + findPenmiss(x[i-1],y[j-1]), dp[i-1][j] + pen_gap, dp[i][j-1] + pen_gap);
            }
        }
    }

    int l = n+m;

    i=m;j=n;

    int xpos=l;
    int ypos=l;

    char xalign[l+1], yalign[l+1];

    while(!(i==0||j==0)){
        if(x[i-1] == y[j-1]) {
            xalign[xpos--] = x[i-1];
            yalign[ypos--] = y[j-1];
            i--;j--;
        }
        else if (dp[i-1][j-1] + findPenmiss(x[i-1],y[j-1]) == dp[i][j]) {
            xalign[xpos--] = x[i-1];
            yalign[ypos--] = y[j-1];
            i--;j--;            
        }
        else if(dp[i-1][j] + pen_gap == dp[i][j]) {
            xalign[xpos--] = x[i-1];
            yalign[ypos--] = '_';
            i--;            
        }
        else if(dp[i][j-1] + pen_gap == dp[i][j]) {
            xalign[xpos--] = '_';
            yalign[ypos--] = y[j-1];
            j--;            
        }
    }
    while (xpos>0) {
        if(i>0) xalign[xpos--] = x[--i];
        else xalign[xpos--] = '_';
    }
    while (ypos>0) {
        if(j>0) yalign[ypos--] = y[--j];
        else yalign[ypos--] = '_';
    }

    int use=1;
    i=1;
    while(yalign[i] == '_' && xalign[i] == '_') {
        use=i+1;
        i++;
    }

    printf("Minimum penalty = %d\n",dp[m][n]);
    printf("Aligned Sequence\n");
    for(i=use;i<=l;i++){
        printf("%c",xalign[i]);
    }
    printf("\n");
    for(i=use;i<=l;i++){
        printf("%c",yalign[i]);
    }    

}

int main() {
    struct timeval begin, end;
    gettimeofday(&begin, 0);
    //write your solution here
    //Please call getTotalMemory() only after calling your solution function. It calculates max memory used by the program.
    char string1[] = "AGGGCT";
    char string2[] = "AGGCA";

    int pen_gap = 30;

    sequenceAlignment(string1,string2,pen_gap);
    double totalmemory = getTotalMemory();
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double totaltime = seconds*1000 + microseconds*1e-3;
    printf("\nTotal time = %f\n", totaltime);
    printf("Total memory = %f\n", totalmemory);
}