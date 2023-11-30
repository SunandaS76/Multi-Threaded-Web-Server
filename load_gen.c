/* run using: ./load_gen localhost <server port> <number of concurrent users>
   <think time (in s)> <test duration (in s)> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>

#include <pthread.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_READ 1024

int time_up;
FILE *log_file;

// user info struct
struct user_info
{
  // user id
  int id;

  // socket info
  int portno;
  char *hostname;
  float think_time;

  // user metrics
  // float throughput;
  int total_count;
  float total_rtt;
};

// error handling function
void error(char *msg)
{
  perror(msg);
  // exit(0);
}

// time diff in seconds
float time_diff(struct timeval *t2, struct timeval *t1)
{
  return (t2->tv_sec - t1->tv_sec) + (t2->tv_usec - t1->tv_usec) / 1e6;
}

// user thread function
void *user_function(void *arg)
{
  /* get user info */
  struct user_info *info = (struct user_info *)arg;
  // printf("inside thread %d\n", info->id);

  int sockfd, n, portno;
  // char buffer[256];
  struct timeval start, end;

  struct sockaddr_in serv_addr;
  struct hostent *server;

  server = gethostbyname(info->hostname);
  if (server == NULL)
    {
      fprintf(stderr, "ERROR, no such host\n");
      exit(0);
    }


  while (1)
  {
    char buffer[MAX_READ];
    // bzero(buffer, MAX_READ);
    /* start timer */

    /* TODO: create socket */
    portno = info->portno;
    // printf("before fault 3\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
      error("ERROR opening socket");

    /* TODO: set server attrs */
    // server = gethostbyname(argv[1]);
    // printf("printing hostname %s\n", info->hostname);
    if (server == NULL)
    {
      fprintf(stderr, "ERROR, no such host\n");
      exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    gettimeofday(&start, NULL);

    /* TODO: connect to server */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      error("ERROR connecting");

    
    /* TODO: send message to server */
    bzero(buffer, MAX_READ);
    strcpy(buffer, "GET html_files/index.html HTTP/1.0");
    n = send(sockfd, buffer, MAX_READ, MSG_NOSIGNAL);
    if (n < 0){
        error("ERROR writing to socket");
        // close(sockfd);
    }
      

    bzero(buffer, MAX_READ);
 
    /* TODO: read reply from server */
    n = read(sockfd, buffer, MAX_READ);
    if (n < 0){
      error("ERROR reading from socket");
      // close(sockfd);
    }
    if(n>=0){
      info->total_count++;
    }

    /* TODO: close socket */
    close(sockfd);
  
    /* end timer */
    gettimeofday(&end, NULL);

    /* if time up, break */
    if (time_up)
      break;

    /* TODO: update user metrics */
    info->total_rtt += time_diff(&end, &start);

     

    /* TODO: sleep for think time */

    float thinktime = info->think_time;

    
    float n = usleep(thinktime*1000*1000);
  
  
    
  }
 
  // info->total_count++;
  // printf("total number of requets in thread %d are %d\n",info->id, info->total_count);
  // 1 / (time_diff(&end, &start) + info->think_time) ;
  /* exit thread */
  fprintf(log_file, "User #%d finished\n", info->id);
  fflush(log_file);
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int user_count, portno, test_duration;
  float think_time;
  char *hostname;

  if (argc != 6)
  {
    fprintf(stderr,
            "Usage: %s <hostname> <server port> <number of concurrent users> "
            "<think time (in s)> <test duration (in s)>\n",
            argv[0]);
    exit(0);
  }

  hostname = argv[1];
  portno = atoi(argv[2]);
  user_count = atoi(argv[3]);
  think_time = atof(argv[4]);
  test_duration = atoi(argv[5]);

  printf("Hostname: %s\n", hostname);
  printf("Port: %d\n", portno);
  printf("User Count: %d\n", user_count);
  printf("Think Time: %f s\n", think_time);
  printf("Test Duration: %d s\n", test_duration);

  /* open log file */
  log_file = fopen("load_gen.log", "w");

  pthread_t threads[user_count];
  struct user_info info[user_count];
  struct timeval start, end;

  /* start timer */
  gettimeofday(&start, NULL);
  time_up = 0;
  float total_rtt=0;
  int total_number_of_requests=0;
  
  for (int i = 0; i < user_count; ++i)
  {
    /* TODO: initialize user info */
    // printf("before fault 1 \n");
    info[i].hostname = hostname;
    info[i].portno = portno;
    info[i].think_time = think_time;
    info[i].id = i ;
    info[i].total_count = 0;
    info[i].total_rtt = 0;
    // printf("after fault 1 \n");
    // info->total_count = user_count;
    // printf("%d\n", i);
    // info->total_count = total
    /* TODO: create user thread */
    pthread_create(&threads[i], NULL, user_function, &info[i]);
    // printf("total rtt %f", info->total_rtt);
    // total_number_of_requests += info->total_count;

    fprintf(log_file, "Created thread %d\n", i);
  }

  /* TODO: wait for test duration */
  sleep(test_duration);
  fprintf(log_file, "Woke up\n");

  /* end timer */
  time_up = 1;
  
  gettimeofday(&end, NULL);

  /* TODO: wait for all threads to finish */
  intptr_t t;
  for (t = 0; t < user_count; t++)
  {
    // printf("In main: joining thread #%ld\n", (long)t);
    int rc = pthread_join(threads[t], NULL);
  }

  // int total_count = 0;
  // for (t = 0; t < user_count; t++)
  // {
  //   // printf("In main: joining thread #%ld\n", (long)t);
  //  total_count += info[t].total_count ;
  // }

  
  /* TODO: print results */
  for(int i=0; i<user_count; i++){

    total_rtt += info[i].total_rtt;
    // printf("added %d to %d", info[i].total_count, total_number_of_requests);
    total_number_of_requests += info[i].total_count;
  }
  

  // total_rtt = total_rtt / user_count;
  float total_time = time_diff(&end, &start);
  printf("total time %f\n", total_time);
  
  printf("throughput %f\n", (float)(total_number_of_requests / total_time));
  printf("total_rtt %f\n", total_rtt/total_number_of_requests);
  // printf ("time %ld\n", end.tv_sec - start.tv_sec);
  /* close log file */
  fclose(log_file);

  return 0;
}
