REM SET comp="C:\Users\keith ctr maggio\Documents\Arduino\libraries\ArdBitmap\extras\compressor\compressor2.0.jar"
REM SET java="C:\Program Files\Java\jdk1.8.0_71\bin\java.exe"
SET comp="C:\Users\utman\Documents\Arduino\libraries\ArdVoice\extras\vocoder\vocoder0.2.jar"
SET java="C:\Program Files (x86)\Java\jre1.8.0_144\bin\java.exe"

%java% -jar %comp% aargh_lowq.wav -gs yellsound -q 10 -anp YELL_SOUND
%java% -jar %comp% bewareilive_lowq.wav -gs beware -q 10 -anp BEWARE_SOUND

COPY /B /Y yellsound\voices.h+beware\voices.h voices.h

PAUSE