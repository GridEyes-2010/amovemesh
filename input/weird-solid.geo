cl__1 = 1;
Point(1) = {-18, 0.4, 0, 1};
Point(2) = {20, 1.3, 0, 1};
Point(3) = {-10.9, 12.5, 0, 1};
Point(4) = {14.5, 13.1, 0, 1};
Point(5) = {-18, 0.5, 5, 1};
Point(6) = {19.8, 1.3, 5, 1};
Point(7) = {14.5, 13, 5, 1};
Point(8) = {-13.6, 13.5, 5, 1};
Point(9) = {-17.7, 2.5, 0, 1};
Point(10) = {-16.7, 5.4, 0, 1};
Point(11) = {-14.6, 9.5, 0, 1};
Spline(1) = {1, 9, 10, 11, 3};
Line(2) = {3, 4};
Line(3) = {4, 2};
Line(4) = {2, 1};
Line(5) = {5, 8};
Line(6) = {8, 7};
Line(7) = {7, 6};
Line(8) = {6, 5};
Line(9) = {5, 1};
Line(10) = {8, 3};
Line(11) = {7, 4};
Line(12) = {6, 2};
Line Loop(14) = {5, 6, 7, 8};
Plane Surface(14) = {14};
Line Loop(16) = {1, -10, -5, 9};
Ruled Surface(16) = {16};
Line Loop(18) = {2, -11, -6, 10};
Plane Surface(18) = {18};
Line Loop(20) = {7, 12, -3, -11};
Plane Surface(20) = {20};
Line Loop(22) = {3, 4, 1, 2};
Plane Surface(22) = {22};
Line Loop(24) = {12, 4, -9, -8};
Plane Surface(24) = {24};
Surface Loop(26) = {14, 16, 22, 20, 24, 18};
Volume(26) = {26};