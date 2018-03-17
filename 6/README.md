# CG
Project for the Computer Graphics course @ RUG

Free Animation:
    -We implemented perspective controls (adws) to scroll the perspective in an espherical coordinate system, being the focus point the center of the sphere.  You can also zoom with '-' and '=', and pause the animation with 'space'.

Water Animation:
    -At the normal coloring part (002_normals.png), in the fragmentation shader we mapped the normal vector coordinates to rgb values with the function f(x) = 2*x + 1, so that it would provide the following example results f(-1) = 0, f(0) = 0.5, f(1) = 1, since we saw it as the most logic way to do it. Therefore, every vertex would always have 0.5 of green (because, as logically said in the document, dV should be 0 for now, and 0 will always be 0 after normalization), the value of red will oscilate in [0,1], because it will be mapped by the funcion r(dU) = (((-dU/sqrt((-dU)^2 + 1)) + 1) / 2). (i.e.: normalization then f(x)). The blue value will oscilate in ]0,1] because it also go through the same process, being mapped by the function b(dU) = (((1/sqrt((-dU)^2 + 1)) + 1) / 2), but the normal vector z component is always 1.
