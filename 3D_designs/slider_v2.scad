
$fn=60;


module drawSlider()
{  
    
    difference(){
        union(){
            linear_extrude(height=10, scale=[0.8, 0.7], center=true){
                square([13,25], center=true);
            }
            
            translate([0, 0, -7])
            cube([7.5, 21, 6], center=true);
            
        }
        
        translate([0,0,7]){
            scale([2,0.8,0.35]){
                sphere(r=9);
        }} 
    }
}

difference(){
    
    drawSlider();
    
    // center cut
    translate([0,0,-10]){ ;
        cube([1.6,19,10], center=true);
    }
    
    // screw headroom
    translate([0, -12.5, -15.1])
    cylinder(h=19, d=5.5, center=false);
    
    // screw headroom
    translate([0, 12.5, -15.1])
    cylinder(h=19, d=5.5, center=false);
    
}