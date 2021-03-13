import matplotlib.pyplot as plt
import csv
import sys

from common import *

def plot_bm(data_dir, plot_dir):

    results = [296675, 302184, 304108, 295551, 278463, 265900]

    fig, ax = plt.subplots(1, 1, figsize=(3, 3))

    xticks = [-0.4, -0.2, 0, 0.2, 0.4, 0.6]
    bar_width = 0.15
    plt.bar(xticks[0], results[0] / 1000 , bar_width, color=PMEM_COLOR)
    plt.bar(xticks[1], results[1] / 1000 , bar_width, color=PMEM_COLOR)
    plt.bar(xticks[2], results[2] / 1000 , bar_width, color=PMEM_COLOR)
    plt.bar(xticks[3], results[3] / 1000 , bar_width, color=PMEM_COLOR)
    plt.bar(xticks[4], results[4] / 1000 , bar_width, color=PMEM_COLOR)
    plt.bar(xticks[5], results[5] / 1000 , bar_width, color=PMEM_COLOR)

    xtick_labels = [1, 2, 4, 12, 18, 36]
    ax.set_xticks(xticks)
    ax.set_xticklabels(xtick_labels)
    SET_LABEL_SIZE(ax)
    ax.yaxis.grid(True)
    ax.set_axisbelow(True)
    ax.set_ylabel('Time [s]', fontsize=18)
    ax.set_xlabel('Worker [\#]', fontsize=18)

    plot_path = f'{plot_dir}/ssb_worker'
    SAVE_PLOT(plot_path)


if __name__ == '__main__':
    INIT_MATPLOT()

    if (len(sys.argv) != 3):
        raise RuntimeError("Need /path/to/data/dir /path/to/plot/dir")

    data_dir = sys.argv[1]
    plot_dir = sys.argv[2]

    plot_bm(data_dir, plot_dir)

    PRINT_PLOT_PATHS()
