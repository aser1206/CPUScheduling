/*Alexandru Seremet
Homework 3 
Section 1
Tested on Debian/Stretch & athena
*/
#include <queue>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <bits/stdc++.h>
using namespace std;
class Process {
    public:
    int pid;        //process number
    int priority;   //process priority
    int arrT;       //arrival time
    int burstT;     //burst time
    int waitT;      //waiting time
    int doneT;      //completion tim
    int leftBT;     //burst time left
    int leftAT;     //arrival time left
    int responseT;
	int outT;
	int inT;
    int tat;        //turn around time
};

void roundRobin(Process p[], int time_quantum, int n); //function to do the round robin calculation
void SJF(Process p[], int n);// shortest job first function
void PR_noPREMP(Process p[], int n); //priority schedule with no preemption
void PR_withPREMP(Process p[], int n);  //priority schedule with preemption
bool first_line_contains_string(const string &message);  //first line to read from the file
int get_time_quantum(); //get the quantum time from the input file
int get_n(); //number of  processess from input file
void update_processes(Process p[]);
Process extract_process(const string &process_s);
string get_nth_line_of_file(int line_num);
void findWaitingTime(Process p[], int n, int quantum);
void findTurnAroundTime(Process p[], int n);
string infile; //input file name
string outfile; //output file name

int main(int argc, char const *argv[]){
    if (argc < 2){     // make sure there are 2 args in the command line
        cerr << "Wrong number of arguments in command line";
        return 1;
    } else{
        infile = argv[1];
        if (first_line_contains_string("RR")){
            int time_quantum = get_time_quantum();
            int n = get_n();
            Process processes[n];
            update_processes(processes);
            roundRobin(processes, n, time_quantum);
        } else if (first_line_contains_string("SJF")){
            int n = get_n();
            Process processes[n];
            update_processes(processes);
            SJF(processes, n);
        } else if (first_line_contains_string("PR_noPREMP")){
            int n = get_n();
            Process processes[n];
            update_processes(processes);
            PR_noPREMP(processes, n);
        } else if (first_line_contains_string("PR_withPREMP")){
            int n = get_n();
            Process processes[n];
            update_processes(processes);
            PR_withPREMP(processes, n);
        }
    }
    return 0;
}
                    //checks if the first line contains message
bool first_line_contains_string(const string &message){
    string line;
    bool found = false;
    line = get_nth_line_of_file(1);
                    //check if line contains our message
    if (line.find(message) != string::npos) //npos check if the reader has reached the end of line
        found = true;
    return found;
}
                    //gets the nth line in a file
string get_nth_line_of_file(int line_num){
    ifstream my_file(infile.c_str());
    string line;
    int current_line = 0;
    if (my_file.is_open()){   // make sure file was opened
        while (getline(my_file, line)){
            current_line++;
            if (current_line == line_num)
                break;
        }
        my_file.close();
    }
    return line;
}
                    //extract time quantum from the file.
int get_time_quantum(){
    string line = get_nth_line_of_file(1);
    size_t space_location = line.find(' '); //skip the space in the file 
    string time_quantum_string = line.substr(space_location + 1); //this line gives the quantum time
    return atoi(time_quantum_string.c_str()); //convert time from string to int
}
                    //extract the number of processess
int get_n(){
    string n_string = get_nth_line_of_file(2); //the number of processes is located in second line
    return atoi(n_string.c_str());
}
                    //store the processes in an array
void update_processes(Process p[]){
    int n = get_n();
    int i;
    string processes_s[n];
                    //skip first two lines. Processess start at line 3
    i = 3;
    while (i < n){
         processes_s[i] = get_nth_line_of_file(i);
        i++;
    }
    i = 0;
    while (i < n){
       p[i] = extract_process(processes_s[i]); //call function to convert processes to object
       i++; 
    }
}
                    //process will become an object and we are using our defined class to create objects
