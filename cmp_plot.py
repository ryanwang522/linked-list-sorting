import matplotlib.pyplot as plt

opt, naive, insertion = [], [], []
with open('output/cmp_output.txt', 'r') as f:
    for line in f.readlines():
        opt_t, n_t, ins_t = line.strip().split(' ')
        opt.append(float(opt_t) * 1e3) # ms
        naive.append(float(n_t) * 1e3) # ms
        insertion.append(float(ins_t) * 1e3)
        # xor.append(float(xor_t) * 1e3)

expr_cnt = list(range(1, len(opt)+1))
plt.plot(expr_cnt, opt, 'b+-', label='optimize merge sort')
plt.plot(expr_cnt, naive, 'r*-', label='naive merge sort')
plt.plot(expr_cnt, insertion, 'yx-', label='insertion sort')
plt.legend(loc='best')
plt.xlabel('trials')
plt.ylabel('time (ms)')
plt.savefig('figure/cmp_sort.png')
plt.show()
