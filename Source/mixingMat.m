% Nick Gang
% Music 222
% Feedback Delay Reverb Tools

clear
clc
close all

%% Building a Mixing Matrix -- Hadamard
m = 12; % 12 delay lines
H = hadamard(m);
H = H/sqrt(12); % Normalize matrix so eigs will have mag of 1
%H = H*H'; % Verify orthonormal by looking at eigenvalues
abs(eigs(H))