import matplotlib.pyplot as plt
import csv
import sys
import copy

from common import *

xticks = [64, 256, 1024, 4096, 8192]
xticks_labels = ["64", "256", "1K", "4K", "8K"]

def get_bandwidth(base_file, thread_count, x_vals, y_vals):
    data_file = f'{base_file}_{thread_count}.csv'
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



def plot_bm(data_dir, dram_dir, plot_dir, file, thread_counts):
    plt.figure()
    fig, axes = plt.subplots(1, 2, figsize=(10, 2.5))
    pmem_ax, dram_ax = axes


    for i, dir_ in enumerate((data_dir, dram_dir)):
        ax = axes[i]
        t2d = copy.deepcopy(BASE_THREAD_TO_DATA)
        plot_thread_data(f"{dir_}/{file}", ax, thread_counts, t2d)
        max_ylim = 60 if "read" in file else 40
        ax.set_ylim(0, max_ylim)
        yticks = [x * 10 for x in range(max_ylim // 10 + 1)]
        ax.set_yticks(yticks)
        ax.set_yticklabels([str(y) for y in yticks])

    for ax in axes:
        ax.set_xscale('log')
        ax.minorticks_off()
        ax.grid(axis='y', which='both')
        ax.set_xticks(xticks)
        ax.set_xticklabels(xticks_labels)
        ax.set_xlabel('Access Size [Byte]', fontsize=18)
        for tick in ax.xaxis.get_major_ticks():
            tick.label.set_fontsize(16)
        for tick in ax.yaxis.get_major_ticks():
            tick.label.set_fontsize(18)

    axes[0].set_ylabel('Bandwidth [GB/s]', fontsize=18)
    fig.legend(frameon=False, labels=[str(x) for x in thread_counts],
               bbox_to_anchor=(0.5, 1.2), loc='upper center', fontsize=18,
               columnspacing=0.8, handletextpad=0.3, labelspacing=0.1, ncol=8)

    axes[0].set_title(f"a) PMEM", fontsize=18)
    axes[1].set_title(f"b) DRAM", fontsize=18)

    plot_path = f'{plot_dir}/{file}'
    SAVE_PLOT(plot_path)


if __name__ == '__main__':
    INIT_MATPLOT()

    if (len(sys.argv) != 4):
        raise RuntimeError("Need /path/to/data/dir /path/to/dram/dir /path/to/plot/dir")

    data_dir = sys.argv[1]
    dram_dir = sys.argv[2]
    plot_dir = sys.argv[3]

    read_thread_counts = [1, 4, 8, 16, 18, 24, 32, 36]
    plot_bm(data_dir, dram_dir, plot_dir, "random_read", read_thread_counts)

    write_thread_counts = [1, 2, 4, 6, 8, 18, 24, 36]
    plot_bm(data_dir, dram_dir, plot_dir, "random_write", write_thread_counts)

    PRINT_PLOT_PATHS()
