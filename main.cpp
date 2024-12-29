
#include <iostream>

#include "DCM.h"



int main()
{
    auto manager = new DCM::Manager();
    manager->openWithRepo("./Test_DCM2/Test_DCM2.dcm");
    //manager->createDCM();
    //manager->parse();
    //manager->saveAsDCM("Test_DCM2_rebuild.dcm");
    //std::cout << "Hello World!\n";
    
    //std::cout << "DCM Parser General Test\t" << (bool)DCM::Parser::test() << std::endl;
    
    /*auto manager = new DCM::Manager();
    manager->open("Test_DCM2.dcm");
    manager->parse();
    std::cout << manager->rebuild() << std::endl;

    //DCM::Parser::rebuildArrayTest();
    //DCM::Parser::rebuildMatrixTest();
    //DCM::Parser::rebuildLineBaseParameterTest();
    //DCM::Parser::rebuildMapBaseParameterTest();
    //DCM::Parser::rebuildDistributionTest();
    //DCM::Parser::createDCMTest();*/
    
    return 0;

}

