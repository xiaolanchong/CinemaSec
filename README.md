
# What's this

CinemaPark surveillance system aiming at measuring the number of visitors in a cinema hall.

## Installations

1) Download Boost library at boost.org (1.76 and later), unpack to ../ dir, change the include dir settings to adjust to the version.
3) Build in vs2022 or later.
2) Install Avisynth to play videos.
3) Install MS SQL Express 2008 later (db tested in that of 2016).
4) Restore CinemaSec database from the backup. It contains chair outlines (contours) and accumulated background images.
5) Edit scripts/CinemaSec_db_admin_mode.REG to change the server name.
6) Run CinemaSec_db_admin_mode.REG file.
7) Run CSClient.exe.

## Todo

1) Test video files isn't playable. Once loaded in no xvid native codec?
2) chcsva.dll can play files to simulated camera videos. Don't remember these settings.
3) Convert to x64?
