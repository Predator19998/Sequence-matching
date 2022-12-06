#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <tuple>

using namespace std;

class IOoperator{
    public:
    string s1 = ""; //original strings generated by input file.
    string s2 = "";
    
    /*
    Generating the raw strings before alignment.
    */
    void stringGenerator(int argc, char *argv[]){
        vector<string> lines = this->fileReader(argc, argv);
        string current_string = "";
        int len = 0;        //length of a base string.
        int numOfIndices = 0;          // number of lines consist of indices(j or k).
            for(auto line :lines){
                char char_array[line.length() + 1];
                strcpy(char_array, line.c_str());
                if (char_array[0] <= 'Z' && char_array[0] >='A'){
                    if(current_string == ""){
                        current_string = line;
                        len = current_string.length();
                    }
                    else{
                        if(numOfIndices >20 || len >20000 || pow(2,numOfIndices)*len >20000){
                            perror("Invalid Input String s1");
                        }
                        s1 = current_string;
                        current_string = line;
                        len = current_string.length();
                        numOfIndices = 0;
                    }
                }
                else {
                    numOfIndices++;
                    string temp;
                    int loc = stoi(line);
                    temp = current_string.substr(0,loc+1);
                    temp.append(current_string,0,current_string.length());
                    temp.append(current_string,loc+1,current_string.length()-loc-1);
                    current_string = temp;
                      
                }
            }
            if(numOfIndices >20 || len >20000 || pow(2,numOfIndices)*len >20000){
                perror("Invalid Input String s2");
                
            }
            s2 = current_string;
    }

    /*
    void fileWriter(int cost, string strAlign1, string strAlign2, float time, float memory)
    Writing output data to output file. If there is a output file given, use it as output file; 
    otherwise build a "output.txt" as output file.
    <Parameter>
    cost: Cost of the alignment (Integer)
    strAlign1: First string alignment ( Consists of A, C, T, G, _ (gap) characters)    
    strAlign2: Second string alignment ( Consists of A, C, T, G, _ (gap) characters )
    time: Time in Milliseconds (Float)
    memory: Memory in Kilobytes (Float)

    */

    void fileWriter(int cost, string strAlign1, string strAlign2, float time, float memory){
        ofstream outfile;
        outfile.open(this->outFile);
        if(!outfile.is_open()){
            ofstream outfile("output.txt");
        }
        // ofstream outfile(this->outFile);
       
    
            outfile<<cost<<endl;
            outfile<<strAlign1<<endl;
            outfile<<strAlign2<<endl;
            outfile<<time<<endl;
            outfile<<memory<<endl;
            outfile.close();
        
    }

    /*
    For debug use, print s1 and s2 to command line.
    */
    void stringPrinter(){
        cout<<"s1: "<<s1<<endl;
        cout<<"s2: "<<s2<<endl;
    }


    private:

    /*
    Read lines from given input file, store lines into a std::vector<std::string>.
    */
    vector<string> fileReader(int argc, char *argv[]){
        vector<string> output;
        // this->argc = argc;
        
        char *inLines = NULL;
        string inFile = "";
        this->outFile = "output.txt";
        if (argc == 1){
        perror("Input file invalid");
        }
        inFile = argv[1];
        if( argc >= 3 ) {
        this->outFile = argv[2];
        }
        ifstream infile(inFile);
        string line;  
        if(infile.is_open()){
            while(getline(infile, line)){
                output.push_back(line);
            }

            infile.close();
        }
        else perror("Input file invalid");
        return output;
    }
    string outFile = "";    //output filename.
};

extern int errno;
int cost = 0;

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

//Lookup table for Mismatch penalty
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

string* sequenceAlignment(string x, string y,int pen_gap){
    int i,j;

    int m = x.length();
    int n = y.length();

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
                dp[i][j] = findMin(dp[i-1][j-1] + findPenmiss(x[i-1],y[j-1]),dp[i-1][j] + pen_gap, dp[i][j-1] + pen_gap);
            }
        }
    }

    int l = n+m;

    i=m;j=n;

    int xpos=l;
    int ypos=l;

    char xalign[l+1], yalign[l+1];

    //Finidng the aligned sequence
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

    //printf("Minimum penalty = %d\n",dp[m][n]);
    cost += dp[m][n]; //The minumum penalty of the sequence alignment
    //printf("Aligned Sequence\n");

    string outs1 ="";
    string outs2 ="";
    string * result = new string[2];

    //Storing the aligned sequence
    for(i=use;i<=l;i++){
        //printf("%c",xalign[i]);
        outs1 = outs1+xalign[i];
    }
    //printf("\n");
    for(i=use;i<=l;i++){
        //printf("%c",yalign[i]);
        outs2 = outs2+yalign[i];
    }

    result[0] = outs1;
    result[1] = outs2;

    return result;
}