Process extract_process(const string &process_s){
    Process p;
    string p_vals[4];
    int i = 0;
    stringstream ssin(process_s);
    while (ssin.good() && i < 4){
        ssin >> p_vals[i];
        ++i;
    }
    p.pid = atoi(p_vals[0].c_str());
    p.arrT = atoi(p_vals[1].c_str());
    p.burstT = atoi(p_vals[2].c_str());
    p.priority = atoi(p_vals[3].c_str());
    return p;
}
                //function needed to sort processes based on their burst time
bool compareBT (Process a, Process b){
    return (a.burstT > b.burstT);
}
                //compare the priority of the processess
bool priorityCompare(Process a, Process b){
    return (a.priority > b.priority);
}
                //find the waiting time. For process 0 waiting time is 0
void findWaitingTime(Process p[], int n){
    p[0].waitT = 0 ;
    int i=1; //because of waiting time 0
    while (i < n){
        p[i].waitT = p[i - 1].burstT + p[i - 1].waitT;
        i++;
    }   
}
void addToHeap(Process Heap[], Process value, int *heapsize, int *currentTime){
    int begin = *heapsize, i;
    Heap[*heapsize] = value;
    if (Heap[*heapsize].inT == -1)
        Heap[*heapsize].inT = *currentTime;
    ++(*heapsize);
    while (begin != 0 && Heap[(begin - 1) / 2].priority > Heap[begin].priority){
        Process temp = Heap[(begin - 1) / 2];
        Heap[(begin - 1) / 2] = Heap[begin];
        Heap[begin] = temp;
        begin = (begin - 1) / 2;
    }
}
            // reorder processes based on the priority level
void reorder(Process Heap[], int *heapsize, int start){
    int low = start;
    int left = 2 * start + 1;
    int right = 2 * start + 2;
     if (right < *heapsize && Heap[right].priority < Heap[low].priority)
        low = right;
    if (left < *heapsize && Heap[left].priority <Heap[low].priority)
        low = left;
    // Ordering the Heap
    if (low != start){
        Process temp = Heap[low];
        Heap[low] = Heap[start];
        Heap[start] = temp;
        reorder(Heap, heapsize, low);
    }
}
Process getMin(Process Heap[], int *heapsize, int *currentTime){
    Process min = Heap[0];
    if (min.responseT == -1)
        min.responseT = *currentTime - min.arrT;
    --(*heapsize);
    if (*heapsize >= 1){
        Heap[0] = Heap[*heapsize];
        reorder(Heap, heapsize, 0);
    }
    return min;
}
                // time interval compare
bool compare(Process p1, Process p2){
    return (p1.arrT > p2.arrT);
}
                // process the highest priority Process from Heap[].
void execHighPriority(Process Heap[], Process array[], int n, int *heapsize, int *currentTime){
    if (heapsize == 0)
        return;
    Process min = getMin(Heap, heapsize, currentTime);
    min.outT = *currentTime + 1;
    --min.burstT;
    stringstream ss, ss2;
    ss << *currentTime;
    outfile.append(ss.str());
    outfile.append("\t");
    ss2 << min.pid;
    outfile.append(ss2.str());
    outfile.append("\n");
    if (min.burstT > 0) {   // not finished processess are added back to heap
        addToHeap(Heap, min, heapsize, currentTime);
        return;
    }
    int i = 0;
    while (i < n){
        if (array[i].pid == min.pid){
            array[i] = min;
            break;
        }
        i++;
    }
}
                //implement round robin on the processes
