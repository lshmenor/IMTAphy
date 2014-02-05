function J = J(sinr)
% according to IEEE 802.16m-08/004r5 4.3.2.1 eq. (52)
    
    % Achtung: das hier klappt so nicht, weil ich einen ganzen Vektor
    % reinschiebe und die Fallunterscheidung aber auf einzelnen Elementen
    % funktionieren muss. 
    J = zeros(length(sinr));
    
    for n = 1:length(sinr)
        x = sinr(n);
        if x < 1.6363
            y = -0.04210661 * x^3 + 0.209252 * x^2 - 0.00640081 * x;

        else
            y = 1 - exp(0.00181492 * x^3 - 0.142675 * x^2 - 0.0822054 * x + 0.0549608);
        end
        J(n) = y;
    end
end