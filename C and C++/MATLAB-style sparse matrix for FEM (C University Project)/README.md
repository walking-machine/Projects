# MATLAB-style sparse matrix for Finite Element Method application (spring 2019)
This is a project created for Numerical Methods subject. Most of the code was written during the laboratories, only the sparse matrix and all connected functions where implemented solely by myself./
The program compares 2 methods of storing the main matrix, which in FEM consists mostly of zeros, as a MATLAB-style sparse matrix (only non-zero values and they indexes are stored) - implemented as my project and as a function (matrix is computed during the multiplication by vector) - this approach was used during the laboratories.\
\
With my current knowledge I think, that, the robustness of my solution could be improved by adding a possibility of increasing the size of the array containing the matrix data twice when the initial limit gets exeeded, what never happend in this application.\
\
*This implementation uses winbgi2 and MesLib libraries, provided by [CCFD group at my faculty (MEiL)](http://ccfd.github.io/courses/)*