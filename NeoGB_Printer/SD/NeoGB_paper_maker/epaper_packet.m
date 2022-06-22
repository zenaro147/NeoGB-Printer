function [output, alpha]=epaper_packet(input,paper_color,darkness,scale_percentage)
% Raphael BOICHOT 10-09-2020
% multi OS compatibility improved by Cristofer Cruz 2022/16/21
% code to simulate the speckle aspect of real Game boy printer
% image come from function call

sourceborders = dir('Borders/*.png');
pixel_sample=imread(['samplePixels/Pixel_sample_3600dpi_darkness_',num2str(darkness,'%03.f'),'.png']);
num_borders=numel(sourceborders);%number of border images in the library
num1=ceil(num_borders*rand);
num2=num1;
while num1==num2
    num2=ceil(num_borders*rand);
end

border_1=imread(['Borders/Border_',num2str(num1),'.png']);
border_2=imread(['Borders/Border_',num2str(num2),'.png']);
IMAGE=input;

mul=20;%size of the mask
overlapping=4;%overlapping

%intensity map for printer head with threshold
[heigth, width,~]=size(IMAGE);
streaks=zeros(heigth, width);
for i=1:1:width
  start=ceil(2*rand)-1;
  for j=1:1:heigth
  streaks(j,i)=start;
  %you can change the streak length here
  if rand<0.2;start=ceil(2*rand)-1;end;
  end
end

speckle_image=uint8(255*ones(heigth*(mul-overlapping)+overlapping,width*(mul-overlapping)+overlapping,3));

for i=1:1:heigth
   for j=1:1:width
       a=1+(i-1)*(mul-overlapping);
       b=a+mul-1;
       c=1+(j-1)*(mul-overlapping);
       d=c+mul-1;
       if IMAGE(i,j)==0
       y=0;
       x=ceil(49*rand);
       burn_dot=pixel_sample(1+20*y:20+20*y,1+20*x:20+20*x,:);
       end

       if IMAGE(i,j)==84
       y=1;
       x=ceil(49*rand);
       burn_dot=pixel_sample(1+20*y:20+20*y,1+20*x:20+20*x,:);
       end

       if IMAGE(i,j)==168
       y=2;
       x=ceil(49*rand);
       burn_dot=pixel_sample(1+20*y:20+20*y,1+20*x:20+20*x,:);
       end

       if not(IMAGE(i,j)==255);
       if rand<0.5; burn_dot=flip(burn_dot,ceil(2*rand));end;
       burn_dot=rot90(burn_dot,ceil(2*rand)-2);
       if streaks(i,j)==0; burn_dot=burn_dot+15;end;
       speckle_image(a:b,c:d,:)=min(burn_dot,speckle_image(a:b,c:d,:));
       end
end
end
%centering image on paper, The width of the print should be 27.09mm compared to the paper at 38mm
[V,W,~]=size(speckle_image);
add_col=round((W*38/27.9)-W)/2;
vert_borders=255*ones(V,add_col,3);
speckle_image=[vert_borders,speckle_image,vert_borders];
%adding margins same size as lateral white borders
horz_borders=255*ones(add_col,add_col*2+W,3);
speckle_image=[horz_borders;speckle_image;horz_borders];
%adding some subtle image noise
surface_in_pixels=V*W;
dot_rate=surface_in_pixels/200000;
for i=1:1:dot_rate
    I=ceil(rand*(V-mul));
    J=ceil(rand*(W-mul));
    y=2;
    x=ceil(49*rand);
    burn_dot=pixel_sample(1+20*y:20+20*y,1+20*x:20+20*x,:)+100;
    if rand<0.5; burn_dot=flip(burn_dot,ceil(2*rand));end;
    burn_dot=rot90(burn_dot,ceil(2*rand)-2);
    speckle_image(I:I+mul-1,J:J+mul-1,:)=min(burn_dot,speckle_image(I:I+mul-1,J:J+mul-1,:));
end
%resizing everything
resize_ratio=scale_percentage/100; %1 for full image, 0.3 for nice looking image
border1_resized=imresize(border_1,resize_ratio);
if rand<0.5,border1_resized=fliplr(border1_resized);end;
border2_resized=imresize(border_2,resize_ratio);
if rand<0.5,border2_resized=fliplr(border2_resized);end;
speckle_image=imresize(speckle_image,resize_ratio);
%creating the alpha layer from black pixels
[V,W,~]=size(speckle_image);
alpha_layer1=(border1_resized(:,:,1));
alpha_layer2=(border2_resized(:,:,1));
alpha_central=255*ones(V,W);
speckle_image=[fliplr(flipud(border1_resized));speckle_image;border2_resized];
alpha=[fliplr(flipud(alpha_layer1));alpha_central;alpha_layer2];
%coloring the paper
if (paper_color==6)
speckle_image(:,:,1)=speckle_image(:,:,1)*((255-ceil(35*rand))/255);
speckle_image(:,:,2)=speckle_image(:,:,2)*((255-ceil(35*rand))/255);
speckle_image(:,:,3)=speckle_image(:,:,3)*((255-ceil(35*rand))/255);
end
if (paper_color==5)
speckle_image(:,:,1)=speckle_image(:,:,1)*(242/255);
speckle_image(:,:,2)=speckle_image(:,:,2)*(230/255);
speckle_image(:,:,3)=speckle_image(:,:,3)*(255/255);
end
if (paper_color==4)
speckle_image(:,:,1)=speckle_image(:,:,1)*(255/255);
speckle_image(:,:,2)=speckle_image(:,:,2)*(221/255);
speckle_image(:,:,3)=speckle_image(:,:,3)*(232/255);
end
if (paper_color==3)
speckle_image(:,:,1)=speckle_image(:,:,1)*(220/255);
speckle_image(:,:,2)=speckle_image(:,:,2)*(250/255);
speckle_image(:,:,3)=speckle_image(:,:,3)*(242/255);
end
if (paper_color==2)
speckle_image(:,:,1)=speckle_image(:,:,1)*(247/255);
speckle_image(:,:,2)=speckle_image(:,:,2)*(250/255);
speckle_image(:,:,3)=speckle_image(:,:,3)*(220/255);
end
if (paper_color==1)
end
output=speckle_image;

