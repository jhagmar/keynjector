/*
* Copyright 2014 Jonas Hagmar
*
* This file is part of keynjector.
*
* keynjector is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* keynjector is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with keynjector.  If not, see <http://www.gnu.org/licenses/>.
*/

/***** Parameters *****/

tol = 0.1; // tolerance
eps = 0.01;
wall_thickness = 1; // thickness of case wall
front_thickness = 0.5; // thickness of front wall
pcb_width = 42.94 + tol;
pcb_drawing_height = 35.96;
pcb_height = 36.2 + tol;
pcb_depth = 1.76 + tol;
component_depth = 2.7 + tol; // largest component is USB connector
screen_width = 42.72 + tol;
screen_height = 60.26 + tol;
screen_back_film_thickness = 0.1;
screen_depth = 3.6 + screen_back_film_thickness + tol;
screen_top = 2.9; // distance from top to viewport
holder_height = 10; // height of screen holder
holder_width = 8;
usb_a = 4.75;
usb_b = 7.86;
usb_c = 2.8;
usb_zoffset = -0.1;
btn_width = 2;
btn_height = 10;
btn_y_offset = -7.24;
btn_base_height = 12;
btn_base_depth = 0.95;
btn_ratio = 0.95; // ration of button size to hole
btn_protrude = 0.5;
sd_y_offset = -18.3;
sd_height = 12;
sd_depth = 2;
nut_depth = 1.6; // max depth of DIN 934 M2 nut
nut_diameter = 4; // diameter of DIN 934 M2 nut
nut_clearance = 0.2;
screw_head_diameter = 3.8; // DIN 965 M2
screw_body_diameter = 2; // DIN 965 M2
screw_depth = 5;
dbg_y_offset = -26; // offset for debug header window
dbg_holder_dent_size = 2;
dbg_x_offset = -8; 
pcb_real_height_offset = (pcb_height - pcb_drawing_height)/2;
usb_coord = [[usb_a/2,0], [usb_b/2, (usb_b - usb_a)/2],
	[usb_b/2, usb_c], [-usb_b/2, usb_c],
	[-usb_b/2, (usb_b - usb_a)/2], [-usb_a/2, 0]];
case_width = pcb_width + 2*wall_thickness;
case_height = screen_height + 2*wall_thickness;
case_depth = pcb_depth + component_depth + screen_depth + wall_thickness
	+ front_thickness;
fillet_radius = 1;
screen_x2 = [screen_width/2, case_height/2 - wall_thickness,
	case_depth/2 - front_thickness];
screen_x1 = screen_x2 - [screen_width, screen_height, screen_depth];
pcb_x2 = [pcb_width/2, case_height/2 - wall_thickness,
	-case_depth/2 + pcb_depth + wall_thickness];
pcb_x1 = pcb_x2 - [pcb_width, pcb_height, pcb_depth];
comp_x2 = pcb_x2 + [0,0,component_depth]; // component layer
comp_x1 = pcb_x1 + [0,0,pcb_depth];
cable_x2 = [case_width/2 - wall_thickness, pcb_x1[1] + eps,
	case_depth/2 - front_thickness];
cable_x1 = [-case_width/2 + wall_thickness, 
	-case_height/2 + wall_thickness, -case_depth/2 + wall_thickness]; 
viewport_fillet_radius = front_thickness;
viewport_width = 36.72;
viewport_height = 48.96;
viewport_x2 = [viewport_width/2 + viewport_fillet_radius,
	case_height/2 - wall_thickness - screen_top + viewport_fillet_radius, 
	case_depth/2 + wall_thickness];
viewport_x1 = [viewport_x2[0] - viewport_width - 2*viewport_fillet_radius,
	viewport_x2[1] - viewport_height - 2*viewport_fillet_radius,
	viewport_x2[2] - case_depth];
holder1_x1 = [-case_width/2 + wall_thickness,
	pcb_x1[1] - holder_height, -case_depth/2 + wall_thickness];
