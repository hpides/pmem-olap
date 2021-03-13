import matplotlib.pyplot as plt
import csv
import sys

from common import *

DISTANCE = 1.5
NUM_QUERIES = 13

X_LABELS = [
    "Q1.1", "Q1.2", "Q1.3",
    "Q2.1", "Q2.2", "Q2.3",
    "Q3.1", "Q3.2", "Q3.3", "Q3.4",
    "Q4.1", "Q4.2", "Q4.3",
    "AVG"
]

X_LABELS = [
    "", "QF1", "",
    "", "QF2", "",
    "", "QF3", "", "",
    "", "QF4", "",
    "AVG"
]


def get_duration(data_dir, mode, data):
    with open(f'{data_dir}/ssb_{mode}.csv') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        avg = 0
        for row in plots:
            # if "qf3.1" in row or "qf4.1" in row:
            #     print("SKIP")
            #     continue
            duration = float(row[1]) / 1000
            data.append(duration)
            avg += duration
        data.append(avg / NUM_QUERIES)
        # print(f'{data_dir}/ssb_{mode}.csv AVG: {avg / NUM_QUERIES}')

def plot_bm(data_dir, db_data_dir, plot_dir):
    fig, axes = plt.subplots(1, 2, figsize=(20, 2))
    # fig.subplots_adjust(hspace=0.5)
    xticks = [idx * DISTANCE for idx in range(len(X_LABELS))]

    for i, version in enumerate([db_data_dir, data_dir]):
        MODE_TO_DATA = {
            'dram': [],
            'pmem': []
        }

        ax = axes[i]
        for mode, data in MODE_TO_DATA.items():
            get_duration(version, mode, data)

        pmem_optimized = MODE_TO_DATA['pmem']
        dram = MODE_TO_DATA['dram']
        plt.rcParams['hatch.linewidth'] = 5

        for idx in range(len(X_LABELS)):
            ax.bar(xticks[idx] - 0.2, pmem_optimized[idx],   0.4, color=PMEM_COLOR)
            ax.bar(xticks[idx] + 0.2, dram[idx],             0.4, color=DRAM_COLOR)

        if "hyrise" in version:
            max_ylim = 12
            yticks = [0, 2, 4, 6, 8, 10 , 12]
            txt_y = -2

            box = dict(boxstyle='square, pad=0.0', facecolor='white', ec='none')
            ax.text(xticks[6] - 0.85, max_ylim * 0.85, str(int(pmem_optimized[6])),
                    fontsize=16, ha='center', bbox=box)
            ax.text(xticks[6] + 0.85, max_ylim * 0.85, str(int(dram[6])),
                    fontsize=16, ha='center', bbox=box)

            ax.text(xticks[10] - 0.85, max_ylim * 0.85, str(int(pmem_optimized[10])),
                    fontsize=16, ha='center', bbox=box)
            ax.text(xticks[10] + 0.20, max_ylim * 1.05, str(int(dram[10])),
                    fontsize=16, ha='center', bbox=box)

            ax.text(xticks[11] - 0.20, max_ylim * 1.05, str(int(pmem_optimized[11])),
                    fontsize=16, ha='center', bbox=box)
        else:
            max_ylim = 12
            yticks = [0, 2, 4, 6, 8, 10 , 12]
            txt_y = -2

        ax.set_xticks(xticks)
        ax.set_xticklabels(["" for _ in xticks])
        ax.yaxis.grid(True, which='both')
        ax.set_axisbelow(True)
        ax.set_ylim(0, max_ylim)
        ax.set_yticks(yticks)
        ax.set_ylabel('Time [s]', fontsize=18)
        SET_LABEL_SIZE(ax)

        labels = ["PMEM", "DRAM"]
        box_x = 0.16 if i == 0 else 0.585

        fig.legend(labels, frameon=True, bbox_to_anchor=(box_x, 0.9), loc='upper center',
                edgecolor='white', columnspacing=0.4, handletextpad=0.2, borderpad=0.1,
                labelspacing=0.1, handlelength=1.8, facecolor='white', framealpha=1,
                fontsize=18, ncol=1)


        ax.axvline(x=xticks[2] + (DISTANCE / 2), ls='-', color='black')
        ax.axvline(x=xticks[5] + (DISTANCE / 2), ls='-', color='black')
        ax.axvline(x=xticks[9] + (DISTANCE / 2), ls='-', color='black')
        ax.axvline(x=xticks[12] + (DISTANCE / 2), ls='-', color='black')

        ax.text(xticks[1], txt_y, "QF1", fontsize=18, ha='center')
        ax.text(xticks[4], txt_y, "QF2", fontsize=18, ha='center')
        ax.text((xticks[7] + xticks[8]) / 2, txt_y, "QF3", fontsize=18, ha='center')
        ax.text(xticks[11], txt_y, "QF4", fontsize=18, ha='center')
        ax.text(xticks[13], txt_y, "AVG", fontsize=18, ha='center')

    axes[0].set_title("a) Hyrise $\it{(sf=50)}$", fontsize=18, fontweight="bold")
    axes[1].set_title("b) Handcrafted C++ $(\it{sf=100})$", fontsize=18, fontweight="bold")

    plot_path = f'{plot_dir}/ssb_qf_full'
    SAVE_PLOT(plot_path)


if __name__ == '__main__':
    INIT_MATPLOT()

    if (len(sys.argv) != 4):
        raise RuntimeError("Need /path/to/data/dir /path/to/db/dir /path/to/plot/dir")

    data_dir = sys.argv[1]
    db_dir = sys.argv[2]
    plot_dir = sys.argv[3]

    plot_bm(data_dir, db_dir, plot_dir)

    PRINT_PLOT_PATHS()