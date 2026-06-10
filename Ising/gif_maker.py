import glob
from PIL import Image
import numpy as np

def make_gif(fold):
    fname="ising_{0:.0f}.jpg"
    ITR = np.arange(0,30,1)
    frames = []
    for t in ITR:
        frames.append(Image.open(fold+fname.format(t)))
    frame_one = frames[0]
    frame_one.save("Ising.gif", format="GIF", append_images=frames, save_all=True, duration=1000, loop=0)


if __name__=='__main__':
    make_gif("Gif_Images/")