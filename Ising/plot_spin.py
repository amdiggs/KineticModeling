import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
import pdb
plt.rcParams['figure.figsize'] = [10,6]

plt.ion()

def q_plot(lat):
    fig = plt.figure(figsize=[10,10])
    ax = fig.add_subplot(projection = '3d')
    x = np.arange(0, lat.cols)
    y= np.arange(0,lat.rows)
    X, Y = np.meshgrid(x, y)
    u = np.zeros([lat.rows, lat.cols])
    v = np.zeros([lat.rows, lat.cols])
    for i in y:
        for j in x:
            u[i][j] = lat.sites[i][j].spin
    ax.quiver(X,Y,1.0, v, v, u, length = 2)
    ax.set_zlim(-10, 10)
    plt.show(block = True)

class spin_plot():
    
    def __init__(self, lat):
        self.fig = plt.figure(figsize=[10,10])
        self.ax = self.fig.add_subplot(title = "Ising Model")
        self.y, self.x = np.arange(0,lat.rows), np.arange(0, lat.cols)
        self.ITR = 0
        self.spins = None
        self.Evals = None
    
    def set_spins(self, arr):
        self.spins = arr
        return
    
    def draw(self,T):
        #pdb.set_trace()
        self.ax.clear()
        txt = "T = {0:.0f}".format(T)
        self.ax.pcolormesh(self.x, self.y, self.spins, shading='auto')
        self.ax.text(0.1,1.025, txt,size=20, transform = self.ax.transAxes)
        self.fig.canvas.draw_idle()
        self.fig.canvas.flush_events()
        #self.fig.savefig("Gif_Images/ising_{0:.0f}.jpg".format(self.ITR))
        self.ITR+=1
        #plt.waitforbuttonpress()
        plt.show(block=False)
        plt.pause(0.1)
        return


def plot_hist(conts, bins, d):
    txt="r = {0:.0f}".format(d)
    w = bins[1] - bins[0]
    fig = plt.figure()
    ax = fig.add_subplot()
    ax.bar(bins, conts, width = 1.8, edgecolor = 'white')
    ax.set_xticks(bins)
    ax.text(0.1, 0.9,txt, transform = ax.transAxes)
    plt.show(block = True)

def plot_multi(x, vals, temps):
    fig = plt.figure()
    ax = fig.add_subplot()
    for v,T in zip(vals,temps):
        plt.scatter(x,v, label = "T={0:.0f}".format(T))
    ax.set_xlabel("r")
    ax.set_ylabel(r"$ \langle \sigma \rangle_r $")
    plt.title(r'$ \langle \sigma \rangle $')
    plt.legend()
    fig.savefig("s_prob.png")
    plt.show(block = True)



def junk():
    print("")
    #ps.plt.ylim(0, 100)
    #plt.legend()
    #plt.xlabel("r")
    #plt.ylabel(r"$ \langle \sigma_i \sigma_j \rangle $")
    #plt.title("Correlation of spins over a range of T")
    #ps.plt.savefig("CORR.png")
    #plt.show(block=True)














