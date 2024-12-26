
#include <iostream>

#include "DCM.h"



int main()
{
    //std::cout << "Hello World!\n";
    
    //std::cout << "DCM Parser General Test\t" << (bool)DCM::Parser::test() << std::endl;
    
    //auto parser = new DCM::Parser();
    //parser->open("Test_DCM2.dcm");
    //std::cout << parser->getRawString() << std::endl;

    DCM::Parser::rebuildUnknownTest();
    DCM::Parser::rebuildCommentTest();

    
    return 0;

}

