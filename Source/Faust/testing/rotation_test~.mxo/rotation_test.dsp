//----------------`rotate`-------------------------
// Rotates the sound field.
//
// #### Usage
//
// ```
// _ : rotate(n, a) : _
// ```
//
// Where:
//
// * `n`: the order
// * `a`: the angle in radian
//-----------------------------------------------------
rotate_test(n, a) = par(i, 2*n+1, _) <: par(i, 2*n+1, rotation(i, a))
with
{
	rotation(i, a) = (par(j, 2*n+1, gain1(i, j, a)), par(j, 2*n+1, gain2(i, j, a)), par(j, 2*n+1, gain3(i, j, a)) :> _)
	with
	{
		indexabs = (int)((i - 1) / 2 + 1);
		gain1(i, j, a) = _ * cos(a * indexabs) * (j == i);
		gain2(i, j, a) = _ * sin(a * indexabs) * (j-1 == i) * (j != 0) * (i%2 == 1);
		gain3(i, j, a) = (_ * sin(a * indexabs)) * (j+1 == i) * (j != 0) * (i%2 == 0);
	};
};

process = rotate_test(3, 90);
