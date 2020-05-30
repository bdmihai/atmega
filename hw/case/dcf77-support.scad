$fn = 100;

difference() {
    cube([10, 8.5, 14]);
    translate([5, 9.9, 5]) rotate([90, 0, 0]) cylinder(d=8, h=10);
    translate([5, 8.5/2, 5]) cylinder(d=2, h=10);
}
