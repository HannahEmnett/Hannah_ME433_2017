function [arr] = procimu()
colors = 'krbgcmyg';

% Best noise reduction: MAF filter 
% Least computational complexity: IIR filter


%OPEN SERIAL COMMUNICATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

port = 'COM21';   % Set up serial port address
if ~isempty(instrfind)                  % If PIC is not connected
    fclose(instrfind);                  % Close serial
    delete(instrfind);                  % ...
end

mySerial = serial(port, 'Baudrate', 230400); %Connect to PIC
set(mySerial,'TimeOut', 30)

fprintf('Opening port %s...\n', port);    % Print to screen
fopen(mySerial);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% fprintf(mySerial, [num2str(samples)]);
% samps_conf = fscanf(mySerial, '%d');
% 
% fprintf(mySerial, [num2str(num_weights)]);
% num_weights_conf = fscanf(mySerial, '%d');
% 
% if num_weights > 2
%     weights=fir1(num_weights, vals);
% else 
%     weights=vals;
% end
% 
% doglog=zeros(num_weights);
%  for jj = 1:num_weights
%      fprintf(mySerial, '%d\n', weights(jj));
%      doglog(jj) = fscanf(mySerial, '%d');
%  end   
% if weights == doglog
%     weights_conf=0;
% else 
%     weights_conf=1;
% end 
% 
% if ((samps_conf~=samples) || (num_weights_conf~=num_weights) || (weights_conf~=weights))
%  error('Configuration values not confirmed by PIC. %d %d %d\n', samps_conf , num_weights_conf, weights_conf);
% end

fprintf('Program starting.\n');
fprintf(mySerial, 'r\n');

data=zeros(5,1);
arr = zeros(5,100); %Preallocate plot vector here
arr(:,:) = NaN; %Initialize plot vector as empty data
for i=1:100
    %[arr(:,i)]= fscanf(mySerial, '%c');
    str = fgetl(mySerial);
    data = textscan(str, '%f', 'Delimiter',','); 
    data = data{1};
    arr(:,i)=data;
    %[arr(1,i)]= fscanf(mySerial, '%d');
end   
figure;
for xx = 2:5
    plot (6:100, arr(xx,6:100),colors(xx))
    hold on;
end
legend('raw', 'maf', 'iir', 'fir');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%CLOSE FUNCTION

fclose(mySerial);
fprintf('Port %s closed.\n', port);

end



