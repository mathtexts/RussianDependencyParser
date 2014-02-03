make
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:~/install/TurboParser-2.1.0/deps/local/lib:"
#./main --syntmark test1142.xml newmmodel.txt newsyntmodel.txt output.txt ~/install/TurboParser-2.1.0/TurboParser
#./main --syntmark testSentence.xml newmmodel.txt newsyntmodel.txt output.txt ~/install/TurboParser-2.1.0/TurboParser
./main --morphmark testSentence.xml output.txt newmmodel.txt
head -n 100 output.txt