int * forwards(string x, string y, int pen_gap){
    int n=x.length();
    int m=y.length();

    int dp[2][m+1];

    for(int i=0; i<2; i++) {
        for(int j=0; j<m+1; j++) {
            dp[i][j] = 0;
        }
    }

    for(int j=1; j<m+1; j++) dp[0][j] = dp[0][j-1] + pen_gap;

    for(int i=1; i<n+1; i++) {
        dp[1][0] = dp[0][0] + pen_gap;
        
        for(int j=1; j<m+1; j++){
            dp[1][j] = findMin(dp[0][j-1] + findPenmiss(x[i-1],y[j-1]), dp[0][j] + pen_gap, dp[1][j-1] + pen_gap);
        }

        for(int j=0; j<m+1; j++) dp[0][j] = dp[1][j];        
    }

    int *result = new int[m+1];
    //cout<<"forward=";
    for(int i=0; i<m+1 ; i++) {
        result[i] = dp[1][i];
        //cout<<result[i]<<" ";
    }

    return result;
}

int * backwards(string x, string y, int pen_gap) {
    int n=x.length();
    int m=y.length();

    int dp[2][m+1];

    for(int i=0; i<2; i++) 
        for(int j=0; j<m+1; j++) {
            dp[i][j] = 0;
        }

    for(int j=0; j<m+1; j++) dp[0][j] = pen_gap*j;

    for(int i=1; i<n+1; i++) {
        dp[1][0] = dp[0][0] + pen_gap;
        
        for(int j=1; j<m+1; j++){
            dp[1][j] = findMin(dp[0][j-1] + findPenmiss(x[n-i],y[m-j]), dp[0][j] + pen_gap, dp[1][j-1] + pen_gap);
        }

        for(int j=0; j<m+1; j++) dp[0][j] = dp[1][j];        
    }

    int *result = new int[m+1];
    //cout<<"backward=";
    for(int i=0; i<m+1 ; i++) {
        result[i] = dp[1][i];
        //cout<<result[i]<<" ";
    }

    return result;
}

string * efficient(string x, string y, int pen_gap){
    int n=x.length();
    int m=y.length();

    string a="";
    string b="";
    string * result = new string[2];

    if (n==0) {
        for(int i=0;i<m;i++) {
            a = a + '_';
            b = b + y[i];
        }
    }
    else if(m==0){
        for(int i=0;i<n;i++){
            a = a + x[i];
            b = b + '_';
        }
    }
    else if(n==1 || m==1) {
        result = sequenceAlignment(x,y,pen_gap);
        a = result[0];
        b = result[1];
        //cout<<result[0]<<","<<result[1]<<endl;
        //return result;
        //cout<<a<<endl<<b<<endl;
    }
    else {
        int partition[m+1];

        int xmid = n/2;
        string xleft = x.substr(0,xmid);
        string xright = x.substr(xmid,n);

        //cout<<"find partition"<<endl;

        int * forward = forwards(xleft,y,pen_gap);

        string Xreverse = xright;
        string Yreverse = y;

        reverse(Xreverse.begin(),Xreverse.end());
        reverse(Yreverse.begin(),Yreverse.end());

        //cout<<"forward found"<<endl;
        //int * backward = backwards(x.substr(n/2,n),y,pen_gap);
        int  * backward = forwards(Xreverse,Yreverse,pen_gap);

        for(int j=0; j<m+1; j++) {
            partition[j] = forward[j] + backward[m-j];
            //cout<<partition[j]<<" ";
        }

        //cout<<endl;

        int * partition_begin = &partition[0];
        int * partition_end = &partition[m];

        int min = distance(partition_begin,min_element(partition_begin,partition_end));
        //cout<<"cut is="<<min<<endl;

        forward = NULL;
        backward = NULL;

        string *callleft = new string[2];
        string *callright = new string[2];

        string yleft = y.substr(0,min);
        string yright = y.substr(min,m);

        // cout<<x.substr(0,n/2)<<","<<y.substr(0,min)<<endl;
        // cout<<x.substr(n/2,n)<<","<<y.substr(min,m)<<endl;

        callleft = efficient(xleft,yleft,pen_gap);
        callright = efficient(xright,yright,pen_gap);

        // cout<<"first string="<<callleft[0]<<","<<callright[0]<<endl;
        // cout<<"second string="<<callleft[1]<<","<<callright[1]<<endl;

        a = callleft[0]+callright[0];
        b = callleft[1]+callright[1];

        // result[0] = callleft[0]+callright[0];
        // result[1] = callleft[1]+callright[1];
    }
    result[0] = a;
    result[1] = b;
    return result;
}

int main(int argc, char *argv[]) {

    IOoperator myio;
    string s1,s2;
    string * res2 = new string[2];
    struct timeval begin, end;
    gettimeofday(&begin, 0);
    //write your solution here
    //Please call getTotalMemory() only after calling your solution function. It calculates max memory used by the program.
    myio.stringGenerator(argc, argv);
    //myio.stringPrinter();
    int pen_gap = 30;
    //cout<<"start the alignment"<<endl;
    res2 = efficient(myio.s1,myio.s2,pen_gap);
    //res2 = efficient("AAAAAA","TATATATATATA",pen_gap);
    double totalmemory = getTotalMemory();
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double totaltime = seconds*1000 + microseconds*1e-3;
    // printf("\nTotal time = %f\n", totaltime);
    printf("Total memory = %f\n", totalmemory);
    //myio.fileWriter(cost,outs1,outs2,totaltime,totalmemory);
    cout<<"cost="<<forwards(myio.s1,myio.s2,pen_gap)[myio.s2.length()]<<endl;
    cout<<res2[0]<<"\n"<<res2[1];
}