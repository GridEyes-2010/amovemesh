cl__1 = 1;
Point(1) = {-10, -5, -5, 1};
Point(2) = {10, -5, -5, 1};
Point(3) = {10, 5, -5, 1};
Point(4) = {-10, 5, -5, 1};
Point(5) = {-10, -5, 5, 1};
Point(6) = {10, -5, 5, 1};
Point(7) = {10, 5, 5, 1};
Point(8) = {-10, 5, 5, 1};
Point(9) = {0, 7, -5, 1};
Point(10) = {0, 7, 5, 1};
Point(11) = {-5, 6, -5, 1};
Point(12) = {-5, 6, 5, 1};
Point(13) = {5, 6, -5, 1};
Point(14) = {5, 6, 5, 1};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {4, 1};
Line(4) = {8, 5};
Line(5) = {5, 6};
Line(6) = {6, 7};
Line(7) = {7, 3};
Line(8) = {8, 4};
Line(9) = {5, 1};
Line(10) = {6, 2};
BSpline(11) = {7, 14, 10, 12, 8};
BSpline(12) = {3, 13, 9, 11, 4};
Line Loop(14) = {11, 8, -12, -7};
Ruled Surface(14) = {14};
Line Loop(16) = {4, 5, 6, 11};
Plane Surface(16) = {16};
Line Loop(18) = {4, 9, -3, -8};
Plane Surface(18) = {18};
Line Loop(20) = {6, 7, -2, -10};
Plane Surface(20) = {20};
Line Loop(22) = {2, 12, 3, 1};
Plane Surface(22) = {22};
Line Loop(24) = {1, -10, -5, 9};
Plane Surface(24) = {24};
Surface Loop(26) = {22, 20, 16, 18, 24, 14};
Volume(26) = {26};
