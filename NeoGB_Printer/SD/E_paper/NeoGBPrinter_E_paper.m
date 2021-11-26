% Raphael BOICHOT 25/11/2021 E-paper module for NeoGB printer
% Compatible with Matlab and Octave
% image must be 4 colors maximum, which is the native output format 

clear
clc
paper_color=6;%6=random, 5=purple, 4=pink, 3=regular blue, 2=regular yellow or 1=regular white
%watermarking='Raphael BOICHOT 2021';

  try
    pkg load image % for compatibility with Octave
  catch 
    % Nothing to do
  end
  
imagefiles = dir('../www/thumb/*.png');% the default format is png, other are ignored
nfiles = length(imagefiles);    % Number of files found

for k=1:1:nfiles
    currentfilename = imagefiles(k).name;
    BandW_image=imread(['../www/thumb/',currentfilename]);
map=BandW_image(:,:,1);
C=unique(map);
switch length(C)
    case 4; map=(map==C(1))*0+(map==C(2))*84+(map==C(3))*168+(map==C(4))*255;
    case 3; map=(map==C(1))*0+(map==C(2))*84+(map==C(3))*255;    
    case 2; map=(map==C(1))*0+(map==C(2))*255;  
end;
[epaper, alpha]=epaper_packet(map,paper_color);
imwrite(epaper,['./E_paper/GameBoy epaper_',currentfilename],'Alpha',alpha);
disp(['GameBoy_epaper_',currentfilename,' written to SD Card'])
end
disp('Done !')

