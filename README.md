[<img src="https://assets.signaloid.io/add-to-signaloid-cloud-logo-dark-v6.svg#gh-dark-mode-only" alt="[Add to signaloid.io]" height="30">](https://signaloid.io/repositories?connect=https://github.com/IgnacioOchoa/BeamSectionCalculator#gh-dark-mode-only)
[<img src="https://assets.signaloid.io/add-to-signaloid-cloud-logo-light-v6.svg#gh-light-mode-only" alt="[Add to signaloid.io]" height="30">](https://signaloid.io/repositories?connect=https://github.com/IgnacioOchoa/BeamSectionCalculator#gh-light-mode-only)

# BeamSectionCalculator
## A project to calculate beam section properties and maximum stresses using some ASCII art ##

![completeProfile](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/170a69d2-b300-4114-a6b4-fc96942892ea)

## Introduction ##

This project presents a quick approach for estimating the strength under bending of a slender beam of any cross section that you are willing to approximate using a text editor and some '#' bricks.

We know from solid mechanics that in sleder beams the prevalent response under shear or flexion loading is bending, and that the prevalent stresses are bending stresses. See for example Strength of Materials, by V. Feodosyev, one of my favourite introductory texts in this field.

Therefore, in order to assess the strength of the beam, we need to calculate the bending stresses, which depend on the bending moment and on the section features.

The most basic formula for pure bending stresses it the following,

$`  \sigma_{max} = \frac{M \cdot y_{max}}{I_x}    `$, 

where $`  \sigma_{max} `$ is the maximum stress on the cross-section, $` M `$ is the applied moment, $` y_{max} `$ is the maximum distance from any point in the cross-section to the neutral axis, and $` I_x `$ is the moment of inertia of the cross-section.

The moment of inertia with respect to the neutral axis is the quantity that concentrates all the information about the section response under bending, and it is defined as

$`  I_x = \int_{A} y^2dA    `$

This is esentially an integration on the section area, where each point contributes to the inertia proportionally to the squared distance to the reference axis. In particular, the expression for the moment of inertia relevant to the strength calculation is the one with respect to the neutral axis. The position of this axis can be calculated as

$`  y_{na} = \frac{\int_{A} y dA}{\int_{A} dA}    `$

And it indicates the location of the fibers that neither stretch nor shrink under pure bending. In symmetric sections, the neutral axis goes through the plane of symmetry. In the following picture, the relevant geometric elements are displayed.

![section](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/00f7a43c-94e6-4baa-982c-e5f02ac4c3be)

Both integrals $`\int_{A} y dA`$ and $`\int_{A} y^2 dA`$ are calculated over the aera of the section. For simple areas these integrals can be calculated analytically, but for more complex shapes they have to be computed numerically. In this project, we explore one approach for doing so.

## Numerical approach
### Discretization

Before going into the proper algorithm, we need to state a useful simple result often shown as a first example of intertia calculation. It is the computation of the moment of inertia of a rectangular section.

![rectangle](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/a488bf3e-9b1b-45c2-b7ea-f16250427905)

The moment of inertia with respect to the neutral axis on the simmetry plane can be easily calculated.

$`  I_x = \int_{A} y^2dA = \int_{-\frac{h}{2}}^{\frac{h}{2}} y^2 bdy = \left[ \frac{y^3}{3} \right]_{-h/2}^{h/2} = \frac{bh^3}{12}    `$

Another necessary result is Steiner's theorem, than indicates how to calculate the moment of inertia with respect to a different axis.

![Steiner](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/5b4d9b40-dc4d-4061-8a18-44dec588187d)

In the picture above, the moment of inertia of an element with respect to axis2 can be calculated as $` I_{axis2} = I_{axis1}+Ad^2  `$

With these results, we can compute the moment of inertia of any section by dividing it into rectangles and adding up the contributions of each rectangle.

![sectionDiscretization1](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/31fbb4a1-1806-46e3-8b1d-8d5cf56ed50f)

The approximated moment of inertia will be $`  I_x = \Sigma_{i=1}^n b_ih_i^3/12 + A_iy_i^2    `$

### ASCII input

In order to work with a discretized section, we will represent the section using the ASCII character '#' and a plain-text application. The idea is to position the characters to tesselate the section as good as possible, as shown below.

![discretization](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/584aaa07-c457-455e-a565-549cf1228517)

The algorithm developed in this proyect reads a file containint ONLY '#' characters or blank spaces and calculates the neutral axis, the moment of inertia and the maximum stress on the section by adding up the contribution of each rectangle.

### Scaling

Besides providing a text file with the shape of the section, the user must provide the height and width of the section, which is used to translate each '#' character into a rectangle with proper width and height. This means that the text file does not provide absoulte dimensions, but only a relative position of the elements of the section. With this approach, we can calculate scaled-up of scaled-down versions of the same sections by only changing the height and with that we provide. Going even further, we can calculate "stretched" versions of the section by keeping one dimension fixed and changing the other one.

## Discretization errors

The approach outlined here has an inherent error associated with the discretization. We will consider the error with which each of the discretized elements contributes to the overall value. Do all elements contribute the same? If not, what is the contribution of each one?
To answer these questions, we will first see where the error is **not**.

### Contour bounding box elements

The elements outlined in the following picture are the ones exclusively in contact with the bounding rectangle, whose dimensions are specified by the user. These elements do not have contact with any other part of the section's perimeter. That means that there is no possible positioning error in these elements, they will always be positioned exactly at the limits of the section. A simple evidence of this can be seen by running the present algorithm on a file containing a single '#', which represents a rectangle of the given dimensions. The outcome will be the exact analytical result.

![errorAnalysis1](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/07de47b3-4d91-4ab4-92f8-8b17ff3f15be)

### Internal elements

These elements are bounded in every direction by other elements. Where one element ends, the next element starts, and the surface of the section is covered without gaps or overlaps. Therefore, there is no possible error here either.

![errorAnalysis2](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/8474bbd0-c843-4a34-8ea4-61075e83cd58)

### Contour non-bounding box elements

Finally we get to the regions were the errors concentrate. These elements represent the boundaries of the section, but those boundaries that might not be flush with underlying geometry because the section scaling does not have a correcting effect on them.

![errorAnalysis3](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/c955a06b-d04f-487a-b3b5-24432dba5da6)

To analyze the errors generated by these elements, we will state the hypothesis that the error in the contour can't be larger than half the size of the '#' character. For example, as indicated in the picture below, if the '#' character goes beyond the limit more than half its size, then it would be more accurate to remove the '#' character altogether, and the previous character won't be farther away from the limit than half its size. 

![error](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/8a4d5419-caaa-4417-8fca-aed6f49eb0ac)

With this error model in mind, the position of the block lateral sides follow a uniform distribution $` U \left(-\frac{b}{2},\frac{b}{2}\right) `$ with respect to the nominal horizontal center of the block, and the vertical sides follow a uniform distribution $` U \left(-\frac{h}{2},\frac{h}{2}\right) `$ with respect to the nomial vertical center of the block. Depending on where a block is located, some of its sides can be fixed and some others can show the aforementioned distributions. Internal sides are fixed, and the sides composing the non-bounding box perimeter will have this distribution. 
The error is propagated through the calculations on many layers, as explained in the following section.

## Uncertainty tracking

In order to provide some bounds for the error of the results, we would have to track the uncertainties of the position of the element sides all along the calculations. As a summary, the error would be propagated trough these seps:
* Area and First Moment of Area calculations for the position of the neutral axis,  $` \int_{A} dA `$ and  $` \int_{A} y dA `$
* Second Moment of Area with respect to symmetry axis for each block  $` \frac{bh^3}{12} `$
* Distance of center of each element to the neutral axis for the calculation of the moment of inertia with respect to the section neutral axis $` I_{axis2} = I_{axis1}+Ad^2 `$

From a probability point of view, we would be working with random variables with Uniform probability distributions (for the positions of the sides of each element), and then other random variables that are functions of these original random variables. For example an element in the top left corner of some internal perimeter region, would have its width defined as a random variable $` W = w + U \left(-\frac{b}{2},\frac{b}{2}\right) `$ and its height defined as another random variable $` H = h + U \left(-\frac{h}{2},\frac{h}{2}\right) `$. Then the area will be yet another random variable $` A = W*H `$. In this manner we can keep creating random variables that represent the other compound magnitudes required for the algorithm. 

Even with simple examples, the tracking of the uncertainties can be quite complex, and this is where the architecture developed by Signlaoid comes in to play to solve this problem very elegantly. By using their Compute Engine and the associated API, we can quickly inject randomized behaviour into our good old floating point variables, obtaining a final result that carries along the propbability distribution of all the intermediate steps.

In the problem at hand we accomplish this by declaring our element sides locations as uniform random variables instead of plain double values. This is all we have to do, the engine takes care of the rest.

## Running the program

In order to execute the program we need to provide as a first argument the name of the text file with the section discretization. Other optional flags are the width [-w] in mm, height [-h] in mm and applied moment [-m] in Nm. Some valid argument lists are shown below. The compilation flag (active by default when importing the project in Signaloids platform) UNCERTAIN enables the use of the *uxhw* library.
- program section1.txt -w 80 -h 120 -m 200
- program section1.txt
- program section1.txt -w 100
- program section1.txt -m 50

## Examples

### Example 1
The first example compares the results of the algorithm with a standard IPE80 profile that has its section properties listed below.

![IPE80_properties](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/30ec5da2-f6cf-4c08-b757-c354c462327d)

We use the following discretization, contained in the file IPE80.txt

![IPE80_discretization](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/fc74283b-9691-4061-ad5f-194cc0fdc582)

### Example 2
This example compares the result distributions of progressive refinements over the discretization of a complex section.

![example1](https://github.com/IgnacioOchoa/BeamSectionCalculator/assets/50671274/61a9708d-fbf2-4cc5-b666-3e5d7cf523b9)


