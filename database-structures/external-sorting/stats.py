import math
import matplotlib.pyplot as plt

observed_io = [40, 760, 4480, 10960, 21920, 40920, 59920, 139840]
observed_reads = [20, 380, 2240, 5480, 10960, 20460, 29960, 69920]
observed_writes = [20, 380, 2240, 5480, 10960, 20460, 29960, 69920]
observed_phases = [1, 7, 33, 67, 133, 233, 333, 667]
records_num = [100, 1000, 5000, 10000, 20000, 35000, 50000, 100000]
n = 5
b = 10

observed_io2 = [20, 400, 2000, 4000, 8000, 18900, 29800, 59600]
observed_reads2 = [10, 200, 1000, 2000, 4000, 9450, 14900, 29800]
observed_writes2 = [10, 200, 1000, 2000, 4000, 9450, 14900, 29800]
observed_phases2 = [0, 1, 1, 1, 1, 2, 3, 5]
n2 = 50

def get_theoretical_io(N, b, n):
    return 2 * (N / b) * math.ceil(math.log(N / b, n))

def get_theoretical_phases(N, b, n):
    phases = 0
    runs = math.ceil(N / (n * b))
    while runs > 1:
        runs /= (n - 1)
        phases += runs
    return phases

def create_graph(x, y_obs, y_theory, xlabel, ylabel, title, label_obs, label_theory):
    plt.figure(figsize=(8,5))
    plt.scatter(x, y_obs, color='red', marker='.', label=label_obs, s=60)
    plt.plot(x, y_theory, marker='', label=label_theory)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend()
    plt.grid(True)
    plt.show()

theoretical_io = [get_theoretical_io(N, b, n) for N in records_num]
theoretical_phases = [get_theoretical_phases(N, b, n) for N in records_num]

create_graph(
    records_num,
    observed_io,
    theoretical_io,
    xlabel="Records (N)",
    ylabel="I/O Operations",
    title="Observed vs Theoretical I/O",
    label_obs="Observed I/O",
    label_theory="Theoretical I/O"
)

create_graph(
    records_num,
    observed_phases,
    theoretical_phases,
    xlabel="Records (N)",
    ylabel="Phases",
    title="Observed vs Theoretical Phases",
    label_obs="Observed Phases",
    label_theory="Theoretical Phases"
)

theoretical_io2 = [get_theoretical_io(N, b, n2) for N in records_num]
theoretical_phases2 = [get_theoretical_phases(N, b, n2) for N in records_num]

create_graph(
    records_num,
    observed_io2,
    theoretical_io2,
    xlabel="Records (N)",
    ylabel="I/O Operations",
    title="Observed vs Theoretical I/O",
    label_obs="Observed I/O",
    label_theory="Theoretical I/O"
)

create_graph(
    records_num,
    observed_phases2,
    theoretical_phases2,
    xlabel="Records (N)",
    ylabel="Phases",
    title="Observed vs Theoretical Phases",
    label_obs="Observed Phases",
    label_theory="Theoretical Phases"
)