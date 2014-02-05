function mi = mib(sinrdB, modulation)
    sinr = 10.^(sinrdB./10);
    
    % according to Table 29 in IEEE 802.16m-08/004r5
    if modulation == 2
        mi = J(2*sqrt(sinr));
    end
    if modulation == 4
        mi = 0.5 * J(0.8 * sqrt(sinr)) + 0.25 * J(2.17 * sqrt(sinr)) + 0.25 * J(0.965 * sqrt(sinr));
    end
    if modulation == 6
        mi = 1/3 * J(1.47 * sqrt(sinr)) + 1/3 * J(0.529 * sqrt(sinr)) + 1/3 * J(0.366 * sqrt(sinr));
    end
end

