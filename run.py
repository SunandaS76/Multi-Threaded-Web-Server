import os
compile_load_gen = 'gcc -o load_gen load_gen.c'
compile_server = "g++ -g server.cpp"
run_server = "taskset -a -c 4 ./a.out 8081"
os.system("rm final_output.txt")

print("Running server")
max_users = 450
for i in range(0, max_users, 50):
    print("Running test for load ", str(i) + ": users 0.01s: think time 60s: test durations")
    run_loag_gen = "taskset -a -c 0,1,2,3,5 ./load_gen localhost 8081 " + str(i) + " 0.01 60 > output.txt"
    os.system(run_loag_gen)
    os.system("cat output.txt | grep throughput >> final_output.txt")
    os.system("cat output.txt | grep total_rtt >> final_output.txt")
    # os.system("cat final_output.txt")




import matplotlib.pyplot as plt


print("Running load generator")

file1 = open('final_output.txt', 'r')
Lines = file1.readlines()
i=0
throughput = []
rtt = []
users = []

    
for line in Lines : 
    if i % 2 == 0 :
        temp = line.split()
        throughput.append(float(temp[1]))
    else :
        temp = line.split()
        # print("rtt ",float(temp[1]))
        rtt.append(float(temp[1]))
    i += 1

# print(throughput)
# print(rtt)

for j in range(0, (int((i-1)/2)+1 )*50, 50):
    users.append(j)
    
# print(users)
# fig, ax = plt.subplots(nrows=1, ncols=2, figsize=(10,4))

fig, axs = plt.subplots(2, figsize=(12,12))
fig.suptitle('Load test')
axs[0].plot(users, throughput)
axs[1].plot(users, rtt)

axs[0].set_ylabel("Throughput")
axs[0].set_xlabel("Number of concurrent users")

axs[1].set_ylabel("Avg RTT")
axs[1].set_xlabel("Number of concurrent users")

plt.savefig("output_graphs.png")

print("done.. check your plots")
# os.system(compile_load_gen)
