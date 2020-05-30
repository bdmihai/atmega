$fn=100;

module hexagon(r){
    polygon(points=[[r,r*(tan(30))], [0,r*(2/sqrt(3))],
                    [-r,r*(tan(30))], [-r,-r*(tan(30))], 
                    [0,-r*(2/sqrt(3))], [r,-r*(tan(30))]]);
}

difference() {
    linear_extrude(height = 11.2, center = false) hexagon(2);
    translate([0, 0, -0.1]) cylinder(d=2, h=20);
}
