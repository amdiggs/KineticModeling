# Kinetic Modeling
This repo contains a few sample projects that demonstrate some of the modeling used in material science.
## Chemical Reaction Network
**Stochastic Solution for coupled differential equations**

$$
\begin{align}
\frac{1}{2}
\end{align}
$$
In this example the chemical reaction network is used to simulate a 1D many atom random walk.
All the mobile atoms start in the same deep minimum and are then allowed to migrate away facing a series of shallow and deep minimum with correspondig low and high activation barriers.
## PN Junction
**Finite Element Method**

This example simulates the diffusion of electrons and holes in a 2D PN junction. This was a side project and it was used for some experimental techniques where intermidiate states were saved to textures in an attempt to utilize some of the more unique features of gpu memory.
### Dependencies
    - GLFW > 3.3
    - OpenCL 1.2
    - OpenGL > 3.3

## Ising
**Markov Cahin Monte Carlo**

This is a simple python script that computes the magnetic ordering of an Ising spin lattice with ferromagnetic coupling.

<p align="center">
    <img src="assets/ising_0.png" width="200"/>
    <img src="assets/ising_4.png" width="200"/>
    <img src="assets/ising_5.png" width="200"/>
    <img src="assets/ising_6.png" width="200"/>
</p>


