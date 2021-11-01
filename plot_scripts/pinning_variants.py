import matplotlib.pyplot as plt
import csv
import sys

from common import *


def get_bandwidth(base_file, pin, x_vals, y_vals):
    with open(f'{base_file}_{pin.lower()}.csv') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        for row in plots:
            threads = int(row[1])
            bandwidth = float(row[4]) / 1000
            x_vals.append(threads)
            y_vals.append(bandwidth)

def plot_bm(data_dir, plot_dir, mode):
    pinnings = {
        'None': ([], []),
        'NUMA': ([], []),
        'Cores': ([], []),
    }

    pin_to_color =  {'None': '#909090', 'NUMA': PMEM_COLOR, 'Cores': DRAM_COLOR}
    pin_to_marker = {'None': 'o', 'NUMA': 'x', 'Cores': 'v'}

    fig, ax = plt.subplots(1, 1, figsize=(4, 3.5))

    for (pin, data) in pinnings.items():
        x_vals, y_vals = pinnings[pin]
        get_bandwidth(f"{data_dir}/sequential_{mode}_pinning", pin, x_vals, y_vals)
        ax.plot(x_vals, y_vals, color=pin_to_color[pin], marker=pin_to_marker[pin],
                markersize=8, label=pin)

    xticks = [1, 4, 8, 16, 18, 24, 32, 36]
    xtick_labels = ["1", "4", "8", "", "18", "24", "", "36"]

    ylim_max = 44 if mode == "read" else 14
    ax.set_xticks(xticks)
    ax.set_xticklabels(xtick_labels)
    ax.yaxis.grid(True)
    ax.set_axisbelow(True)
    ax.set_ylim(0, ylim_max)
    ax.set_ylabel('Bandwidth [GB/s]', fontsize=18)
    ax.set_xlabel('Threads [#]', fontsize=18)
    SET_LABEL_SIZE(ax)

    fig.legend(frameon=False, bbox_to_anchor=(0.43, 1.05), loc='upper center', fontsize=18,
               columnspacing=0.2, handletextpad=0.1, labelspacing=-2.1, ncol=3,
               handlelength=1.6)

    plot_path = f'{plot_dir}/{mode}_pinning_variants'
    SAVE_PLOT(plot_path)


if __name__ == '__main__':
    INIT_MATPLOT()

    if (len(sys.argv) != 3):
        raise RuntimeError("Need /path/to/data/dir /path/to/plot/dir")

    data_dir = sys.argv[1]
    plot_dir = sys.argv[2]

    plot_bm(data_dir, plot_dir, "read")
    plot_bm(data_dir, plot_dir, "write")

    PRINT_PLOT_PATHS()