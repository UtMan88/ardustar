SET comp="C:\Users\keith ctr maggio\Documents\Arduino\libraries\ArdBitmap\extras\compressor\compressor2.0.jar"
SET java="C:\Program Files\Java\jdk1.8.0_71\bin\java.exe"

%java% -jar %comp% player.png -gs player -anp PLYR
%java% -jar %comp% player_L.png -gs player_L -anp PLYR_L
%java% -jar %comp% enemy.png -gs enemy -anp ENMY
%java% -jar %comp% enemy_L.png -gs enemy_L -anp ENMY_L
%java% -jar %comp% villain.png -gs villain -anp VILL

COPY /B /Y player\bitmaps.h+player_L\bitmaps.h+enemy\bitmaps.h+enemy_L\bitmaps.h+villain\bitmaps.h bitmaps.h

PAUSE