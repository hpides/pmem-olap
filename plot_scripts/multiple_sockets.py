import matplotlib.pyplot as plt
import csv
import sys
from collections import OrderedDict
from matplotlib.legend import Legend

from common import *

VAR_TO_STYLE =  {
    '2_far':        (PMEM_COLOR, 'o'),
    '2_near':       (DRAM_COLOR, 'x'),
    '1_near_1_far': ('#909090', 'v'),
    '1_far':        ('#009900', '^'),
    '1_near':       ('#CC6600', 'D'),
}


def get_bandwidth(base_file, var, x_vals, y_vals):
    halve = var.startswith('2') or var.count('1') == 2
    halve_val = 2 if halve else 1
    with open(f'{base_file}.csv') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        for row in plots:
            threads = int(row[1]) / halve_val
            bandwidth = float(row[4]) / 1000
            x_vals.append(threads)
            y_vals.append(bandwidth)

def individual_plot(data_dir, plot_dir, mode):
    variants = OrderedDict()
    variants['1_near'] = ([], [])
    variants['2_near'] = ([], [])
    variants['1_far'] = ([], [])
    variants['2_far'] = ([], [])
    variants['1_near_1_far'] = ([], [])

    labels = []
    lines = []
    fig, ax = plt.subplots(1, 1, figsize=(4, 3))

    for (var, data) in variants.items():
        x_vals, y_vals = data
        get_bandwidth(f"{data_dir}/sequential_{mode}_{var}", var, x_vals, y_vals)
        print(f"{mode}: {x_vals}")
        color, marker = VAR_TO_STYLE[var]
        label = var.replace('_', ' ').replace(' sockets', '').replace(' socket', '').title()
        labels.append(label)
        lines += ax.plot(x_vals, y_vals, color=color, marker=marker, markersize=8, label=label)

    xticks = [1, 4, 8, 16, 18, 24, 32, 36]
    xtick_labels = ['1', '4', '8', '', '18', '24', '32', '36']

    ylim_max = 85 if mode == "read" else 27
    if "dram" in data_dir:
        ylim_max = 200 if mode == "read" else 140

    ax.set_xticks(xticks)
    ax.set_xticklabels(xtick_labels)
    ax.yaxis.grid(True)
    ax.set_axisbelow(True)
    ax.set_ylim(0, ylim_max)
    ax.set_ylabel('Bandwidth [GB/s]', fontsize=18)
    ax.set_xlabel('Threads per Socket [#]', fontsize=18)
    SET_LABEL_SIZE(ax)

    ax.legend(labels[:-1], frameon=False, bbox_to_anchor=(0.4, 1.35), loc='upper center', fontsize=18,
              columnspacing=0.4, handletextpad=0.3, labelspacing=0.0, ncol=3, handlelength=1.6)

    legend_1n1f = Legend(ax, lines[-1:], labels[-1:],
                        frameon=False, bbox_to_anchor=(0.58, 1.25),
                        loc='upper center', fontsize=18, handlelength=1.6,
                        handletextpad=0.3, labelspacing=0.5, ncol=1)
    ax.add_artist(legend_1n1f)

    plot_suffix = "_dram" if "dram" in data_dir else ""
    plot_path = f'{plot_dir}/{mode}_multiple_sockets{plot_suffix}'
    SAVE_PLOT(plot_path)

def combined_plot(data_dir, dram_dir, plot_dir, mode):
    fig, axes = plt.subplots(1, 2, figsize=(10, 3))
    fig.subplots_adjust(hspace=10)

    dram_max = 0

    for i, dir_ in enumerate([data_dir, dram_dir]):
        ax = axes[i]

        variants = OrderedDict()
        variants['1_near'] = ([], [])
        variants['2_near'] = ([], [])
        variants['1_far'] = ([], [])
        variants['2_far'] = ([], [])
        variants['1_near_1_far'] = ([], [])

        labels = []
        lines = []

        for var, data in variants.items():
            x_vals, y_vals = data
            get_bandwidth(f"{dir_}/sequential_{mode}_{var}", var, x_vals, y_vals)
            color, marker = VAR_TO_STYLE[var]
            label = var.replace('_', ' ').replace(' sockets', '').replace(' socket', '').title()
            labels.append(label)
            lines += ax.plot(x_vals, y_vals, color=color, marker=marker, markersize=8, label=label)
            dram_max = max(dram_max, max(y_vals))

        xticks = [1, 4, 8, 16, 18, 24, 32, 36]
        xtick_labels = ['1', '4', '8', '', '18', '24', '', '36']

        ylim_max = 110 if mode == "read" else 140

        ax.set_xticks(xticks)
        ax.set_xticklabels(xtick_labels)
        ax.yaxis.grid(True, which='both')
        ax.set_axisbelow(True)
        ax.set_yticks([0, 25, 50 ,75, 100], minor=True)
        ax.tick_params(axis='y', which='minor')
        ax.set_ylim(0, ylim_max)
        ax.set_xlabel('Threads per Socket [#]', fontsize=18)
        SET_LABEL_SIZE(ax)

    axes[0].set_title("a) PMEM", fontsize=18)
    axes[1].set_title("b) DRAM", fontsize=18)
    axes[0].set_ylabel('Bandwidth [GB/s]', fontsize=18)
    fig.legend(labels, frameon=False, bbox_to_anchor=(0.475, 1.15), loc='upper center', fontsize=18,
                   columnspacing=0.4, handletextpad=0.3, labelspacing=0.0, ncol=5, handlelength=1.6)

    axes[1].text(5, 101, f"max={int(dram_max)}", fontsize=16, color=VAR_TO_STYLE['2_near'][0])

    plot_path = f'{plot_dir}/{mode}_multiple_sockets_combined'
    SAVE_PLOT(plot_path)


def plot_bm(data_dir, dram_dir, plot_dir, mode, combine=False):
    if not combine:
        individual_plot(data_dir, plot_dir, mode)
        individual_plot(dram_dir, plot_dir, mode)
    else:
        combined_plot(data_dir, dram_dir, plot_dir, mode)


if __name__ == '__main__':
    INIT_MATPLOT()

    if (len(sys.argv) != 4):
        raise RuntimeError("Need /path/to/data/dir /path/to/dram/dir /path/to/plot/dir")

    data_dir = sys.argv[1]
    dram_dir = sys.argv[2]
    plot_dir = sys.argv[3]

    plot_bm(data_dir, dram_dir, plot_dir, "read", combine=True)
    plot_bm(data_dir, dram_dir, plot_dir, "write", combine=False)

    PRINT_PLOT_PATHS()