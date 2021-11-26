import matplotlib.colors as colors
import matplotlib.pyplot as plt
import csv
import math
import sys

from common import *

def get_bandwidth(base_file, type, data):
    with open(f'{base_file}_{type}_fenced.csv') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        for row in plots:
            threads = int(row[1])
            access_size = int(row[2])
            bandwidth = float(row[4]) / 1000

            if (access_size <= 65536):
                data[threads - 1][int(math.log2(access_size)) - 6] = bandwidth


def plot_bm(data_dir, plot_dir, types):
    fig, axes = plt.subplots(1, 2, figsize=(10, 3))

    cmap = colors.ListedColormap(['#000033', 'midnightblue', 'darkslateblue', 'fuchsia', 'orange', 'yellow'])

    xticks = [0, 2, 4, 6, 8, 10]
    xtick_labels = ["64", "256", "1K", "4K", "16K", "64K"]

    for i, (t, _) in enumerate(types):
        ax = axes[i]
        data = [[0 for x in range(11)] for y in range(36)]
        get_bandwidth(f'{data_dir}/sequential_write', t, data)
        im = ax.imshow(data, cmap=cmap, interpolation='bicubic', aspect='auto',
                       vmin=0, vmax=13)
        ax.invert_yaxis()
        ax.set_xlabel('Access Size [Byte]', fontsize=18)
        ax.set_xticks(xticks)
        ax.set_xticklabels(xtick_labels)
        SET_LABEL_SIZE(ax)

    axes[0].set_ylabel('Threads [\#]', fontsize=18)

    axes[0].set_title(f"a) {types[0][1]}", fontsize=18)
    axes[1].set_title(f"b) {types[1][1]}", fontsize=18)

    # start at x=0.25, y=1, length=0.5, height=0.05
    cb_ax = fig.add_axes([0.25, 1.1, 0.5, 0.05])
    cbar = fig.colorbar(im, cax=cb_ax, orientation='horizontal')
    cbar.ax.tick_params(labelsize=18)
    fig.text(0.5, 1.2, "Bandwidth [GB/s]", ha='center', fontsize=18)

    plot_path = f'{plot_dir}/write_heatmap'
    SAVE_PLOT(plot_path)


if __name__ == '__main__':
    INIT_MATPLOT()

    if (len(sys.argv) != 3):
        raise RuntimeError("Need /path/to/data/dir /path/to/plot/dir")

    data_dir = sys.argv[1]
    plot_dir = sys.argv[2]
    types = [("log", "Grouped Access"), ("disjoint", "Individual Access")]
    plot_bm(data_dir, plot_dir, types)

    PRINT_PLOT_PATHS()
