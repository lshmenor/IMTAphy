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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clc
clear

for scenarios = [ 1 2 3 4 6 ]   % The reference curves for the 5th scenario UMiO2I are not available

    scenario = ScenarioNameMapping(scenarios);

    for propConds = 0:1

        propCond = PropCondNameMapping(propConds);
        calibrationDataFileName = ['../results/calibrationData_' scenario '_'  propCond];

        if  (exist([calibrationDataFileName '.it'], 'file') == 0)
            fprintf(['Skipping scenario ', scenario,  ' propagation ', propCond, ' because .it file  does not exist\n'])
            continue
        end


        fprintf(['Now running for scenario ', scenario ' and ' propCond ' condition ...' , '\n'])

        for angleTypes = 1:2

            angleType = AnglesNameMapping(angleTypes);



            % Load the file. This will contain AoA/Ds, Powers, Delays and K-factors
            itload(calibrationDataFileName);

            % Put the AoAs and AoDs angles in a 3D matrix
            angles(:,:,1) = AoAs(:,:);
            angles(:,:,2) = AoDs(:,:);

            % Also put the LOS ray angles
            anglesLoS(:,1) = AoAsLOS;
            anglesLoS(:,2) = AoDsLOS;

            % Get the number of clusters and number of links
            [K, N] = size(Powers);

            %K = 2000;

            % Change the K-factors to dB
            % KFactors = 10*log10(sigmas(:,end));
            KFactors = sigmas(:,end);

            % The Delay Spread (DS) to be computed
            DS = zeros(1,K);

            % The Angle Spread (AS) to be computed
            sigmaAS = zeros(1,K);

            % Get a large number of DS and AS values by considering all the links
            for k = 1:K

                % Calculate the DS for that link but only do that for one
                % angle type
                if (angleTypes == 1)
                   DS(k) = computeDS(taus(k,:), PowersScaled(k,:), KFactors(k), propConds);
                end

                % Calculate the AS for that link
                sigmaAS(k) = computeSigmaAS(PowersScaled(k,:), angles(k, :, angleTypes), propConds, anglesLoS(k, angleTypes), KFactors(k));

            end

            if(angleTypes == 1)  % To avoid plotting DS CDF twice

                % Get the DS CDF and plot it
                figure
                hLoS = cdfplot(DS);
                set(gcf,'visible','off')
                x = 1e6*get(hLoS,'XData'); y = 100*get(hLoS,'YData');
                figurePointer = gca;

                hold on

                % Get the reference results and plot them
                ReferenceDS;
                refCurvesDataDS = ['DS_' propCond '_ref_' scenario];

                % Plot the reference curves keeping into account the fact that
                % different numbers of reference available for different scenarios
                plot(eval([ refCurvesDataDS '(:,1)']), 0:100, 'LineWidth', 1.5, 'color','red')
                plot(eval([refCurvesDataDS '(:,2)']), 0:100, 'LineWidth',1.5, 'color', 'magenta')

                if (scenarios~=1)
                    plot(eval([refCurvesDataDS '(:,3)']), 0:100, 'LineWidth',1.5, 'color', 'cyan')
                end

                if(scenarios ~= 3 && scenarios ~= 1)
                    plot(eval([refCurvesDataDS '(:,4)']), 0:100, 'LineWidth',1.5, 'color', 'green')
                    plot(eval([refCurvesDataDS '(:,5)']), 0:100, 'LineWidth',1.5, 'color', 'blue')
                end

                if(scenarios == 1)
                    plot(eval([refCurvesDataDS '(:,3)']), 0:100, 'LineWidth',1.5, 'color', 'green')
                    plot(eval([refCurvesDataDS '(:,4)']), 0:100, 'LineWidth',1.5, 'color', 'blue')
                end

				 semilogx(x,y,'color','r', 'LineWidth', 1.5, 'color', 'k')

                % Put the legend accordingly
                if (scenarios == 3)
                    legend( 'org1', 'org2', 'org3', 'LKN-TUM')
                elseif(scenarios ~= 1)
                    legend('org1', 'org2', 'org3', 'CATR', 'CMCC','LKN-TUM')
                elseif (scenarios == 1)
                    legend(  'org1', 'org2', 'CATR', 'CMCC', 'LKN-TUM')
                end

                xlabel('Delay (\musec)', 'FontWeight','bold','FontSize', 11)
                ylabel('C.D.F (%)', 'FontWeight','bold','FontSize', 11) 
                set(figurePointer, 'XScale', 'log');
                set(figurePointer, 'XLim', [0.001 10]);
                title(['CDF of Delay Spread - (' propCond ' - ' scenario ')'], 'FontWeight','bold', 'FontSize', 12)

                % Hide the figure and out in ./out folder that must exist
                set(gcf,'visible', 'off')
                saveas(gcf, ['../results/out/' refCurvesDataDS] , 'pdf')

            end

            figure
            hold on


            % Get the variables for reference plots
            ReferenceAS;
            refCurvesDataAS = ['AS_' angleType  '_' propCond '_ref_' scenario];

            % Plot the reference curves keeping into account the fact that
            % different numbers of reference available for different scenarios
            plot(eval([ refCurvesDataAS '(:,1)']), 0:100, 'LineWidth', 1.5, 'color','red')
            plot(eval([refCurvesDataAS '(:,2)']), 0:100, 'LineWidth',1.5, 'color', 'magenta')

            if (scenarios~=1)
                plot(eval([refCurvesDataAS '(:,3)']), 0:100, 'LineWidth',1.5, 'color', 'cyan')
            end

            if(scenarios ~= 3 && scenarios ~= 1)
                plot(eval([refCurvesDataAS '(:,4)']), 0:100, 'LineWidth',1.5, 'color', 'green')
                plot(eval([refCurvesDataAS '(:,5)']), 0:100, 'LineWidth',1.5, 'color', 'blue')
            end

            if(scenarios == 1)
                plot(eval([refCurvesDataAS '(:,3)']), 0:100, 'LineWidth',1.5, 'color', 'green')
                plot(eval([refCurvesDataAS '(:,4)']), 0:100, 'LineWidth',1.5, 'color', 'blue')
            end

			[hAoA,statsAoA] = cdfplot(sigmaAS);

			set(hAoA,'YData',100*get(hAoA,'YData'), 'XData',get(hAoA,'XData'),'LineWidth',1.5,'color', 'black')
            xlabel('AoA (degrees)', 'FontWeight','bold', 'FontSize', 11)
            ylabel('C.D.F (%)', 'FontWeight','bold', 'FontSize', 11)

            % Put the legend accordingly
            if (scenarios == 3)
                legend('org1', 'org2', 'org3', 'LKN-TUM')
            elseif(scenarios ~= 1)
                legend('org1', 'org2', 'org3', 'CATR', 'CMCC', 'LKN-TUM')
            elseif (scenarios == 1)
                legend('org1', 'org2', 'CATR', 'CMCC', 'LKN-TUM')
            end

            title(['CDF of Angle Spread - ' angleType ' (' propCond ' - ' scenario ')'], 'FontWeight','bold', 'FontSize', 12)

            % Hide the figure and out in ./out folder that must exist
            set(gcf,'visible', 'off')
            saveas(gcf, ['../results/out/' refCurvesDataAS] , 'pdf')

            clear angles
            clear anglesLoS
            clear anglesNLoS

        end    % loop end for AoAs and AoDs
    end    % loop end for LoS/NLoS
end    % loop end for all scenarios
