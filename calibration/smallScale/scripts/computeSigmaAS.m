%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This file is part of IMTAphy
% _____________________________________________________________________________
%
% Copyright (C) 2010
% Institute of Communication Networks (LKN)
% Department of Electrical Engineering and Information Technology (EE & IT)
% Technische Universitaet Muenchen
% Arcisstr. 21
% 80333 Muenchen - Germany
% http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
% 
% _____________________________________________________________________________
%
%   IMTAphy is free software: you can redistribute it and/or modify
%   it under the terms of the GNU General Public License as published by
%   the Free Software Foundation, either version 3 of the License, or
%   (at your option) any later version.
%
%   IMTAphy is distributed in the hope that it will be useful,
%   but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%   GNU General Public License for more details.
%
%   You should have received a copy of the GNU General Public License
%   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function sigmaAS = computeSigmaAS(powers, angles, isLoS, phiLoS, KFactor)

N = length(powers);
M = 20;

% Change the NaNs to zeros, replicate powers to match the angles and scale down by M
powers(isnan(powers)) = 0;

if (isLoS == 1)
    angles(end+1) = phiLoS;
    % the powers from C++ were already scaled and contained the LoS ray power as part
    % of the first cluster's power -> we need to remove it here
    powers(1) = powers(1) - KFactor/(1 + KFactor);
end

powers = repmat(powers, 1, M);
powers = powers / M;

if (isLoS == 1)
    % add power for single LoS ray
    powers(end+1) = KFactor/(1 + KFactor);
end

% Change the NaNs to zeros and convert the angles to degrees
angles(isnan(angles)) = 0;
angles = 180*angles/pi;

% see also p. 186 of part II of D.1.1.2
sigma = [];
Offsets = 360;

for offset = 0:Offsets
	
    anglesDelta = angles + offset;
    
    % wrap with modulo according to Winner:
    anglesDelta = mod(anglesDelta, 360);
    for m = 1:N*M + isLoS
        if anglesDelta(m) > 180
            anglesDelta(m) = anglesDelta(m) - 360;
        end
    end
    
    % contains LoS ray in case of LoS as last entry
    muDelta = sum(anglesDelta.* powers) / sum(powers);
    
    % now compute the difference theta and shift it into -180..180
    thetas = anglesDelta - muDelta;
    thetas = mod(thetas, 360);
	
    for m = 1:N*M + isLoS
        if thetas(m) > 180
            thetas(m) = thetas(m) - 360;
        end
    end % end of loop shifting thetas
    
    sigma = [sigma sqrt(sum((thetas.^2) .* powers) / sum(powers));];
	
end

sigmaAS = min(sigma);
