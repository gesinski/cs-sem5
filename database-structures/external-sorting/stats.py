observed_io = [40, 760, 2240, 4480, 10960, 16440, 21920, 35440]
observed_reads = [20, 380, 1120, 2240, 5480, 8220, 10960, 17720]
observed_writes = [20, 380, 1120, 2240, 5480, 8220, 10960, 17720]
observed_phases = [1, 7, 17, 33, 67, 100, 133, 200]
records_num = [100, 1000, 2500, 5000, 10000, 15000, 20000, 30000]
n = 5
b = 10

def get_theoretical_io(N, b, n):
    import math
    return 2*(N/b)*math.ceil(math.log(N/b, n))

for reocrds_num in observed_io:
    print(get_theoretical_io(reocrds_num, b, n))
