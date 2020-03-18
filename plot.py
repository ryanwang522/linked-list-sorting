import matplotlib.pyplot as plt

opt_exec_times, naive_exec_times = [], []
with open('output/opt_output.txt', 'r') as f:
    for line in f.readlines():
        opt_t, naive_t = line.strip().split(' ')
        opt_exec_times.append(float(opt_t) * 1e3) # ms
        naive_exec_times.append(float(naive_t) * 1e3) # ms

split_sizes = list(range(1, len(opt_exec_times)+1))
plt.plot(split_sizes, opt_exec_times, 'b+-', markersize=7, label='optimized')
plt.plot(split_sizes, naive_exec_times, 'r*-', markersize=7, linestyle='dashed', linewidth=1, label='naive')
plt.legend(loc='best')
plt.xlabel('split size')
plt.ylabel('time (ms)')
plt.savefig('figure/split_size_trial.png')
plt.show()