void roundRobin(Process p[], int n, int q){
    ofstream myfile("output.txt");  //output file
    // result of average times
    int res = 0;
    int resc = 0;
    string seq; // for sequence storage
    // copy the burst array and arrival array -> actual array unmodified
    int i = 0;
    while (i < n){
        p[i].leftBT = p[i].burstT;
        p[i].leftAT = p[i].arrT;
        i++;
    }
    int t = 0;  // critical time of system
    while (true) {
        bool flag = true;
        for (int i = 0; i < n; i++){
            // these condition for if arrival is not on zero
            // check that if there come before quantum time
            if (p[i].leftAT <= t){
                if (p[i].leftAT <= q){
                    if (p[i].leftBT > 0){
                        flag = false;
                        if (p[i].leftBT > q){
                            stringstream timeT, procPid; //process number and time when it starts execution
                            timeT << t;
                            seq.append(timeT.str());
                            seq.append("\t");
                            procPid << p[i].pid;
                            seq.append(procPid.str()); 
                            seq.append("\n");
                            //remaining time after quantum expiration
                            t = t + q;
                            p[i].leftBT = p[i].leftBT - q;
                            p[i].leftAT = p[i].leftAT + q;
                        } else{
                            // add sequence
                            stringstream timeT, procPid; //process number and time when it starts execution
                            timeT << t;
                            seq.append(timeT.str());
                            seq.append("\t");
                            procPid << p[i].pid;
                            seq.append(procPid.str()); 
                            seq.append("\n");
                            // for last time
                            t = t + p[i].leftBT;
                            // store comp time
                            p[i].doneT = t - p[i].arrT;
                            // store wait time
                            p[i].waitT = t - p[i].burstT - p[i].arrT;
                            p[i].leftBT = 0;
                        }
                    }
                }
                else if (p[i].leftAT > q){
                    // is any have less arrival time
                    // the coming process then execute them
                    for (int j = 0; j < n; j++){
                        // compare
                        if (p[j].leftAT < p[i].leftAT){
                            if (p[j].leftBT > 0){
                                flag = false;
                                if (p[j].leftBT > q){
                                   stringstream timeT, procPid; //process number and time when it starts execution
                                    timeT << t;
                                    seq.append(timeT.str());
                                    seq.append("\t");
                                    procPid << p[i].pid;
                                    seq.append(procPid.str()); 
                                    seq.append("\n");
                                    t = t + q;
                                    p[j].leftBT = p[j].leftBT - q;
                                    p[j].leftAT = p[j].leftAT + q;
                                } else{
                                    stringstream timeT, procPid; //process number and time when it starts execution
                                    timeT << t;
                                    seq.append(timeT.str());
                                    seq.append("\t");
                                    procPid << p[i].pid;
                                    seq.append(procPid.str()); 
                                    seq.append("\n");
                                    t = t + p[j].leftBT;
                                    p[j].doneT = t - p[j].arrT;
                                    p[j].waitT = t - p[j].burstT - p[j].arrT;
                                    p[j].leftBT = 0;
                                }
                            }
                        }
                    }
                    // now the previous porcess according to
                    // ith is process
                    if (p[i].leftBT > 0){
                        flag = false;
                        // Check for greaters
                        if (p[i].leftBT > q){
                           stringstream timeT, procPid; //process number and time when it starts execution
                            timeT << t;
                            seq.append(timeT.str());
                            seq.append("\t");
                            procPid << p[i].pid;
                            seq.append(procPid.str()); 
                            seq.append("\n");
                            t = t + q;
                            p[i].leftBT = p[i].leftBT - q;
                            p[i].leftAT = p[i].leftAT + q;
                        } else{
                            stringstream timeT, procPid; //process number and time when it starts execution
                            timeT << t;
                            seq.append(timeT.str());
                            seq.append("\t");
                            procPid << p[i].pid;
                            seq.append(procPid.str()); 
                            seq.append("\n");
                            t = t + p[i].leftBT;
                            p[i].doneT = t - p[i].arrT;
                            p[i].waitT = t - p[i].burstT - p[i].arrT;
                            p[i].leftBT = 0;
                        }
                    }
                }
            }
            // if no process is come on thse critical
            else if (p[i].leftAT > t){
                t++;
                i--;
            }
        }
        // for exit the while loop
        if (flag){
            break;
        }
    }
    for (int i = 0; i < n; i++){
        res = res + p[i].waitT;
        resc = resc + p[i].doneT;
    }
    if (myfile.is_open()){
        myfile << "RR " << q << endl;
        myfile << seq;
        myfile << "Average waiting time is " << (float)res / n << endl;
        myfile.close();
    } else
        cout << "Unable to open file";
}
void SJF(Process p[], int n){
    ofstream myfile("output.txt"); //output file
    int waitTotal = 0, tatTotal = 0;
    sort(p, p + n, compareBT);  // sorting processes by burst time.
    findWaitingTime(p, n); // function to find waiting time of all processes
    // calculate total waiting time and total turn around time
    if (myfile.is_open()) {
        myfile << "SJF\n";
        int i = 0;
        while (i < n){
           waitTotal = waitTotal + p[i].waitT;
            tatTotal = tatTotal + p[i].tat;
            myfile << p[i].waitT << "\t";
            myfile << p[i].pid << endl;
            i++;
        }
        myfile << "Average waiting time = " << (float)waitTotal / (float)n;
        myfile.close();
    } else
        std::cout << "Unable to open file";
}
                //Priority Scheduling without Preemption
