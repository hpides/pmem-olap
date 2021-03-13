import matplotlib.pyplot as plt
import csv
import sys
import copy

from common import *

xticks = [64, 256, 1024, 4096, 16384, 65536]
xticks_labels = ["64", "256", "1K", "4K", "16K", "64K"]

def get_bandwidth(base_file, thread_count, x_vals, y_vals):
    base_suffix = "_fenced" if "write" in base_file else ""
    data_file = f'{base_file}{base_suffix}_{thread_count}.csv'
    with open(data_file, newline='') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        for row in plots:
            access_size = int(row[2])
            bandwidth = float(row[4]) / 1000

            if access_size <= xticks[-1]:
                x_vals.append(access_size)
                y_vals.append(bandwidth)

def plot_thread_data(file, ax, thread_counts, thread_to_data):
    for tc in thread_counts:
        x_vals, y_vals = thread_to_data[tc]
        get_bandwidth(file, tc, x_vals, y_vals)
        ax.plot(x_vals, y_vals, label=f'{tc}', color=THREAD_TO_COLOR[tc], marker=THREAD_TO_MARKER[tc])

def plot_bm(data_dir, plot_dir, file, types, thread_counts):
    plt.figure()
    fig, axes = plt.subplots(1, 2, figsize=(10, 2.5))

    for i, (t, _) in enumerate(types):
        t2d = copy.deepcopy(BASE_THREAD_TO_DATA)
        plot_thread_data(f"{data_dir}/{file}_{t}", axes[i], thread_counts, t2d)

    is_dram = "dram" in data_dir or "dram" in file
    ylim_max = 0
    if is_dram:
        ylim_max = 110 if "read" in file else 80
    else:
        ylim_max = 42 if "read" in file else 13

    for ax in axes:
        ax.set_ylim(0, ylim_max)
        ax.set_xscale('log')
        ax.minorticks_off()
        ax.grid(axis='y', which='major')
        ax.set_xticks(xticks)
        ax.set_xticklabels(xticks_labels)
        ax.set_xlabel('Access Size [Byte]', fontsize=18)
        SET_LABEL_SIZE(ax, xsize=16)

    axes[0].set_ylabel('Bandwidth [GB/s]', fontsize=18)
    fig.legend(frameon=False, labels=[str(x) for x in thread_counts],
               bbox_to_anchor=(0.5, 1.2), loc='upper center', fontsize=18,
               columnspacing=0.8, handletextpad=0.3, labelspacing=0.1, ncol=8)

    axes[0].set_title(f"a) {types[0][1]}", fontsize=18)
    axes[1].set_title(f"b) {types[1][1]}", fontsize=18)

    plot_file = file if not is_dram else f'dram_{file}'
    plot_path = f'{plot_dir}/{plot_file}'
    SAVE_PLOT(plot_path)


if __name__ == '__main__':
    INIT_MATPLOT()

    if (len(sys.argv) != 3):
        raise RuntimeError("Need /path/to/data/dir /path/to/plot/dir")

    data_dir = sys.argv[1]
    plot_dir = sys.argv[2]
    seq_types = [("log", "Grouped Access"), ("disjoint", "Individual Access")]

    read_thread_counts = [1, 4, 8, 16, 18, 24, 32, 36]
    plot_bm(data_dir, plot_dir, "sequential_read", seq_types, read_thread_counts)

    write_thread_counts = [1, 2, 4, 6, 8, 18, 24, 36]
    plot_bm(data_dir, plot_dir, "sequential_write", seq_types, write_thread_counts)

    PRINT_PLOT_PATHS()
