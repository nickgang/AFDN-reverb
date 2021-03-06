% Nick Gang
% Music 222
% Feedback Delay Reverb Tools

clear
clc
close all

%% Building a Mixing Matrix -- Hadamard
m = 8; % 12 delay lines
H = hadamard(m);
H = H/sqrt(m); % Normalize matrix so eigs will have mag of 1
%H = H*H'; % Verify orthonormal by looking at eigenvalues
abs(eigs(H))