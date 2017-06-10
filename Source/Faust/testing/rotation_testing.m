% rotate_test(n, a) = par(i, 2*n+1, _) <: par(i, 2*n+1, rotation(i, a))
% with
% {
% 	rotation(i, a) = (par(j, 2*n+1, gain1(i, j, a)), par(j, 2*n+1, gain2(i, j, a)), ...
%     par(j, 2*n+1, gain3(i, j, a)) :> _)
% 	with
% 	{
% 		indexabs = (int)((i - 1) / 2 + 1);
% 		gain1(i, j, a) = _ * cos(a * indexabs) * (j == i);
% 		gain2(i, j, a) = _ * sin(a * indexabs) * (j-1 == i) * (j != 0) * (i%2 == 1);
% 		gain3(i, j, a) = (_ * sin(a * indexabs)) * (j+1 == i) * (j != 0) * (i%2 == 0);
% 	};
% };
%% 
clc 
clear 

%%

n = 1;
% a = pi/2;

inputchs = 0:2*n;
j = 0:2*n; % output channels 

indexAbs = floor((inputchs - 1)./2 + 1);


%%

i = 0;

indexabs_1 = floor(((i - 1)/2 + 1));
%       cos(a * indexabs)
gain1_1 = indexabs_1.*(j == i); 
%       sin(a * indexabs)
gain2_1 = indexabs_1.*(j-1 == i).*(j ~= 0).*(mod(i,2) == 1); % grabbing odds
%       sin(a * indexabs)
gain3_1 = indexabs_1.*(j+1 == i).*(j ~= 0).*(mod(i,2) == 0); % grabbing evens


i = 1;
class(i)

indexabs_2 = floor((i - 1)/2 + 1);
%       cos(a * indexabs)
gain1_2 = indexabs_2.*(j == i);
%       sin(a * indexabs)
gain2_2 = indexabs_2.*(j-1 == i).*(j ~= 0).*(mod(i,2) == 1); % grabbing odds
%       sin(a * indexabs)
gain3_2 = indexabs_2.*(j+1 == i).*(j ~= 0).*(mod(i,2) == 0); % grabbing evens

i = 2;

indexabs_3 = floor((i - 1)/2 + 1);
%       cos(a * indexabs)
gain1_3 = indexabs_3.*(j == i);
%       sin(a * indexabs)
gain2_3 = indexabs_3.*(j-1 == i).*(j ~= 0).*(mod(i,2) == 1); % grabbing odds
%       sin(a * indexabs)
gain3_3 = indexabs_3.*(j+1 == i).*(j ~= 0).*(mod(i,2) == 0); % grabbing evens



%% 

indexabs_3 = floor((i - 1)/2 + 1);
%       cos(a * indexabs)
gain1_3 = indexabs_3.*(j == i);
%       sin(a * indexabs)
gain2_3 = indexabs_3.*(j-1 == i).*(j ~= floor()).*(mod(i,2) == 1); % grabbing odds
%       sin(a * indexabs)
gain3_3 = indexabs_3.*(j+1 == i).*(j ~= 0).*(mod(i,2) == 0); % grabbing evens