holder1_x2 = [holder1_x1[0] + holder_width, holder1_x1[1] + holder_height,
	case_depth/2 - front_thickness - screen_depth];
holder2_x1 = holder1_x1 
	+ [case_width - 2*wall_thickness - holder_width,0,0];
holder2_x2 = holder1_x2 
	+ [case_width - 2*wall_thickness - holder_width,0,0];
dbg_x2 = [case_width/2 - wall_thickness + eps,
	case_height/2 - wall_thickness + dbg_y_offset,
	case_depth/2 - front_thickness - eps];
dbg_x1 = [dbg_x2[0] + dbg_x_offset,
	pcb_x1[1] - dbg_holder_dent_size, -case_depth/2 - eps];
sup_size = 1; // size of display support
sup1_x1 = [pcb_x1[0], pcb_x2[1] - sup_size, pcb_x1[2]-eps];
sup1_x2 = [sup1_x1[0]+sup_size, sup1_x1[1]+sup_size, 
	sup1_x1[2]+pcb_depth+component_depth+eps];
sup2_x1 = sup1_x1 + [pcb_width - sup_size, 0, 0];
sup2_x2 = sup1_x2 + [pcb_width - sup_size, 0, 0];


/***** Renders *****/

//$fn=20;

case_back();
//case_back_normalized();

//case_back_dbg();
//case_back_dbg_normalized();

holder();
//holder_normalized();

case_front();
//case_front_normalized();

btn();
//btn_normalized();

/***** Modules *****/

module translated_sphere(r, x) {
	translate(x)
		sphere(r);
}

// construct a rounded cube by taking the hull of
// spheres in the corners
module rounded_cube(w, h, d, r) {
	hull() {
		translated_sphere(r, [w/2 - r, h/2 - r, 
			d/2 - r]);
		translated_sphere(r, [-w/2 + r, h/2 - r, 
			d/2 - r]);
		translated_sphere(r, [w/2 - r, -h/2 + r, 
			d/2 - r]);
		translated_sphere(r, [-w/2 + r, -h/2 + r, 
			d/2 - r]);
		translated_sphere(r, [w/2 - r, h/2 - r, 
			-d/2 + r]);
		translated_sphere(r, [-w/2 + r, h/2 - r, 
			-d/2 + r]);
		translated_sphere(r, [w/2 - r, -h/2 + r, 
			-d/2 + r]);
		translated_sphere(r, [-w/2 + r, -h/2 + r, 
			-d/2 + r]);	
	}
}

// convenience module to draw cube at specified coordinates
module box(x1, x2) {
	translate(x1)
			cube(x2 - x1);
}

// the solid body of the case
module case_body() {
	rounded_cube(case_width, case_height, case_depth, fillet_radius);
}

// viewport cut-out
module viewport_cut_out() {
	box(viewport_x1, viewport_x2);
}

// viewport fillet
module viewport_fillet() {
	union() {
		hull() {
			translated_sphere(viewport_fillet_radius,
				[viewport_x1[0], viewport_x1[1], 
					case_depth/2-viewport_fillet_radius]);
			translated_sphere(viewport_fillet_radius,
				[viewport_x1[0], viewport_x2[1], 
					case_depth/2-viewport_fillet_radius]);
		}
		hull() {
			translated_sphere(viewport_fillet_radius,
				[viewport_x1[0], viewport_x1[1], 
					case_depth/2-viewport_fillet_radius]);
			translated_sphere(viewport_fillet_radius,
				[viewport_x2[0], viewport_x1[1], 
					case_depth/2-viewport_fillet_radius]);
		}
		hull() {
			translated_sphere(viewport_fillet_radius,
				[viewport_x2[0], viewport_x2[1], 
					case_depth/2-viewport_fillet_radius]);
			translated_sphere(viewport_fillet_radius,
				[viewport_x1[0], viewport_x2[1], 
					case_depth/2-viewport_fillet_radius]);
		}
		hull() {
			translated_sphere(viewport_fillet_radius,
				[viewport_x2[0], viewport_x2[1], 
					case_depth/2-viewport_fillet_radius]);
			translated_sphere(viewport_fillet_radius,
				[viewport_x2[0], viewport_x1[1], 
					case_depth/2-viewport_fillet_radius]);
		}
	}
}

