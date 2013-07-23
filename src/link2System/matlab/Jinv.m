function effSINR = Jinv(mib)
    
    effSINR = zeros(length(mib));

    for n = 1:length(mib)
        y = mib(n);
        if y < 0.3646
            x = 1.09542 * y^2 + 0.214217 * y + 2.33727 * sqrt(y); 
        else
            x = -0.706692 * log(-0.386013 * (y - 1)) + 1.75017 * y;
        end
        effSINR(n) = x;
    end
end