*** Kinetic Modeling
This repo contains a few sample projects that demonstrate some of the modeling used in material science.
** Chemical Reaction Network
Stochastic Solution for coupled differential equations.
In this example the chemical reaction network is used to simulate a 1D many atom random walk.
All the mobile atoms start in the same deep minimum and are then allowed to migrate away facing a series of shallow and deep minimum with correspondig low and high activation barriers.
This simulation was used *Hydrogen-induced degradation dynamics in silicon heterojunction solar cells via machine learning* to model the kinetics of hydrogen atoms in amorphous Si.
** PN Junction
Finite Element Method
This example simulates the diffusion of electrons and holes in a 2D PN junction. This was a side project and it was used for some experimental techniques where intermidiate states were saved to textures in an attempt to utilize some of the more unique features of gpu memory.
This example requires access to the GLFW > 3.3, OpenCL 1.2, and OpenGL > 3.3.
