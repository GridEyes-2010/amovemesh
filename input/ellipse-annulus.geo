cl__1 = 1;
cl__2 = 0.5;
Point(1) = {0, 0, 0, cl__1};
Point(2) = {2, 0, 0, cl__2};
Point(3) = {4, 0, 0, cl__2};
Point(4) = {0, 1, 0, cl__2};
Point(5) = {0, 2, 0, cl__2};
Point(6) = {0, -1, 0, cl__2};
Point(7) = {0, -2, 0, cl__2};
Point(8) = {-2, 0, 0, cl__2};
Point(9) = {-4, 0, 0, cl__2};
Ellipse(1) = {4, 1, 2, 2};
Ellipse(2) = {2, 1, 2, 6};
Ellipse(3) = {6, 1, 2, 8};
Ellipse(4) = {8, 1, 8, 4};
Ellipse(5) = {7, 1, 3, 9};
Ellipse(6) = {9, 1, 3, 5};
Ellipse(7) = {5, 1, 9, 3};
Ellipse(8) = {3, 1, 9, 7};
Line Loop(11) = {6, 7, 8, 5, -3, -2, -1, -4};
Plane Surface(11) = {11};
Physical Line(12) = {1, 2, 3, 4};
Physical Line(13) = {5, 6, 7, 8};
Physical Surface(14) = {11};
Recombine Surface {11};
Recombine Surface {11};
