/* run using ./server <port> */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <bits/stdc++.h>
using namespace std;
#define NO_THREADS 50
#define queue1_SIZE 1000
#define MAX_READ 1024
int cnt = 0;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
int queue1[queue1_SIZE]; // queue1 declaration
int front = -1;
int rear = -1;

void enqueue1(int element)
{
    if (front == -1 && rear == -1)
    {
        front = 0;
        rear = 0;
        queue1[rear] = element;
    }
    else if ((front == 0 && rear == queue1_SIZE - 1) || (rear == (front - 1) % (queue1_SIZE - 1))) // condition to check queue1 is full
    {
        printf("queue1 is overflow..");
    }
    else
    {
        rear = (rear + 1) % queue1_SIZE;
        queue1[rear] = element;
    }
}

int dequeue1()
{
    if ((front == -1) && (rear == -1))
    {
        return 0;
    }
    else if (front == rear)
    {
        int ret = queue1[front];
        front = -1;
        rear = -1;

        return ret;
    }
    else
    {
        int ret = queue1[front];
        front = (front + 1) % queue1_SIZE;
        return ret;
    }
}

void display()
{
    if (front == -1)
    {
        printf("\nqueue1 is Empty");
        return;
    }
    printf("\nElements in Circular queue1 are: ");
    if (rear >= front)
    {
        for (int i = front; i <= rear; i++)
            printf("%d ", queue1[i]);
    }
    else
    {
        for (int i = front; i < queue1_SIZE; i++)
            printf("%d ", queue1[i]);

        for (int i = 0; i <= rear; i++)
            printf("%d ", queue1[i]);
    }
}
void error(char *msg)
{
    perror(msg);
    return;
}
struct request_struct
{
    string HTTP_version;
    string method;
    string url;
    // char HTTP_version[10];
    // char method[15];
    // char url[256];
};

struct response_struct
{
    string HTTP_version = "";
    string status_code = "";
    string status_text = "";
    string body = "";
    string content_length = "";
    // char HTTP_version[10]; // 1.0 for this assignment
    // char status_code[10];  // ex: 200, 404, etc.
    // char status_text[15];  // ex: OK, Not Found, etc.
    // char body[MAX_READ];
    // char content_length[10];
};

struct request_struct split(char *buffer)
{
    struct request_struct r;
    char delim[] = " ";
    // string delim = " ";
    char *ptr = strtok(buffer, delim);
    int i = 0;
    string base_path;
    // char base_path[256] = {};
    // r.method = {};
    // r.HTTP_version = {}
    base_path = "/home/sunanda/Desktop/Course-Files-IITB/CS_744/ASS_3/";
    // strcat(base_path, "/home/sunanda/Desktop/Course-Files-IITB/CS_744/ASS_3/html_files");
    while (ptr != NULL)
    {
        if (i == 0)
        {
            r.method = ptr;
            // strcpy(r.method, ptr);
            if (strcmp(ptr, "GET"))
            {
                printf("Not a GET Method\n");
            }
        }
        if (i == 1)
        {
            base_path += ptr;
            // strcat(base_path, ptr);
            r.url = base_path;
            // cout<<endl<< "url set" << r.url << endl;
            // strcpy(r.url, base_path);
        }
        if (i == 2)
        {
            r.HTTP_version = ptr;
            // strncat(r.HTTP_version, ptr, 8);
        }
        i++;
        ptr = strtok(NULL, delim);
    }
    return r;
}

string response_string(struct response_struct rs)
{
    string response = "";
    // cout<<"response1 "<<response1<<endl;

    response = "HTTP/1.0 " +
               rs.status_code + " " +
               rs.status_text + "\r\n" +
               "Date: Fri, 31 Dec 1999 23:59:59 GMT" + "\r\n" +
               "Content-Type: text/html" + "\r\n" +
               "Content-Length: " +
               rs.content_length +
               "\r\n\n" +
               rs.body + "\r\n";

    // cout << "Response string inside function" << response << endl;

    // strcat(response, "HTTP/1.0");
    // strcat(response, " ");
    // strcat(response, rs.status_code);
    // strcat(response, " ");
    // strcat(response, rs.status_text);
    // strcat(response, "\r\n");
    // strcat(response, "Date: Fri, 31 Dec 1999 23:59:59 GMT");
    // strcat(response, "\r\n");
    // strcat(response, "Content-Type: text/html");
    // strcat(response, "\r\n");
    // strcat(response, "Content-Length: ");
    // strcat(response, rs.content_length);
    // strcat(response, "\r\n\n");
    // strcat(response, rs.body);
    // strcat(response, "\r\n");
    return response;
}

char *read_file(FILE *f, char *read_buffer)
{
    int len = fread(read_buffer, 1, MAX_READ, f);
    fclose(f);
    // printf("%d", len);
}

int check_html(string url)
{
    // char str[256];
    // str = url.c_str;
    // strcat(str, url);
    int found = 0;
    char search[] = ".html";

    char *ptr = strstr((char *)url.c_str(), search);

    if (ptr != NULL) /* Substring found */
    {
        found = 1;
    }

    return found;
}
struct response_struct process_request(char *buffer)
{
    struct request_struct r;
    struct response_struct rs;
    struct stat stats;
    // string temp_buff(buffer)
    char temp_buff[MAX_READ];
    bzero(temp_buff, MAX_READ);
    char read_buffer[MAX_READ];
    bzero(read_buffer, MAX_READ);
    // temp_buff = buffer;
    strcpy(temp_buff, buffer);

