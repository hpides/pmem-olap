import matplotlib.pyplot as plt
import sys
import csv

from common import *


def get_bandwidth(data_dir, mode, data):
    with open(f'{data_dir}/sequential_mixed_{mode}.csv') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        for row in plots:
            bandwidth = float(row[4]) / 1000
            data.append(bandwidth)



def plot_bm(data_dir, plot_dir):
    write_performance = []
    read_performance = []
    get_bandwidth(data_dir, "read", read_performance)
    get_bandwidth(data_dir, "write", write_performance)
    assert len(write_performance) == len(read_performance)

    fig, ax = plt.subplots(1, 1, figsize=(9, 2.5))

    xticks = [
        0, 0.4, 0.8, 1.2,
        1.75, 2.15, 2.55, 2.95,
        3.5, 3.9, 4.3, 4.7
    ]
    xticks_labels = [
        "1/1", "1/8", "1/18", "1/30",
        "4/1", "4/8", "4/18", "4/30",
        "6/1", "6/8", "6/18", "6/30"
    ]

    bar_width = 0.15
    bar_offset = bar_width / 2
    for idx in range(len(write_performance)):
        plt.bar(xticks[idx] - bar_offset, write_performance[idx], bar_width, color=PMEM_COLOR)
        plt.bar(xticks[idx] + bar_offset, read_performance[idx],  bar_width, color=DRAM_COLOR)

    ax.set_xticks(xticks)
    ax.set_xticklabels(xticks_labels)
    ax.yaxis.grid(True)
    ax.set_axisbelow(True)
    ax.set_yticks([0, 5, 10, 15, 20, 25])
    ax.set_ylabel('Bandwidth [GB/s]', fontsize=18)
    ax.set_xlabel("# Write/Read Threads", fontsize=18)
    SET_LABEL_SIZE(ax, xsize=16)

    labels = ["Write", "Read"]
    colors = [PMEM_COLOR, DRAM_COLOR]
    handles = [plt.Rectangle((0,0),1,1, color=color) for color in colors]
    # fig.legend(handles, labels, frameon=False, bbox_to_anchor=(0.5, 1.1),
    #            loc='upper center', fontsize=18, columnspacing=1,
    #            handletextpad=0.3, ncol=3)

    fig.legend(labels, frameon=True, bbox_to_anchor=(0.2, 0.9), loc='upper center',
                edgecolor='white', columnspacing=0.4, handletextpad=0.2, borderpad=0.1,
                labelspacing=0.1, handlelength=1.8, facecolor='white', framealpha=1,
                fontsize=18, ncol=1)

    ax.set_xlim([-0.3,5])

    ax.axvline(x=1.475, ls='--', color='black')
    ax.axvline(x=3.225, ls='--', color='black')

    plot_path = f'{plot_dir}/mixed_performance'
    SAVE_PLOT(plot_path)


if __name__ == '__main__':
    INIT_MATPLOT()

    if (len(sys.argv) != 3):
        raise RuntimeError("Need /path/to/data/dir /path/to/plot/dir")

    data_dir = sys.argv[1]
    plot_dir = sys.argv[2]

    plot_bm(data_dir, plot_dir)

    PRINT_PLOT_PATHS()