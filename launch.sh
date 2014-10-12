#sudo cp ~/install/TurboParser-2.1.0/deps/local/lib/libgflags.so.2 /usr/lib/libgflags.so.2
#sudo cp ~/install/TurboParser-2.1.0/deps/local/lib/libglog.so.0 /usr/lib/libglog.so.0
python generateSentence.py "$1"
make
SESSION_MANAGER=''
export DISPLAY=:0.0
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/steve/install/TurboParser-2.2.0/deps/local/lib:"
./main --synttrain test1142.xml newmmodel.txt newsyntmodel.txt ~/install/TurboParser-2.1.0/TurboParser
#./main --syntmark test1142.xml newmmodel.txt newsyntmodel.txt output.txt /home/steve/install/TurboParser-2.2.0/TurboParser
./main --morphmark testSentence.xml morphmark_output.txt newmmodel.txt
./main --syntmark testSentence.xml newmmodel.txt newsyntmodel.txt output_upper.txt /home/steve/install/TurboParser-2.2.0/TurboParser
head -n 100 output_upper.txt
#python toLowerCase.py