    r = split(temp_buff);
    // rs.HTTP_version = r.HTTP_version
    // strncat(rs.HTTP_version, r.HTTP_version, 8);
    int present = check_html(r.url);

    if (present == 0)
    {
        r.url = "/index.html";
        // strcat(r.url, "/index.html");
    }
    FILE *fptr = fopen((char *)r.url.c_str(), "r");

    if (fptr != NULL)
    {
        int p = stat((char *)r.url.c_str(), &stats);
        int file_size = stats.st_size;
        char content_length[5];
        sprintf(content_length, "%d", file_size);
        // cout << "content length " << content_length << endl;
        rs.content_length = content_length;
        read_file(fptr, read_buffer);

        if (read_buffer)
        {
            rs.status_code = "200";
            rs.status_text = "OK";
            // strcpy(rs.status_code, "200");
            // strcpy(rs.status_text, "OK");
        }
        rs.body = read_buffer;
        // strcpy(rs.body, read_buffer);
    }
    else
    {
        rs.status_code = "404";
        rs.status_text = "Not Found";
        rs.body = "<html>404 NOT FOUND</html>";
        rs.content_length = "24";
        // strcpy(rs.status_code, "404");
        // strcpy(rs.status_text, "Not Found");
        // strcpy(rs.body, "<html>404 NOT FOUND</html>");
        // strcpy(rs.content_length, "24");
    }
    // fclose(fptr);
    return rs;
}

void handle_socket(int args)
{
    // string buffer ;
    char buffer[MAX_READ];
    bzero(buffer, MAX_READ);
    struct response_struct rs;
    int n = read(args, buffer, MAX_READ);
    if (n <= 0)
    {
        strcat(buffer, "");
        error("ERROR reading from socket");
        close(args);
        return;
    }

    rs = process_request(buffer);

    // cout << "Before response_string function" << endl;
    string response1 = "";
    response1 = response_string(rs);
    // string responsee = "Hello";
    // cout << "Printing response string: " << response1 << endl;
    n = write(args, (char *)response1.c_str(), MAX_READ);
    if (n < 0)
    {
        error("ERROR writing to socket");
        close(args);

        return;
    }
    close(args);
}

void *handle_thread(void *)
{
    while (1)
    {

        pthread_mutex_lock(&lock1);
        int args = (int)dequeue1();
        // printf("dequening\n");
        if (front == 1 || front == rear+2)
        {
            pthread_cond_signal(&cond1);
            // printf("removed from queue\n");
        }

        if (args == 0)
        {
            pthread_cond_wait(&cond, &lock1);
            
        }

        pthread_mutex_unlock(&lock1);
        if (args)
        {
            // printf("thread is up\n");
            handle_socket(args);
            // printf("%ld completed this request from request %d\n", pthread_self(), cnt);
        }
    }
}

int main(int argc, char *argv[])
{
    pthread_t worker_thread_pool[NO_THREADS];

    for (int i = 0; i < NO_THREADS; i++)
    {
        pthread_create(&worker_thread_pool[i], NULL, handle_thread, NULL);
    }

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    // string buffer;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    /* create socket */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* fill in port number to listen on. IP address can be anything (INADDR_ANY) */

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* bind socket to this port number on this machine */

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    /* listen for incoming connection requests */
    printf("server started\n");
    listen(sockfd, 10000);
    clilen = sizeof(cli_addr);
    /* accept a new request, create a newsockfd */
    /* read message from client */
    
    while (1)
    {
        int thread_arg = 0;
        pthread_t thread_id;
        // printf("waiting for request number %d\n", cnt);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        // printf("Request from fd %d\n", newsockfd);
        if (newsockfd < 0)
            error("ERROR on accept");

        // printf("Request from fd * %d\n", newsockfd);
        // printf("request %d waiting for lock\n", cnt);
        pthread_mutex_lock(&lock1);
        // printf("request %d got the lock\n", cnt);
        while ((front == 0 && rear == queue1_SIZE - 1) || (rear == (front - 1) % (queue1_SIZE - 1)))
        {
            printf("queue1Full with front %d and rear %d\n", front, rear);
            // printf("server went to sleep\n");
            pthread_cond_wait(&cond1, &lock1);
            printf("Server is awake and listeing\n");
        }
        
        enqueue1(newsockfd);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock1);
        //         intptr_t t;
        //     for( t=0; t<NO_THREADS; t++){
        //       printf("In main: joining thread #%ld\n", (long) t);
        //       int rc = pthread_join(worker_thread_pool[t], NULL);
        //       if (rc) { fprintf(stderr, "failed to join thread #%ld - %s\n",
        //                                 (long)t, strerror(rc));
        //                exit(EXIT_FAILURE);
        //       }
        //    }
        cnt++;
        printf("request count %d\n", cnt);
    }

    return 0;
}

//     GET html_files/index.html HTTP/1.0

// curl http://127.0.0.1:8080/apart2 > response1.html
