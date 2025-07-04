#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ostream>
#include <cstdlib>
#include <unistd.h> 
using namespace std;

class getVals {
    public:
        int valAmount;
        std::string flag;
        std::vector<std::string> originalURLs;
        getVals(int vals) {
            valAmount = vals;
        }
        void addURLs(std::vector<std::string>inputURLs) {
            originalURLs = inputURLs;
        }
        void forkCFile(void) {
            int fd[2];
            if (pipe(fd) == -1) {
                std::cerr<<"Pipe failed to work"<<std::endl;
                exit(1);
            }
            char* args[valAmount+3];
                args[0] = "./childCrawler";
                args[1] = const_cast<char*>(flag.c_str());
                for (int i=0; i<valAmount; ++i) {
                    args[i+2] = const_cast<char*>(originalURLs[i].c_str());
                } 
                args[valAmount+2] = nullptr;
            pid_t pid = fork();
            if (pid == -1) {
                std::cout<<"Error forking process"<<std::endl;
                exit(1);
            }
            if (pid == 0) {
                close(fd[0]);
                if (dup2(fd[1], STDOUT_FILENO) == -1) {
                    std::cerr<<"Error copying child information to read pipe"<<std::endl;
                }
                close(fd[1]);
                execvp(args[0], args);
                std::cout<<"failed to load process image"<<std::endl;
                exit(EXIT_FAILURE);
            } else {
                std::cout<<"hello from parent"<<std::endl;
                close(fd[1]);
                char *buffer = new char[30000];
                ssize_t n = read(fd[0], buffer, 30000);
                if (n > 0) {
                    std::cout << "Read " << n << " bytes from pipe" << std::endl;
                } else if (n == 0) {
                    std::cout << "EOF reached" << std::endl;
                } else {
                    std::cerr << "Error reading from pipe" << std::endl;
                }
                std::cout<< buffer <<std::endl;
                close(fd[0]);
                //TESTING ONLY
                
            }
        }
};
int main(int argc, char* argv[]) {
    getVals* intoC = new getVals(argc-1);
    std::vector<std::string> originalURLs;
    if (argc < 3) {
        std::cout<<"Not enough commands were inputted, aborting..."<< std::endl;
        return 1;
    }
    if (std::string (argv[1]) == "-c") {
        for (int i=0; i<argc-2; ++i) {
            originalURLs.push_back(std::string (argv[i+2]));
        }
        intoC->addURLs(originalURLs);
        intoC->flag = "-c";
        intoC->forkCFile();
    }
    if (std::string (argv[1]) == "-f") {
        std::ifstream inputfile;
        inputfile.open(std::string(argv[2]), std::ios::in);
        if (!inputfile.is_open()) {
            std::cout<<"Error opening file"<<std::endl;
        }
        std::string line;
        while(std::getline(inputfile, line)) {
            originalURLs.push_back(line);
        }
        intoC->addURLs(originalURLs);
        intoC->flag = "-f";
        intoC->forkCFile();
    }
    return 0;
}
