import matplotlib.pyplot as plt
import csv
import sys

from common import *

def get_bandwidth(base_file, x_vals, y_vals):
    with open(f'{base_file}.csv') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        for row in plots:
            threads = int(row[1])
            bandwidth = float(row[4]) / 1000
            x_vals.append(threads)
            y_vals.append(bandwidth)

xticks = [1,    4,    8,   16,  18,     24,    32, 36]
xticks_labels = ["1", "4", "8",  "", "18",    "24",      "", "36"]

def plot_bm(data_dir, plot_dir):
    runs = {
        'far': ([], []),
        'far2': ([], []),
        'near': ([], []),
    }

    run_to_color =  {'far': '#909090', 'far2': PMEM_COLOR, 'near': DRAM_COLOR}
    run_to_marker = {'far': 'o', 'far2': 'x', 'near': 'v'}

    fig, ax = plt.subplots(1, 1, figsize=(4, 3.5))

    for (run, data) in runs.items():
        x_vals, y_vals = runs[run]
        if "2" in run:
            f = f"{data_dir}/sequential_read_far_second"
        else:
            f = f"{data_dir}/sequential_read_{run}"

        get_bandwidth(f, x_vals, y_vals)
        label = run.title() if "2" not in run else "2nd Far"
        ax.plot(x_vals, y_vals, color=run_to_color[run], marker=run_to_marker[run],
                markersize=8, label=label)

    xticks = [1, 4, 8, 16, 18, 24, 32, 36]
    xtick_labels = ["1", "4", "8", "", "18", "24", "", "36"]

    ax.set_xticks(xticks)
    ax.set_xticklabels(xtick_labels)
    ax.yaxis.grid(True)
    ax.set_axisbelow(True)
    # ax.set_ylim(0, 4)
    ax.set_ylabel('Bandwidth [GB/s]', fontsize=18)
    ax.set_xlabel('Threads [#]', fontsize=18)
    SET_LABEL_SIZE(ax)

    fig.legend(frameon=False, bbox_to_anchor=(0.43, 1.05), loc='upper center', fontsize=18,
               columnspacing=0.2, handletextpad=0.1, labelspacing=-2.1, ncol=3)

    plot_path = f'{plot_dir}/warm_up_behavior'
    SAVE_PLOT(plot_path)


if __name__ == '__main__':
    INIT_MATPLOT()

    if (len(sys.argv) != 3):
        raise RuntimeError("Need /path/to/data/dir /path/to/plot/dir")

    data_dir = sys.argv[1]
    plot_dir = sys.argv[2]
    plot_bm(data_dir, plot_dir)

    PRINT_PLOT_PATHS()