// lcd
module screen() {
	box(screen_x1, screen_x2);
}

// pcb with components
module pcb() {
	box(pcb_x1, pcb_x2 + [0,0,eps]);
}

module components() {
	box(comp_x1, comp_x2 + [0,0,eps]);
}

// empty space for cable
module cable_hole() {
	box(cable_x1, cable_x2);
}

// hole for micro usb B plug
module usb_hole() {
	translate([0, case_height/2, 
		-case_depth/2 + wall_thickness + pcb_depth + usb_zoffset])
	rotate(90, [1,0,0])
	linear_extrude(wall_thickness + eps)
	polygon(usb_coord);
}

// hole for user button
module btn_hole() {
	translate([case_width/2 + eps,
		case_height/2 - wall_thickness + btn_y_offset 
		- pcb_real_height_offset,
		- case_depth/2 + wall_thickness + pcb_depth + btn_width/2])
	rotate(-90, [0,0,1])
	rotate(90, [1,0,0])
	linear_extrude(wall_thickness + 2*eps) {
		hull() {
			translate([-btn_height/2 + btn_width/2,0,0])
				circle(btn_width/2);
			translate([btn_height/2 - btn_width/2,0,0])
				circle(btn_width/2);
		}
	}
}

// user button translated to have center at 0
module btn_normalized() {
	union() {
		linear_extrude(btn_base_depth)
		translate([-btn_base_height/2, -btn_width/2,0])
		square([btn_base_height, btn_width]);
		color([1,0,0])
		translate([0,0,btn_base_depth - eps])	
		linear_extrude(wall_thickness + btn_protrude + eps) {
			hull() {
				translate([btn_ratio * (-btn_height/2 + btn_width/2),0,0])
					circle(btn_ratio * btn_width/2);
				translate([btn_ratio *  (btn_height/2 - btn_width/2),0,0])
					circle(btn_ratio * btn_width/2);
			}
		}
	}
}

// user button
module btn() {
	translate([case_width/2,
		case_height/2 - wall_thickness + btn_y_offset 
		- pcb_real_height_offset,
		- case_depth/2 + wall_thickness + pcb_depth + btn_width/2])
	rotate(90, [0,0,1])
	rotate(90, [1,0,0])
	translate([0,0,-(btn_base_depth + wall_thickness)])
	btn_normalized();
}

// micro sd card slot
module sd_hole() {
	translate([-case_width/2 - eps,
		case_height/2 - wall_thickness + sd_y_offset 
		- pcb_real_height_offset,
		- case_depth/2 + wall_thickness + pcb_depth])
	rotate(90, [0,0,1])
	rotate(90, [1,0,0])
	linear_extrude(wall_thickness + 2*eps)
	translate([-sd_height/2, 0, 0])
	square([sd_height, sd_depth]);

}

// screen holders
module holder() {

	difference() {
		box(holder1_x1, holder1_x2);
		screws();
		nuts();
	}
	
	difference() {
		box(holder2_x1, holder2_x2);
		screws();
		nuts();
	}
	
}

// screen support
module sup() {
	box(sup1_x1, sup1_x2);
	box(sup2_x1, sup2_x2);
}

// screen holder at 0
module holder_normalized() {
	translate([-(holder1_x2[0] + holder2_x1[0])/2,
		-(holder1_x1[1] + holder2_x2[1])/2,
		-holder1_x1[2]]) {
		holder();
	}
}

// mask for the case front
module front_mask() {
	
