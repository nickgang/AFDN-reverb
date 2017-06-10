declare name "rotateFirst";
declare version "0.0";
declare author "Nick Gang";
declare description "A first order ambisonic rotation matrix";

import("stdfaust.lib");
import("AFDN.lib");

process = hoaRotate(3,ma.PI/2);
