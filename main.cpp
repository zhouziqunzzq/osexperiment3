/*************************************************************************
    > File Name: main.cpp
    > Author: Bittersweet
    > ################### 
    > Mail: zhouziqun@cool2645.com
    > Created Time: 2018年05月02日 星期三 13时26分01秒
 ************************************************************************/
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int *wfd, *rfd;
int msgLen = 0;
const int MAXLEN = 256;

void parentProcess(int childNum)
{
    int status, pid;
    char msg[MAXLEN];
    for (int i = 0; i < childNum; ++i)
    {
        pid = wait(&status);
        read(*rfd, msg, msgLen);
        cout << "Message from child process " << pid << ": " << msg << endl;
    }
}

void childProcess(int i)
{
    // pipe mutex lock
    lockf(*wfd, 1, 0);
    string msg = "Child process ";
    msg += char(i + '0');
    msg += " is sending message!";
    write(*wfd, msg.c_str(), msg.length());
    sleep(0.01);
    // pipe mutex unlock
    lockf(*wfd, 0, 0);
    exit(0);
}

void forkProcess(int now, int maxNum)
{
    if (now > maxNum)
        return;

    pid_t fpid;
    fpid = fork();
    if (fpid < 0)
    {
        cout << "Failed to fork, exiting..." << endl;
        exit(-1);
    }
    else if (fpid == 0)
    {
        // Start child process
        childProcess(now);
    }
    else
    {
        // Continue to fork
        forkProcess(now + 1, maxNum);
    }
}

int main()
{
    // Count msg length
    string ts = "Child process 1 is sending message!";
    msgLen = ts.length();
    // Create pipe file descriptor
    int fd[2];
    wfd = &fd[1], rfd = &fd[0];
    // Try to create a pipe
    int r = pipe(fd);
    if (r == -1)
    {
        cout << "Failed to create pipe, exiting..." << endl;
        exit(-1);
    }

    // Fork child process
    cout << "Input total number of child process: ";
    int n;
    cin >> n;
    forkProcess(1, n);
    // Start parent process
    parentProcess(n);
    return 0;
}
