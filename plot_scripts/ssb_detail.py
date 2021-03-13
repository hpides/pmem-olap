import matplotlib.pyplot as plt
import sys

from common import *

def plot_bm(data_dir, plot_dir):
    C_1_128_none_single = 46.999
    C_36_128_none_single = 35.311
    C_36_4096_none_single = 28.941
    C_36_4096_cores_single = 2.622
    C_36_4096_cores_multi = 1.272
    C_DRAM = .682

    fig, ax = plt.subplots(1, 1, figsize=(3, 3))

    bar_width = 0.8
    plt.bar(0, C_1_128_none_single,    bar_width, color=PMEM_COLOR)
    plt.bar(1, C_36_128_none_single,   bar_width, color=PMEM_COLOR)
    plt.bar(2, C_36_4096_none_single,  bar_width, color=PMEM_COLOR)
    plt.bar(3, C_36_4096_cores_single, bar_width, color=PMEM_COLOR)
    plt.bar(4, C_36_4096_cores_multi,  bar_width, color=PMEM_COLOR)
    plt.bar(5, C_DRAM,                 bar_width, color=DRAM_COLOR)

    xticks = [0, 1, 2, 3, 4, 5]
    xtick_labels = [
        ("Unoptimized",   3, 'white'),
        ("+ 36 Threads",  2, 'white'),
        ("+ 4 KB Acc.",   2, 'white'),
        ("+ NUMA",        5, 'black'),
        ("+ Multisocket", 3, 'black'),
        ("DRAM",          3, 'black'),
    ]

    for (tick, (label, height, color)) in zip(xticks, xtick_labels):
        bbox_color = 'white' if tick >= 3 else 'none'
        bbox={'fc': bbox_color, 'edgecolor': 'none', 'pad': 0}
        ax.text(tick + 0.05, height, label, fontsize=14, bbox=bbox,
                color=color, ha='center', rotation=90)

    ax.set_xticks([])
    ax.set_xticklabels([])
    SET_LABEL_SIZE(ax)
    ax.yaxis.grid(True)
    ax.set_axisbelow(True)
    ax.set_ylabel('Time [s]', fontsize=18)

    plot_path = f'{plot_dir}/ssb_detail'
    SAVE_PLOT(plot_path)


if __name__ == '__main__':
    INIT_MATPLOT()

    if (len(sys.argv) != 3):
        raise RuntimeError("Need /path/to/data/dir /path/to/plot/dir")

    data_dir = sys.argv[1]
    plot_dir = sys.argv[2]

    plot_bm(data_dir, plot_dir)

    PRINT_PLOT_PATHS()
