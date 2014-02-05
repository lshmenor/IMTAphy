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

  % TODO: Vectorize this function as the computeSigmaAS()

  function DS = computeDS(taus, powers, KFactor, isLoS)

  N = length(powers);
  M = 20;

  powers(isnan(powers)) = 0;

  if (isLoS == 1)
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

  taus(isnan(taus)) = 0;
  taus = repmat(taus, 1, M);

  taus(end+1)= 0;

  Dnumerator = 0;
  Ddenominator = 0;

  % Summation for finding D
    for n = 1:(N*M) + isLoS
	Dnumerator = Dnumerator + taus(n)*powers(n); % powers are already scaled in case of LoS, the last tau is 0 in case of LoS
	Ddenominator = Ddenominator + powers(n); % powers are already scaled in case of LoS and K/(K+1) is the last entry of powers
    end

  D = Dnumerator / Ddenominator;

  DSnumerator = 0;

  % Summation for finding DS
    for n = 1:(N*M) + isLoS
      % the taus and powers contain as the last entry the tau_LoS (=0) as well as the LoS power K / (K + 1)
      DSnumerator = DSnumerator + (taus(n)-D)^2 * powers(n);
    end

  DS = sqrt( DSnumerator / Ddenominator );

  end
