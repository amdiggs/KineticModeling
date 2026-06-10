import sys
import pdb
import numpy as np
import Site
import plot_spin as ps


class LATTICE():
    J = -0.004
    kb = 8.617e-5
    Beta = None
    time = 0
    num_flips = 0
    M = 0
    TC= (2.269 * J) / kb
    
    def __init__(self, MxN, T):
        self.Dim= len(MxN)
        self.TEMP = T
        self.sites = None
        if(self.Dim == 1):
            self.num_sites = MxN[0]
            self.set_sites_1D()
        else:
            self.rows = MxN[0]
            self.cols = MxN[1]
            self.num_sites = MxN[0] * MxN[1] 
            self.set_sites_2D()
        LATTICE.set_Beta(T)
        Site.SITE.set_J(LATTICE.J)
        self.calc_mag()
    
    def set_sites_1D(self):
        temp_lattice = np.empty([self.num_sites], dtype = Site.SITE)
        for i in range(self.num_sites):
            temp_lattice[i] = Site.SITE([i,i])
        self.sites=temp_lattice
        for s in self.sites:
            s.set_neighbors(self)
        return
    
    def set_sites_2D(self):
        temp_lattice = np.empty([self.rows, self.cols], dtype = Site.SITE)
        for i in range(self.rows):
            for j in range(self.cols):
                temp_lattice[i][j] = Site.SITE([i,j])
        self.sites=temp_lattice
        for row in self.sites:
            for s in row:
                s.set_neighbors(self)
        return
    
    def energy(self):
        return LATTICE.J * LATTICE.M
    
    def calc_mag(self):
        for row in self.sites:
            for s in row:
                LATTICE.M += s.spin
        return
    
    def desimate(self):
        d_sites = []
        n_rows = self.rows/2
        n_cols = self.cols/2
        return 
    
    @classmethod
    def set_Beta(cls,T):
        cls.Beta = 1 / (cls.kb*T)
    
    @classmethod
    def set_time(cls):
        cls.time+=1
    
    @classmethod
    def itr_flips(cls):
        cls.num_flips+=1
    
    @classmethod
    def itr_mag(cls, val):
        cls.M += val
    

def MET_HAS(site):
    current=site.energy()
    proposed = site.energy(flipped =True)
    delta = proposed - current
    #pdb.set_trace()
    if(delta <= 0.0):
        site.spin*= (-1)
        LATTICE.itr_flips()
        LATTICE.itr_mag(site.spin)
    else:
        prob = np.exp(-1 * LATTICE.Beta * delta)
        rand  = Site.gen.random()
        if( rand <= prob):
            site.spin*= (-1)
            LATTICE.itr_flips()
            LATTICE.itr_mag(site.spin)
            return
        else:
            return


def save_state(lat):
    if(lat.Dim ==1):
        tmp = np.empty([lat.num_sites], dtype = int)
        i=0
        for s in lat.sites:
            tmp[i] = s.spin
            i+=1
        return tmp
    else:
        tmp = np.empty([lat.rows,lat.cols], dtype = int)
        for i in range(lat.rows):
            for j in range(lat.cols):
                tmp[i][j] = lat.sites[i][j].spin
        return tmp

def spin_prob(lat):
    dic={-1:0, 1:0}
    for row in lat.sites:
        for s in row:
            dic[s.spin]+=1
    counts = [dic[-1]/2, dic[1]/2]
    bins = [-1, 1]
    c, b = np.asarray(counts, dtype = int), np.asarray(bins, dtype = int)
    pmf = counts/np.sum(counts)
    exp = np.sum(bins*pmf)
    return exp, pmf

def itr_latt(lat):
    if(lat.Dim == 1):
        for s in lat.sites:
            MET_HAS(s)
        return
    else:
        for row in lat.sites:
            for s in row:
                MET_HAS(s)
                s.update_hist()
        return

def corr(lat, d, prob_plot =False):
    my_dict = {0:0}
    svals =[]
    counts = []
    bins = []
    for row in lat.sites:
        for s in row:
            is_in = False
            val = s.con_prob(lat,d)
            #pdb.set_trace()
            for key in my_dict:
                if(val == key):
                    my_dict[key] += 1
                    is_in = True
            if not(is_in):
                my_dict[val] = 1
    for key in my_dict:
        counts.append(my_dict[key]/ 2)
        bins.append(key)
    return np.asarray(counts, dtype = int), np.asarray(bins, dtype = int)

def NP_corr(lat,d):
    dic = {-1:0, 1:0}
    for row in lat.sites:
        for s in row:
            key = s.n_point(lat,d)
            dic[key] += 1
    counts = [dic[-1]/2, dic[1]/2]
    bins = [-1, 1]
    c, b = np.asarray(counts, dtype = int), np.asarray(bins, dtype = int)
    pmf = counts/np.sum(counts)
    exp = np.sum(bins*pmf)
    return exp, pmf

def temp(Temp, lat, delta):
    if(Temp > 0):
        lat.TEMP = Temp
        LATTICE.set_Beta(lat.TEMP)
    else:
        lat.TEMP = 1.0
        LATTICE.set_Beta(lat.TEMP)
    return Temp - delta

def H(probs):
    hl = []
    for p in probs:
        if(p > 0):
            hl.append(p*np.log2(1/p))
    return np.sum(hl)


def cor_len(lat, dist, plot_hist= False):
    c, b, = corr(lat, dist)
    pmf = c/np.sum(c)
    h=H(pmf)
    expc = np.sum(b*pmf)/(8*dist)
    if(plot_hist):
        ps.plot_hist(c,b,dist)
    return expc, h

if __name__=='__main__':
    dist = 1
    dvals = np.arange(1,2, 1, dtype=int)
    #my_plot = ps.spin_plot(lat)
    Temp = 150.0
    lat = LATTICE([50,50], Temp)
    Hvals =[]
    EXvals =[]
    temps = []
    Evals=[]
    tmp = []
    for i in range(300):
        itr_latt(lat)
        if not(i % 10):
            #Evals=[]
            #tmp = []
            #my_plot.set_spins(save_state(lat))
            #my_plot.draw(lat.TEMP)
            #Evals.append(lat.energy())
            for d in dvals:
                #EXP, HL = cor_len(lat,d)
                exp , pmf = spin_prob(lat)
                HL = H(pmf)
                Evals.append(exp)
                tmp.append(HL)
            EXvals.append(Evals)
            Hvals.append(tmp)
            temps.append(lat.TEMP)
            Temp = temp(Temp,lat, 5)
    ps.plt.scatter(temps,tmp, label = r"H( $\sigma $)")
    ps.plt.scatter(temps, Evals, label = r'$\langle \sigma \rangle $')
    ps.plt.legend()
    ps.plt.xlabel("Temperature")
    ps.plt.title("Entropy and Avarage Magnitization")
    ps.plt.savefig("s_prob.png")
    ps.plt.show(block=True)
    #ps.plot_multi(dvals, EXvals, temps)