void PR_noPREMP(Process p[], int limit){
    int i, j, sum = 0, location, temp;
    float avgWaitT, avgTaT;
    ofstream myfile("output.txt");
    i = 0;
    while (i < limit){
        location = i;
        j = i+1;
        while (j < limit){
            if (p[j].priority < p[location].priority){
                location = j;
            }
            j++;
        }
        temp = p[i].priority;
        p[i].priority = p[location].priority;
        p[location].priority = temp;
        temp = p[i].burstT;
        p[i].burstT = p[location].burstT;
        p[location].burstT = temp;
        temp = p[i].pid;
        p[i].pid = p[location].pid;
        p[location].pid = temp;
        i++;
    }
    p[0].waitT = 0;
    i = 1;
    while (i < limit){
        p[i].waitT = 0;
        j = 0;
        while (j < i) {
            p[i].waitT = p[i].waitT + p[j].burstT;
            j++;
        }
        sum = sum + p[i].waitT;
        i++;
    }
    avgWaitT = (float)sum / limit;
    sum = 0;
    if (myfile.is_open())
    {
        myfile << "PR_noPREMP\n";
        i = 0;
        while (i < limit){
            p[i].tat = p[i].burstT + p[i].waitT;
            sum = sum + p[i].tat;
            myfile << p[i].waitT << "\t" << p[i].pid << "\n";
            i++;
        }
        avgTaT = sum / limit;
        myfile << "Average Waiting Time: " << avgWaitT << "\n";
        myfile.close();
    }
    else
        std::cout << "Unable to open file";
}
                //Priority Scheduling with Preemption
void PR_withPREMP(Process p[], int n){
    sort(p, p + n, compare);
    int allWaitT = 0, allBurstT = 0;
    int totalturnaroundtime = 0, i, insertedprocess = 0;
    int heapsize = 0, currentTime = p[0].arrT,totalresponseT = 0;
    Process Heap[4 * n];
    // Calculating the total burst time of the processes
    i = 0;
    while ( i < n){
        allBurstT += p[i].burstT;
        p[i].leftBT = p[i].burstT;
        i++;
    }
    // Inserting the processes in Heap according to arrival time
    do{
        if (insertedprocess != n){
            i = 0;
            while ( i < n){
                if (p[i].arrT == currentTime){
                    ++insertedprocess;
                    p[i].inT = -1;
                    p[i].responseT = -1;
                    addToHeap(Heap, p[i], &heapsize, &currentTime);
                }
                i++;
            }
        }
        execHighPriority(Heap, p, n, &heapsize, &currentTime);
        ++currentTime;
        if (heapsize == 0 && insertedprocess == n)
            break;
    } while (1);
    i = 0;
    while (i < n){
        totalresponseT += p[i].responseT;
        allWaitT += (p[i].outT - p[i].inT - p[i].leftBT);
        allBurstT += p[i].burstT;
        i++;
    }
    ofstream myfile("output.txt"); //output file
    if (myfile.is_open()){
        myfile << "PR_withPREMP\n";
        myfile << outfile;
        myfile << "Average waiting time = " << ((float)allWaitT / (float)n) << "\n";
        myfile.close();
    }
    else
        std::cout << "Unable to open file";
}