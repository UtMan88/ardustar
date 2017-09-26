SET comp="C:\Users\utman\Documents\Arduino\libraries\ArdVoice\extras\vocoder\vocoder0.2.jar"
SET java="C:\Program Files (x86)\Java\jre1.8.0_144\bin\java.exe"

%java% -jar %comp% aargh_lowq.wav -gs yellsound -q 10 -anp YELL_SOUND
%java% -jar %comp% bewareilive_lowq.wav -gs beware -q 10 -anp BEWARE_SOUND

COPY /B /Y yellsound\voices.h+beware\voices.h voices.h

PAUSE
