import matplotlib.pyplot as plt

BASE_THREAD_TO_DATA = {
     1: ([], []), 2: ([], []), 3: ([], []), 4: ([], []), 5: ([], []), 6: ([], []), 7: ([], []), 8: ([], []),
     9: ([], []), 10: ([], []), 11: ([], []), 12: ([], []), 13: ([], []), 14: ([], []), 15: ([], []), 16: ([], []),
     17: ([], []), 18: ([], []), 19: ([], []), 20: ([], []), 21: ([], []), 22: ([], []), 23: ([], []), 24: ([], []),
     25: ([], []), 26: ([], []), 27: ([], []), 28: ([], []), 29: ([], []), 30: ([], []), 31: ([], []), 32: ([], []),
     33: ([], []), 34: ([], []), 35: ([], []), 36: ([], [])
}

THREAD_TO_MARKER = {
    1: 'x', 2: '>', 4: 's', 6: '<', 8: 'o', 16: '^', 18: 'v', 24: '.', 32: 'P', 36: 'D'
}

THREAD_TO_COLOR = {
    1: '#000000', 2: '#009999', 4: '#4C0099', 6: '#999900',
    8: '#990000', 16: '#CC6600', 18: '#0080FF', 24: '#009900',
    32: '#CCCC00', 36: '#404040',
}

DRAM_COLOR = '#0080FF'
PMEM_COLOR = '#303030'

PLOT_PATHS = []


def PRINT_PLOT_PATHS():
    print(f"To view new plots, run:\n\topen {' '.join(PLOT_PATHS)}")

IMG_TYPES = ['.pdf'] #, '.png']

def SAVE_PLOT(plot_path, img_types=None):
    if img_types is None:
        img_types = IMG_TYPES

    for img_type in img_types:
        img_path = f"{plot_path}{img_type}"
        PLOT_PATHS.append(img_path)
        plt.savefig(img_path, bbox_inches='tight')

    plt.figure()


GLOBAL_MATPLOT_PARAMS = {
    #'text.usetex': True,
    'font.size': 18,
    #'font.family': 'lmodern',
}

def INIT_MATPLOT():
    plt.rcParams.update(GLOBAL_MATPLOT_PARAMS)


def SET_LABEL_SIZE(ax, xsize=18, ysize=18):
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(xsize)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(ysize)