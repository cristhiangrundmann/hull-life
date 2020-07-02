# hull-life
A half-life 1 tool that detects collision holes

## Half-Life collision system
To detect collision, the game considers the player (and projectiles) to be points in 3d space. For projectiles, there is only one collision map. For the player there are two: one for when the player is standing and one for crouching. To compensate the fact that the player is not actually a point, these two maps have the floor and ceiling *extruded* by half the height of the player, while the walls by half the player width/height. Now, to detect collision, the game can check whether a point is inside a *solid* volume or an *empty* volume. There are different kinds of volumes, like water and acid. To do that, the game uses the BSP technique, which stands for Binary Space Partitioning.

Consider a binary tree whose nodes are planes in 3d space or *volume type*. The nodes are also called clipnodes. Each child of a clipnode correspond to a side of the clipnode's plane(since a plane cuts the world into two pieces). Given a point and starting at the root, the side of the plane this point lie on will tell us which child to advance in this recursion. When the child is not a plane but a volume type, then this type will indicate whether we hit a solid, water, acid, or nothing. This tree *partitions* the world into (disjoint) and convex polyhedra. Actually, the game wants to detect whether a *line* hits something, not a point. This is still possible and it's very useful for Ray Tracing, but I won't explain here.

This technique allows the game for a very efficient detection. However, it restricts the volume regions to have polygonal surfaces.

## Holes in collision
The BSP trees used for collision were generated automatically. However, this the map compiler used to make the original map files didn't do that flawlessly. Some maps have holes that the player (or maybe projectiles) can go through even though it would seem impossible by looking at the visible map. These flaws aren't compatible between the three collision worlds.

## Detecting possible holes
Every hole must be made out one or more **non-solid** clipnode leaves, since the player(or projectiles) can pass through. There is no other alternative. These clipnodes must be small(we are looking for flaws anyway) i.e, thin like a needle or with very small volume. The problem is that the map files only specify the BSP for the collision. To know whether a region is thin or of small volume, we need to know the it's exact vertices and edges, which is not given. This tool calculates them.

This will give us a set of possible holes. However, most of these regions are totally midair, meaning they cannot possibly be holes. They are filtered out so that only those that touch some solid regions are left. This filtering process makes the running time **much** slower. This is only a matter of seconds not minutes though.

At this time, only the volume is calculated. If this volume is bigger than a threshold, then the region is rejected. Some tweaking of parameters may be needed to be sure that no actual holes are rejected.

# In-tool visualization
When the tool is finished, it opens an OpenGL+Glut application showing the solid nodes and highlighting the non-solid clipnodes that are possibly holes. The navigation is similar to that of in-game noclip, that is, same camera control.

You can move front or back with **W** and **S** and left or rigth with **A** and **D**. To turn the camera, you can click and move your mouse or use the **YGHJ** keys, in a similar fashion of **WASD** (don't shoot me). You can scale by 2 the speeds for "strafing" and "turning" with the keys **Q** and **E** for strafing and **T** and **U** for turning.

You can change which hull to see with **123**.

The **P** and **O** keys control the wireframe drawings. The **+** key turns on/off the solid regions. When off, only the possible holes are shown. Since they are small, there are lines pointing at them from the origin. The **/** key turn on/off wallhack for these regions.

The candidate regions are at all times flashing black and white, while the solid part are static with constant color(determined by surface normal).