	union() {
		difference() {
			box([-case_width/2-eps, -case_height/2-eps,
				- case_depth/2 + wall_thickness + pcb_depth + btn_width/2],
				[case_width/2 + eps, case_height/2+eps, case_depth/2 + eps]);
	
			box([-case_width/2 + wall_thickness, -case_height/2 - eps, 
				-case_depth/2], [case_width/2 - wall_thickness, 
				-case_height/2 + wall_thickness +eps, 
				case_depth/2 - front_thickness]);
			
			box([-case_width/2 + wall_thickness, case_height/2 
				-wall_thickness - eps, -case_depth/2], 
				[case_width/2 - wall_thickness, 
				case_height/2 + eps, 
				-case_depth/2 + wall_thickness + pcb_depth + usb_zoffset
				+ usb_c]);
	
			box(sup1_x1 - [0,eps,0], sup2_x2);
			
		}

		box([-case_width/2-eps, holder1_x1[1], holder1_x1[2]],
			[-case_width/2 + wall_thickness + eps, holder1_x2[1], 
			holder1_x2[2]]);

		box([case_width/2 - wall_thickness - eps, holder2_x1[1], 
			holder2_x1[2]],
			[case_width/2+eps, holder2_x2[1], holder2_x2[2]]);
	}
}

// mask for the case back
module back_mask() {

	difference() {
		box([-case_width/2, -case_height/2,
			-case_depth/2], [case_width/2, case_height/2, case_depth/2]);

		front_mask();
		
	}
}

// the case
module case() {
  	union() {
		difference() {
			union() {
				difference() {
					case_body();
					viewport_cut_out();
				}
				viewport_fillet();
			}
			screen();
			pcb();
			components();
			cable_hole();
			usb_hole();
			btn_hole();
			sd_hole();
		}
		sup();
	}
}

// the front part of the case, suitable for 3D printing
module case_front() {
	intersection() {
		case();
		front_mask();
	}
}

// front part of case at 0
module case_front_normalized() {
	rotate(180, [0, 1, 0])
		translate([0,0,-case_depth/2])
			case_front();
}

// the back part of the case, suitable for 3D printing
module case_back() {
	difference() {
		intersection() {
			case();
			back_mask();
		}
		screws();
	}
}

// back part of case at 0
module case_back_normalized() {
	translate([0,0,case_depth/2])
		case_back();	
}

// the back part of the case, suitable for 3D printing, with hole
// for debug header
module case_back_dbg() {
	difference() {
		case_back();
		dbg();
	}
}

// the back part of the case, with debug header hole, at 0
module case_back_dbg_normalized() {
	translate([0,0,case_depth/2])
		case_back_dbg();	
}

module hexagon(r) {
	a = 2*r / sqrt(3);
	polygon([[r, a/2], [0, a], [-r, a/2], 
		[-r, -a/2], [0, -a], [r, -a/2]]); 
}

// hole for DIN 965 screw
module din965() {
	union() {
		translate([0,0,-eps]) {
			cylinder(h=screw_head_diameter/2 + eps, 
				d1=screw_head_diameter, d2=0);
		}
		cylinder(h=screw_depth, d=screw_body_diameter);
	}
}

// screw holes
module screws() {
	translate([(holder1_x1[0] + holder1_x2[0])/2,
		(holder1_x1[1] + holder1_x2[1])/2,
		-case_depth/2]) {
	
		din965();
	}
	translate([(holder2_x1[0] + holder2_x2[0])/2,
		(holder2_x1[1] + holder2_x2[1])/2,
		-case_depth/2]) {
	
		din965();
	}
}

// hole for nut
module nut() {

	h = holder1_x2[2] - holder1_x1[2] 
		- (screw_depth - wall_thickness)
		+ nut_depth;

	translate([0,0,eps])
		rotate(180, [1,0,0])
			linear_extrude(h + eps)
				hexagon(nut_diameter/2 + nut_clearance);
}

// nut holes
module nuts() {

	translate([(holder1_x1[0] + holder1_x2[0])/2,
		(holder1_x1[1] + holder1_x2[1])/2,
		holder1_x2[2]]) {
	
		nut();
	}
	translate([(holder2_x1[0] + holder2_x2[0])/2,
		(holder2_x1[1] + holder2_x2[1])/2,
		holder2_x2[2]]) {
	
		nut();
	}

}

// debug header hole
module dbg() {
	box(dbg_x1, dbg_x2);
}