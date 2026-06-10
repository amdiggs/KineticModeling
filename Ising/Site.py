import sys
import numpy as np
import time
import pdb

seed = int(time.time())
gen = np.random.default_rng(seed)

class SITE():
    J = None
    
    def __init__(self, vals):
        self.x = vals[0]
        self.y = vals[1]
        self.spin = None
        self.history = []
        self.init_spin()
        self.neighbors = None
    
    def init_spin(self):
        rand = gen.random()
        if(rand <= 0.5):
            self.spin = -1
        else:
            self.spin = 1
        return
    
    def energy(self, flipped = False):
        debug = []
        neb_spin = 0
        if(flipped):
            s = self.spin*(-1)
        else:
            s = self.spin
        for n in self.neighbors:
            neb_spin+= s*n.spin
        return SITE.J * neb_spin
    
    def set_neighbors(self, lat):
        idx = [[-1,0], [1,0],[0,-1], [0,1]]
        tmp = np.empty(2*lat.Dim, dtype = SITE)
        itr = 0
        if(lat.Dim == 1):
            if(self.x > 0 and self.x < lat.num_sites-1):
                tmp[0] = lat.sites[self.x -1]
                tmp[1] = lat.sites[self.x +1]
            else:
                a, b = boundary_wrapper(self.x, lat.num_sites)
                tmp[0] = lat.sites[a]
                tmp[1] = lat.sites[b]
        elif(lat.Dim == 2):
            for dx,dy in idx:
                a, b = boundary_wrapper(self.x + dx ,lat.cols), boundary_wrapper(self.y + dy, lat.rows)
                tmp[itr]= lat.sites[a][b]
                itr+=1
        else:
            print("0 dimensional problem are easy")
        self.neighbors = tmp
        return
    
    def update_hist(self):
        self.history.append(self.spin)
    
    def con_prob(self,lat, d):
        idy = np.arange(-d,d+1, 1, dtype=int)
        idx = np.arange(-d +1, d, 1, dtype = int)
        #pdb.set_trace()
        tmp = 0 #np.empty(2*lat.Dim, dtype = SITE)
        itr = 0
        for dx in [-d,d]:
            for dy in idy:
                a, b = boundary_wrapper(self.x + dx ,lat.cols), boundary_wrapper(self.y + dy, lat.rows)
                tmp+= lat.sites[a][b].spin
                itr+=1
        for dy in [-d,d]:
            for dx in idx:
                a, b = boundary_wrapper(self.x + dx ,lat.cols), boundary_wrapper(self.y + dy, lat.rows)
                tmp+= lat.sites[a][b].spin
                itr+=1
        return self.spin * tmp
    
    def n_point(self, lat, d):
        idx = np.arange(-d,d+1, 1, dtype=int)
        #pdb.set_trace()
        tmp = 1 #np.empty(2*lat.Dim, dtype = SITE)
        itr = 0
        for dx in idx:
            dy = 0
            a, b = boundary_wrapper(self.x + dx ,lat.cols), boundary_wrapper(self.y + dy, lat.rows)
            if not (a == self.x and b == self.y):
                tmp*= lat.sites[a][b].spin
                itr+=1
        for dy in idx:
            dx = 0
            a, b = boundary_wrapper(self.x + dx ,lat.cols), boundary_wrapper(self.y + dy, lat.rows)
            if not (a == self.x and b == self.y):
                tmp*= lat.sites[a][b].spin
                itr+=1
        return self.spin * tmp
    
    @classmethod
    def set_J(cls,J):
        cls.J = J

def boundary_wrapper(x, ub):
    if(x >= 0 and x <= ub-1):
        return x
    elif(x < 0):
        return ub + x
    elif(x > ub -1):
        return x - ub
    else:
        quit("what are you doing???")






if __name__ == '__main__':
    test = SITE([1,1])
    print("What are you doing???\n This is a class file!")

















