import math
import matplotlib.pyplot as plt

disk_reads_insert_b4_a7 = [2, 2, 2, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3,
                           5, 5, 3, 3, 3, 3, 5, 5, 3, 3, 6, 8, 5, 7, 8, 
                           8, 9, 3, 3, 8, 3, 8, 5, 3, 8, 9, 9, 9, 12, 12,
                           9, 6, 5, 3, 5, 2, 11, 3, 11, 14, 3, 12, 8, 15,
                           6, 12, 5, 6, 7, 2, 3, 14, 3, 15, 5, 5, 3, 14,
                           9, 19, 18, 14, 15, 11, 14, 6, 14, 15, 9, 12, 23,
                           14, 15, 22, 6, 24, 20, 8, 19, 6, 24, 26, 3]
disk_reads_reorganization_b4_a7 = [2, 4, 6, 7, 9, 10, 12, 14, 16, 17, 19, 21, 24, 27, 29, 30]
disk_writes_reorganization_b4_a7 = [4, 7, 8, 10, 12, 14, 17, 19, 20, 23, 25, 29, 33, 35, 37, 42]
disk_reads_delete_b4_a7 = 5
disk_reads_update_b4_a7 = 7
disk_reads_fetch_b4_a7 = 4

disk_reads_insert_b4_a5 = [2, 2, 2, 3, 2, 3, 3, 3, 2, 3, 4, 3, 3, 6, 4, 2, 5, 6, 3, 
                           3, 3, 5, 6, 4, 3, 6, 8, 9, 9, 8, 9,12, 4, 12, 3, 5,10, 2, 12, 
                           8, 3, 11, 12, 11, 8, 12,17, 12, 8, 6, 3, 6, 3, 14, 3, 17, 18, 
                           2, 17, 11, 23, 6, 18, 8, 5, 11, 3, 3, 22, 2, 21, 8, 9, 3, 
                           23, 13, 26, 26, 17, 20, 14, 17, 9, 20, 23, 15, 17, 32, 21, 20, 
                           32, 9, 34, 31, 15, 29, 5, 33, 35, 3]

disk_reads_reorganization_b4_a5 = [2, 5, 8, 11, 14, 17, 19, 24, 27, 30, 35, 38, 43, 46]
disk_writes_reorganization_b4_a5 = [5, 9, 13, 17, 20, 23, 29, 33, 37, 43, 47, 53, 57, 63]
disk_reads_delete_b4_a5 = 51
disk_reads_update_b4_a5 = 10
disk_reads_fetch_b4_a5 = 3

disk_reads_insert_b10_a7 = [2, 2, 2, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
                            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 2, 3, 3, 3, 3, 3, 3, 
                            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
                            3, 2, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 2, 2, 3, 
                            3, 3, 3, 3, 2, 3, 3, 3, 5, 3, 3, 5, 3, 6, 6, 3, 6, 2, 6, 5,
                            3]
disk_reads_reorganization_b10_a7 = [2, 3, 5, 7, 8, 9, 11, 13, 15]
disk_writes_reorganization_b10_a7 = [3, 5, 7, 8, 9, 11, 14, 16, 17]
disk_reads_delete_b4_a5 = 2
disk_reads_update_b4_a5 = 2
disk_reads_fetch_b4_a5 = 4

disk_reads_insert_b10_a5 = [2, 2, 2, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                            3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                            3, 2, 3, 3, 3, 3, 3, 3, 2, 3, 2, 2, 3, 3, 3, 3, 2, 3, 3, 2,
                            3, 3, 5, 3, 3, 2, 3, 2, 6, 3, 5, 3, 2, 3, 5, 3, 6, 6, 5, 6,
                            3, 5, 3, 5, 5, 3, 3, 5, 5, 5, 6, 3, 6, 6, 3, 5, 3, 6, 6, 3]
disk_reads_reorganization_b10_a5 = [2, 4, 6, 8, 11, 14, 16, 18]
disk_writes_reorganization_b10_a5 = [4, 6, 8, 11, 15, 17, 19, 22]
disk_reads_delete_b4_a7 = 2
disk_reads_update_b4_a7 = 6
disk_reads_fetch_b4_a7 = 2



def avg(lst):
    return sum(lst) / len(lst)

def plot_lists(data_dict, title, ylabel):
    plt.figure(figsize=(10, 6))

    for label, values in data_dict.items():
        x = list(range(1, len(values) + 1))
        plt.plot(x, values, marker='', label=f"{label}")
        plt.axhline(
            avg(values),
            linestyle='--',
            alpha=0.6,
            label=f"{label} avg = {avg(values):.2f}"
        )

    plt.xlabel("Operation number")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend()
    plt.grid(True)
    plt.show()

insert_reads = {
    "b = 4, alpha = 0.7": disk_reads_insert_b4_a7,
    "b = 4, alpha = 0.5": disk_reads_insert_b4_a5,
    "b = 10, alpha = 0.7": disk_reads_insert_b10_a7,
    "b = 10, alpha = 0.5": disk_reads_insert_b10_a5,
}

plot_lists(
    insert_reads,
    title="Disk reads during INSERT operations",
    ylabel="Disk reads"
)

for k, v in insert_reads.items():
    print(f"INSERT {k} avg disk_reads = {avg(v):.2f}")

reorg_reads = {
    "b = 4, alpha = 0.7": disk_reads_reorganization_b4_a7,
    "b = 4, alpha = 0.5": disk_reads_reorganization_b4_a5,
    "b = 10, alpha = 0.7": disk_reads_reorganization_b10_a7,
    "b = 10, alpha = 0.5": disk_reads_reorganization_b10_a5,
}

plot_lists(
    reorg_reads,
    title="Disk reads during REORGANIZATION",
    ylabel="Disk reads"
)

for k, v in reorg_reads.items():
    print(f"REORG {k} avg disk_reads = {avg(v):.2f}")

reorg_writes = {
    "b = 4, alpha = 0.7": disk_writes_reorganization_b4_a7,
    "b = 4, alpha = 0.5": disk_writes_reorganization_b4_a5,
    "b = 10, alpha = 0.7": disk_writes_reorganization_b10_a7,
    "b = 10, alpha = 0.5": disk_writes_reorganization_b10_a5,
}

plot_lists(
    reorg_writes,
    title="Disk writes during REORGANIZATION",
    ylabel="Disk writes"
)

for k, v in reorg_writes.items():
    print(f"REORG {k} avg disk_writes = {avg(v):.2f}")