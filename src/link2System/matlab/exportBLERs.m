% In link level results the results from link level simulations
% are stored. For each SINR of a certain range the block error rate
% determined over 5000 subframes is computed by comparing the demodulated +
% decoded Transport Blocks to the ones originally transmitted. The TB sizes
% are chosen in a range from 50 to 6144 bits to fit into a single code 
% block of that size. Simulations are performed for a single eNB-UE link
% of SISO system with a total bandwidth of 20 MHz assuming an AWGN channel. 
% Simulation dates June 12-14, 2011

% open the file with write permission
fid = fopen('output.txt', 'w');

plotFiles = {'QPSK.txt' 'QAM16.txt' 'QAM64.txt'};

path = '../linkLevelResults/'
N_subframes = 5000;

minSinr = -10;
maxSinr = 23.5;
stepSize = 0.1;
sinrs = minSinr:stepSize:maxSinr;
Modulations = {'QPSK', 'QAM16', 'QAM64'};
blockLengths = [50, 150, 500, 2000, 6144];

% CQIs 5-6 are the LTE QPSK CQIs with code rates > 1/3
% CQI 16 is a dummy CQI that was simulated with QPSK CR=1/3
cqisQPSK = [16,       5,   6];
crsQPSK  = [1024/3, 449, 602] / 1024;

% CQIs 7-9 are the LTE QAM16 CQIs with code rates > 1/3
% CQI 17 is a dummy CQI that was simulated with QAM CR=1/3
cqisQAM16 = [17,       7,   8,   9];
crsQAM16  = [1024/3, 378, 490, 616] / 1024;

% CQIs 10-15 are the LTE QAM64 CQIs with code rates > 1/3
% CQI 18 is a dummy CQI that was simulated with QAM CR=1/3
cqisQAM64 = [18,      10,  11,  12,  13,  14,  15];
crsQAM64  = [1024/3, 466, 567, 666, 772, 873, 948] / 1024;

% % for plotting the actual LTE CQIs:
% % comment the fprintf block below as well
% % CQIs 5-6 are the LTE QPSK CQIs with code rates > 1/3
% % CQI 16 is a dummy CQI that was simulated with QPSK CR=1/3
% cqisQPSK = [1, 2, 3, 4,       5,   6];
% crsQPSK  = [78, 120, 193, 308, 449, 602] / 1024;
% 
% % CQIs 7-9 are the LTE QAM16 CQIs with code rates > 1/3
% % CQI 17 is a dummy CQI that was simulated with QAM CR=1/3
% cqisQAM16 = [       7,   8,   9];
% crsQAM16  = [ 378, 490, 616] / 1024;
% 
% % CQIs 10-15 are the LTE QAM64 CQIs with code rates > 1/3
% % CQI 18 is a dummy CQI that was simulated with QAM CR=1/3
% cqisQAM64 = [     10,  11,  12,  13,  14,  15];
% crsQAM64  = [466, 567, 666, 772, 873, 948] / 1024;


fprintf(fid, 'unsigned int numSINRs = %d;\n', numel(sinrs));

fprintf(fid, 'unsigned int numQPSKcodeRates = %d;\n', numel(crsQPSK));
fprintf(fid, 'unsigned int numQAM16codeRates = %d;\n', numel(crsQAM16));
fprintf(fid, 'unsigned int numQAM64codeRates = %d;\n', numel(crsQAM64));

fprintf(fid, 'unsigned int numBlockLengths = %d;\n', numel(blockLengths));
fprintf(fid, 'unsigned int blockLengths[5] = {50, 150, 500, 2000, 6144};\n');
fprintf(fid, 'float qpskCodeRates[3] = {%f, %f, %f};\n',...
        crsQPSK(1), crsQPSK(2), crsQPSK(3));
fprintf(fid, 'float qam16CodeRates[4] = {%f, %f, %f, %f};\n',...
        crsQAM16(1), crsQAM16(2), crsQAM16(3), crsQAM16(4));
fprintf(fid, 'float qam64CodeRates[7] = {%f, %f, %f, %f, %f, %f, %f};\n',...
        crsQAM64(1), crsQAM64(2), crsQAM64(3), crsQAM64(4), crsQAM64(5), crsQAM64(6), crsQAM64(7));

fprintf(fid, 'double sinrs[%d] = { ', numel(sinrs)); 
fprintf(fid, '%f', sinrs(1));

for i = 2:numel(sinrs)
    fprintf(fid, ', %f', sinrs(i));
end
fprintf(fid, ' };\n\n');

for m = 1:3
    eval(['cqis = cqis' char(Modulations(m)) ';']);
    eval(['crs = crs' char(Modulations(m)) ';']);
    
    index = 1;
    rows = numel(sinrs);
    cols = numel(cqis)*numel(blockLengths);
    curves = zeros(rows, cols);
    for BlockSize = blockLengths
        for cqi = cqis
           % consolidate all BLER curves of one modulation into one big matrix "curves"
           % the columns hold all code rates for all block sizes with block
           % sizes being the outer loop
           filename_prefix = 'BLER_'
           output_filename = sprintf('CQI%d_BL%d_subframes%d_MHz20_StepSize%2.1f',...
                                      cqi, BlockSize, N_subframes, stepSize);
           filename_suffix = '.mat';

           dataFile = [path filename_prefix output_filename filename_suffix]
           if exist(dataFile, 'file')
              load(dataFile, 'resultSNR_vector', 'resultBLER_overall');

              % fill all smaller entries with BLER=1
              firstIndex = round((resultSNR_vector(1) - minSinr) / stepSize) + 1;
              if firstIndex > 1
                  curves(1:firstIndex-1, index) = 1;
              end
              % fill all bigger entries with BLER=0
              lastIndex = round((resultSNR_vector(numel(resultSNR_vector)) - minSinr) / stepSize) + 1;
              if lastIndex < numel(sinrs)
                  curves(lastIndex+1:numel(sinrs), index) = 0;
              end
              for entry = 1:numel(resultSNR_vector)
                  targetIndex = round((resultSNR_vector(entry) - minSinr) / stepSize) + 1
                  curves(targetIndex, index) = resultBLER_overall(entry);
              end
           end
           index = index + 1;
        end
    end
    
    % now write this matrix to the output file
    fprintf(fid, 'static double %s[%d][%d] = {\n', char(Modulations(m)), rows, cols);

    for row = 1:rows
       fprintf(fid, '{');
       for col = 1:cols
           fprintf(fid, ' %6.5f', curves(row, col));
           if col ~= cols
              fprintf(fid, ',');
           end
       end
       if row==rows
           fprintf(fid, '}\n');
       else
           fprintf(fid, '},\n');
       end
    end
    fprintf(fid, '};\n\n');
    
    % write output files used for plotting
    plotFile = fopen(plotFiles{m}, 'w');
    fprintf(plotFile, '#SINR\t');
    for col = 1:cols
        fprintf(plotFile, '%6.5f/%d\t', crs(1+mod(col-1, numel(crs))), blockLengths(1+floor((col-1) / numel(crs))));
    end
    fprintf(plotFile, '\n');
    for row = 1:rows
       fprintf(plotFile, '%6.5f\t', sinrs(row));
       for col = 1:cols
           fprintf(plotFile, ' %6.5f\t', curves(row, col));
       end
       fprintf(plotFile, '\n');
    end
    fclose(plotFile);
    
end


fclose(fid);
