g++ -mwindows -static -s -Os putno.cxx -o putno.exe
strip -s -R .comment -R .gnu.version --strip-unneeded putno.exe
upx --best putno.exe
