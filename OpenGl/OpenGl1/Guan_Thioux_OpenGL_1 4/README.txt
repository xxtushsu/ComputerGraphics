Changes:
First we defined the shapes Cube and Pyramid in initializeGL().
Then we painted these shapes on the screen in paintGL().
After we successfully painted the shapes on the screen, we defined the perspective and translation matrices.
We use these matrices on MVP which is passed to the shaders.
Then we make 2 more matrices for scaling and rotation which are updated in setScale(...) and setRotation(...).
These 2 matrices are applied on MVP to apply the transformations.

Instructions:
	- open in QT Creator
	- ctrl+R to run
	- use rotation nodes to change the rotation of the cube & pyramid
	- use scale slider to change the size of the cube & pyramid
	- close with x top right
